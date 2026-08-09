#ifndef OCCA_INTERNAL_MODES_XRT_UTILS_HEADER
#define OCCA_INTERNAL_MODES_XRT_UTILS_HEADER

#include <occa/internal/modes/xrt/polyfill.hpp>

namespace occa {
  namespace xrt {
    int getDeviceCount();
  }

  void error(const std::exception &e,
               const char *filename,
               const char *function,
               const int line,
               const std::string &message);

  void warn(const std::exception &e,
            const char *filename,
            const char *function,
            const int line,
            const std::string &message);
}

#endif