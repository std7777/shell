#pragma once

#include <optional>
#include <string>
#include <vector>

namespace shell {
    struct Command {
        std::vector<std::string> arguments;
        std::optional<std::string> stdout_file;
    };

    struct ExecutionResult {
        int status = 0;
        bool should_exit = false;
    };
}
