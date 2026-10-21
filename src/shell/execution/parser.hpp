#pragma once

#include "command.hpp"
#include <cctype>
#include <stdexcept>

namespace shell {
    inline Command parse_command(const std::string& input){
        Command command;
        std::string word;
        bool word_started = false;
        bool plain_word = true;
        bool expecting_filename = false;
        bool redirect_stderr = false;
        char quote = '\0';

        auto finish_word = [&] {
            if(!word_started) return;
            if(expecting_filename){
                if(redirect_stderr) command.stderr_file = word;
                else command.stdout_file = word;
                expecting_filename = false;
            }else{
                command.arguments.push_back(word);
            }
            word.clear();
            word_started = false;
            plain_word = true;
        };

        for(size_t i = 0; i < input.size(); ++i){
            char ch = input[i];
            if(quote == '\''){
                if(ch == '\'') quote = '\0';
                else word += ch;
            }else if(ch == '\\'){
                if(i + 1 == input.size()) throw std::runtime_error("unfinished escape");
                char next = input[i + 1];
                if(quote == '"' && next != '"' && next != '\\'
                    && next != '$' && next != '`' && next != '\n'){
                    word += ch;
                }else{
                    ++i;
                    if(next != '\n') word += next;
                }
                word_started = true;
                plain_word = false;
            }else if(quote == '"'){
                if(ch == '"') quote = '\0';
                else word += ch;
            }else if(ch == '\'' || ch == '"'){
                quote = ch;
                word_started = true;
                plain_word = false;
            }else if(std::isspace(static_cast<unsigned char>(ch))){
                finish_word();
            }else if(ch == '>'){
                if(expecting_filename) throw std::runtime_error("missing redirection filename");
                // Only an adjacent, unquoted descriptor selects the stream.
                redirect_stderr = plain_word && word_started && word == "2";
                if(plain_word && word_started && (word == "1" || word == "2")){
                    word.clear();
                    word_started = false;
                }else{
                    finish_word();
                }
                bool append = i + 1 < input.size() && input[i + 1] == '>';
                if(append){
                    ++i;
                }
                if(redirect_stderr) command.stderr_append = append;
                else command.stdout_append = append;
                expecting_filename = true;
            }else{
                word += ch;
                word_started = true;
            }
        }
        if(quote != '\0') throw std::runtime_error("unclosed quote");
        finish_word();
        if(expecting_filename) throw std::runtime_error("missing redirection filename");
        return command;
    }
}
