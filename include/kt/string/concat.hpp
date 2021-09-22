#ifndef concat_hpp_20210909_224708_PDT
#define concat_hpp_20210909_224708_PDT
#include <boost/lexical_cast.hpp>
/*****************************************************************************
 * string concatenation 
 ****************************************************************************/
namespace kt {
  
template<typename HeadT>
  auto concat(HeadT&& head)
  {
    return boost::lexical_cast<std::string>(head);
  }
template<typename HeadT, typename...TailTs>
  auto concat(HeadT&& head, TailTs&&...tail)
  {
    return concat(head) + concat(std::forward<TailTs>(tail)...);
  }
} /* namespace kt */
#endif//concat_hpp_20210909_224708_PDT
