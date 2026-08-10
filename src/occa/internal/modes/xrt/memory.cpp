#include <occa/internal/modes/xrt/memory.hpp>
#include <occa/internal/modes/xrt/buffer.hpp>

#include <memory>

namespace occa {
    namespace xrt {
        memory::memory(buffer *buffer_, udim_t size_, dim_t offset_) :
        occa::modeMemory_t(buffer_, size_, offset_){}

        memory::~memory() = default;

        void *memory::getKernelArgPtr() const {
#if OCCA_XRT_ENABLED
            auto *xrtBuffer = static_cast<occa::xrt::buffer*>(modeBuffer);

            OCCA_ERROR("XRT buffer has no allocated BO", xrtBuffer->xrtBo != nullptr);

            return xrtBuffer->xrtBo.get();
#else
            OCCA_FORCE_ERROR(
                "XRT is disabled in the OCCCA build"
            );

            return nullptr;
#endif
        }

        void memory::copyTo(
            void *dest,
            const udim_t bytes,
            const udim_t offset, 
            const occa::json &props
        ) const {

#if OCCA_XRT_ENABLED
            OCCA_ERROR(
                "XRT memory copy exceeds the memory view",
                offset <= size && bytes <= (size - offset)
            );

            if (!bytes) {
                return;
            }

            OCCA_ERROR("Cannot copy XRT memory to a null destination", dest != nullptr);

            auto *xrtBuffer = static_cast<occa::xrt::buffer*>(modeBuffer);

            OCCA_ERROR("XRT buffer has no allocated BO", xrtBuffer->xrtBo != nullptr);

            ::xrt::bo *xrtBo = xrtBuffer->xrtBo.get();
            const udim_t absoluteOffset = this->offset + offset;

            xrtBo->sync(XCL_BO_SYNC_BO_FROM_DEVICE, bytes, absoluteOffset);
            xrtBo->read(dest, bytes, absoluteOffset);
#else
            OCCA_FORCE_ERROR(
                "XRT is disabled in the OCCCA build"
            );
#endif
        }

        void memory::copyFrom(
            const void *src,
            const udim_t bytes,
            const udim_t offset,
            const occa::json &props
        ){

#if OCCA_XRT_ENABLED
            OCCA_ERROR(
                "XRT memory copy exceeds the memory view",
                offset <= size && bytes <= (size - offset)
            );

            if (!bytes) {
                return;
            }

            OCCA_ERROR("Cannot copy from a null source into XRT memory", src != nullptr);

            auto *xrtBuffer = static_cast<occa::xrt::buffer*>(modeBuffer);

            OCCA_ERROR("XRT buffer has no allocated BO", xrtBuffer->xrtBo != nullptr);

            ::xrt::bo *xrtBo = xrtBuffer->xrtBo.get();
            const udim_t absoluteOffset = this->offset + offset;

            xrtBo->write(src, bytes, absoluteOffset);

            xrtBo->sync(XCL_BO_SYNC_BO_TO_DEVICE, bytes, absoluteOffset);
#else
            OCCA_FORCE_ERROR(
                "XRT is disabled in the OCCCA build"
            );
#endif
        }

        void memory::copyFrom(
            const modeMemory_t *src,
            const udim_t bytes,
            const udim_t destOffset,
            const udim_t srcOffset,
            const occa::json &props
        ){
            OCCA_FORCE_ERROR("XRT device-to-device memory copies are not implemented yet");
        }

        void *memory::unwrap() {
#if OCCA_XRT_ENABLED
            auto *xrtBuffer = static_cast<occa::xrt::buffer*>(modeBuffer);

            OCCA_ERROR("XRT buffer has no allocated BO", xrtBuffer->xrtBo != nullptr);

            return xrtBuffer->xrtBo.get();
#else
            OCCA_FORCE_ERROR(
                "XRT is disabled in the OCCCA build"
            );

            return nullptr;
#endif
        }
    }
}
