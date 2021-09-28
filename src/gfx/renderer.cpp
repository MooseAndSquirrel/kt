#include <kt/gfx/renderer.hpp>
#include <functional>
namespace kt {
namespace gfx {
Renderer::Renderer(SDL_Window* _w, int _index, uint32_t _flags)
  {
    renderer_ = sdl_assert(SDL_CreateRenderer(_w, _index, _flags));
  }
Renderer::~Renderer()
  {
    release();
  }
auto Renderer::get() -> SDL_Renderer*
  {
    return renderer_;
  }
auto Renderer::release() -> void
  {
    if(renderer_)
    {
      SDL_DestroyRenderer(renderer_);
      renderer_ = nullptr;
    }
  }
auto Renderer::color(uint8_t _r, uint8_t _g, uint8_t _b) -> void
  {
    color(_r, _g, _b, SDL_ALPHA_OPAQUE);
  }
auto Renderer::color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) -> void
  {
    sdl_assert(SDL_SetRenderDrawColor(renderer_, _r, _g, _b, _a));
  }
auto Renderer::clear() -> void
  {
    sdl_assert(SDL_RenderClear(renderer_));
  }
auto Renderer::present() -> void
  {
    SDL_RenderPresent(renderer_);
  }
auto Renderer::point(int _x, int _y) -> void
  {
    sdl_assert(SDL_RenderDrawPoint(renderer_, _x, _y));
  }
auto Renderer::line(int _x1, int _y1, int _x2, int _y2) -> void
  {
    sdl_assert(SDL_RenderDrawLine(renderer_, _x1, _y1, _x2, _y2));
  }
auto Renderer::line_f(float _x1, float _y1, float _x2, float _y2) -> void
  {
    sdl_assert(SDL_RenderDrawLineF(renderer_, _x1, _y1, _x2, _y2));
  }
auto Renderer::color(const Color&  c) -> void
  {
    color(c.r(), c.g(), c.b(), c.a());
  }
auto Renderer::color(uint8_t c) -> void
  {
    color(c, c, c);
  }
auto Renderer::circle(int _cx, int _cy, int _radius, bool _fill) -> void
  {
    using value = std::decay_t<int>;
    auto x = _radius;
    auto x_sq = x * x;
    auto x_trigger = x_sq - 2 * x - 1;;
    auto y = value(0);
    std::function<void(int, int)> plot_circle = [&](auto x, auto y)
      {
        point(_cx - x, _cy + y);
        point(_cx + x, _cy + y);
        point(_cx - x, _cy - y);
        point(_cx + x, _cy - y);
        point(_cx - y, _cy + x);
        point(_cx + y, _cy + x);
        point(_cx - y, _cy - x);
        point(_cx + y, _cy - x);
      };
    std::function<void(int, int)> plot_fill = [&](auto x, auto y)
      {
        auto horz_line = [&](int _x1, int _x2, int _y)
          {
            auto x1 = std::min(_x1, _x2);
            auto x2 = std::max(_x1, _x2);
            for(int p = x1; p <= x2; ++p)
            {
              point(p, _y);
            }
          };
        horz_line(_cx + 0, _cx - x, _cy + y);
        horz_line(_cx + 0, _cx + x, _cy + y);
        horz_line(_cx + 0, _cx - x, _cy - y);
        horz_line(_cx + 0, _cx + x, _cy - y);
        horz_line(_cx + 0, _cx - y, _cy + x);
        horz_line(_cx + 0, _cx + y, _cy + x);
        horz_line(_cx + 0, _cx - y, _cy - x);
        horz_line(_cx + 0, _cx + y, _cy - x);
      };
    auto plot_all = _fill? plot_fill : plot_circle;
    auto next_y = [&]
      {
        if(x_sq <= x_trigger)
        {
          --x;
          x_trigger -= 2 * x - 1;
        }
        x_sq -= 2 * y + 1;
        plot_all(x, y);
        ++y;
      };
    while(x > y)
    {
      next_y();
    }
  };
auto Renderer::circle_fill(int _cx, int _cy, int _radius) -> void
  {
    circle(_cx, _cy, _radius, true);
  }
auto Renderer::set_target(Texture& _t) -> void
  {
    SDL_SetRenderTarget(renderer_, _t.get());
  }
auto Renderer::set_default_target() -> void
  {
    SDL_SetRenderTarget(renderer_, NULL);
  }
auto Renderer::copy(const Texture& _t) -> void
  {
    sdl_assert(SDL_RenderCopy(renderer_, _t.get(), NULL, NULL));
  }
auto Renderer::copy(const Texture& _t, SDL_Rect& _dest) -> void
  {
    sdl_assert(SDL_RenderCopy(renderer_, _t.get(), NULL, &_dest));
  }
auto Renderer::copy(const Texture& _t, SDL_Rect& _src, SDL_Rect& _dest) -> void
  {
    sdl_assert(SDL_RenderCopy(renderer_, _t.get(), &_src, &_dest));
  }
auto Renderer::copy(const Texture& _t, int _x, int _y) -> void
  {
    int w, h;
    sdl_assert(SDL_QueryTexture(_t.get(), NULL, NULL, &w, &h));
    SDL_Rect dest { _x, _y, w, h };
    sdl_assert(SDL_RenderCopy(renderer_, _t.get(), NULL, &dest));
  }
auto Renderer::copy(const Texture& _t, int _x, int _y, double _angle) -> void
  {
    int w, h;
    sdl_assert(SDL_QueryTexture(_t.get(), NULL, NULL, &w, &h));
    SDL_Rect dest { _x, _y, w, h };
    sdl_assert(SDL_RenderCopyEx(renderer_, _t.get(), NULL, &dest, _angle, NULL, SDL_FLIP_NONE));
  }
auto Renderer::set_draw_blend(SDL_BlendMode _m) -> void
  {
    SDL_SetRenderDrawBlendMode(renderer_, _m);
  }
auto Renderer::get_color() -> Color
  {
    uint8_t _r, _g, _b, _a;
    sdl_assert(SDL_GetRenderDrawColor(renderer_, &_r, &_g, &_b, &_a));
    return Color { _r, _g, _b, _a };
  }
} /* namespace gfx */
} /* namespace kt */
