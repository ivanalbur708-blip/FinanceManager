#pragma once
#include "Account.h"
class DebitCard : public Account
{
private:	
	std::string bankName;
public:
	DebitCard(const std::string& name,const std::string& bankName);

	const std::string& getBankName() const { return bankName; }
};

