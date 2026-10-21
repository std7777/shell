#pragma once

#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <optional>
#include <string>
#include <unistd.h>

namespace shell {
    inline void flush_stdout(){
        std::cout.flush();
        std::fflush(stdout);
    }

    // Builtins run in the shell process, so restore the stream when this scope ends.
    class OutputRedirection {
        int target_fd;
        int saved_fd = -1;

        void flush_stream(){
            if(target_fd == STDOUT_FILENO) flush_stdout();
            else{
                std::cerr.flush();
                std::fflush(stderr);
            }
        }
    public:
        explicit OutputRedirection(int descriptor) : target_fd(descriptor) {}
        OutputRedirection(const OutputRedirection&) = delete;
        OutputRedirection& operator=(const OutputRedirection&) = delete;

        bool apply(const std::optional<std::string>& filename, bool append = false){
            if(!filename) return true;
            flush_stream();
            saved_fd = dup(target_fd);
            if(saved_fd == -1){
                std::perror("dup");
                return false;
            }
            int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
            int file = open(filename->c_str(), flags, 0644);
            if(file == -1){
                std::perror(filename->c_str());
                return false;
            }
            bool success = dup2(file, target_fd) != -1;
            if(!success) std::perror("dup2");
            close(file);
            return success;
        }

        ~OutputRedirection(){
            if(saved_fd != -1){
                flush_stream();
                if(dup2(saved_fd, target_fd) == -1) std::perror("dup2");
                close(saved_fd);
            }
        }
    };
}
