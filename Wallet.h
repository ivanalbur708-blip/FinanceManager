#pragma once
#include <string>
#include "Account.h"
class Wallet : public Account
{
public:
	explicit Wallet(const std::string& name);
};

