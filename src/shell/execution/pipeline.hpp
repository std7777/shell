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

    inline int execute_pipeline(const Command& left, const Command& right){
        int pipe_fds[2];
        if(pipe(pipe_fds) == -1){
            std::perror("pipe");
            return 1;
        }

        flush_stdout();
        std::cerr.flush();
        pid_t producer = fork();
        if(producer == -1){
            std::perror("fork");
            close(pipe_fds[0]);
            close(pipe_fds[1]);
            return 1;
        }
        if(producer == 0){
            if(dup2(pipe_fds[1], STDOUT_FILENO) == -1){
                std::perror("dup2");
                _exit(1);
            }
            close(pipe_fds[0]);
            close(pipe_fds[1]);
            run_pipeline_child(left);
        }

        // Start the reader before waiting: the writer may fill the pipe buffer.
        pid_t consumer = fork();
        if(consumer == -1){
            std::perror("fork");
            close(pipe_fds[0]);
            close(pipe_fds[1]);
            kill(producer, SIGKILL);
            wait_for_child(producer);
            return 1;
        }
        if(consumer == 0){
            if(dup2(pipe_fds[0], STDIN_FILENO) == -1){
                std::perror("dup2");
                _exit(1);
            }
            close(pipe_fds[0]);
            close(pipe_fds[1]);
            run_pipeline_child(right);
        }

        // The parent must hold neither end, allowing EOF and SIGPIPE to propagate.
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        wait_for_child(producer);
        return wait_for_child(consumer);
    }
}
