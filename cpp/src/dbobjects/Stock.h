#ifndef STOCK_H
#define STOCK_H

#include "O_Stock.hpp"

class Stock: public O_Stock{
public:
    // Given a report date, calculate to wich quarter it belongs
    size_t quarterNum(date reportDate);

};
#endif
