cmake_minimum_required(VERSION 3.11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(l2-cv-bot)

# Add sources
add_executable(
    l2-cv-bot
    src/main.cpp
    src/Bot.cpp
    src/Utils.h
    src/Options.cpp
    src/Window.cpp
    src/Capture.cpp
    src/FPS.h
    src/Eyes.cpp
    src/Input.cpp
)

# Configure compiler
set_property(TARGET l2-cv-bot PROPERTY CXX_STANDARD 17)
set_property(TARGET l2-cv-bot PROPERTY CXX_STANDARD_REQUIRED ON)

# Link OpenCV
find_package(OpenCV REQUIRED)
target_link_libraries(l2-cv-bot ${OpenCV_LIBS})

# Link Interception API library
add_subdirectory(lib/interception)
target_link_libraries(l2-cv-bot interception)
target_include_directories(l2-cv-bot PRIVATE lib/interception)

# Bundle executable with DLLs
if(MSVC)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
    
    # \${BUILD_TYPE} - hack to find path to executable for current configuration (Release/Debug)
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(${CMAKE_INSTALL_PREFIX}/\${BUILD_TYPE}/l2-cv-bot.exe \"\" ${OpenCV_LIBS})
        configure_file(${CMAKE_SOURCE_DIR}/run.bat ${CMAKE_INSTALL_PREFIX}/\${BUILD_TYPE}/run.bat COPYONLY)
    " COMPONENT Runtime)
endif()

# Show more warnings
if(MSVC)
    target_compile_options(l2-cv-bot PRIVATE /W4)
else()
    target_compile_options(l2-cv-bot PRIVATE -Wall -Wextra -pedantic)
endif()
