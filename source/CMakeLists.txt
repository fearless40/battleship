
message(STATUS "ROOT_DIR = ${ROOT_DIR}")


add_library(
   adam.util
   INTERFACE
)

target_sources( adam.util
   PUBLIC
      FILE_SET HEADERS
      BASE_DIRS ${ROOT_DIR}/include/util/
      FILES
        ${ROOT_DIR}/include/util/array2d.hpp
        ${ROOT_DIR}/include/util/baseconv.hpp
        ${ROOT_DIR}/include/util/randomutil.hpp
        ${ROOT_DIR}/include/util/soa.hpp
        ${ROOT_DIR}/include/util/soamemorylayout.hpp
        ${ROOT_DIR}/include/util/stackhelpers.hpp
)

# target_include_directories(
#    util
#    PUBLIC
#    ${ROOT_DIR}/include/util
# )
#
add_library( adam.geom
   STATIC
      geom/rowcol.cpp
)

target_sources( adam.geom
   PUBLIC
   FILE_SET HEADERS
   BASE_DIRS ${ROOT_DIR}/include/geom/
   FILES
      ${ROOT_DIR}/include/geom/aabb.hpp
      ${ROOT_DIR}/include/geom/collisions.hpp
      ${ROOT_DIR}/include/geom/onedrange.hpp
      ${ROOT_DIR}/include/geom/rect.hpp
      ${ROOT_DIR}/include/geom/rowcol.hpp
      ${ROOT_INCLUDE}/geom/relative_dimension.hpp
      ${ROOT_INCLUDE}/geom/typed_position.hpp
)


add_library( adam.terminal
   STATIC
      terminal/keyparser.cpp
      terminal/term_control.cpp
)

target_sources( adam.terminal
   PUBLIC
      FILE_SET HEADERS
      BASE_DIRS ${ROOT_DIR}/include/terminal/
      FILES
         ${ROOT_DIR}/include/terminal/color.hpp
         ${ROOT_DIR}/include/terminal/control_codes.hpp
         ${ROOT_DIR}/include/terminal/pixel.hpp
         ${ROOT_DIR}/include/terminal/scanline_stack_render.hpp
         ${ROOT_DIR}/include/terminal/compositor.hpp
         ${ROOT_DIR}/include/terminal/image.hpp
         ${ROOT_DIR}/include/terminal/render.hpp
         ${ROOT_DIR}/include/terminal/scanline_stack_row.hpp
         ${ROOT_DIR}/include/terminal/compositor_shared.hpp
         ${ROOT_DIR}/include/terminal/keyparser.hpp
         ${ROOT_DIR}/include/terminal/scanlinerender_painter.hpp
         ${ROOT_DIR}/include/terminal/term_control.hpp
   )

# target_include_directories(
#    terminal
#    PUBLIC
#    "${ROOT_DIR}/include/terminal"
# )

target_link_libraries( adam.terminal
   PUBLIC
   adam.util
   adam.geom
)


add_library(
   battleship.AIBasic
   STATIC
      ai/averageai.cpp
      ai/bestai.cpp
      ai/guessallai.cpp
      ai/stupidai.cpp
)

target_sources( battleship.AIBasic
   PUBLIC
      FILE_SET HEADERS
      BASE_DIRS ${ROOT_DIR}/include/ai/
      FILES
        ${ROOT_DIR}/include/ai/ai.hpp
        ${ROOT_DIR}/include/ai/averageai.hpp
        ${ROOT_DIR}/include/ai/bestai.hpp
        ${ROOT_DIR}/include/ai/guessallai.hpp
        ${ROOT_DIR}/include/ai/smartai.hpp
        ${ROOT_DIR}/include/ai/stupidai.hpp
)

# target_include_directories(
#    battleship.AIBasic
#    PUBLIC
#    "${ROOT_DIR}/include/ai"
# )

target_link_libraries(
   battleship.AIBasic
   PUBLIC
   adam.util
   battleship.gameserver
)

target_link_libraries(adam.geom
   PUBLIC
      adam.util
)

add_library(
   battleship.gameserver
   STATIC
      gameserver/ship.cpp
   )

target_sources( battleship.gameserver
   PUBLIC
      FILE_SET HEADERS
      BASE_DIRS ${ROOT_DIR}/include/gameserver/
      FILES
         ${ROOT_DIR}/include/gameserver/action.hpp
         ${ROOT_DIR}/include/gameserver/actionresponse.hpp
         ${ROOT_DIR}/include/gameserver/api.hpp
         ${ROOT_DIR}/include/gameserver/gamelayout.hpp
         ${ROOT_DIR}/include/gameserver/server.hpp
         ${ROOT_DIR}/include/gameserver/ship.hpp
)


# target_include_directories(
#    gameserver
#    PUBLIC
#    "${ROOT_DIR}/include/gameserver"
# )

target_link_libraries( battleship.gameserver
   PUBLIC
   adam.geom
   adam.util
)


add_executable(
   battleship.battleship
   main.cpp
   simpleui.cpp
)

target_link_libraries(
   battleship.battleship
   PUBLIC
   battleship.gameserver
   adam.util
   battleship.AIBasic
   adam.terminal
)

# target_include_directories (
#    battleship.battleship
#    PUBLIC ${ROOT_DIR}/include
#
#    # For auto generated files from CMAKE
#    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include"
# )
#






