project(pgen)

include(${PROJECT_SOURCE_DIR}/../CMakeBuildOpts.txt)

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_files}
    templates.cpp
)

target_link_libraries(${PROJECT_NAME}
    #gc
    #cord
)

file(GLOB_RECURSE ${PROJECT_NAME}_template_files
    ${PROJECT_SOURCE_DIR}/templates/*.txt
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/templates.cpp ${CMAKE_CURRENT_SOURCE_DIR}/templates.h
    COMMENT "Making templates"
    DEPENDS ${${PROJECT_NAME}_template_files} ${CMAKE_CURRENT_SOURCE_DIR}/templates/make_templates.py
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/templates/make_templates.py ${CMAKE_CURRENT_SOURCE_DIR} ${${PROJECT_NAME}_template_files}
)
