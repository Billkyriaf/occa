#include <cstdlib>
#include <iostream>
#include <vector>

#include <occa.hpp>
#include <xrt/xrt_kernel.h>

int main() {
    constexpr int entries = 1024;
    const std::size_t bytes = entries * sizeof(int);

    std::vector<int> input(entries);
    std::vector<int> output(entries, 0);

    for (int i = 0; i < entries; ++i) {
        input[i] = i * 3;
    }

    try {
        std::cout << "1. Creating OCCA device" << std::endl;

        occa::device device({
            {"mode", "XRT"},
            {"device_id", 0}
        });

        std::cout << "2. Device created: " << device.mode() << std::endl;

        std::cout << "3. Loading xclbin and opening kernel" << std::endl;

        occa::kernel kernel = device.buildKernelFromBinary(
            "/home/vkyriaf/Git/Vadd_OCCA/fpga/hw_emu/vadd_hw_emu.xclbin",
            "vadd"
        );

        std::cout << "4. Kernel opened" << std::endl;

        std::cout << "5. Allocating memory" << std::endl;

        occa::memory memory = device.malloc<int>(
            entries,
            {
                {"kernel", "vadd"},
                {"arg_index", 0}
            }
        );

        std::cout << "6. Memory allocated" << std::endl;

        memory.copyFrom(input.data());

        std::cout << "6.1. Input copied" << std::endl;

        std::cout << "7. Copying back" << std::endl;

        memory.copyTo(output.data());

        std::cout << "8. Copy completed" << std::endl;
        for (int i = 0; i < entries; ++i) {
            if (output[i] != input[i]) {
                std::cerr
                    << "Mismatch at index " << i
                    << ": expected " << input[i]
                    << ", received " << output[i]
                    << '\n';

                return EXIT_FAILURE;
            }
        }

        std::cout << "XRT memory round trip passed\n";
    } catch (const std::exception &error) {
        std::cerr << "Error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}