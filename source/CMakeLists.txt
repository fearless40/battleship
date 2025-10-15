add_library(
   gameserver 
   STATIC 
   gameserver/rowcol.cpp
   gameserver/ship.cpp
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
