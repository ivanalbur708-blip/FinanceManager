#include "CreditCard.h"
#include <stdexcept>


CreditCard::CreditCard(const std::string& name, long long limit) : 
Account(name),creditLimits(limit)
{   
	//Перевіряємо, чи не є кредитний ліміт від'ємним
	if (limit < 0)
	{
		throw std::invalid_argument("Credit limit cannot be negative");
	}
}

// Знімаємо кошти з урахуванням кредитного ліміту
bool CreditCard::withdraw(long long amount)
{
	//Перевіряємо суму та доступні кошти разом із кредитним лімітом
	if (amount > 0 && (balance + creditLimits) >= amount)
	{
		balance -= amount;
		return true;
	}
	else return false;
}


