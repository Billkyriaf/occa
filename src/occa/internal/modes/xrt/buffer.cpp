#include <exception>
#include <memory>

#include <occa/internal/modes/xrt/buffer.hpp>
#include <occa/internal/modes/xrt/device.hpp>
#include <occa/internal/modes/xrt/memory.hpp>

namespace occa {
    namespace xrt {
        buffer::buffer(modeDevice_t *modeDevice_, udim_t size_, const occa::json &properties_) :
            occa::modeBuffer_t(modeDevice_, size_, properties_) {
#if OCCA_XRT_ENABLED
                xrtBo = nullptr;
#endif          
                groupId = 0;
                
                auto *xrtDevice = (occa::xrt::device*) modeDevice_;

                if (xrtDevice->kernels.size() == 0){
                    OCCA_FORCE_ERROR("No kernels are loaded yet. Make sure you load the kernel before memory allocation");
                }
                
                auto kernelName = properties_.get<std::string>("kernel");

                auto it = xrtDevice->kernels.find(kernelName);

                if (it == xrtDevice->kernels.end()) {
                    OCCA_FORCE_ERROR("The requested kernel does not exist!");
                }

                xrtKernelInfo kernel = it->second;

                for (auto const &arg : kernel.args){
                    // ::std::cout << "Argument index: " << arg.index << " props.index: " << properties_.get<int>("arg_index") << ::std::endl;
                    
                    if (arg.index == properties_.get<int>("arg_index")) {
                        groupId = arg.groupId;
                        // ::std::cout << "GroupId is now: " << groupId << ::std::endl;
                    }
                }
                
                // ::std::cout << "GroupId ended being: " << groupId << ::std::endl;
                
                // if groupId is still -1 somthing went wrong
                OCCA_ERROR("The argument index is wrong OR the argument is a scalar. Can not create xrt buffer", groupId != -1);
            }

        buffer::~buffer() = default;

        void buffer::malloc(udim_t bytes) {
#if OCCA_XRT_ENABLED
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
#else
            OCCA_FORCE_ERROR(
                "OCCA was not built with XRT support"
            );
#endif
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