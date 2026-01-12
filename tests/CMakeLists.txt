

# if( ${Catch2_FOUND} )
#    message(WARNING "Catch2 Not found. Not building tests.")
#    return()
# endif()

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

message("Cath2 Dir: ${Catch2_SOURCE_DIR}")
message("CMAKE_MODULES: ${CMAKE_MODULE_PATH}")

project(
   Tests
   LANGUAGES CXX
   VERSION 0.0.1
)

add_executable(scanlinerender
   test_scanlinerender.cpp
)

target_link_libraries(
   scanlinerender
   PRIVATE Catch2::Catch2WithMain
   PRIVATE util
   PRIVATE terminal
)

include(CTest)
include(Catch)

enable_testing()

catch_discover_tests(scanlinerender)



