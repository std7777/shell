#pragma once

#include "command.hpp"
#include <builtins/echo.hpp>
#include <builtins/exit.hpp>
#include <builtins/type.hpp>
#include <builtins/pwd.hpp>
#include <builtins/cd.hpp>
#include <functional>
#include <unordered_map>

namespace shell {
    inline const std::unordered_map<std::string, std::function<int(const std::string&)>> string_to_command{
        {"echo", &echo::execute},
        {"exit", &exit::execute},
        {"type", &type::execute},
        {"pwd", &pwd::execute},
        {"cd", &cd::execute}
    };

    // Adapt parsed arguments to the existing builtin functions' string interface.
    inline std::string builtin_arguments(const Command& command){
        std::string arguments;
        for(size_t i = 1; i < command.arguments.size(); ++i){
            if(i > 1) arguments += ' ';
            arguments += command.arguments[i];
        }
        return arguments;
    }
}
