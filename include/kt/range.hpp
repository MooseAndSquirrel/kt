#ifndef range_hpp_20210925_151840_PDT
#define range_hpp_20210925_151840_PDT
#include <utility>

namespace kt {
 
template<typename T>
using range = std::pair<T, T>;

template<typename T, typename U>
constexpr auto convert_fn(const range<T>& _r1, const range<U>& _r2)
  {
    return [=](const T& _value) -> U
      {
        auto a_min = std::min(_r1.first, _r1.second);
        auto a_max = std::max(_r1.first, _r1.second);
        auto b_min = std::min(_r2.first, _r2.second);
        auto b_max = std::max(_r2.first, _r2.second);
        auto scale = double(b_max - b_min) / double(a_max - a_min);
        return b_min + (((U)_value - a_min) * scale);
      };
  }
template<typename T, typename U>
constexpr auto convert(const T& _value, const range<T>& _r1, const range<U>& _r2) -> U
  {
    return convert_fn(_r1, _r2)(_value);
  }

} /* namespace kt */
#endif//range_hpp_20210925_151840_PDT
