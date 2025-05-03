# CMakeList.txt: proyecto de CMake para Exeon, incluya el origen y defina
# la lógica específica del proyecto aquí.
#

# Agregue un origen al ejecutable de este proyecto.
add_executable (Exeon  "main.cpp"  "public/Core/Core.h" "private/Core/Core.cpp" "public/Core/Renderer/Renderer.h" "private/Core/Renderer/Renderer.cpp" "public/Core/Renderer/D3D12.h" "private/Core/Renderer/D3D12.cpp" "public/Util.h" "public/Core/Renderer/DescriptorHeap.h" "private/Core/Renderer/DescriptorHeap.cpp" "public/Core/Renderer/Descriptor.h" "public/Core/Renderer/Shader.h" "private/Core/Renderer/Shader.cpp" "public/Core/Renderer/ScreenQuad.h" "private/Core/Renderer/ScreenQuad.cpp" "public/Core/Scene/SceneManager.h" "private/Core/Scene/SceneManager.cpp" "public/Core/Scene/Scene.h" "private/Core/Scene/Scene.cpp" "public/Core/GameObject/GameObject.h" "private/Core/GameObject/GameObject.cpp" "private/Core/GameObject/Component/Component.cpp" "private/Core/GameObject/Component/Component.cpp" "public/Core/GameObject/Component/Mesh.h" "private/Core/GameObject/Component/Mesh.cpp" "public/Core/Renderer/ResourceManager.h" "private/Core/Renderer/ResourceManager.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Exeon PROPERTY CXX_STANDARD 20)
  set_property(TARGET Exeon PROPERTY WIN32_EXECUTABLE TRUE)
endif()

set("PRIVATE_INCLUDE" "${CMAKE_CURRENT_SOURCE_DIR}/public")
set("PUBLIC_INCLUDE" "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set("LIB" "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

target_link_libraries(Exeon "d3d12.lib;dxgi.lib;d3dcompiler.lib")
target_link_libraries(Exeon "${LIB}/assimp-vc143-mt.lib;${LIB}/DirectXTK12.lib")

target_include_directories(Exeon PRIVATE "${PRIVATE_INCLUDE}")
target_include_directories(Exeon PUBLIC "${PUBLIC_INCLUDE}")