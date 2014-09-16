#include "dmmm_utils.hpp"

namespace DMMM {
namespace UTILS {

template<>
std::string
fromString<std::string>(const std::string& s)
{
    return s;
}

} //namespace UTILS
} //namespace DMMM
