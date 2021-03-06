#ifndef DI_SYSTEMS_VR_TIMING_MODE_HPP_
#define DI_SYSTEMS_VR_TIMING_MODE_HPP_

#include <openvr.h>

namespace di
{
enum class timing_mode
{
  implicit                                  = vr::VRCompositorTimingMode_Implicit                                      ,
  runtime_performs_post_present_handoff     = vr::VRCompositorTimingMode_Explicit_RuntimePerformsPostPresentHandoff    ,
  application_performs_post_present_handoff = vr::VRCompositorTimingMode_Explicit_ApplicationPerformsPostPresentHandoff,
};
}

#endif