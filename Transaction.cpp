#include "Transaction.h"

Transaction::Transaction(double amount, std::string category, std::string date, bool isIncome) : 
amount(amount),category(category),date(date),isIncome(isIncome){}

