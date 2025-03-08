#ifndef MACROS_HPP_INCLUDED
#define MACROS_HPP_INCLUDED

#include <memory>
#include <string>

template <typename T>
using __Ptr = std::shared_ptr<T>;

extern std::string IMPLEMENTATION_MISSING;

#endif