add_subdirectory(lib)
add_subdirectory(plugin)

install(
  DIRECTORY include/
  DESTINATION "${GZ_INCLUDE_INSTALL_DIR_FULL}")
