set(SOURCES
    cli.cpp
    main.cpp
    states/cmd_state.cpp
    states/logged_in_state.cpp
    states/main_state.cpp
    storage.hpp
)

set(HEADERS
    application.hpp
    cli.hpp
    cmds/cmd_help.hpp
    cmds/cmd_inbox.hpp
    cmds/cmd_keyassoc.hpp
    cmds/cmd_keyexp.hpp
    cmds/cmd_keyimp.hpp
    cmds/cmd_keygen.hpp
    cmds/cmd_keyassoc.hpp
    cmds/cmd_rmkeyassoc.hpp
    cmds/cmd_list_keys.hpp
    cmds/cmd_list_users.hpp
    cmds/cmd_login.hpp
    cmds/cmd_logout.hpp
    cmds/cmd_quit.hpp
    cmds/cmd_readmsg.hpp
    cmds/cmd_register.hpp
    cmds/cmd_rmmsg.hpp
    cmds/cmd_sendmsg.hpp
    cmds/cmd_args.hpp
    profile.hpp
    logged_user.hpp
    color.hpp
    fsm.hpp
    inbox.hpp
    inbox_message.hpp
    state.hpp
    states/cli_state.hpp
    states/cmd_state.hpp
    states/init_state.hpp
    states/logged_in_state.hpp
    states/main_state.hpp
    types.hpp
    user.hpp
    utility.hpp
)

add_executable(lmail ${SOURCES} ${HEADERS})
target_include_directories(lmail PRIVATE ${CMAKE_CURRENT_LIST_DIR})

install(TARGETS lmail RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

target_link_libraries(lmail PRIVATE CONAN_PKG::boost)
target_link_libraries(lmail PRIVATE CONAN_PKG::sqlite_orm)
target_link_libraries(lmail PRIVATE CONAN_PKG::cryptopp)
target_link_libraries(lmail PRIVATE CONAN_PKG::readline)
