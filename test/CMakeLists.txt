find_package(Boost REQUIRED)
find_package(cryptopp REQUIRED)
find_package(GTest REQUIRED)

set(SOURCES
    main.cpp
)

set(HEADERS
)

add_executable(lmail_test ${SOURCES} ${HEADERS})
target_include_directories(lmail_test PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${LMAIL_SRC_DIR})

add_test(NAME GTestLMail COMMAND lmail_test)

target_link_libraries(lmail_test PRIVATE GTest::GTest)
target_link_libraries(lmail_test PRIVATE Boost::boost)
target_link_libraries(lmail_test PRIVATE cryptopp::cryptopp)

