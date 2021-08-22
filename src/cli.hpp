#pragma once

#include "application.hpp"

namespace lmail
{

class Cli final
{
public:
    explicit Cli(Application::Conf const &conf);
    void run();

private:
    Application::Conf conf_;
};

} // namespace lmail
