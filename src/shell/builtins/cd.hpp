#include <filesystem>
#include <iostream>

namespace shell::cd{
    int execute(const std::string& command){
        try{
            std::filesystem::current_path(command);
        }catch(const std::filesystem::filesystem_error e){
            std::cerr << "cd: "<<command<<": No such file or directory" << std::endl;
        }
        return 0;
    }
}