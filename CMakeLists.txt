cmake_minimum_required(VERSION 3.11)
project(l2-cv-bot)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(
    l2-cv-bot
    src/main.cpp
    src/Runloop.cpp
    src/Utils.cpp
    src/Options.cpp
    src/Window.cpp
    src/Capture.cpp
    src/FPS.h
    src/Eyes.cpp
    src/Intercept.cpp
    src/Input.cpp
    src/Hands.h
    src/Brain.cpp
)

set_target_properties(l2-cv-bot PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# OpenCV
find_package(OpenCV REQUIRED)
target_link_libraries(l2-cv-bot ${OpenCV_LIBS})
target_include_directories(l2-cv-bot PRIVATE ${OpenCV_HEADERS})

# Interception API library
add_subdirectory(lib/interception)
target_link_libraries(l2-cv-bot interception)
target_include_directories(l2-cv-bot PRIVATE lib/interception)

# Bundle executable with DLLs
if(MSVC)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
    
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(${CMAKE_INSTALL_PREFIX}/\${BUILD_TYPE}/l2-cv-bot.exe \"\" ${OpenCV_LIBS})
        configure_file(${CMAKE_SOURCE_DIR}/install/run.bat ${CMAKE_INSTALL_PREFIX}/\${BUILD_TYPE}/run.bat COPYONLY)
    " COMPONENT Runtime)
endif()

# Compiler options
if(MSVC)
    target_compile_options(l2-cv-bot PRIVATE /W4)
else()
    target_compile_options(l2-cv-bot PRIVATE -Wall -Wextra -pedantic)
endif()

# IWYU
find_program(IWYU_PATH NAMES include-what-you-use iwyu)

if(IWYU_PATH)
    list(APPEND IWYU_PATH "--driver-mode=cl")
    set_property(TARGET l2-cv-bot PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})
endif()

# cppcheck
find_program(CPPCHECK_PATH NAMES cppcheck)

if(CPPCHECK_PATH)
    set_property(TARGET l2-cv-bot PROPERTY CXX_CPPCHECK ${CPPCHECK_PATH})
endif()
