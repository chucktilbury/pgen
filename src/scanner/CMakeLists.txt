project(scanner)

include(${PROJECT_SOURCE_DIR}/../../CMakeBuildOpts.txt)

add_library(${PROJECT_NAME} STATIC
    scanner.cpp
    fileio.cpp
    tokens.cpp
)

