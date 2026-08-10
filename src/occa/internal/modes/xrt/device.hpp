#ifndef OCCA_INTERNAL_MODES_XRT_DEVICE_HEADER
#define OCCA_INTERNAL_MODES_XRT_DEVICE_HEADER

#include <optional>

#include <occa/internal/core/device.hpp>
#include <occa/internal/modes/xrt/polyfill.hpp>

namespace occa {
    namespace xrt {
        class device : public occa::modeDevice_t {
        private:
#if OCCA_XRT_ENABLED
            std::optional<::xrt::uuid> loadedUuid;
#endif
      		mutable hash_t hash_;
			std::string loadedXclbin;

		public:
			int deviceId;

#if OCCA_XRT_ENABLED
      		::xrt::device xrtDevice;
#endif

			device(const occa::json &properties_);
			~device() override;

			bool hasSeparateMemorySpace() const override;

			hash_t hash() const override;

			hash_t kernelHash(
				const occa::json &props
			) const override;

			// Streams
			modeStream_t* createStream(
				const occa::json &props
			) override;

			modeStream_t* wrapStream(
				void *ptr,
				const occa::json &props
			) override;

			streamTag tagStream() override;

			void waitFor(
				streamTag tag
			) override;

			double timeBetween(
				const streamTag &startTag,
				const streamTag &endTag
			) override;

			// Kernels
			modeKernel_t* buildKernel(
				const std::string &filename,
				const std::string &kernelName,
				const hash_t kernelHash,
				const occa::json &props
			) override;

			modeKernel_t* buildKernelFromBinary(
				const std::string &filename,
				const std::string &kernelName,
				const occa::json &props
			) override;

			// Memory
			modeMemory_t* malloc(
				const udim_t bytes,
				const void *src,
				const occa::json &props
			) override;

			modeMemory_t* wrapMemory(
				const void *ptr,
				const udim_t bytes,
				const occa::json &props
			) override;

			modeMemoryPool_t* createMemoryPool(
				const occa::json &props
			) override;

			udim_t memorySize() const override;

			void* unwrap() override;
    	};
  	}
}
#endif