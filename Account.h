#pragma once
#include <string>

class Account
{
protected:
	std::string name;
	long long balance = 0;
public:
	Account(const std::string& name);

	virtual ~Account() = default;
	
	virtual bool deposit(long long amount);
	virtual bool withdraw(long long amount);

	long long getBalance()const { return balance; }

	const std::string& getName() const { return name; }
};

