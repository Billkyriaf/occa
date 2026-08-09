#ifndef OCCA_INTERNAL_MODES_XRT_MEMORY_HEADER
#define OCCA_INTERNAL_MODES_XRT_MEMORY_HEADER

#include <occa/internal/core/memory.hpp>

namespace occa {
    namespace xrt {
        class buffer;

        class memory : public occa::modeMemory_t {
        public:
            memory(buffer *buffer_, udim_t size_, dim_t offset_);
            
            ~memory() override;

            void* getKernelArgPtr() const override;


            void copyTo(void *dest,
                        const udim_t bytes,
                        const udim_t offset = 0,
                        const occa::json &props = occa::json()) const override;

            void copyFrom(const void *src,
                          const udim_t bytes,
                          const udim_t offset = 0,
                          const occa::json &props = occa::json()) override;

            void copyFrom(const modeMemory_t *src,
                          const udim_t bytes,
                          const udim_t destOffset = 0,
                          const udim_t srcOffset = 0,
                          const occa::json &props = occa::json()) override;

            void* unwrap() override;

        private:
            
        };
    }
}



#endif