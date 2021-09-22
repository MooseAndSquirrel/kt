#include <kt/terminal.hpp>
#include <sys/ioctl.h>
#include <unistd.h>

namespace kt::terminal {
auto get_size() -> std::pair<size_t, size_t>
  {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return std::make_pair(w.ws_col, w.ws_row);
  }
auto get_columns() -> size_t
  {
    return get_size().first;
  }
auto get_rows() -> size_t
  {
    return get_size().second;
  }
} /* namespace kt::terminal */
