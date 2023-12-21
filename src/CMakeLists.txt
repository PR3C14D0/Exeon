# CMakeList.txt: proyecto de CMake para Exeon, incluya el origen y defina
# la lógica específica del proyecto aquí.
#

# Agregue un origen al ejecutable de este proyecto.
add_executable (Exeon  "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Exeon PROPERTY CXX_STANDARD 20)
  set_property(TARGET Exeon PROPERTY WIN32_EXECUTABLE TRUE)
endif()
