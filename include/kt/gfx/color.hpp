#ifndef color_hpp_20210924_182117_PDT
#define color_hpp_20210924_182117_PDT
#include <kt/gfx/defs.hpp>
namespace kt {
namespace gfx  {
class Color final
{
public:
  constexpr Color(uint8_t _r, uint8_t _g, uint8_t _b)
    : Color(_r, _g, _b, SDL_ALPHA_OPAQUE)
  {}
  constexpr Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
    : r_(_r)
    , g_(_g)
    , b_(_b)
    , a_(_a)
  {}

  auto r() const -> uint8_t { return r_; }
  auto g() const -> uint8_t { return g_; }
  auto b() const -> uint8_t { return b_; }
  auto a() const -> uint8_t { return a_; }

  auto set_r(uint8_t _v)    { r_ = _v; }
  auto set_g(uint8_t _v)    { g_ = _v; }
  auto set_b(uint8_t _v)    { b_ = _v; }
  auto set_a(uint8_t _v)    { a_ = _v; }

  auto operator+=(const Color& _c) -> Color&
    {
      r_ += _c.r_;
      g_ += _c.g_;
      b_ += _c.b_;
      a_ += _c.a_;
      return *this;
    }
  auto operator-=(const Color& _c) -> Color&
    {
      r_ -= _c.r_;
      g_ -= _c.g_;
      b_ -= _c.b_;
      a_ -= _c.a_;
      return *this;
    }
  static constexpr auto black()       -> Color { return Color { 0x00, 0x00, 0x00 }; }
  static constexpr auto red()         -> Color { return Color { 0xFF, 0x00, 0x00 }; }
  static constexpr auto green()       -> Color { return Color { 0x00, 0xFF, 0x00 }; }
  static constexpr auto yellow()      -> Color { return Color { 0xFF, 0xFF, 0x00 }; }
  static constexpr auto blue()        -> Color { return Color { 0x00, 0x00, 0xFF }; }
  static constexpr auto magenta()     -> Color { return Color { 0xFF, 0x00, 0xFF }; }
  static constexpr auto cyan()        -> Color { return Color { 0x00, 0xFF, 0xFF }; }
  static constexpr auto white()       -> Color { return Color { 0xFF, 0xFF, 0xFF }; }
  static constexpr auto transparent() -> Color { return Color { 0x00, 0x00, 0x00, SDL_ALPHA_TRANSPARENT }; }
  static constexpr auto gray()        -> Color { return Color { 0x88, 0x88, 0x88 }; }
  static constexpr auto grey()        -> Color { return gray(); }
private:
  uint8_t r_;
  uint8_t g_;
  uint8_t b_;
  uint8_t a_;

};
} /* namespace  gfx */
} /* namespace kt */
#endif//color_hpp_20210924_182117_PDT
