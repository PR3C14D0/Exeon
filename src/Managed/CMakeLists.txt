cmake_minimum_required(VERSION 3.10)
project(GameFramework NONE)

set(CSHARP_PROJECT ExeonScript.csproj)
set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB_RECURSE CSHARP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cs")

add_custom_command(
        OUTPUT ${OUTPUT_DIR}/ExeonScript.dll
        COMMAND dotnet build ${CSHARP_PROJECT} --configuration Debug /p:OutputPath=${OUTPUT_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CSHARP_PROJECT} ${CSHARP_SOURCES}
        COMMENT "Compiling C# scripts..."
)

add_custom_target(GameFramework ALL
    DEPENDS ${OUTPUT_DIR}/ExeonScript.dll
)
