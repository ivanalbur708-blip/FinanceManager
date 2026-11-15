#include "Account.h"

Account::Account(const std::string& name) : name(name){}


//Поповнюємо баланс рахунку
bool Account::deposit(long long amount)
{
	//Перевірємо, чи сума поповнення додатна
	if (amount > 0)
	{
		balance += amount;
		return  true;
	}
	else return false;
}

//Знімаєсо кошти з рахунку
bool Account::withdraw(long long amount)
{
	//Перевіряємо суму та наявність достатніх коштів
	if (amount > 0 && balance >= amount)
	{
		balance -= amount;
		return true;
	}
	else return false;
}


