#pragma once

#include <iostream>
#include <stdexcept>

#include "boost/sml.hpp"

#include "color.hpp"
#include "readline.hpp"
#include "types.hpp"

#include "states/cli_state.hpp"
#include "states/init_state.hpp"

#include "cmd_interface.hpp"

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

    std::shared_ptr<CliState> state_;
    Readline& rl_;

public:
    explicit CliCtx(Readline& rl)
        : rl_(rl)
        , state_(std::make_shared<InitState>())
    {
    }
    ~CliCtx() = default;

    CliCtx(CliCtx const&)            = delete;
    CliCtx& operator=(CliCtx const&) = delete;

    CliCtx(CliCtx&&)            = delete;
    CliCtx& operator=(CliCtx&&) = delete;

    [[nodiscard]] bool operator()(user_input_t& user_input) { return rl_(user_input, state_->prompt()); }
    [[nodiscard]] std::unique_ptr<ICmd> parse(args_t args) { return state_->parse(std::move(args)); }
};

namespace ev
{

struct event {
    std::shared_ptr<CliState> state;
};

struct on_entry {
    std::function<void()> on_entry;
};

struct on_exit {
    std::function<void()> on_exit;
};

struct run : event {
};

struct login : event, on_entry {
};

struct logout : event, on_exit {
};

struct quit {
};

} // namespace ev

class CliTransitions
{
public:
    auto operator()() const
    {
        using namespace boost::sml;
        // clang-format off
        auto set_state        = [] (CliCtx &ctx, auto const &ev) { ctx.set_state(ev.state); };
        auto on_entry_initial = [] { std::cout << "Welcome to " << cbrown("lmail") << '!' << std::endl; };
        auto on_entry         = [] (CliCtx &/*ctx*/, auto const &ev) { ev.on_entry(); };
        auto on_exit          = [] (CliCtx &/*ctx*/, auto const &ev) { ev.on_exit(); };
        auto on_quit          = [] { std::cout << "Quitting " << cbrown("lmail") << ". Bye!" << std::endl; };
        return make_transition_table(
            // idle state
            *"idle"_s     + boost::sml::on_entry<initial>    / on_entry_initial,
            "idle"_s      + event<ev::run>                                           = "main"_s,
            // main state
            "main"_s      + boost::sml::on_entry<ev::run>    / set_state,
            "main"_s      + boost::sml::on_entry<ev::logout> / set_state,
            "main"_s      + event<ev::login>                                         = "logged-in"_s,
            "main"_s      + event<ev::quit>                  / on_quit               = X,
            // logged-in state
            "logged-in"_s + boost::sml::on_entry<ev::login>  / (set_state, on_entry),
            "logged-in"_s + boost::sml::on_exit<ev::logout>  / on_exit,
            "logged-in"_s + event<ev::logout>                                        = "main"_s,
            "logged-in"_s + event<ev::quit>                  / on_quit               = X
        );
        // clang-format on
    }
};

using Cli = boost::sml::sm<CliTransitions>;

} // namespace lmail::sm
