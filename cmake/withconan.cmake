## Build using Conan if enabled

if (ENABLE_CONAN AND NOT CONAN_DIR)
    message(FATAL_ERROR "Please provide CONAN_DIR path when using ENABLE_CONAN")
endif()

if (ENABLE_CONAN OR CONAN_DIR)
    include (${CONAN_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif()
