#pragma once

#include <stdexcept>

#include "boost/sml.hpp"

#include "readline.hpp"
#include "types.hpp"

#include "states/cli_state.hpp"
#include "states/init_state.hpp"

namespace lmail
{

class Cli;
namespace sm
{

class CliSmCtx
{
public:
    explicit CliSmCtx(Readline &rl) : rl_(rl), state_(std::make_shared<InitState>()) {}

    auto prompt() const { return state_->prompt(); }
    void set_state(std::shared_ptr<CliState> state)
    {
        if (!state)
            throw std::invalid_argument("state provided cannot be empty");
        state_->OnExit();
        state_ = std::move(state);
        state_->OnEnter();
        rl_.set_cmd_lister(state_);
    }
    void process(args_t args) { state_->process(std::move(args)); }

private:
    std::shared_ptr<CliState> state_;
    Readline &                rl_;
};

namespace ev
{

struct event
{
    std::shared_ptr<CliState> state;
};

struct run : event
{
};

struct login : event
{
};

struct logout : event
{
};

struct quit
{
};

} // namespace ev

namespace act
{

// clang-format off
constexpr auto set_state = [](CliSmCtx &ctx, auto ev) { ctx.set_state(std::move(ev.state)); };
// clang-format on

} // namespace act

struct CliSm
{
    auto operator()() const
    {
        using namespace boost::sml;
        // clang-format off
        return make_transition_table(
            // idle state
            *"idle"_s     + event<ev::run>  / act::set_state     = "main"_s,
            // main state
            "main"_s      + event<ev::login>  / act::set_state   = "logged-in"_s,
            "main"_s      + event<ev::quit>                      = X,
            // logged-in state
            "logged-in"_s + event<ev::logout> / act::set_state   = "main"_s,
            "logged-in"_s + event<ev::quit>                      = X
        );
        // clang-format on
    }
};

using Cli = boost::sml::sm<CliSm>;

} // namespace sm

} // namespace lmail
