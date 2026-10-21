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

    // Builtins run in the shell process, so restore stdout when this scope ends.
    class StdoutRedirection {
        int saved_stdout = -1;
    public:
        StdoutRedirection() = default;
        StdoutRedirection(const StdoutRedirection&) = delete;
        StdoutRedirection& operator=(const StdoutRedirection&) = delete;

        bool apply(const std::optional<std::string>& filename){
            if(!filename) return true;
            flush_stdout();
            saved_stdout = dup(STDOUT_FILENO);
            if(saved_stdout == -1){
                std::perror("dup");
                return false;
            }
            int file = open(filename->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if(file == -1){
                std::perror(filename->c_str());
                return false;
            }
            bool success = dup2(file, STDOUT_FILENO) != -1;
            if(!success) std::perror("dup2");
            close(file);
            return success;
        }

        ~StdoutRedirection(){
            if(saved_stdout != -1){
                flush_stdout();
                if(dup2(saved_stdout, STDOUT_FILENO) == -1) std::perror("dup2");
                close(saved_stdout);
            }
        }
    };
}
