#ifndef OCCA_INTERNAL_MODES_XRT_KERNEL_HEADER
#define OCCA_INTERNAL_MODES_XRT_KERNEL_HEADER

#include <memory>

#include <occa/internal/core/kernel.hpp>
#include <occa/internal/modes/xrt/polyfill.hpp>

namespace occa {
    namespace xrt {
        class kernel : public occa::modeKernel_t {
        public:
            kernel(
                modeDevice_t *modeDevice_,
                const std::string &name_,
                const std::string &binaryFilename_,
                const occa::json &properties_
            );

#if OCCA_XRT_ENABLED
            kernel(
                std::unique_ptr<::xrt::kernel> xrtKernel_,
                modeDevice_t *modeDevice_,
                const std::string &name_,
                const std::string &binaryFilename_,
                const occa::json &properties_
            );
#endif

            ~kernel() override;

            int maxDims() const override;
            dim maxOuterDims() const override;
            dim maxInnerDims() const override;

            const lang::kernelMetadata_t &getMetadata() const override;

            void run() const override;

            void *unwrap();


        private:

#if OCCA_XRT_ENABLED
            std::unique_ptr<::xrt::kernel> xrtKernel;
#endif 
           
        };
    }
}

#endif