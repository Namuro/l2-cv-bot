cmake_minimum_required(VERSION 3.11)

project(interception)

# Add sources
add_library(interception SHARED interception.c interception.rc)

# Compile definitions
target_compile_definitions(interception PRIVATE INTERCEPTION_EXPORT)

# Show more warnings
if(MSVC)
    target_compile_options(l2-cv-bot PRIVATE /W4)
else()
    target_compile_options(l2-cv-bot PRIVATE -Wall -Wextra -pedantic)
endif()
