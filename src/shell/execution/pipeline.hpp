#pragma once

#include "executor.hpp"
#include <signal.h>

namespace shell {
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
            run_external_child(left);
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
            run_external_child(right);
        }

        // The parent must hold neither end, allowing EOF and SIGPIPE to propagate.
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        wait_for_child(producer);
        return wait_for_child(consumer);
    }
}
