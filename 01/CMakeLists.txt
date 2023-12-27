project(01)

set(SOURCE_FILES
    Structures.hpp
    StartInit.hpp
    GameLogic.hpp
    Animation.hpp
    main.cpp
    StartInit.cpp
    GameLogic.cpp
    Animation.cpp
)

set(SFML_STATIC_LIBRARIES TRUE)

find_package(Freetype REQUIRED)
find_package(JPEG REQUIRED)
find_package(SFML 2 COMPONENTS window graphics system REQUIRED)

add_executable(01 ${SOURCE_FILES})

target_include_directories(01 PRIVATE ${SFML_INCLUDE_DIR})
target_compile_features(01 PUBLIC cxx_std_17)
target_compile_definitions(01 PRIVATE SFML_STATIC)

target_link_libraries(01 ${SFML_LIBRARIES} ${SFML_DEPENDENCIES}) 
