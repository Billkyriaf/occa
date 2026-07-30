#include <occa/core/base.hpp>
#include <occa/internal/modes/xrt/registration.hpp>
#include <occa/internal/modes/xrt/utils.hpp>
#include <occa/internal/modes/xrt/device.hpp>

namespace occa {
    namespace xrt {
        xrtMode::xrtMode() : mode_t("XRT") {}

        bool xrtMode::init() {
#if OCCA_XRT_ENABLED
            return xrt::getDeviceCount() > 0;
#else
            return false;
#endif
        }

        styling::section& xrtMode::getDescription() {
            static styling::section section("XRT");
            return section;
        }

        modeDevice_t* xrtMode::newDevice(const occa::json &props) {
            return new occa::xrt::device(setModeProp(props));
        }

        int xrtMode::getDeviceCount(const occa::json &props) {
            return xrt::getDeviceCount();
        }

        xrtMode mode;
    }
}