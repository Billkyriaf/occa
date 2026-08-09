#include <cstdlib>
#include <exception>

#include <occa/internal/utils/string.hpp>
#include <occa/internal/utils/sys.hpp>
#include <occa/internal/modes/xrt/utils.hpp>

namespace occa {
    namespace xrt {
        int getDeviceCount() {
    
#if OCCA_XRT_ENABLED
            const char *emulationMode = std::getenv("XCL_EMULATION_MODE");

            if (emulationMode && emulationMode[0] != '\0') {
                try {
                    ::xrt::device device(0);
                    return 1;
                } catch (const std::exception &) {
                    return 0;
                }
            }

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

        void warn(
            const std::exception &e,
            const std::string &filename,
            const std::string &function,
            const int line,
            const std::string &message
        ) {
            std::stringstream ss;

            ss << message << '\n' << "    XRT Error: " << e.what();

            occa::warn(filename, function, line, ss.str());
        }

        void error(
            const std::exception &e,
            const std::string &filename,
            const std::string &function,
            const int line,
            const std::string &message
        ) {
            std::stringstream ss;

            ss << message << '\n' << "XRT Error: " << e.what();

            occa::error(filename, function, line, ss.str());
        }
    }
}