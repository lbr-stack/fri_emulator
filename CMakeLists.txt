cmake_minimum_required(VERSION 3.18)
project(fri_emulator)

add_subdirectory(fri)

add_executable(
    fri_emulator
    main.cpp
)

target_link_libraries(
    fri_emulator
    PRIVATE
        FRIClient::FRIClient
        FRIClient::nanopb
        FRIClient::protobuf
)

add_executable(encode
    encode.cpp
)

target_link_libraries(encode
    FRIClient::FRIClient
    FRIClient::nanopb
    FRIClient::protobuf
)