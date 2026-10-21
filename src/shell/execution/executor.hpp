#pragma once

#include "command.hpp"
#include "redirection.hpp"
#include <common/common.hpp>
#include <cerrno>
#include <sys/wait.h>

namespace shell {
    inline int execute_external(const Command& command){
        const std::string& name = command.arguments.front();
        std::string path = name.find('/') != std::string::npos
            ? name : common::get_command_path(name);
        std::vector<char*> arguments;
        for(const auto& argument : command.arguments){
            arguments.push_back(const_cast<char*>(argument.c_str()));
        }
        arguments.push_back(nullptr);

        flush_stdout();
        std::cerr.flush();
        pid_t child = fork();
        if(child == -1){
            std::perror("fork");
            return 1;
        }
        if(child == 0){
            OutputRedirection stdout_redirect(STDOUT_FILENO);
            OutputRedirection stderr_redirect(STDERR_FILENO);
            if(!stdout_redirect.apply(command.stdout_file, command.stdout_append)
                || !stderr_redirect.apply(command.stderr_file)) _exit(1);
            if(path.empty()){
                std::cerr << name << ": command not found" << std::endl;
                _exit(127);
            }
            execv(path.c_str(), arguments.data());
            std::perror(name.c_str());
            _exit(126);
        }

        int status;
        while(waitpid(child, &status, 0) == -1){
            if(errno == EINTR) continue;
            std::perror("waitpid");
            return 1;
        }
        if(WIFEXITED(status)) return WEXITSTATUS(status);
        if(WIFSIGNALED(status)) return 128 + WTERMSIG(status);
        return 1;
    }
}
