cmake_minimum_required(VERSION 4.2)

project(Minecraft)

option(RELEASE_OPTIMIZATIONS_IN_DEBUG "Use release optimizations in Debug build" OFF)

find_package(bgfx CONFIG REQUIRED)
#find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

#add_subdirectory("libs/bgfx.cmake")

add_executable(Minecraft
	"minecraft.cpp"
	"registry.cpp" 
	"debug.cpp"
	"libs/glad/glad.c"
	"libs/stb/stb_image.cpp"
)

if(RELEASE_OPTIMIZATIONS_IN_DEBUG)
	target_compile_definitions(Minecraft PRIVATE RELEASE_OPTIMIZATIONS_IN_DEBUG)
	target_compile_options(Minecraft PRIVATE
    $<$<CONFIG:Debug_Optimized>:/O2>
    $<$<CONFIG:Debug_Optimized>:/Zi>
	)
endif()

set_target_properties(Minecraft PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    #CXX_EXTENSIONS OFF
)

target_include_directories(Minecraft PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad
	${CMAKE_CURRENT_SOURCE_DIR}/libs/stb
)

target_link_libraries(Minecraft PRIVATE 
	bgfx::bgfx
#	Vulkan::Vulkan
	OpenGL::GL
	glfw
	glm::glm
)

add_custom_command(TARGET Minecraft POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Minecraft>/assets"
)
