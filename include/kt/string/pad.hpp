#ifndef kt_string_pad_hpp_20210922_123157_PDT
#define kt_string_pad_hpp_20210922_123157_PDT

namespace kt {
 
template<typename StringT>
  auto pad(StringT&& _s, std::string::size_type _sz) -> void
  {
    if(_s.size() < _sz)
    {
      auto pad_sz = _sz - _s.size();
      _s += std::string(pad_sz, ' ');
    }
  }
template<typename StringT>
  auto pad_copy(StringT&& _s, std::string::size_type _sz) -> std::string
  {
    auto result = std::string { _s };
    pad(result, _sz);
    return result;
  }
} /* namespace kt */
#endif//kt_string_pad_hpp_20210922_123157_PDT
