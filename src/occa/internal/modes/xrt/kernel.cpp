#include <occa/internal/modes/xrt/kernel.hpp>
#include <occa/internal/modes/xrt/memory.hpp>
#include "kernel.hpp"


namespace occa{
    namespace xrt{
        kernel::kernel(
            modeDevice_t *modeDevice_,
            const std::string &name_,
            const std::string &binaryFilename_,
            const occa::json &properties_
        ) : occa::modeKernel_t(modeDevice_, name_, "", properties_){

            this->binaryFilename = binaryFilename_;
        }

        kernel::kernel(
            std::unique_ptr<::xrt::kernel> xrtKernel_, 
            modeDevice_t *modeDevice_, 
            const std::string &name_, 
            const std::string &binaryFilename_, 
            const occa::json &properties_
        ) : occa::modeKernel_t(modeDevice_, name_, "", properties_), xrtKernel(std::move(xrtKernel_)){    
            
            this->binaryFilename = binaryFilename_;
        }

        kernel::~kernel() = default;

        int kernel::maxDims() const {
            // same as serial mode
            return 3;
        }

        dim kernel::maxOuterDims() const {
            // same as serial mode
            return dim(occa::UDIM_DEFAULT, occa::UDIM_DEFAULT, occa::UDIM_DEFAULT);
        }

        dim kernel::maxInnerDims() const {
            // same as serial mode
            return dim(occa::UDIM_DEFAULT, occa::UDIM_DEFAULT, occa::UDIM_DEFAULT);
        }

        const lang::kernelMetadata_t &kernel::getMetadata() const {
            // same as serial mode
            return metadata;
        }

        void kernel::run() const {
            ::xrt::run xrtRun(*xrtKernel);

            for (int i = 0; i < arguments.size(); ++i) {
                const kernelArgData &arg = arguments[i];

                if (arg.getModeMemory()) {
                    // unwrap modeMemory → xrt::bo
                    auto *mem = dynamic_cast<occa::xrt::memory*>(arg.getModeMemory());

                    OCCA_ERROR("Kernel memory argument is not XRT memory", mem != nullptr);

                    auto *xrtBo = static_cast<::xrt::bo*>(mem->unwrap());

                    xrtRun.set_arg(i, *xrtBo);

                } else {
                    // Since we don't know what type is the scalar value we pass the pointer to it and it's size
                    xrtRun.set_arg(i, arg.value.ptr(), arg.value.sizeof_());
                }
            }

            xrtRun.start();
            xrtRun.wait();
        }

        void *kernel::unwrap(){
            // return a pointer to a native xrtKernel
            return xrtKernel.get();
        }
    }
}

