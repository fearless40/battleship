Include( FetchContent )

# message("Current Dir: ${CMAKE_CURRENT_SOURCE_DIR}")

FetchContent_Declare(
   Catch2
   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
   GIT_PROGRESS true
   GIT_TAG v3.12.0
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/catch2
   # FETCHCONTENT_UPDATES_DISCONNECTED true
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
