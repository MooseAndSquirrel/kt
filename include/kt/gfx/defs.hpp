#ifndef kt_gfx_defs_hpp_20210924_182603_PDT
#define kt_gfx_defs_hpp_20210924_182603_PDT
#include <stdexcept>
#include <SDL2/SDL.h>
namespace kt {
namespace gfx {

constexpr SDL_PixelFormatEnum default_pixel_format = SDL_PIXELFORMAT_RGBA32;

template<typename T>
auto sdl_assert(T* _api_result) -> T*
  {
    if(_api_result == nullptr)
    {
      throw std::runtime_error(SDL_GetError());
    }
    return _api_result;
  }
inline auto sdl_assert(int _api_result) -> int
  {
    if(_api_result != 0)
    {
      throw std::runtime_error(SDL_GetError());
    }
    return _api_result;
  }
} /* namespace gfx */
} /* namespace kt */
#endif//kt_gfx_defs_hpp_20210924_182603_PDT
