set(SOURCES
    cli.cpp
    cli_states.cpp
    main.cpp
    storage.hpp
)

set(HEADERS
    cli.hpp
    cli_states.hpp
    cmds/cmd_quit.hpp
    cmds/cmd_help.hpp
    cmds/cmd_login.hpp
    cmds/cmd_logout.hpp
    cmds/cmd_register.hpp
    cmds/cmd_send.hpp
    cmds/cmd_users_list.hpp
    cmds/cmd_inbox.hpp
    cmds/cmd_read.hpp
    cmds/cmd_remove.hpp
    fsm.hpp
    state.hpp
    types.hpp
    user.hpp
    utility.hpp
    application.hpp
    inbox.hpp
    inbox_message.hpp
    color.hpp
)

add_executable(lmail ${SOURCES} ${HEADERS})
target_include_directories(lmail PRIVATE ${CMAKE_CURRENT_LIST_DIR})

install(TARGETS lmail RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

target_link_libraries(lmail PRIVATE CONAN_PKG::boost)
target_link_libraries(lmail PRIVATE CONAN_PKG::sqlite_orm)
target_link_libraries(lmail PRIVATE CONAN_PKG::cryptopp)
target_link_libraries(lmail PRIVATE CONAN_PKG::readline)
