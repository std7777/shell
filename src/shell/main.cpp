#include <command_handler.hpp>
#include <common/common.hpp>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]){
    std::cout<<std::unitbuf;
    std::cerr<<std::unitbuf;

    const std::string PATH = std::getenv("PATH");
    // std::cerr << PATH<<std::endl;
    shell::common::setup_paths(PATH);
    shell::repl();
}