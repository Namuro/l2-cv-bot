cmake_minimum_required(VERSION 3.11)

set(TARGET l2-cv-bot)
project(${TARGET})

# Add sources
add_executable(
    ${TARGET}
    src/main.cpp
)

# Link OpenCV
find_package(OpenCV REQUIRED)
target_link_libraries(${TARGET} ${OpenCV_LIBS})

# Bundle executable with DLLs
if(MSVC)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

    install(CODE
        # \${BUILD_TYPE} - hack to find path to executable |
        # for current Release/Debug configuration          |-------------
        # (see buid/cmake_install.cmake for details)       |            v
        "include(BundleUtilities); fixup_bundle(${CMAKE_INSTALL_PREFIX}/\${BUILD_TYPE}/${TARGET}.exe \"\" ${OpenCV_LIBS})"
    COMPONENT Runtime)
endif()
