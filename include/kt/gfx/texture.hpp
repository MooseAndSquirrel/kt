#ifndef texture_hpp_20210924_182238_PDT
#define texture_hpp_20210924_182238_PDT
#include <kt/gfx/defs.hpp>
#include <kt/gfx/color.hpp>
namespace kt {
namespace gfx {
class Renderer;
class Texture final
{
public:
  static constexpr int default_access = SDL_TEXTUREACCESS_STATIC | SDL_TEXTUREACCESS_TARGET;
  Texture(const Texture&) = delete;
  Texture(Texture&&)      = default;
  Texture() noexcept      = default;

  Texture(Renderer&, int _w, int _h, int _access = default_access);
  Texture(Renderer&, int _w, int _h, Color _c, int _access = default_access);
  ~Texture();

  auto reset(Renderer& _r, int _w, int _h, int _access = default_access) -> void;
  auto set(SDL_BlendMode) -> void;
  auto get() const ->       SDL_Texture*;
private:
  mutable SDL_Texture*  texture_ = nullptr;

  auto release() -> void;
};
} /* namespace gfx */  
} /* namespace kt */
#endif//texture_hpp_20210924_182238_PDT
