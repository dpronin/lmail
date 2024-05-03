cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(lmail VERSION 1.8.2 LANGUAGES CXX)

include(CTest)

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

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(LMAIL_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

if (ENABLE_PROFILING)
    add_compile_options(-pg)
endif()

add_compile_definitions(
    RSA_KEY_SIZE_DEFAULT=3072
    PROJECT_VERSION=${PROJECT_VERSION}
)

find_package(Boost CONFIG QUIET COMPONENTS headers system)
find_package(sml CONFIG REQUIRED)
find_package(SqliteOrm CONFIG REQUIRED)
find_package(cryptopp CONFIG QUIET)
find_package(readline CONFIG QUIET)
find_package(termcolor CONFIG QUIET)

if (NOT ${Boost_FOUND} OR
    # NOT ${sml_FOUND} OR
    # NOT ${SqliteOrm_FOUND} OR
    NOT ${cryptopp_FOUND} OR
    NOT ${readline_FOUND})
    find_package(PkgConfig CONFIG REQUIRED)
endif()

if (NOT ${Boost_FOUND})
    pkg_check_modules(Boost REQUIRED)
    add_library(Boost::boost INTERFACE IMPORTED)
    target_include_directories(Boost::boost INTERFACE "${Boost_INCLUDE_DIRS}")
    target_link_libraries(Boost::boost INTERFACE "${Boost_LIBRARIES}")
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

if (NOT ${termcolor_FOUND})
    pkg_check_modules(termcolor REQUIRED termcolor)
    add_library(termcolor::termcolor INTERFACE IMPORTED)
    target_include_directories(termcolor::termcolor INTERFACE "${readline_INCLUDE_DIRS}")
    target_link_libraries(termcolor::termcolor INTERFACE "${readline_LIBRARIES}")
endif()

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
