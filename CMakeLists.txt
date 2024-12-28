cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Project configuration
project("ProcCom")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to choose between Static or Dynamic Library
option(BUILD_DYNAMIC "Build as a dynamic library (DLL)" OFF)

# Detect platform
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PLATFORM "x86_64")
else()
    set(PLATFORM "x86")
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(PLATFORM "arm")
endif()

# Source directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CLIENT_SRC_DIR ${SRC_DIR}/client)
set(SERVER_SRC_DIR ${SRC_DIR}/server)

# Include directories
include_directories(${SRC_DIR})

# Source files
set(COMMON_SOURCES
    ${SRC_DIR}/proccom_common.c
)
set(CLIENT_SOURCES
    "src/client/proccom_client.c")
set(SERVER_SOURCES
    ${SERVER_SRC_DIR}/proccom_server.c
)

# Build static libraries if the dynamic option is off
if (NOT BUILD_DYNAMIC)
    # Static libraries
    add_library(NetRelayClientStatic STATIC ${COMMON_SOURCES} ${CLIENT_SOURCES})
    set_target_properties(NetRelayClientStatic PROPERTIES OUTPUT_NAME "client")

    add_library(NetRelayServerStatic STATIC ${COMMON_SOURCES} ${SERVER_SOURCES})
    set_target_properties(NetRelayServerStatic PROPERTIES OUTPUT_NAME "server")

    # Install static libraries
    install(TARGETS NetRelayClientStatic
        ARCHIVE DESTINATION static/client/${PLATFORM}/lib
    )
    install(TARGETS NetRelayServerStatic
        ARCHIVE DESTINATION static/server/${PLATFORM}/lib
    )

    # Install headers
    install(FILES
        ${CLIENT_SRC_DIR}/proccom_client.h
        DESTINATION static/include/client
    )
    install(FILES
        ${SERVER_SRC_DIR}/proccom_server.h
        DESTINATION static/include/server
    )

elseif (BUILD_DYNAMIC)
    # Dynamic library (DLL)
    add_library(NetRelay SHARED ${COMMON_SOURCES} ${CLIENT_SOURCES} ${SERVER_SOURCES})
    set_target_properties(NetRelay PROPERTIES OUTPUT_NAME "NetRelay")

    # Install DLL and dynamic libraries
    install(TARGETS NetRelay
        RUNTIME DESTINATION dynamic
        LIBRARY DESTINATION dynamic/client/${PLATFORM}/lib
    )

    # Install dynamic client and server libraries (linked to the DLL)
    install(TARGETS NetRelayClientStatic
        ARCHIVE DESTINATION dynamic/client/${PLATFORM}/lib
    )
    install(TARGETS NetRelayServerStatic
        ARCHIVE DESTINATION dynamic/server/${PLATFORM}/lib
    )

    # Create dynamic link libraries (.lib) that are linked to DLL
    add_custom_command(TARGET NetRelay POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/lib${CMAKE_BUILD_TYPE}/client/${PLATFORM}/lib/netRelayClientStatic.lib
            ${CMAKE_BINARY_DIR}/dynamic/client/${PLATFORM}/lib/client_dynamic.lib
    )
    add_custom_command(TARGET NetRelay POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/lib${CMAKE_BUILD_TYPE}/server/${PLATFORM}/lib/netRelayServerStatic.lib
            ${CMAKE_BINARY_DIR}/dynamic/server/${PLATFORM}/lib/server_dynamic.lib
    )

    # Install headers
    install(FILES
        ${CLIENT_SRC_DIR}/proccom_client.h
        DESTINATION dynamic/include/client
    )
    install(FILES
        ${SERVER_SRC_DIR}/proccom_server.h
        DESTINATION dynamic/include/server
    )
endif()
