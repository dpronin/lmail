set(SOURCES
    cli.cpp
    cmds/cmd_logout.cpp
    cmds/cmd_parser_default.cpp
    main.cpp
    readline.cpp
)

set(HEADERS
    application.hpp
    cli.hpp
    cmds/cmd.hpp
    cmds/cmd_args.hpp
    cmds/cmd_help.hpp
    cmds/cmd_inbox.hpp
    cmds/cmd_init_parser.hpp
    cmds/cmd_interface.hpp
    cmds/cmd_invoker.hpp
    cmds/cmd_keyassoc.hpp
    cmds/cmd_keyassoc.hpp
    cmds/cmd_keyexp.hpp
    cmds/cmd_keygen.hpp
    cmds/cmd_keyimp.hpp
    cmds/cmd_list_keys.hpp
    cmds/cmd_list_users.hpp
    cmds/cmd_lister_interface.hpp
    cmds/cmd_login.hpp
    cmds/cmd_logout.hpp
    cmds/cmd_parser_default.hpp
    cmds/cmd_parser_interface.hpp
    cmds/cmd_quit.hpp
    cmds/cmd_readmsg.hpp
    cmds/cmd_register.hpp
    cmds/cmd_rmkeyassoc.hpp
    cmds/cmd_rmmsg.hpp
    cmds/cmd_sendmsg.hpp
    color.hpp
    crypt.hpp
    db/message.hpp
    db/user.hpp
    inbox.hpp
    inbox_message.hpp
    logged_user.hpp
    logged_user_utility.hpp
    profile.hpp
    readline.hpp
    sm/cli.hpp
    sm/cli_events.hpp
    states/cli_state.hpp
    states/cmd_state.hpp
    storage.hpp
    types.hpp
    uread.hpp
    utility.hpp
)

add_executable(lmail ${SOURCES} ${HEADERS})
target_compile_definitions(lmail PRIVATE CONF_PREFIX="${CONF_PREFIX}" SCHEMA_DB_PREFIX="${SCHEMA_DB_PREFIX}")

target_include_directories(lmail PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(lmail PRIVATE
    Boost::headers
    Boost::system
    sml::sml
    cryptopp::cryptopp
    readline::readline
    sqlite_orm::sqlite_orm
    termcolor::termcolor
)

install(TARGETS lmail RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
