#pragma once

#include "application.hpp"
#include "cli_sm.hpp"

namespace lmail
{

class Cli final
{
public:
    explicit Cli(Application::Conf const &conf);
    void run();

private:
    Application::Conf conf_;
    sm::CliSmCtx ctx_;
    sm::Cli fsm_;
};

} // namespace lmail
