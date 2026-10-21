#pragma once

#include "parser.hpp"
#include "executor.hpp"
#include "pipeline.hpp"
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

    inline ExecutionResult execute(const std::string& input){
        std::vector<Command> commands;
        try{
            commands = parse_pipeline(input);
            if(commands.size() > 2){
                throw std::runtime_error("only two-command pipelines are supported yet");
            }
        }catch(const std::runtime_error& error){
            std::cerr << "shell: " << error.what() << std::endl;
            return {2, false};
        }

        if(commands.size() == 2){
            return {execute_pipeline(commands[0], commands[1]), false};
        }
        const Command& command = commands.front();

        if(command.arguments.empty()){
            OutputRedirection stdout_redirect(STDOUT_FILENO);
            OutputRedirection stderr_redirect(STDERR_FILENO);
            return {stdout_redirect.apply(command.stdout_file, command.stdout_append)
                && stderr_redirect.apply(command.stderr_file, command.stderr_append) ? 0 : 1, false};
        }
        auto builtin = string_to_command.find(command.arguments.front());
        if(builtin == string_to_command.end()){
            return {execute_external(command), false};
        }

        OutputRedirection stdout_redirect(STDOUT_FILENO);
        OutputRedirection stderr_redirect(STDERR_FILENO);
        if(!stdout_redirect.apply(command.stdout_file, command.stdout_append)
            || !stderr_redirect.apply(command.stderr_file, command.stderr_append)) return {1, false};
        // Preserve the existing builtin interface while passing parsed arguments.
        std::string arguments;
        for(size_t i = 1; i < command.arguments.size(); ++i){
            if(i > 1) arguments += ' ';
            arguments += command.arguments[i];
        }
        int status = builtin->second(arguments);
        bool should_exit = command.arguments.front() == "exit";
        return {should_exit ? 0 : status, should_exit};
    }

    inline void repl(){
        while(true){
            std::cout << "$ ";
            std::string input;
            if(!std::getline(std::cin, input)) break;
            if(execute(input).should_exit) break;
        }
    }
}
