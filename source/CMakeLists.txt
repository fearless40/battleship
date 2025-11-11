
add_library( 
   util 
   STATIC 
   util/rowcol.cpp
)

target_include_directories( 
   util
   PUBLIC
   ${ROOT_DIR}/include/util
)


add_library(
   gameserver 
   STATIC 
   gameserver/ship.cpp
)

add_library(
   AIBasic
   STATIC
   ai/averageai.cpp
   ai/bestai.cpp
   ai/guessallai.cpp
   ai/stupidai.cpp
)

target_include_directories(
   AIBasic 
   PUBLIC
   "${ROOT_DIR}/include/ai" 
)

target_link_libraries(
   AIBasic
   PUBLIC
   util
   gameserver
)

target_include_directories(
   gameserver 
   PUBLIC
   "${ROOT_DIR}/include/gameserver" 
)

target_link_libraries( gameserver 
   PUBLIC
   util
)

add_executable( 
   battleship 
   main.cpp
   simpleui.cpp
)

target_link_libraries( 
   battleship
   PUBLIC 
   gameserver
   util
)

target_include_directories ( 
   battleship 
   PUBLIC ${ROOT_DIR}/include 

   # For auto generated files from CMAKE
   PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include" 
)






