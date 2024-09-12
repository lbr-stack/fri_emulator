cmake_minimum_required(VERSION 3.18)
project(fri_emulator)

add_subdirectory(fri) # TODO: replace with local install i.e. find package or similar?

add_library(${PROJECT_NAME}
    src/commanding_message_decoder.cpp
    src/monitoring_message_encoder.cpp
    src/udp_server.cpp
    src/state_machine.cpp
    src/virtual_hardware.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        FRIClient::FRIClient # system specific states
        FRIClient::nanopb # encoding / decoding
        FRIClient::protobuf # FRI message definitions
)

add_executable(
    fri_emulator_test
    main.cpp
)

target_link_libraries(
    fri_emulator_test
    PRIVATE
        FRIClient::FRIClient
        FRIClient::nanopb
        FRIClient::protobuf
        ${PROJECT_NAME}
)

add_executable(encode
    encode.cpp
)

target_link_libraries(encode
    FRIClient::FRIClient
    FRIClient::nanopb
    FRIClient::protobuf
)
