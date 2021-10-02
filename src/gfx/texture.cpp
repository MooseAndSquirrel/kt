#include <kt/gfx/renderer.hpp>
namespace kt {
namespace gfx  {
Texture::Texture(Renderer& _r, int _w, int _h, Color _c, int _access)
  {
    reset(_r, _w, _h, _access);
    _r.set_target(*this);
    _r.color(_c);
    _r.set_draw_blend(SDL_BLENDMODE_NONE);
    _r.clear();
    _r.set_draw_blend(SDL_BLENDMODE_BLEND);
    set(SDL_BLENDMODE_BLEND);
  }
Texture::Texture(Texture&& _src)
    : texture_(_src.texture_)
  {
    _src.texture_ = nullptr;
  }
Texture::Texture(Renderer& _r, size _size, int _access)
    : Texture(_r, _size.w, _size.h, _access)
  {
  }
Texture::Texture(Renderer& _r, int _w, int _h, int _access)
    : Texture(_r, _w, _h, Color::transparent(), _access)
  {
  }
Texture::Texture(Renderer& _r, const std::string& _path)
  {
    auto surface  = sdl_assert(SDL_LoadBMP(_path.c_str()));
    texture_      = sdl_assert(SDL_CreateTextureFromSurface(_r.get(), surface));
    SDL_FreeSurface(surface);
  }
Texture::~Texture()
  {
    release();
  }
auto Texture::get() const -> SDL_Texture*
  {
    return texture_;
  }
auto Texture::reset(Renderer& _r, int _w, int _h, int _access) -> void
  {
    release();
    texture_ = sdl_assert(SDL_CreateTexture(_r.get(), default_pixel_format, _access, _w, _h));
  }
auto Texture::release() -> void
  {
    if(texture_)
    {
      SDL_DestroyTexture(texture_);
    }
    texture_ = nullptr;
  }
auto Texture::set(SDL_BlendMode _bm) -> void
  {
    sdl_assert(SDL_SetTextureBlendMode(texture_, _bm));
  }
auto Texture::get_size() const -> size
  {
    size sz;
    sdl_assert(SDL_QueryTexture(texture_, NULL, NULL, &sz.w, &sz.h));
    return sz;
  }
} /* namespace gfx */
} /* namespace kt */
