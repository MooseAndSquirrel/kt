#include "kt-ui.hpp"
#include <stdexcept>

UserInterface::UserInterface()
    : view_(init_view())
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
auto UserInterface::start() -> void
  {
    SDL_Event event;
    quit_app_ = false;
    while(quit_app_ == false)
    {
      while(SDL_PollEvent(&event))
      {
        switch(event.type)
        {
        case SDL_WINDOWEVENT:
          handle(event.window);
          break;
        case SDL_KEYDOWN:
          handle_keydown(event.key);
          break;
        }
      }
      view().color(Color::blue());
      view().line(0,0,100,100);
      view().color(Color::yellow());
      view().point(100, 50);
      view().present();
    }
  }
auto UserInterface::init_sdl() -> void
  {
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
      throw std::runtime_error("Could not init SDL2");
    }
  }
auto UserInterface::handle_keydown(const SDL_KeyboardEvent& _event) -> void
  {
    switch(_event.keysym.sym)
    {
    case SDLK_ESCAPE:
      quit_app_ = true;
      break;
    }
  }
auto UserInterface::handle(const SDL_WindowEvent& _event) -> void
  {
    switch(_event.event)
    {
      case SDL_WINDOWEVENT_CLOSE:
        quit_app_ = true;
        break;
      case SDL_WINDOWEVENT_EXPOSED:
        view().present();
        break;
    }
  }

