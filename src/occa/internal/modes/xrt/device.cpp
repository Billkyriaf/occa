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
#if OCCA_XRT_ENABLED
            // load the xclbin file from the filesystem
            ::xrt::xclbin xclbin(filename);

            // Load the xclbin file if no xclbin file is loaded
            if (!loadedUuid.has_value()){
                
                // The name of the file (path)
                loadedXclbin = filename;

                // The uui of the xclbin
                loadedUuid = xrtDevice.load_xclbin(xclbin);


            } else if (loadedXclbin != filename) {
                // If the path is set a file is already loaded.
                OCCA_FORCE_ERROR("A different xclbin is already loaded on this XRT device");
                return nullptr;
            }

            // extract the kernel metadata
            auto metadata = xclbin.get_kernel(kernelName);
            
            auto args = metadata.get_args();

            // debugging info
            // int counter = 0;
            // for (const auto &arg :args) {
            //     ::std::cout << ::std::endl;
            //     ::std::cout << "Argument " << ++counter << ::std::endl;
            //     ::std::cout << "    Name     : " << arg.get_name()      << ::std::endl;
            //     ::std::cout << "    Host Type: " << arg.get_host_type() << ::std::endl;
            //     ::std::cout << "    Index    : " << arg.get_index()     << ::std::endl;
                
            //     auto mems = arg.get_mems();
            //     ::std::cout << "    Mems     : " << ::std::endl;
            //     for (const auto &mem : mems){
            //         ::std::cout << "        Address  : " << mem.get_base_address() << ::std::endl;
            //         ::std::cout << "        Mem Index: " << mem.get_index()        << ::std::endl;
            //         ::std::cout << "        Size (kb): " << mem.get_size_kb()      << ::std::endl;
            //         ::std::cout << "        Tag      : " << mem.get_tag()          << ::std::endl;
            //         ::std::cout << "        Used?    : " << mem.get_used()         << ::std::endl;

            //     }
                
            //     ::std::cout << "    Handle   : " << arg.get_handle()    << ::std::endl;
            //     ::std::cout << "    Offset   : " << arg.get_offset() << ::std::endl;
            //     ::std::cout << "    Port     : " << arg.get_port()   << ::std::endl;
            //     ::std::cout << "    Size     : " << arg.get_size()   << ::std::endl;

            //     ::std::cout << ::std::endl;
            // }
            
            // Reference to the XRT kernel
            auto xrtKernel = std::make_unique<::xrt::kernel>(xrtDevice, *loadedUuid, kernelName);
            
            xrtKernelInfo kernelInfo;

            // the metadat for all the arguments are saved
            for (const auto &arg : args) {
                kernelArg argument;

                argument.index = arg.get_index();
                argument.port = arg.get_port();
                argument.size = arg.get_size();

                auto mems = arg.get_mems();

                if (mems.size() != 0){
                    // an argument is not a scalr if the mems vector is not empty
                    argument.isMemory = true;
                    
                    // group id is relevant only for memory arguments
                    argument.groupId = xrtKernel->group_id(argument.index);

                    // arguments can connect to multiple interfaces
                    for (const auto &mem : mems) {
                        argumentMem argMem;

                        argMem.memIndex = mem.get_index();
                        argMem.memTag = mem.get_tag();
                        argMem.memUsed = (mem.get_used() == 1) ? true : false;
                        
                        argument.mems.push_back(argMem);
                    }
                    
                } else {
                    argument.isMemory = false;
                    argument.groupId = -1;
                }

                kernelInfo.args.push_back(argument);
            }
            
            kernels[kernelName] = kernelInfo;

            // create the OCCA kernel. This will own the XRT kernel
            auto *occaKernel = new occa::xrt::kernel(std::move(xrtKernel), this, kernelName, filename, props);

            return occaKernel;
#else
            OCCA_FORCE_ERROR(
                "OCCA was not built with XRT support"
            );

            return nullptr;
#endif
        }

        modeMemory_t* device::malloc(const udim_t bytes, const void *src, const occa::json &props) {
            // the required properties are kernel name and argument_index
            OCCA_ERROR("XRT memory allocation requires a kernel property (kernel name)", props.has("kernel"));
            OCCA_ERROR("XRT memory allocation requires a arg_index property (argument index)", props.has("arg_index")); 

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
            OCCA_FORCE_ERROR(
                "OCCA was not built with XRT support"
            );
            
            return nullptr;
#endif
        }
    }
}