#ifndef STOCKHELPER_H
#define STOCKHELPER_H

#include "O_Stock.hpp"
#include "O_Ep.hpp"

class StockHelper {
public:
    StockHelper(DMMM::O_Stock stock): _stock(stock) {}
    DMMM::O_Ep get_latest_quarter();

private:
    DMMM::O_Stock _stock;
};

#endif // STOCKHELPER_H
