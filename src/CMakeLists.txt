set(SOURCES
        main.cpp
        storage/disk.cpp
        query/query.cpp
        query/esquema.cpp
        storage/headFile.cpp
        query/archivo.cpp
        control/control.cpp
)

set(HEADERS include)

add_executable(megatron3000 ${SOURCES} ${HEADERS})

target_include_directories(megatron3000 PUBLIC include)