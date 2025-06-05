set(SOURCES
        main.cpp
        storage/disk.cpp
        storage/headFile.cpp
        storage/bloque.cpp
        query/query.cpp
        query/esquema.cpp
        query/archivo.cpp
        control/control.cpp
)

set(HEADERS include)

add_executable(Megatron3000 ${SOURCES} ${HEADERS})

target_include_directories(Megatron3000 PUBLIC include)