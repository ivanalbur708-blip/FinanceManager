#pragma once
#include <string>
class Transaction
{
private:
	double amount;
	std::string category;
	std::string date;
	bool isIncome;
public:
	Transaction(double amount,std::string category,std::string date,bool isIncome);

	double getAmount() const { return amount; }
	const std::string& getCategory() const { return category; }
	const std::string& getDate() const { return date; }
	bool IsIncome() const { return isIncome; }
};

