#ifndef ui_hpp_20210914_162913_PDT
#define ui_hpp_20210914_162913_PDT
#include "view.hpp"
namespace kt {
namespace gfx {
class UserInterface
{
private:
public:
  ~UserInterface();
  UserInterface(const UserInterface&) = delete;
  UserInterface(UserInterface&&)      = default;
  UserInterface();
  UserInterface(int, int);

  auto start() -> void;

  auto view() const -> const View& { return view_; }
  auto view()       ->       View& { return view_; }

  auto quit() -> void;
  auto restart() -> void;

  auto is_restarting() -> bool;
private:
  View  view_;
  bool  quit_app_      = false;
  bool  is_restarting_ = false;

  auto init_view() -> View;
  auto init_view(int, int) -> View;
  auto init_sdl() -> void;
  auto handle(const SDL_Event&) -> void;

  virtual auto on_keyboard_event(const SDL_KeyboardEvent&) -> void;
  virtual auto on_window_event(const SDL_WindowEvent&) -> void;
  virtual auto on_controller_button_event(const SDL_ControllerButtonEvent& _event) -> void;
  virtual auto on_controller_axis_event(const SDL_ControllerAxisEvent&) -> void;
  virtual auto do_render() -> void;
};
  
} /* namespace gfx */
} /* namespace kt */
#endif//ui_hpp_20210914_162913_PDT
