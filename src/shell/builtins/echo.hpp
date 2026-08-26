#include <iostream>
#include <unistd.h>

namespace shell::echo{
    int execute(const std::string& command){
        std::cout << command << std::endl;
        return 0;
    }
}