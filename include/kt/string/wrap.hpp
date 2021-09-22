#ifndef wrap_hpp_20210921_203454_PDT
#define wrap_hpp_20210921_203454_PDT
#include <functional>
namespace kt {
 
template<typename StringT>
auto word_wrap(const StringT& _s, size_t _width) -> std::vector<StringT>
  {
    std::vector<StringT> rows;
    size_t src_end    = _s.size();
    std::function<void(size_t)> start;
    start = [&](size_t row_start)
      {
        // set the row end to the row start + column width, limited to source end
        auto row_end = std::min(src_end, row_start + _width);
        // avoid empty or negative-length row string (probably redundant check)
        if(row_start < src_end)
        {
          StringT result;
          // if the end of the row is also the end of the source, no word wrap
          if(row_end == src_end)
          {
            result = _s.substr(row_start, row_end - row_start); // use whole row
          }
          else
          {
            // if the char at `row_end` (one-past actual row data) 
            // is not whitespace then we need to word wrap
            if(!isspace(_s[row_end]))
            {
              // walk backwards adjusting row end until it follows the last
              // whitespace character in the row
              auto new_row_end = row_end;
              for(; new_row_end > row_start && !isspace(_s[new_row_end - 1]); --new_row_end);
              // if no whitespace was found, use the whole row
              if(row_start == new_row_end)
              {
                result = _s.substr(row_start, row_end - row_start);
              }
              else
              {
                // adjust the row end and then get the result
                row_end = new_row_end;
                result = _s.substr(row_start, row_end - row_start);
              }
            }
          }
          // add the row to the collection
          rows.push_back(result);
          // if the row end is not the end of the source, recurse
          if(row_end < src_end)
          {
            start(row_end);
          }
        }
      };
    start(0);
    return rows;
  }
} /* namespace kt */
#endif//wrap_hpp_20210921_203454_PDT
