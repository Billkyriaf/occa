#include <sstream>
#include <memory>

#include <occa/core/base.hpp>
#include <occa/internal/modes/xrt/device.hpp>
#include <occa/internal/modes/xrt/stream.hpp>
#include <occa/internal/modes/xrt/kernel.hpp>
#include <occa/internal/modes/xrt/memory.hpp>
#include <occa/internal/modes/xrt/buffer.hpp>


namespace occa {
    namespace xrt {
        device::device(const occa::json &properties_) : occa::modeDevice_t(properties_) {
            OCCA_ERROR("XRT device requires a device_id property", properties_.has("device_id"));
            
            deviceId = properties_.get<int>("device_id");

#if OCCA_XRT_ENABLED
        // Raise error if the device id is negative
        OCCA_ERROR(
            "XRT device_id must be non-negative",
            deviceId >= 0
        );

        // Create the XRT device with the provided deviceId
        xrtDevice = ::xrt::device(
            static_cast<unsigned int>(deviceId)
        );

        // Get the devie name
        arch = xrtDevice.get_info<::xrt::info::device::name>();
#else
        OCCA_FORCE_ERROR(
            "OCCA was not built with XRT support"
        );
#endif
        }

        device::~device() {
            freeResources();
        }

        bool device::hasSeparateMemorySpace() const {
            // The FPGA has separate memory space so data transfers should be executed
            return true;
        }

        hash_t device::hash() const {
            // create a unique has from the deviceId and name
            if (!hash_.initialized) {
                std::stringstream ss;

                ss << "device_id: " << deviceId
                << ", name: " << arch;

                hash_ = occa::hash(ss.str());
            }

            return hash_;
        }

        hash_t device::kernelHash(const occa::json &props) const {            
            // The kernelHash function belongs to the buildKernel flow. 
            // Since XRT only uses the buildKernelFromBinary this function is implementd only because the parent
            // class requires it. 
            // There is no real value in this hash.
            return occa::hash(props);
        }

        modeStream_t* device::createStream(const occa::json &props) {
            // The first version of the backend will only support synchronus kernel execution
            return new occa::xrt::stream(this, props);
        }

        modeStream_t* device::wrapStream( void*, const occa::json&) {
            // Since XRT does not support streams natively there is nothing to adopt here
            OCCA_FORCE_ERROR(
                "XRT does not currently support wrapping native streams"
            );

            return nullptr;
        }

        streamTag device::tagStream() {
            OCCA_FORCE_ERROR(
                "XRT stream tags are not implemented yet"
            );

            return streamTag();
        }

        void device::waitFor(streamTag) {
            OCCA_FORCE_ERROR(
                "XRT stream tags are not implemented yet"
            );
        }

        double device::timeBetween(const streamTag&, const streamTag&) {
            OCCA_FORCE_ERROR(
                "XRT stream timing is not implemented yet"
            );

            return 0;
        }

        modeKernel_t* device::buildKernel(const std::string&, const std::string&, const hash_t, const occa::json&) {
            // Building kernels from source will not be supported because it takes 3 hours for 
            // place and route for each one
            OCCA_FORCE_ERROR(
                "XRT source compilation is not supported; "
                "use buildKernelFromBinary() with an .xclbin"
            );

            return nullptr;
        }

        modeKernel_t* device::buildKernelFromBinary(const std::string &filename, const std::string &kernelName, const occa::json&props) {
            
            // Load the xclbin file if no xclbin file is loaded
            if (!loadedUuid.has_value()){
                // The uui of the xclbin
                loadedUuid = xrtDevice.load_xclbin(filename);

                // The name of the file (path)
                loadedXclbin = filename;

            } else if (loadedXclbin != filename) {
                // If the path is set a file is already loaded.
                OCCA_FORCE_ERROR("A different xclbin is already loaded on this XRT device");
            }
            
            // Reference to the XRT kernel
            auto xrtKernel = std::make_unique<::xrt::kernel>(xrtDevice, *loadedUuid, kernelName);

            // create the OCCA kernel. This will own the XRT kernel
            auto *occaKernel = new occa::xrt::kernel(std::move(xrtKernel), this, kernelName, filename, props);

            return occaKernel;
        }

        modeMemory_t* device::malloc(const udim_t bytes, const void *src, const occa::json &props) {
            auto *xrtBuffer = new occa::xrt::buffer(this, bytes, props);

            xrtBuffer->malloc(bytes);

            modeMemory_t *memory = xrtBuffer->slice(0, bytes);

            if (src != nullptr) {
                memory->copyFrom(src, bytes);
            }

            return memory;
        }

        modeMemory_t* device::wrapMemory(const void*, const udim_t, const occa::json&) {
            OCCA_FORCE_ERROR(
                "XRT memory wrapping is not implemented yet"
            );

            return nullptr;
        }

        modeMemoryPool_t* device::createMemoryPool(const occa::json&) {
            OCCA_FORCE_ERROR(
                "XRT memory pools are not implemented yet"
            );

            return nullptr;
        }

        udim_t device::memorySize() const {
            return 0;
        }

        void* device::unwrap() {
#if OCCA_XRT_ENABLED
            return &xrtDevice;
#else
            return nullptr;
#endif
        }
  }
}