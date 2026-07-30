#include <occa/internal/modes/xrt/utils.hpp>

namespace occa {
    namespace xrt {
        int getDeviceCount() {
    
#if OCCA_XRT_ENABLED
            int deviceCount = 0;
            
            // count the devices available to XRT using deviceId
            while (true) {
                try {
                    ::xrt::device device(
                        static_cast<unsigned int>(deviceCount)
                    );
                
                    ++deviceCount;
                } catch (const std::exception&) {
                    break;
                }
            }     

            return deviceCount;
#else
            return 0;
#endif
        }
    }
}