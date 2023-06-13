cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(lmail VERSION 1.6.3 LANGUAGES CXX)

include(CTest)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (NOT DEFINED SCHEMA_DB_PREFIX)
    set(SCHEMA_DB_PREFIX "/var/lib")
endif()

if (NOT DEFINED CONF_PREFIX)
    set(CONF_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()

option(INSTALL_EMPTY_SCHEMA_DB "install an empty database into the ${SCHEMA_DB_PREFIX}/lmail")
option(INSTALL_DEFAULT_CONF "install a default configuration into the ${CONF_PREFIX}/etc")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(LMAIL_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

if (ENABLE_PROFILING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif()

add_definitions(-DCONF_PREFIX="${CONF_PREFIX}" -DSCHEMA_DB_PREFIX="${SCHEMA_DB_PREFIX}")

find_package(Boost REQUIRED)
find_package(cryptopp QUIET)
find_package(readline QUIET)

if (NOT ${cryptopp_FOUND} OR NOT ${readline_FOUND})
    find_package(PkgConfig REQUIRED)
endif()

if (NOT ${cryptopp_FOUND})
    pkg_check_modules(cryptopp REQUIRED libcryptopp)
    add_library(cryptopp::cryptopp INTERFACE IMPORTED)
    target_include_directories(cryptopp::cryptopp INTERFACE "${cryptopp_INCLUDE_DIRS}")
    target_link_libraries(cryptopp::cryptopp INTERFACE "${cryptopp_LIBRARIES}")
endif()

if (NOT ${readline_FOUND})
    pkg_check_modules(readline REQUIRED readline)
    add_library(readline::readline INTERFACE IMPORTED)
    target_include_directories(readline::readline INTERFACE "${readline_INCLUDE_DIRS}")
    target_link_libraries(readline::readline INTERFACE "${readline_LIBRARIES}")
endif()

find_package(SQLite3 REQUIRED)

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
