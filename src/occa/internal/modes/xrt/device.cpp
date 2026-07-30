#include <sstream>

#include <occa/core/base.hpp>
#include <occa/internal/modes/xrt/device.hpp>
#include <occa/internal/modes/xrt/stream.hpp>

namespace occa {
    namespace xrt {
        device::device(const occa::json &properties_) : occa::modeDevice_t(properties_), 
                                                        deviceId(properties_.get("device_id", 0)) {

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

        device::~device() {}

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

        hash_t device::kernelHash(const occa::json&) const {
            // TODO implement
            return occa::hash("xrt-binary");
        }

        modeStream_t* device::createStream(const occa::json &props) {
            // Streams are not natively supported in XRT. 
            return new occa::xrt::stream(this, props);
        }

        modeStream_t* device::wrapStream( void*, const occa::json&) {
            OCCA_FORCE_ERROR(
                "XRT does not currently support wrapping native streams"
            );

            return nullptr;
        }

        streamTag device::tagStream() {
            // TODO implement
            OCCA_FORCE_ERROR(
                "XRT stream tags are not implemented yet"
            );

            return streamTag();
        }

        void device::waitFor(streamTag) {
            // TODO implement
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
            OCCA_FORCE_ERROR(
                "XRT source compilation is not supported; "
                "use buildKernelFromBinary() with an .xclbin"
            );

            return nullptr;
        }

        modeKernel_t* device::buildKernelFromBinary(const std::string&, const std::string&, const occa::json&) {
            OCCA_FORCE_ERROR(
                "XRT binary kernels are not implemented yet"
            );

            return nullptr;
        }

        modeMemory_t* device::malloc(const udim_t, const void*, const occa::json&) {
            OCCA_FORCE_ERROR(
                "XRT memory allocation is not implemented yet"
            );

            return nullptr;
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