#include <exception>
#include <memory>

#include <occa/internal/modes/xrt/buffer.hpp>
#include <occa/internal/modes/xrt/device.hpp>
#include <occa/internal/modes/xrt/memory.hpp>

namespace occa {
    namespace xrt {
        buffer::buffer(modeDevice_t *modeDevice_, udim_t size_, const occa::json &properties_) :
            occa::modeBuffer_t(modeDevice_, size_, properties_),
            xrtBo(nullptr) {
                OCCA_ERROR("XRT memory allocation requires a group_id property", properties_.has("group_id"));

                groupId = properties_.get<int>("group_id");
            }

        buffer::~buffer() = default;

        void buffer::malloc(udim_t bytes) {
            OCCA_ERROR("Cannot allocate an XRT buffer with a negative group_id", groupId >= 0);

            if (!bytes) {
                size = 0;
                return;
            }

            auto *nativeDevice = static_cast<::xrt::device*>(modeDevice->unwrap());

            OCCA_ERROR("XRT device unwrap returned a null pointer", nativeDevice != nullptr);

            try {
                xrtBo = std::make_unique<::xrt::bo>(*nativeDevice, bytes, groupId);

            } catch (const std::exception &error) {
                OCCA_FORCE_ERROR(
                    "Failed to allocate an XRT buffer"
                    "\n  Bytes: " << bytes <<
                    "\n  Group ID: " << groupId <<
                    "\n  XRT error: " << error.what()
                );
            }

            size = bytes;
        }

        modeMemory_t *buffer::slice(const dim_t offset, const udim_t bytes) {
            OCCA_ERROR(
                "XRT buffer slice exceeds the buffer size",
                offset <= size && bytes <= (size - offset)
            );

            return new occa::xrt::memory(this, bytes, offset);
        }

        void buffer::detach() {
            OCCA_FORCE_ERROR("XRT buffer detach is not implemented");
        }
    }
}