#include <kt/gfx/kt-view.hpp>
#include <stdexcept>

namespace /* anon */ {
template<typename T>
auto sdl_assert(T* _api_result) -> T*
  {
    if(_api_result == nullptr)
    {
      throw std::runtime_error(SDL_GetError());
    }
    return _api_result;
  }
auto sdl_assert(int _api_result) -> int
  {
    if(_api_result != 0)
    {
      throw std::runtime_error(SDL_GetError());
    }
    return _api_result;
  }
} /* namespace anon  */
View::View(int _w, int _h)
    : window_(init_window(_w, _h))
    , renderer_(init_renderer())
  {
    reset_presentation_texture();
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
    if(renderer_)             { SDL_DestroyRenderer(renderer_); }
    if(presentation_texture_) { SDL_DestroyTexture(presentation_texture_); }
  }
auto View::window_size() -> std::tuple<int, int>
  {
    std::tuple<int, int> result;
    auto& x = std::get<0>(result);
    auto& y = std::get<1>(result);
    SDL_GetWindowSize(window_, &x, &y);
    return result;
  }
auto View::window_width() -> int
  {
    return std::get<0>(window_size());
  }
auto View::window_height() -> int
  {
    return std::get<1>(window_size());
  }
auto View::init_window(size_t _width, size_t _height) -> SDL_Window*
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
auto View::destroy_presentation_texture() -> void
  {
    SDL_DestroyTexture(presentation_texture_);
    presentation_texture_ = nullptr;
  }
auto View::reset_presentation_texture() -> void
  {
    if(presentation_texture_)
    {
      destroy_presentation_texture();
    }
    auto window_sz = window_size();
    presentation_texture_ = sdl_assert
      ( SDL_CreateTexture
        ( renderer_ 
        , SDL_PIXELFORMAT_RGBA32
        , SDL_TEXTUREACCESS_TARGET
        , std::get<0>(window_sz)
        , std::get<1>(window_sz)
        )
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
auto View::color(uint8_t _r, uint8_t _g, uint8_t _b) -> void
  {
    color(_r, _g, _b, SDL_ALPHA_OPAQUE);
  }
auto View::color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) -> void
  {
    sdl_assert(SDL_SetRenderDrawColor(renderer_, _r, _g, _b, _a));
  }
auto View::clear() -> void
  {
    sdl_assert(SDL_RenderClear(renderer_));
  }
auto View::present() -> void
  {
    SDL_RenderPresent(renderer_);
  }
auto View::point(int _x, int _y) -> void
  {
    sdl_assert(SDL_RenderDrawPoint(renderer_, _x, _y));
  }
auto View::line(int _x1, int _y1, int _x2, int _y2) -> void
  {
    sdl_assert(SDL_RenderDrawLine(renderer_, _x1, _y1, _x2, _y2));
  }
auto View::line_f(float _x1, float _y1, float _x2, float _y2) -> void
  {
    sdl_assert(SDL_RenderDrawLineF(renderer_, _x1, _y1, _x2, _y2));
  }
auto View::color(const Color&  c) -> void
  {
    color(c.r(), c.g(), c.b(), c.a());
  }
auto View::right_bottom() -> std::tuple<int, int>
  {
    auto result = window_size();
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
auto View::color(uint8_t c) -> void
  {
    color(c, c, c);
  }
auto View::size() const -> std::pair<int, int>
  {
    int  w, h;
    SDL_GetWindowSize(window_, &w, &h);
    return std::make_pair(w, h);
  }
auto View::width() const -> int
  {
    return size().first;
  }
auto View::height() const -> int
  {
    return size().second;
  }
auto View::start_scene() -> void
  {
    clear();
    SDL_SetRenderTarget(renderer_, presentation_texture_);
  }
auto View::stop_scene() -> void
  {
    sdl_assert(SDL_SetRenderTarget(renderer_, NULL));
    sdl_assert(SDL_RenderCopy(renderer_, presentation_texture_, NULL, NULL));
    present();
  }
