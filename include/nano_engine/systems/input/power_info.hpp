#ifndef NANO_ENGINE_SYSTEMS_INPUT_POWER_INFO_HPP_
#define NANO_ENGINE_SYSTEMS_INPUT_POWER_INFO_HPP_

#include <cstddef>

#include <SDL2/SDL_power.h>

#include <nano_engine/systems/input/power_state.hpp>

namespace ne
{
struct power_info
{
  power_info           ()
  {
    int  native_seconds, native_percentage;
    auto native_state = SDL_GetPowerInfo(&native_seconds, &native_percentage);
    state             = static_cast<power_state>(native_state     );
    seconds           = static_cast<std::size_t>(native_seconds   );
    percentage        = static_cast<float>      (native_percentage) / 100.0F;
  }
  power_info           (const power_info&  that) = default;
  power_info           (      power_info&& temp) = default;
  ~power_info          ()                        = default;
  power_info& operator=(const power_info&  that) = default;
  power_info& operator=(      power_info&& temp) = default;

  power_state state     ;
  std::size_t seconds   ;
  float       percentage;
};
}

#endif