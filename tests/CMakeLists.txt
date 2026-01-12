

find_package(Catch2 REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
# if( ${Catch2_FOUND} )
#    message(WARNING "Catch2 Not found. Not building tests.")
#    return()
# endif()

message("Cath2 Dir: ${catch2_SOURCE_DIR}")


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

catch_discover_tests(scanlinerender)



