###############################################################################
# Find AMD/Xilinx XRT
###############################################################################

find_path(
  XRT_INCLUDE_DIR
  NAMES 
    xrt/xrt_device.h
    xrt.h
  PATHS
    ENV XILINX_XRT
    /opt/xilinx/xrt
  PATH_SUFFIXES include
)

find_library(
  XRT_COREUTIL_LIBRARY
  NAMES xrt_coreutil
  PATHS
    ENV XILINX_XRT
    /opt/xilinx/xrt
  PATH_SUFFIXES lib lib64
)

set(XRT_INCLUDE_DIRS "${XRT_INCLUDE_DIR}")
set(XRT_LIBRARIES "${XRT_COREUTIL_LIBRARY}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  XRT
  REQUIRED_VARS
    XRT_INCLUDE_DIR
    XRT_COREUTIL_LIBRARY
)

if(XRT_FOUND AND NOT TARGET OCCA::depends::XRT)

  add_library(OCCA::depends::XRT INTERFACE IMPORTED)

  set_target_properties(
    OCCA::depends::XRT
    PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${XRT_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${XRT_LIBRARIES}"
  )

endif()

mark_as_advanced(
  XRT_INCLUDE_DIR
  XRT_COREUTIL_LIBRARY
)