CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)

add_subdirectory(tree)
add_subdirectory(InitReader)
add_subdirectory(RecipeReader)
add_subdirectory(plugins)
add_subdirectory(Plugin)
add_subdirectory(Chef)
add_subdirectory(cooker)
add_subdirectory(visualcooker)
add_subdirectory(Reconstruction)
add_subdirectory(combine)
#add_subdirectory(gdml)
add_subdirectory(udpLog)
add_subdirectory(MagneticField)
add_subdirectory(treeinfo)

set (BUILD_WITHDB off CACHE bool "Enable DB link for database
(normally off)")

set (BUILD_ALACARTE on CACHE bool "Enable build of alacarte")
IF(BUILD_ALACARTE)
  add_subdirectory(alacarte)
ENDIF ()
