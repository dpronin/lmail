set(SOURCES
    cli.cpp
    main.cpp
    states/cmd_state.cpp
    states/logged_in_state.cpp
    states/main_state.cpp
    readline.cpp
)

set(HEADERS
    application.hpp
    cli.hpp
    cmd_interface.hpp
    cmds/cmd_args.hpp
    cmds/cmd_help.hpp
    cmds/cmd_inbox.hpp
    cmds/cmd_keyassoc.hpp
    cmds/cmd_keyassoc.hpp
    cmds/cmd_keyexp.hpp
    cmds/cmd_keygen.hpp
    cmds/cmd_keyimp.hpp
    cmds/cmd_list_keys.hpp
    cmds/cmd_list_users.hpp
    cmds/cmd_login.hpp
    cmds/cmd_logout.hpp
    cmds/cmd_quit.hpp
    cmds/cmd_readmsg.hpp
    cmds/cmd_register.hpp
    cmds/cmd_rmkeyassoc.hpp
    cmds/cmd_rmmsg.hpp
    cmds/cmd_sendmsg.hpp
    color.hpp
    command_lister_interface.hpp
    crypt.hpp
    db/message.hpp
    db/user.hpp
    inbox.hpp
    inbox_message.hpp
    logged_user.hpp
    logged_user_utility.hpp
    profile.hpp
    readline.hpp
    states/cli_state.hpp
    states/cmd_state.hpp
    states/init_state.hpp
    states/logged_in_state.hpp
    states/main_state.hpp
    storage.hpp
    types.hpp
    uread.hpp
    utility.hpp
)

add_executable(lmail ${SOURCES} ${HEADERS})
target_compile_definitions(lmail PRIVATE CONF_PREFIX="${CONF_PREFIX}" SCHEMA_DB_PREFIX="${SCHEMA_DB_PREFIX}")

target_include_directories(lmail PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(lmail PRIVATE Boost::boost)
target_link_libraries(lmail PRIVATE sml::sml)
target_link_libraries(lmail PRIVATE fmt::fmt)
target_link_libraries(lmail PRIVATE cryptopp::cryptopp)
target_link_libraries(lmail PRIVATE readline::readline)
target_link_libraries(lmail PRIVATE sqlite_orm::sqlite_orm)
target_link_libraries(lmail PRIVATE SQLite::SQLite3)

install(TARGETS lmail RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
