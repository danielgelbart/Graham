#ifndef DMMM_IDENTIFIERS_HPP
#define DMMM_IDENTIFIERS_HPP

#include "dmmm_id.hpp"

namespace DMMM {
class DummyO_BalanceSheet;
typedef ID::Id<DummyO_BalanceSheet> I_BalanceSheet;
class DummyO_Dividend;
typedef ID::Id<DummyO_Dividend> I_Dividend;
class DummyO_Ep;
typedef ID::Id<DummyO_Ep> I_Ep;
class DummyO_Note;
typedef ID::Id<DummyO_Note> I_Note;
class DummyO_Numshare;
typedef ID::Id<DummyO_Numshare> I_Numshare;
class DummyO_Search;
typedef ID::Id<DummyO_Search> I_Search;
class DummyO_ShareClass;
typedef ID::Id<DummyO_ShareClass> I_ShareClass;
class DummyO_Split;
typedef ID::Id<DummyO_Split> I_Split;
class DummyO_Stock;
typedef ID::Id<DummyO_Stock> I_Stock;
class DummyO_User;
typedef ID::Id<DummyO_User> I_User;

} //namespace DMMM
#endif //DMMM_IDENTIFIERS_HPP
