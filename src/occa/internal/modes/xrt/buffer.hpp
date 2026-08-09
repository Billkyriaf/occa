#ifndef OCCA_INTERNAL_MODES_XRT_BUFFER_HEADER
#define OCCA_INTERNAL_MODES_XRT_BUFFER_HEADER

#include <memory>

#include <occa/internal/core/buffer.hpp>
#include <occa/internal/core/memory.hpp>
#include <occa/internal/modes/xrt/polyfill.hpp>

namespace occa {
    namespace xrt {
        class memory;

        class buffer : public occa::modeBuffer_t {
            friend class xrt::memory;

        public:
            buffer(modeDevice_t *modeDevice_, udim_t size_, const occa::json &properties_ = occa::json());

            virtual ~buffer();

            void malloc(udim_t bytes) override;

            modeMemory_t* slice(const dim_t offset, const udim_t bytes) override;

            void detach() override;

        private:
            std::unique_ptr<::xrt::bo> xrtBo;

            int groupId;
        };
    }
}

#endif