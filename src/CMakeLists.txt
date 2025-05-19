# CMakeList.txt: proyecto de CMake para Exeon, incluya el origen y defina
# la lógica específica del proyecto aquí.
#

# Agregue un origen al ejecutable de este proyecto.
add_executable (Exeon  "main.cpp" "icon.rc"  "public/Core/Core.h" "private/Core/Core.cpp" "public/Core/Renderer/Renderer.h" "private/Core/Renderer/Renderer.cpp" "public/Core/Renderer/D3D12.h" "private/Core/Renderer/D3D12.cpp" "public/Util.h" "public/Core/Renderer/DescriptorHeap.h" "private/Core/Renderer/DescriptorHeap.cpp" "public/Core/Renderer/Descriptor.h" "public/Core/Renderer/Shader.h" "private/Core/Renderer/Shader.cpp" "public/Core/Renderer/ScreenQuad.h" "private/Core/Renderer/ScreenQuad.cpp" "public/Core/Scene/SceneManager.h" "private/Core/Scene/SceneManager.cpp" "public/Core/Scene/Scene.h" "private/Core/Scene/Scene.cpp" "public/Core/GameObject/GameObject.h" "private/Core/GameObject/GameObject.cpp" "private/Core/GameObject/Component/Component.cpp" "private/Core/GameObject/Component/Component.cpp" "public/Core/GameObject/Component/Mesh.h" "private/Core/GameObject/Component/Mesh.cpp" "public/Core/Renderer/ResourceManager.h" "private/Core/Renderer/ResourceManager.cpp" "public/Math/Vector3.h" "private/Math/Vector3.cpp" "public/Math/Transform.h" "private/Math/Transform.cpp" "public/Core/Input/Input.h" "private/Core/Input/Input.cpp" "private/Core/GameObject/Camera/Camera.cpp" "public/Core/GameObject/Camera/Camera.h" "private/Core/GameObject/Camera/EditorCamera.cpp" "public/Core/GameObject/Camera/EditorCamera.h" "public/Core/Time.h" "private/Core/Time.cpp" "public/Core/Editor/Editor.h" "private/Core/Editor/Editor.cpp" "public/Core/Editor/Fonts/Roboto.h" "private/Core/Editor/Fonts/Roboto.cpp" "public/Runtime/MonoScript.h" "private/Runtime/MonoScript.cpp" "public/Core/Editor/Console.h" "private/Core/Editor/Console.cpp"
		public/Runtime/SceneImpl.h
		private/Runtime/SceneImpl.cpp
		public/Runtime/ConsoleImpl.h
		private/Runtime/ConsoleImpl.cpp
		public/Runtime/GameObjectImpl.h
		private/Runtime/GameObjectImpl.cpp
		public/Subsystem/Discord.h
		private/Subsystem/Discord.cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Exeon PROPERTY CXX_STANDARD 20)
  set_property(TARGET Exeon PROPERTY WIN32_EXECUTABLE TRUE)
endif()

if (MSVC)
	target_compile_options(Exeon PUBLIC $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/utf-8>)
endif()


if(WIN32)
	target_sources(Exeon PRIVATE icon.rc)
endif()

add_subdirectory("Managed")

add_dependencies(Exeon GameFramework)

set("PRIVATE_INCLUDE" "${CMAKE_CURRENT_SOURCE_DIR}/public")
set("PUBLIC_INCLUDE" "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set("LIB" "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

file(GLOB "IMGUI_SRC" 
	"${PUBLIC_INCLUDE}/imgui/*.cpp"
	"${PUBLIC_INCLUDE}/imgui/*.h"
)

file(GLOB "IMGUIZMO_SRC" 
	"${PUBLIC_INCLUDE}/ImGuizmo/*.cpp"
	"${PUBLIC_INCLUDE}/ImGuizmo/*.h"
)

add_library("ImGUI" STATIC ${IMGUI_SRC})
add_library("ImGuizmo" STATIC ${IMGUIZMO_SRC})

target_link_libraries(Exeon "d3d12.lib;dxgi.lib;d3dcompiler.lib")
target_link_libraries(Exeon "${LIB}/assimp-vc143-mt.lib;${LIB}/DirectXTK12.lib;${LIB}/spdlogd.lib;${LIB}/mono-2.0-sgen.lib;${LIB}/discord_partner_sdk.lib")
target_link_libraries(Exeon ImGUI)
target_link_libraries(Exeon ImGuizmo)

target_include_directories(Exeon PRIVATE "${PRIVATE_INCLUDE}")
include_directories("${PUBLIC_INCLUDE}")