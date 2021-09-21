#ifndef ui_hpp_20210914_162913_PDT
#define ui_hpp_20210914_162913_PDT
#include "kt-view.hpp"
class UserInterface
{
private:
public:
  ~UserInterface();
  UserInterface();
  UserInterface(const UserInterface&) = delete;
  UserInterface(UserInterface&&)      = delete;
  auto start() -> void;

  auto view() const -> const View& { return view_; }
  auto view()       ->       View& { return view_; }
private:
  View  view_;
  bool  quit_app_ = false;

  auto init_view() -> View;
  auto init_sdl() -> void;
  auto handle_keydown(const SDL_KeyboardEvent& _event) -> void;
  auto handle(const SDL_WindowEvent& _event) -> void;

};
#endif//ui_hpp_20210914_162913_PDT
