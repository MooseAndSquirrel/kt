#ifndef terminal_hpp_20210921_192454_PDT
#define terminal_hpp_20210921_192454_PDT
#include <utility>
namespace kt::terminal {
 
auto get_size() -> std::pair<std::size_t, std::size_t>;
auto get_columns() -> std::size_t;
auto get_rows() -> std::size_t;

} /* namespace kt::terminal */
#endif//terminal_hpp_20210921_192454_PDT
