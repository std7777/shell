#pragma once

#include "parser.hpp"
#include "executor.hpp"
#include "pipeline.hpp"
#include "builtin_dispatch.hpp"

namespace shell {
    inline ExecutionResult execute(const std::string& input){
        std::vector<Command> commands;
        try{
            commands = parse_pipeline(input);
        }catch(const std::runtime_error& error){
            std::cerr << "shell: " << error.what() << std::endl;
            return {2, false};
        }

        if(commands.size() > 1){
            return {execute_pipeline(commands), false};
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
        int status = builtin->second(builtin_arguments(command));
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
