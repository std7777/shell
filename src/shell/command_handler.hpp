#pragma once

#include <common/common.hpp>
#include <builtins/echo.hpp>
#include <builtins/exit.hpp>
#include <builtins/type.hpp>
#include <builtins/pwd.hpp>
#include <builtins/cd.hpp>

#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include <vector>

namespace shell{
    static std::unordered_map<std::string, std::function<int(const std::string&)>> string_to_command{
        {"echo", &echo::execute},
        {"exit",&exit::execute},
        {"type",&type::execute},
        {"pwd",&pwd::execute},
        {"cd",&cd::execute}
    };

    std::pair<std::string, std::string> extract_keyword_input(const std::string& command){
        size_t first_word_idx = command.find(' ');
        if(first_word_idx==std::string::npos){
            return {command, ""};
        }
        std::string keyword = command.substr(0,first_word_idx);
        std::string input = command.substr(first_word_idx+1);
        return {keyword,input};
    }

    int execute(const std::string& command){
        const auto [keyword,input] = extract_keyword_input(command);
        if(string_to_command.contains(keyword)){
            return string_to_command[keyword](input);
        }else if(!common::get_command_path(keyword).empty()){
            return std::system(command.c_str());
        }else{
            std::cout << command << ": command not found" <<std::endl;
            return 0;
        }
    }
    
    void repl(){
        while(true){
            std::cout << "$ ";
            std::string command;
            std::getline(std::cin, command);
            int exit_code = execute(command);
            if(exit_code!=0){
                break;
            }
        }
    }
}



// std::stringstream ss(command);
// int cnt = 0;
// std::string arg;
// std::vector<std::string> args;
// while(ss>>arg){
//     cnt++;
//     args.push_back(arg);
// }
// std::cout << "Program was passed "<<cnt<<" args (including program name)."<<std::endl;
// std::cout << "Arg #"<<0<<" (program name): "<<args[0]<<std::endl;
// for(int i=1;i<args.size();i++){
//     std::cout << "Arg #"<<i<<": "<<args[i]<<std::endl;
// }
// return 1;