#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <fstream>
#include <exception>


#include "Account.h"
#include "CreditCard.h"
#include "DebitCard.h"
#include "Transaction.h"
#include "Wallet.h"

class FinanceManager
{
private:
	//Зберігаємо рахунки різних типів через вказівник на базовий клас
	std::vector<std::unique_ptr<Account>> accounts;

	//Зберігаємо історію доходів і витрат
	std::vector<Transaction> transactions;
public:

	//Створюємо гаманець і додаємо його до списку рахунків
	void addWallet(const std::string& name)
	{
		accounts.push_back(std::make_unique<Wallet>(name));
	}

	//Створюємо дебетову картку
	void addDebitCard(const std::string& name, const std::string& bankName)
	{
		accounts.push_back(std::make_unique<DebitCard>(name, bankName));
	//Створюємо кредитну картку із заданим лімітом
	}

	void addCreditCard(const std::string& name, long long limit)
	{
		accounts.push_back(std::make_unique<CreditCard>(name, limit));
	}

	//Поповнюємо вибраний рахунок 
	bool depositToAccount(int index, long long amount)
	{
		//Перевіряємо правильність індексу та суми
		if (index < 0 || index >= accounts.size() || amount <= 0) return false;

		//Повертаємо результат поповнення вибраного рахунку 
		return accounts[index]->deposit(amount);
	
	}

	//Додаємо витрату та списуємо кошти з рахунку 
	bool addExpense(int index, long long amount, const std::string& category, const std::string& date)
	{
		//Перевіряємо індекс рахунку та суму витрати
		if (index < 0 || index >= accounts.size() || amount <= 0) return false;


		//Якщо коштів недостатньо, операція не виконується 
		if (!accounts[index]->withdraw(amount)) return false;

		//Створюємо запис витрати у векторі транзакцій
		//false означає,що це витрата 
		transactions.emplace_back(amount, category, date, false);

		return true;
	}

	//Додаємо дохід та поповнюємо баланс рахунку 
	bool addIncome(int index, long long amount, const std::string& category, const std::string& date)
	{
		//Перевіряємо індекс рахунку та суму доходу
		if (index < 0 || index >= accounts.size() || amount <= 0) return false;

		//Виконуємо поповнення рахунку 
		if (!accounts[index]->deposit(amount)) return false;

		//Зберігаємо дохід 	до списку транзакцій
		//true означає,що це дохід
		transactions.emplace_back(amount, category, date, true);

		return true;
	}
	//Виводимо інформацію про всі рахунки 
	void showAccounts() const
	{

		//Перевіряємо, чи є створені рахунки
		if (accounts.empty())
		{
			std::cout << "No accounts\n";
			return;
		}

		//Перебираємо рахунки та виводимо їхні баланси
		for (std::size_t i = 0;i < accounts.size();i++)
		{
			std::cout << i << ". " 
			<< accounts[i]->getName() << " | Balance: " 
			<< accounts[i]->getBalance() << '\n';
		}
	}

	//Виводимо історію фінансових операцій
	void showTransactions() const
	{
		//Перевіряємо, чи є збережені транзакції
		if (transactions.empty())
		{
			std::cout << "No  transactions\n";
			return;
		}
		
		//Перебираємо всі транзакції
		for (std::size_t i = 0; i < transactions.size(); i++)
		{
			//Тернарний оператор визначає тип операції
			std::cout << i << ". "
				<< (transactions[i].IsIncome() ? "Income" : "Expense")
				<< " | Category: " << transactions[i].getCategory()
				<< " | Amount: " << transactions[i].getAmount()
				<< " | Date: " << transactions[i].getDate()
				<< '\n';
		}
	}

	//Формуємо звіт про витрати за день,тиждень або місяць 
	//out дозволяє виводити дані в консоль або файл
	void showReport(int period, const std::string& date,const std::string& endDate = "",std::ostream& out = std::cout) const
	{
		//Загальна сума витрат
		double total = 0;

		//Зберігаємо загальні суми витрат за категоріями
		std::map<std::string, double> categoryTotals;

		//Перебираємо всі фінансові операції
		for (const Transaction& transaction : transactions)
		{
			//Пропускаємо доходи, оскільки звіт стосується витрат
			if (transaction.IsIncome()) continue;

			//Перевіряємо дату для звіту за день
			if (period == 1 && transaction.getDate() != date) continue;

			//Для місяця порівнюємо рік і місяць
			if (period == 3 && transaction.getDate().substr(0, 7) != date.substr(0, 7)) continue;

			//Для тижня перевіряємо, чи входить дата у вибраний діапазон
			if (period == 2 && (transaction.getDate() < date || transaction.getDate() > endDate)) continue;

			//Додаємо суму витрати до загального підсумку
			total += transaction.getAmount();

			//Додаємо загальну суму витрат
			categoryTotals[transaction.getCategory()] += transaction.getAmount();

		}
		//Виводимо загальну суму витрат
		out << "\n Total expenses: " << total << '\n';
		//Виводимо витрати окремо за кожною категорією
		for (const auto& category : categoryTotals)
		{
			out << category.first << ": " 
			<< category.second << '\n';
		}
		
	}


	//Знаходимо ТОП-3 витрат і категорій за вибраний період
	void showTop3(int period, const std::string& date,const std::string& endDate = "",std::ostream& out = std::cout) const
	{
		//Зберігаємо витрати за вибраний період
		std::vector<Transaction> expenses;
		//Підраховуємо загальні витрати за категоріями
		std::map<std::string, double> categoryTotals;

		//Перебираємо всі транзакції
		for (const Transaction& transaction : transactions)
		{
			//Пропускаємо доходи
			if (transaction.IsIncome()) continue;

			//Перевіряємо діапазон дат для тижня
			if (period == 2 && (transaction.getDate() < date || transaction.getDate() > endDate))
				continue;

			//Перевіряємо місяць і рік
			if (period == 3 && transaction.getDate().substr(0, 7) != date.substr(0, 7))
				continue;

			//Додаємо витрату до списку
			expenses.push_back(transaction);

			//Підраховуємо суму витрат для категорії
			categoryTotals[transaction.getCategory()] += transaction.getAmount();
		}


		//Сортуємо витрати за спаданням суми
		std::sort(expenses.begin(), expenses.end(),
			[](const Transaction& a, const Transaction& b)
			{
				return a.getAmount() > b.getAmount();
			});

		out << "\n===== TOP-3 Expenses =====\n";

		//Виводимо не більше трьох найбільших витрат
		for (std::size_t i = 0; i < 3 && i < expenses.size(); i++)
		{
			out << i + 1 << ". "
				<< expenses[i].getCategory()
				<< " | Amount: " << expenses[i].getAmount()
				<< " | Date: " << expenses[i].getDate()
				<< '\n';
		}

		//Перетворюємо map у vector пар для подальшого сортування
		std::vector<std::pair<std::string, double>> categories(
			categoryTotals.begin(), categoryTotals.end());

		//Сортуємо категорії за загальною сумою витрат
		//first — назва категорії, second — сума
		std::sort(categories.begin(), categories.end(),
			[](const auto& a, const auto& b)
			{
				return a.second > b.second;
			});

		out << "\n===== TOP-3 Categories =====\n";

		//Виводимо три категорії з найбільшими сумами витрат
		for (std::size_t i = 0; i < 3 && i < categories.size(); i++)
		{
			out << i + 1 << ". "
				<< categories[i].first
				<< " | Total: " << categories[i].second
				<< '\n';
		}
	}

	//Головний метод для роботи з консольним меню
	void Run()
	{
		int choice;

		//Повторюємо меню, поки користувач не вибере вихід
		while (true)
		{
			std::cout << "\n===== Finance Manager =====\n";
			std::cout << "1. Add wallet\n";
			std::cout << "2. Add debit card\n";
			std::cout << "3. Add credit card\n";
			std::cout << "4. Show accounts\n";
			std::cout << "5. Deposit money\n";
			std::cout << "6. Add expense\n";
			std::cout << "7. Add income\n";
			std::cout << "8. Show transactions\n";
			std::cout << "9. Reports\n";
			std::cout << "10. TOP-3\n";
			std::cout << "11. Save report\n";
			std::cout << "0. Exit\n";

			//Отримуємо вибір користувача
			std::cin >> choice;

			//Завершуємо роботу меню
			if (choice == 0) break;

			//Виконуємо дію залежно від вибраного пункту
			switch (choice)
			{
				//Створення нового гаманця
			case 1: {
				std::string name;
				std::cout << "Enter wallet name: ";
				//Зчитуємо назву гаманця повністю
				std::getline(std::cin >> std::ws, name);
				addWallet(name);
				break;
			}
				  //Створення дебетової картки
			case 2: 
				{
				std::string name;
				std::string bankName;
				std::cout << "Enter name: ";
				std::getline(std::cin >> std::ws, name);
				std::cout << "Enter bank name:";
				std::getline(std::cin >> std::ws, bankName);
				addDebitCard(name, bankName);
				break;
			}
				//Створення кредитної картки
			case 3: {
				std::string name;
				long long limit;
				std::cout << "Enter name: ";
				std::getline(std::cin >> std::ws, name);
				std::cout << "Enter limit: ";
				std::cin >> limit;
				//Перехоплюємо можливу помилку кредитного ліміту
				try {
				addCreditCard(name,limit);
				}
				catch (const std::exception& e)
				{
					std::cout << "Error: " << e.what() << '\n';
				}
				break;
			}
				  //Виведення всіх рахунків
			case 4:
				showAccounts();
				break;
				//Поповнення рахунку
			case 5: {
				int index;
				long long amount;
				std::cout << "Enter index: ";
				std::cin >> index;
				std::cout << "Enter amount: ";
				std::cin >> amount;
				//Перевіряємо результат поповнення рахунку
				if (depositToAccount(index, amount))
					std::cout << "Deposit successful\n";
				else
					std::cout << "Deposit failed\n";

				break;
			}
				  //Додавання витрати
			case 6: {
				int index;
				long long amount;
				std::string category;
				std::string date;
				std::cout << "Enter index: ";
				std::cin >> index;
				std::cout << "Enter amount: ";
				std::cin >> amount;
				std::cout << "Enter category: ";
				std::getline(std::cin >> std::ws, category);
				std::cout << "Enter date: ";
				std::getline(std::cin >> std::ws, date);
				//Виконуємо списання та зберігаємо витрату
				if (addExpense(index, amount, category, date))
					std::cout << "Expense added successfully\n";
				else
					std::cout << "Failed to add expense\n";

               break;
			   }
				  //Додавання доходу
			case 7:
				{
				int index;
				long long amount;
				std::string category;
				std::string date;
				std::cout << "Enter index: ";
				std::cin >> index;
				std::cout << "Enter amount: ";
				std::cin >> amount;
				std::cout << "Enter category: ";
				std::getline(std::cin >> std::ws, category);
				std::cout << "Enter date: ";
				std::getline(std::cin >> std::ws, date);
				//Виконуємо поповнення та зберігаємо дохід
				if (addIncome(index, amount, category, date))
					std::cout << "Income added successfully\n";
				else
					std::cout << "Failed to add income\n";

			   break;
				}
				//Виведення історії транзакцій
			case 8:
				showTransactions();
				break;
				//Формування звіту за день, тиждень або місяць
			case 9:{
				int period;
				std::string date;
				std::string endDate;

				std::cout << "Select period:\n";
				std::cout << "1. Day\n";
				std::cout << "2. Week\n";
				std::cout << "3. Month\n";
				std::cout << "Choice: ";
				std::cin >> period;

				//Перевіряємо правильність вибору періоду
				if (period < 1 || period > 3)
				{
					std::cout << "Invalid period\n";
					break;
				}

				std::cout << "Enter date (YYYY-MM-DD): ";
				std::getline(std::cin >> std::ws, date);

				//Для тижня додатково отримуємо кінцеву дату
				if (period == 2)
				{
					std::cout << "Enter Sunday date (YYYY-MM-DD): ";
					std::getline(std::cin >> std::ws, endDate);
				}
				//Виводимо звіт у консоль
				showReport(period, date, endDate);
				break;
			    }
				  //Виведення ТОП-3 витрат і категорій
			case 10:
			{
				int period;
				std::string date;
				std::string endDate;

				std::cout << "Select period:\n";
				std::cout << "2. Week\n";
				std::cout << "3. Month\n";
				std::cout << "Choice: ";
				std::cin >> period;

				//Перевіряємо допустимий період
				if (period != 2 && period != 3)
				{
					std::cout << "Invalid period\n";
					break;
				}

				std::cout << "Enter date (YYYY-MM-DD): ";
				std::getline(std::cin >> std::ws, date);

				if (period == 2)
				{
					std::cout << "Enter Sunday date (YYYY-MM-DD): ";
					std::getline(std::cin >> std::ws, endDate);
				}

				//Формуємо рейтинг за вибраний період
				showTop3(period, date, endDate);
				break;
			  }
			       //Збереження звіту у файл
			case 11:
			{
				int period;
				std::string date;
				std::string endDate;
				std::string filename;

				std::cout << "Select period:\n";
				std::cout << "1. Day\n";
				std::cout << "2. Week\n";
				std::cout << "3. Month\n";
				std::cout << "Choice: ";
				std::cin >> period;

				if (period < 1 || period > 3)
				{
					std::cout << "Invalid period\n";
					break;
				}

				std::cout << "Enter date (YYYY-MM-DD): ";
				std::getline(std::cin >> std::ws, date);

				if (period == 2)
				{
					std::cout << "Enter Sunday date (YYYY-MM-DD): ";
					std::getline(std::cin >> std::ws, endDate);
				}

				std::cout << "Enter file name: ";
				std::getline(std::cin >> std::ws, filename);

				//Відкриваємо файл у режимі додавання нових записів
				std::ofstream file(filename, std::ios::app);

				//Перевіряємо, чи вдалося відкрити файл
				if (!file)
				{
					std::cout << "Failed to open file\n";
					break;
				}

				//Записуємо заголовок звіту та дату
				file << "\n===== Finance Manager Report =====\n";
				file << "Date: " << date << '\n';

				//Записуємо звіт у файл через файловий потік
				showReport(period, date, endDate, file);

				//Для тижня та місяця додатково записуємо ТОП-3
				if (period == 2 || period == 3)
					showTop3(period, date, endDate, file);

				//Закриваємо файл після запису
				file.close();

				//Перевіряємо стан файлового потоку після закриття
				if (file)
					std::cout << "Report saved successfully\n";
				else
					std::cout << "Failed to save report\n";

				break;
			  }

			}

		}
	}
};

int main()
{
	//Створюємо об'єкт FinanceManager та запускаємо меню
	FinanceManager manager;
	manager.Run();
}

