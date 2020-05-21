#pragma once

#include "fsm.hpp"
#include "application.hpp"

namespace lmail
{

class CliState;
class Cli : public Fsm<CliState>
{
public:
    explicit Cli(Application::Conf const &conf);
    int run() noexcept;

private:
    Application::Conf conf_;
};

} // namespace lmail
