#pragma once
#include "Account.h"
class CreditCard :  public Account
{
private:
	long long creditLimits;
public:
	CreditCard(const std::string& name,long long limit);

	bool withdraw(long long amount) override;

	long long getCreditLimit() const { return creditLimits; }
};

