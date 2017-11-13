#ifndef NANO_ENGINE_SYSTEMS_DISPLAY_WINDOW_HPP_
#define NANO_ENGINE_SYSTEMS_DISPLAY_WINDOW_HPP_

#include <array>
#include <cstddef>
#include <iostream>
#include <string>

#include <boost/signals2.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <nano_engine/systems/display/window_mode.hpp>

namespace ne
{
class window
{
public:
  window           (const std::string& title, const std::array<std::size_t, 2>& position, const std::array<std::size_t, 2>& size, std::uint32_t flags = 0u) 
  : native_(SDL_CreateWindow(title.c_str(), static_cast<int>(position[0]), static_cast<int>(position[1]), static_cast<int>(size[0]), static_cast<int>(size[1]), flags |= SDL_WINDOW_ALLOW_HIGHDPI))
  {
    if (!native_)
      throw std::runtime_error("Failed to create SDL window. SDL Error: " + std::string(SDL_GetError()));
  }
  explicit window  (const std::string& title, std::uint32_t flags = 0u) 
  : window(title, {32, 32}, {800, 600}, flags)
  {
    set_fullscreen_windowed();
  }
  window           (const window&  that) = delete ;
  window           (      window&& temp) = default;
  virtual ~window  ()
  {
    SDL_DestroyWindow(native_ );
  }
  window& operator=(const window&  that) = delete ;
  window& operator=(      window&& temp) = default;

  void set_focus       () const
  {
    SDL_SetWindowInputFocus(native_);
  }
  void bring_to_front  () const
  {
    SDL_RaiseWindow(native_);
  }
  void minimize        () const
  {
    SDL_MinimizeWindow(native_);
  }
  void maximize        () const
  {
    SDL_MaximizeWindow(native_);
  }
  void restore         () const
  {
    SDL_RestoreWindow(native_);
  }
  
  void set_visible     (bool                                                 visible           )
  {
    visible ? SDL_ShowWindow(native_) : SDL_HideWindow(native_);
  }
  void set_resizable   (bool                                                 resizable         )
  {
    SDL_SetWindowResizable(native_, SDL_bool(resizable));
  }
  void set_bordered    (bool                                                 bordered          )
  {
    SDL_SetWindowBordered(native_, SDL_bool(bordered));
  }
  void set_input_grab  (bool                                                 input_grab        )
  {
    SDL_SetWindowGrab(native_, SDL_bool(input_grab));
  }
  void set_opacity     (float                                                opacity           )
  {
    SDL_SetWindowOpacity(native_, opacity);
  }
  void set_brightness  (float                                                brightness        )
  {
    SDL_SetWindowBrightness(native_, brightness);
  }
  void set_title       (const std::string&                                   name              )
  {
    SDL_SetWindowTitle(native_, name.c_str());
  }
  void set_position    (const std::array<std::size_t, 2>&                    position          )
  {
    SDL_SetWindowPosition(native_, static_cast<int>(position[0]), static_cast<int>(position[1]));
  }
  void set_size        (const std::array<std::size_t, 2>&                    size              )
  {
    SDL_SetWindowSize(native_, static_cast<int>(size[0]), static_cast<int>(size[1]));
    on_resize(this->size());
  }
  void set_minimum_size(const std::array<std::size_t, 2>&                    minimum_size      )
  {
    SDL_SetWindowMinimumSize(native_, static_cast<int>(minimum_size[0]), static_cast<int>(minimum_size[1]));
    on_resize(this->size());
  }
  void set_maximum_size(const std::array<std::size_t, 2>&                    maximum_size      )
  {
    SDL_SetWindowMaximumSize(native_, static_cast<int>(maximum_size[0]), static_cast<int>(maximum_size[1]));
    on_resize(this->size());
  }
  void set_gamma_ramp  (const std::array<std::array<std::uint16_t, 256>, 3>& translation_tables)
  {
    SDL_SetWindowGammaRamp(
      native_,
      translation_tables[0].data(), 
      translation_tables[1].data(), 
      translation_tables[2].data());
  } 
  void set_mode        (window_mode mode  )
  {
    SDL_SetWindowFullscreen(native_, mode == window_mode::fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    if (mode == window_mode::fullscreen_windowed)
      set_fullscreen_windowed();
    on_resize(size());
  }
  void set_parent      (window*     parent)
  {
    SDL_SetWindowModalFor(native_, parent->native_);
  }
  // FIX!
  template<typename pixel_type>
  void set_icon        (const std::array<std::size_t, 3>& dimensions, 
                        const std::vector<pixel_type>&    pixels    )
  {
    auto surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels.data(), dimensions.x, dimensions.y, dimensions.z, dimensions.x * 4, SDL_PIXELFORMAT_RGBA32);
    SDL_SetWindowIcon(native_, surface);
    SDL_FreeSurface(surface);
  }

  bool                                          visible     () const
  {
    return SDL_GetWindowFlags(native_) & SDL_WINDOW_SHOWN;
  }
  bool                                          resizable   () const
  {
    return SDL_GetWindowFlags(native_) & SDL_WINDOW_RESIZABLE;
  }
  bool                                          bordered    () const
  {
    return !(SDL_GetWindowFlags(native_) & SDL_WINDOW_BORDERLESS);
  }
  bool                                          input_grab  () const
  {
    return static_cast<bool>(SDL_GetWindowGrab(native_));
  }
  bool                                          input_focus () const
  {
    return SDL_GetWindowFlags(native_) & SDL_WINDOW_INPUT_FOCUS;
  }
  bool                                          mouse_focus () const
  {
    return SDL_GetWindowFlags(native_) & SDL_WINDOW_MOUSE_FOCUS;
  }
  float                                         opacity     () const
  {
    float opacity;
    SDL_GetWindowOpacity(native_, &opacity);
    return opacity;
  }
  float                                         brightness  () const
  {
    return SDL_GetWindowBrightness(native_);
  }
  std::string                                   title       () const
  {
    return std::string(SDL_GetWindowTitle(native_));
  }
  std::array<std::size_t, 2>                    position    () const
  {
    std::array<std::size_t, 2> position;
    SDL_GetWindowPosition(native_, reinterpret_cast<int*>(&position[0]), reinterpret_cast<int*>(&position[1]));
    return position;
  }
  std::array<std::size_t, 2>                    size        () const
  {
    std::array<std::size_t, 2> size;
    SDL_GetWindowSize(native_, reinterpret_cast<int*>(&size[0]), reinterpret_cast<int*>(&size[1]));
    return size;
  }
  std::array<std::size_t, 2>                    minimum_size() const
  {
    std::array<std::size_t, 2> minimum_size;
    SDL_GetWindowMinimumSize(native_, reinterpret_cast<int*>(&minimum_size[0]), reinterpret_cast<int*>(&minimum_size[1]));
    return minimum_size;
  }
  std::array<std::size_t, 2>                    maximum_size() const
  {
    std::array<std::size_t, 2> maximum_size;
    SDL_GetWindowMaximumSize(native_, reinterpret_cast<int*>(&maximum_size[0]), reinterpret_cast<int*>(&maximum_size[1]));
    return maximum_size;
  }
  std::array<std::size_t, 4>                    border_size () const // top, left, bottom, right
  {
    std::array<std::size_t, 4> border_size;
    SDL_GetWindowBordersSize(native_, reinterpret_cast<int*>(&border_size[0]), reinterpret_cast<int*>(&border_size[1]), reinterpret_cast<int*>(&border_size[2]), reinterpret_cast<int*>(&border_size[3]));
    return border_size;
  }
  std::array<std::array<std::uint16_t, 256>, 3> gamma_ramp  () const
  {
    std::array<std::array<std::uint16_t, 256>, 3> translation_tables;
    SDL_GetWindowGammaRamp(
      native_, 
      translation_tables[0].data(),
      translation_tables[1].data(),
      translation_tables[2].data());
    return translation_tables;
  }
  // FIX.
  window_mode                                   mode        () const
  {
    if (SDL_GetWindowFlags(native_) & SDL_WINDOW_FULLSCREEN_DESKTOP)
      return window_mode::fullscreen;
    //else if(TODO)
    //  return window_mode::fullscreen_windowed;
    return window_mode::windowed;
  }

  SDL_Window* native() const 
  {
    return native_;
  }

  boost::signals2::signal<void(const std::array<std::size_t, 2>&)> on_resize;

#if   defined(SDL_VIDEO_DRIVER_ANDROID)
  std::tuple<ANativeWindow*, EGLSurface>                  os_specific_native()
  {
    SDL_SysWMinfo sys_info;
    SDL_VERSION(&sys_info.version);
    SDL_GetWindowWMInfo(native_, &sys_info);
    return {sys_info.info.android.window, sys_info.info.android.surface};
  }
#elif defined(SDL_VIDEO_DRIVER_WINDOWS)
  std::tuple<HWND, HDC, HINSTANCE>                        os_specific_native()
  {
    SDL_SysWMinfo sys_info;
    SDL_VERSION(&sys_info.version);
    SDL_GetWindowWMInfo(native_, &sys_info);
    return {sys_info.info.win.window, sys_info.info.win.hdc, reinterpret_cast<HINSTANCE>(GetWindowLong(sys_info.info.win.window, -6))};
  }
#elif defined(SDL_VIDEO_DRIVER_X11)
  std::tuple<Display*, Window>                            os_specific_native()
  {
    SDL_SysWMinfo sys_info;
    SDL_VERSION(&sys_info.version);
    SDL_GetWindowWMInfo(native_, &sys_info);
    return {sys_info.info.x11.display, sys_info.info.x11.window};
  }
#elif defined(SDL_VIDEO_DRIVER_WAYLAND)
  std::tuple<wl_display*, wl_surface*, wl_shell_surface*> os_specific_native()
  {
    SDL_SysWMinfo sys_info;
    SDL_VERSION(&sys_info.version);
    SDL_GetWindowWMInfo(native_, &sys_info);
    return {sys_info.info.wl.display, sys_info.info.wl.surface, sys_info.info.wl.shell_surface};
  }
#elif defined(SDL_VIDEO_DRIVER_MIR)
  std::tuple<MirConnection*, MirSurface*>                 os_specific_native()
  {
    SDL_SysWMinfo sys_info;
    SDL_VERSION(&sys_info.version);
    SDL_GetWindowWMInfo(native_, &sys_info);
    return {sys_info.info.mir.connection, sys_info.info.mir.surface};
  }
#endif

protected:
  // FIX.
  void set_fullscreen_windowed(std::size_t display_index = 0)
  {
    SDL_DisplayMode video_mode;
    SDL_GetCurrentDisplayMode(static_cast<int>(display_index), &video_mode);
    set_position(std::array<std::size_t, 2>{0u, 0u});
    set_size    (std::array<std::size_t, 2>{std::size_t(video_mode.w - 1), std::size_t(video_mode.h - 1)});
  }

  SDL_Window* native_ = nullptr;
};
}

#endif
