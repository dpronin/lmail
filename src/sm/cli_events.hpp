#pragma once

#include <functional>
#include <memory>

#include "cmds/cmd_parser_interface.hpp"

#include "states/cli_state.hpp"

namespace lmail::sm::ev
{

struct event {
    std::shared_ptr<CliState> state;
    std::shared_ptr<ICmdParser> cmd_parser;
};

struct on_entry {
    std::function<void()> on_entry;
};

struct on_exit {
    std::function<void()> on_exit;
};

struct event2 {
    std::function<void()> executor;
};

struct inbox : event2 {
};

struct keyassoc : event2 {
};

struct keyexp : event2 {
};

struct keygen : event2 {
};

struct keyimp : event2 {
};

struct lskeys : event2 {
};

struct lsusers : event2 {
};

struct readmsg : event2 {
};

struct login : event2 {
};

struct rmkey : event2 {
};

struct rmkeyassoc : event2 {
};

struct rmkeyimp : event2 {
};

struct rmmsg : event2 {
};

struct sendmsg : event2 {
};

struct reg : event2 {
};

struct run : event {
};

struct login_finish : event, on_entry {
};

struct logout : event, on_exit {
};

struct quit {
};

} // namespace lmail::sm::ev
