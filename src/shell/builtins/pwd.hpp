#include <filesystem>
#include <iostream>

namespace shell::pwd{
    int execute(const std::string& command){
        std::string curr_path= std::filesystem::current_path();
        std::cout << curr_path << std::endl;
        return 0;
    }
}
