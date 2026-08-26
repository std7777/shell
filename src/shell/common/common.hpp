#pragma once

#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <unistd.h>

namespace shell::common{
    static std::unordered_set<std::string> builtins {
            "echo",
            "exit",
            "type"
    };

    static std::vector<std::string> paths;

    void setup_paths(const std::string& path){

        std::stringstream ss(path);
        std::string current_path;
        while(getline(ss, current_path,':'))
        {
            if(std::filesystem::exists(current_path))
            {
                paths.push_back(current_path);
            }
        }
    }

    std::string get_command_path(const std::string& command){
        for(const std::string path : paths){
            const std::string full_path = path +"/"+command;
            if(std::filesystem::exists(full_path)
                && std::filesystem::is_regular_file(full_path)
                && access(full_path.c_str(), X_OK) == 0 ){
                    return full_path;
            }
        }
        return "";  
    }
}
