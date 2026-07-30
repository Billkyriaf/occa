#ifndef OCCA_INTERNAL_MODES_XRT_STREAM_HEADER
#define OCCA_INTERNAL_MODES_XRT_STREAM_HEADER

#include <occa/internal/core/stream.hpp>

namespace occa {
    namespace xrt {
        class stream : public occa::modeStream_t {
            public:
            stream(
                modeDevice_t *modeDevice_,
                const occa::json &properties_
            );

            ~stream() override;

            void finish() override;

            void* unwrap() override;
        };
  }
}

#endif