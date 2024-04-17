find_package(GTest CONFIG QUIET)

if (NOT ${GTest_FOUND})
    if (NOT ${PkgConfig_FOUND})
        find_package(PkgConfig CONFIG REQUIRED)
    endif ()
    pkg_check_modules(GTest REQUIRED)
    add_library(gtest::gtest INTERFACE IMPORTED)
    target_include_directories(gtest::gtest INTERFACE "${GTest_INCLUDE_DIRS}")
    target_link_libraries(gtest::gtest INTERFACE "${GTest_LIBRARIES}")
endif ()

set(SOURCES
    main.cpp
)

set(HEADERS
)

add_executable(lmail_test ${SOURCES} ${HEADERS})
target_include_directories(lmail_test PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${LMAIL_SRC_DIR})

add_test(NAME GTestLMail COMMAND lmail_test)

target_link_libraries(lmail_test PRIVATE
    gtest::gtest
    Boost::boost
    cryptopp::cryptopp
)
