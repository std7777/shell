#pragma once

#include "executor.hpp"
#include "builtin_dispatch.hpp"
#include <signal.h>

namespace shell {
    [[noreturn]] inline void run_pipeline_child(const Command& command){
        auto builtin = string_to_command.find(command.arguments.front());
        if(builtin == string_to_command.end()) run_external_child(command);

        OutputRedirection stdout_redirect(STDOUT_FILENO);
        OutputRedirection stderr_redirect(STDERR_FILENO);
        if(!stdout_redirect.apply(command.stdout_file, command.stdout_append)
            || !stderr_redirect.apply(command.stderr_file, command.stderr_append)) _exit(1);

        int status = builtin->second(builtin_arguments(command));
        // _exit does not flush C or C++ output buffers automatically.
        flush_stdout();
        std::cerr.flush();
        std::fflush(stderr);
        // A pipeline's exit builtin ends only this child, leaving the REPL alive.
        _exit(command.arguments.front() == "exit" ? 0 : status);
    }

    inline int execute_pipeline(const std::vector<Command>& commands){
        std::vector<pid_t> children;
        children.reserve(commands.size());
        int previous_read = -1;

        // If setup fails halfway through, stop and reap children already started.
        auto stop_children = [&] {
            if(previous_read != -1) close(previous_read);
            for(pid_t child : children) kill(child, SIGKILL);
            for(pid_t child : children) wait_for_child(child);
        };

        flush_stdout();
        std::cerr.flush();
        for(size_t i = 0; i < commands.size(); ++i){
            bool has_next = i + 1 < commands.size();
            int next_pipe[2] = {-1, -1};
            if(has_next && pipe(next_pipe) == -1){
                std::perror("pipe");
                stop_children();
                return 1;
            }

            pid_t child = fork();
            if(child == -1){
                std::perror("fork");
                if(has_next){
                    close(next_pipe[0]);
                    close(next_pipe[1]);
                }
                stop_children();
                return 1;
            }
            if(child == 0){
                if(previous_read != -1 && dup2(previous_read, STDIN_FILENO) == -1){
                    std::perror("dup2");
                    _exit(1);
                }
                if(has_next && dup2(next_pipe[1], STDOUT_FILENO) == -1){
                    std::perror("dup2");
                    _exit(1);
                }
                if(previous_read != -1) close(previous_read);
                if(has_next){
                    close(next_pipe[0]);
                    close(next_pipe[1]);
                }
                run_pipeline_child(commands[i]);
            }

            children.push_back(child);
            // Keep only the read end needed to launch the next command.
            if(previous_read != -1) close(previous_read);
            if(has_next) close(next_pipe[1]);
            previous_read = next_pipe[0];
        }

        // Every stage must be running before we wait, so full pipes cannot stall setup.
        int status = 0;
        for(pid_t child : children) status = wait_for_child(child);
        return status;
    }
}
