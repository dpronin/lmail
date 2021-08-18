cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(lmail VERSION 1.1.0 LANGUAGES CXX)

include(CTest)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message(FATAL_ERROR "conanbuildinfo.cmake required is missing")
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

if (NOT DEFINED SCHEMA_DB_PREFIX)
    set(SCHEMA_DB_PREFIX "/var/lib")
endif()

if (NOT DEFINED CONF_PREFIX)
    set(CONF_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()

option(INSTALL_EMPTY_SCHEMA_DB "install an empty database into the ${SCHEMA_DB_PREFIX}/lmail")
option(INSTALL_DEFAULT_CONF "install a default configuration into the ${CONF_PREFIX}/etc")

set(CMAKE_CXX_STANDARD 17)

set(LMAIL_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

if (ENABLE_PROFILING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif()

if (ENABLE_GDB_SYMBOLS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
endif()

add_definitions(-DCONF_PREFIX="${CONF_PREFIX}" -DSCHEMA_DB_PREFIX="${SCHEMA_DB_PREFIX}")

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(test)
endif()

if (INSTALL_DEFAULT_CONF)
    configure_file(etc/lmail.conf.in lmail.conf @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/lmail.conf DESTINATION ${CONF_PREFIX}/etc/ COMPONENT config)
endif()

if (INSTALL_EMPTY_SCHEMA_DB)
    install(CODE "execute_process(COMMAND /usr/bin/env bash ${CMAKE_CURRENT_LIST_DIR}/db/prepare_sys_db.sh ${CMAKE_CURRENT_LIST_DIR}/db/empty-schema.db ${SCHEMA_DB_PREFIX}/lmail/schema.db)")
endif()
