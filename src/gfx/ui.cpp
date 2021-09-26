#include <kt/gfx/ui.hpp>
#include <stdexcept>

namespace kt {
namespace gfx {
UserInterface::UserInterface()
    : view_(init_view())
  {
  }
UserInterface::UserInterface(int _w, int _h)
    : view_(init_view(_w, _h))
  {
  }
UserInterface::~UserInterface()
  {
    SDL_Quit();
  }
auto UserInterface::init_view() -> View
  {
    init_sdl();
    return View();
  }
auto UserInterface::init_view(int _w, int _h) -> View
  {
    init_sdl();
    return View(_w, _h);
  }
auto UserInterface::start() -> void
  {
    SDL_Event event;
    quit_app_ = false;
    while(quit_app_ == false)
    {
      while(SDL_PollEvent(&event))
      {
        handle(event);
      }
      view().renderer().set_default_target();
      do_render();
      view().renderer().present();
    }
  }
auto UserInterface::init_sdl() -> void
  {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK) != 0)
    {
      throw std::runtime_error("Could not init SDL2");
    }
  }
auto UserInterface::handle(const SDL_Event& _event) -> void
  {
    switch(_event.type)
    {
    case SDL_WINDOWEVENT:
      on_window_event(_event.window);
      break;
    case SDL_CONTROLLERAXISMOTION:
      on_controller_axis_event(_event.caxis);
      break;
    case SDL_CONTROLLERBUTTONDOWN:
      on_controller_button_event(_event.cbutton);
      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      on_keyboard_event(_event.key);
      break;
    }
  }
auto UserInterface::on_keyboard_event(const SDL_KeyboardEvent& _event) -> void
  {
  }
auto UserInterface::on_controller_button_event(const SDL_ControllerButtonEvent& _event) -> void
  {
  }
auto UserInterface::on_controller_axis_event(const SDL_ControllerAxisEvent& _event) -> void
  {
  }
auto UserInterface::on_window_event(const SDL_WindowEvent& _event) -> void
  {
    switch(_event.event)
    {
    case SDL_WINDOWEVENT_CLOSE:
      quit();
      break;
    }
  }
auto UserInterface::do_render() -> void
  {
  }
auto UserInterface::quit() -> void
  {
    quit_app_ = true;
  }
auto UserInterface::restart() -> void
  {
    is_restarting_ = true;
    quit();
  }
auto UserInterface::is_restarting() -> bool
  {
    return is_restarting_;
  }
} /* namespace gfx */
} /* namespace kt */
