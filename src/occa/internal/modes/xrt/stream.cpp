#include <occa/internal/modes/xrt/stream.hpp>

namespace occa {
    namespace xrt {
        stream::stream(modeDevice_t *modeDevice_, const occa::json &properties_) : 
                                   occa::modeStream_t( modeDevice_, properties_) {}

        stream::~stream() {}

        void stream::finish() {
        // Kernel execution will initially be synchronous,
        // so there is nothing to wait for yet.
        }

        void* stream::unwrap() {
        // XRT has no native stream object corresponding
        // to this synchronous OCCA stream.
        return nullptr;
        }
    }
}