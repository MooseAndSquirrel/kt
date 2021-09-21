#ifndef view_hpp_20210914_164707_PDT
#define view_hpp_20210914_164707_PDT
#include <SDL2/SDL.h>
#include <tuple>
class Color
{
public:
  Color(uint8_t _r, uint8_t _g, uint8_t _b)
    : Color(_r, _g, _b, SDL_ALPHA_OPAQUE)
  {}
  Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
    : r_(_r)
    , g_(_g)
    , b_(_b)
    , a_(_a)
  {}

  auto r() const -> uint8_t { return r_; }
  auto g() const -> uint8_t { return g_; }
  auto b() const -> uint8_t { return b_; }
  auto a() const -> uint8_t { return a_; }

  static auto black()   -> Color { return Color { 0x00, 0x00, 0x00 }; };
  static auto red()     -> Color { return Color { 0xFF, 0x00, 0x00 }; };
  static auto green()   -> Color { return Color { 0x00, 0xFF, 0x00 }; };
  static auto yellow()  -> Color { return Color { 0xFF, 0xFF, 0x00 }; };
  static auto blue()    -> Color { return Color { 0x00, 0x00, 0xFF }; };
  static auto magenta() -> Color { return Color { 0xFF, 0x00, 0xFF }; };
  static auto cyan()    -> Color { return Color { 0x00, 0xFF, 0xFF }; };
  static auto white()   -> Color { return Color { 0xFF, 0xFF, 0xFF }; };
private:
  uint8_t r_;
  uint8_t g_;
  uint8_t b_;
  uint8_t a_;

};

class View final
{
  static constexpr int default_width_   = 640;
  static constexpr int default_height_  = 480;
public:
  ~View();
  View();
  View(int _w, int _h);
  View(const View&)     = delete;
  View(View&&)          = default;


  auto clear() -> void;
  auto present() -> void;
  auto color(uint8_t) -> void;
  auto color(uint8_t, uint8_t, uint8_t) -> void;
  auto color(uint8_t, uint8_t, uint8_t, uint8_t) -> void;
  auto color(const Color& c) -> void;
  auto point(int, int) -> void;
  auto line(int, int, int, int) -> void;
  auto line_f(float, float, float, float) -> void;

  constexpr auto left()   const -> int { return 0; }
  constexpr auto top()    const -> int { return 0; }

  auto right_bottom() -> std::tuple<int, int>;
  auto right()  -> int;
  auto bottom() -> int;

  auto window_size() -> std::tuple<int, int>;
  auto window_width() -> int;
  auto window_height() -> int;

  auto width()  const -> int;
  auto height() const -> int;
  auto size()   const -> std::pair<int, int>;

  template<typename FnT>
    auto scene(FnT&& fn) -> void
    {
      start_scene();
      fn();
      stop_scene();
    }
private:
  SDL_Window*   window_                     = nullptr;
  SDL_Renderer* renderer_                   = nullptr;
  SDL_Texture*  presentation_texture_       = nullptr;

  auto init_window(size_t, size_t) -> SDL_Window*;
  auto init_renderer() -> SDL_Renderer*;
  auto destroy_presentation_texture() -> void;
  auto reset_presentation_texture() -> void;
  auto start_scene()  -> void;
  auto stop_scene()    -> void;
};
#endif//view_hpp_20210914_164707_PDT
