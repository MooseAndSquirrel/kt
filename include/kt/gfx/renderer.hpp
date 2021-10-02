#ifndef renderer_hpp_20210924_181737_PDT
#define renderer_hpp_20210924_181737_PDT
#include <kt/gfx/texture.hpp>
namespace kt {
namespace gfx {
class Renderer final
{
public:
  static constexpr uint32_t default_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC;
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&)      = default;
  Renderer(SDL_Window*, int _index = -1, uint32_t _flags = default_flags);
  ~Renderer();

  auto get() -> SDL_Renderer*;

  auto clear() -> void;
  auto present() -> void;
  auto color(uint8_t) -> void;
  auto color(uint8_t, uint8_t, uint8_t) -> void;
  auto color(uint8_t, uint8_t, uint8_t, uint8_t) -> void;
  auto color(const Color& c) -> void;
  auto get_color() -> Color;
  auto point(int, int) -> void;
  auto line(int, int, int, int) -> void;
  auto line_f(float, float, float, float) -> void;

  auto circle(int _cx, int _cy, int _radius, bool _fill = false) -> void;
  auto circle_fill(int _cx, int _cy, int _radius) -> void;

  auto set_target(Texture& t) -> void;
  auto set_default_target() -> void;

  auto copy(const Texture& t) -> void;
  auto copy(const Texture& t, SDL_Rect& _dest) -> void;
  auto copy(const Texture& t, SDL_Rect& _src, SDL_Rect& _dest) -> void;
  auto copy(const Texture& t, int _x, int _y) -> void;
  auto copy(const Texture& t, int _x, int _y, double _angle) -> void;
  auto copy(const Texture& _t, SDL_Rect& _src, SDL_Rect& _dest, double _angle) -> void;

  auto set_draw_blend(SDL_BlendMode) -> void;

private:
  SDL_Renderer* renderer_ = nullptr;

  auto release() -> void;
};
} /* namespace gfx */
} /* namespace kt */
#endif//renderer_hpp_20210924_181737_PDT
