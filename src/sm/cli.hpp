#pragma once

#include <iostream>
#include <stdexcept>

#include "boost/sml.hpp"

#include "color.hpp"
#include "readline.hpp"
#include "types.hpp"

#include "cmds/cmd_init_parser.hpp"
#include "cmds/cmd_interface.hpp"
#include "cmds/cmd_parser_interface.hpp"

#include "sm/cli_events.hpp"

#include "states/cli_state.hpp"
#include "states/cmd_state.hpp"

namespace lmail::sm
{

class CliTransitions;
class CliCtx final
{
    friend class CliTransitions;

private:
    void set_state(std::shared_ptr<CliState> state)
    {
        if (!state)
            throw std::invalid_argument("state provided cannot be empty");
        state_ = std::move(state);
        rl_.set_cmd_lister(state_);
    }

    void set_cmd_parser(std::shared_ptr<ICmdParser> cmd_parser)
    {
        if (!cmd_parser)
            throw std::invalid_argument("cmd_parser provided cannot be empty");
        cmd_parser_ = std::move(cmd_parser);
    }

    std::shared_ptr<CliState> state_;
    std::shared_ptr<ICmdParser> cmd_parser_;
    Readline& rl_;

public:
    explicit CliCtx(Readline& rl)
        : rl_(rl)
        , state_(std::make_shared<CmdState>())
        , cmd_parser_(std::make_shared<CmdInitParser>())
    {
    }
    ~CliCtx() = default;

    CliCtx(CliCtx const&)            = delete;
    CliCtx& operator=(CliCtx const&) = delete;

    CliCtx(CliCtx&&)            = delete;
    CliCtx& operator=(CliCtx&&) = delete;

    [[nodiscard]] auto operator()() { return rl_(state_->prompt()); }
    auto& cmd_parser() noexcept { return *cmd_parser_; }
};

class CliTransitions
{
public:
    auto operator()() const
    {
        using namespace boost::sml;
        // clang-format off
        auto const set_state      = [] (CliCtx &ctx, auto const &ev) { ctx.set_state(ev.state); };
        auto const set_cmd_parser = [] (CliCtx &ctx, auto const &ev) { ctx.set_cmd_parser(ev.cmd_parser); };
        auto const welcome        = [] { std::cout << "Welcome to " << colorize::cyellow("lmail") << '!' << std::endl; };
        auto const on_entry       = [] (CliCtx &/*ctx*/, auto const &ev) { ev.on_entry(); };
        auto const on_exit        = [] (CliCtx &/*ctx*/, auto const &ev) { ev.on_exit(); };
        auto const on_quit        = [] { std::cout << "Quitting " << colorize::cyellow("lmail") << ". Bye!" << std::endl; };
        auto const exec           = [] (CliCtx &/*ctx*/, auto const &ev){ ev.executor(); return true; };
        return make_transition_table(
            // idle state
            *"idle"_s     + boost::sml::on_entry<initial>                   / welcome,
            "idle"_s      + event<ev::run>                                                                           = "main"_s,
            // main state
            "main"_s      + boost::sml::on_entry<ev::run>                   / (set_state, set_cmd_parser),
            "main"_s      + boost::sml::on_entry<ev::logout>                / (set_state, set_cmd_parser),
            "main"_s      + boost::sml::on_exit<ev::quit>                   / on_quit,
            "main"_s      + event<ev::login>                        [exec],
            "main"_s      + event<ev::quit>                                                                          = X,
            "main"_s      + event<ev::reg>                          [exec],
            "main"_s      + event<ev::login_finish>                                                                  = "logged-in"_s,
            // logged-in state
            "logged-in"_s + boost::sml::on_entry<ev::login_finish>          / (set_state, set_cmd_parser, on_entry),
            "logged-in"_s + boost::sml::on_exit<ev::logout>                 / on_exit,
            "logged-in"_s + boost::sml::on_exit<ev::quit>                   / on_quit,
            "logged-in"_s + event<ev::inbox>                        [exec],
            "logged-in"_s + event<ev::keyassoc>                     [exec],
            "logged-in"_s + event<ev::keyexp>                       [exec],
            "logged-in"_s + event<ev::keygen>                       [exec],
            "logged-in"_s + event<ev::keyimp>                       [exec],
            "logged-in"_s + event<ev::logout>                                                                        = "main"_s,
            "logged-in"_s + event<ev::lskeys>                       [exec],
            "logged-in"_s + event<ev::lsusers>                      [exec],
            "logged-in"_s + event<ev::quit>                                                                          = X,
            "logged-in"_s + event<ev::readmsg>                      [exec],
            "logged-in"_s + event<ev::rmkey>                        [exec],
            "logged-in"_s + event<ev::rmkeyassoc>                   [exec],
            "logged-in"_s + event<ev::rmkeyimp>                     [exec],
            "logged-in"_s + event<ev::rmmsg>                        [exec],
            "logged-in"_s + event<ev::sendmsg>                      [exec]
        );
        // clang-format on
    }
};

using Cli = boost::sml::sm<CliTransitions>;

} // namespace lmail::sm
