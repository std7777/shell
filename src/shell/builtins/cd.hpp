#include <filesystem>
#include <iostream>
#include <string>

namespace shell::cd{
    int execute(const std::string& input){

        std::string command = input;
        if(command[0]=='~'){
            const char* home= std::getenv("HOME");
            if(home != nullptr){
                command.replace(0,1,home);
            }
        }
        try{
            // std::filesystem::path target = std::filesystem::absolute(command);
            std::filesystem::current_path(command);
        }catch(const std::filesystem::filesystem_error e){
            std::cerr << "cd: "<<command<<": No such file or directory" << std::endl;
        }
        return 0;
    }
}