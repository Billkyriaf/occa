#ifndef OCCA_INTERNAL_MODES_XRT_REGISTRATION_HEADER
#define OCCA_INTERNAL_MODES_XRT_REGISTRATION_HEADER

#include <occa/internal/modes.hpp>

namespace occa {
    namespace xrt {
        class xrtMode : public mode_t {
            public:
            xrtMode();

            bool init() override;

            styling::section& getDescription() override;

            modeDevice_t* newDevice(
                const occa::json &props
            ) override;

            int getDeviceCount(
                const occa::json &props
            ) override;
        };

        extern xrtMode mode;
  }
}

#endif