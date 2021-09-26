#include <kt/gfx/view.hpp>
#include <stdexcept>
#include <functional>
namespace kt {
namespace gfx {
View::View(int _w, int _h)
    : window_(init_window(_w, _h))
    , renderer_(window_)
    , width_(_w)
    , height_(_h)
  {
    presentation_texture_.reset(renderer_, _w, _h);
    //color(Color::black());
    //clear();
    //present();
  }
View::View()
    : View(default_width_, default_height_)
  {
  }
View::~View()
  {
    if(window_)               { SDL_DestroyWindow(window_);     }
  }
auto View::update_window_size() -> void
  {
    std::tuple<int, int> result;
    auto& x = std::get<0>(result);
    auto& y = std::get<1>(result);
    SDL_GetWindowSize(window_, &x, &y);
    width_  = x;
    height_ = y;
  }
auto View::init_window(int _width, int _height) -> SDL_Window*
  {
    return sdl_assert
      ( SDL_CreateWindow
        ( "worldgen"
        , SDL_WINDOWPOS_CENTERED
        , SDL_WINDOWPOS_CENTERED
        , _width 
        , _height 
        , 0
        )
      );
  }
auto View::reset_presentation_texture() -> void
  {
    auto window_sz = size();
    presentation_texture_.reset 
        ( renderer_
        , std::get<0>(window_sz)
        , std::get<1>(window_sz)
        , SDL_TEXTUREACCESS_TARGET
        );
  }
auto View::init_renderer() -> SDL_Renderer*
  {
    return sdl_assert
      ( SDL_CreateRenderer
        ( window_
        , -1
        , SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED
        )
      );
  }
auto View::renderer() const -> const Renderer& 
  {
    return renderer_;
  }
auto View::renderer() -> Renderer& 
  {
    return renderer_;
  }
auto View::right_bottom() -> std::tuple<int, int>
  {
    auto result = size();
    auto& w = std::get<0>(result);
    auto& h = std::get<1>(result);
    w = w > 0? w - 1 : 0;
    h = h > 0? h - 1 : 0;
    return result;
  }
auto View::right()  -> int
  {
    return std::get<0>(right_bottom());
  }
auto View::bottom() -> int
  { 
    return std::get<1>(right_bottom());
  }
auto View::size() const -> std::pair<int, int>
  {
    return std::make_pair(width(), height());
  }
auto View::width() const -> int
  {
    return width_;
  }
auto View::height() const -> int
  {
    return height_;
  }
auto View::start_scene() -> void
  {
    renderer().clear();
    renderer().set_target(presentation_texture_);
  }
auto View::stop_scene() -> void
  {
    renderer_.set_default_target();
    renderer_.copy(presentation_texture_);
    renderer().present();
  }
auto View::make_texture(int _w, int _h, int _access) -> Texture
  {
    return Texture(renderer_, _w, _h, _access);
  }

} /* namespace gfx */
} /* namespace kt */
