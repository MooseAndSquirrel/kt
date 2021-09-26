#ifndef view_hpp_20210914_164707_PDT
#define view_hpp_20210914_164707_PDT
#include "renderer.hpp"
#include <SDL2/SDL.h>
#include <tuple>
namespace kt {
namespace gfx {

class View final
{
  static constexpr int default_width_   = 640;
  static constexpr int default_height_  = 480;
public:
  ~View();
  View(const View&) = delete;
  View(View&&)      = default;
  View();
  View(int _w, int _h);



  constexpr auto left()   const -> int { return 0; }
  constexpr auto top()    const -> int { return 0; }

  auto right_bottom() -> std::tuple<int, int>;
  auto right()  -> int;
  auto bottom() -> int;


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

  auto make_texture(int _w, int _h, int _access  = Texture::default_access) -> Texture;

  auto renderer() const -> const Renderer&;
  auto renderer()       ->       Renderer&;

private:
  SDL_Window*   window_                     = nullptr;
  Renderer      renderer_;
  Texture       presentation_texture_;
  int           width_;
  int           height_;

  auto init_window(int, int) -> SDL_Window*;
  auto init_renderer() -> SDL_Renderer*;
  auto destroy_presentation_texture() -> void;
  auto reset_presentation_texture() -> void;
  auto start_scene()  -> void;
  auto stop_scene()    -> void;
  auto update_window_size() -> void;
};
} /* namespace gfx */
} /* namespace kt */
#endif//view_hpp_20210914_164707_PDT
