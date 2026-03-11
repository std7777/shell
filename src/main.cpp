#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>

int main()
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (1)
    {
        std::cout << "$ ";
        std::string line;
        std::getline(std::cin, line);
        std::stringstream ss(line);
        std::string command;
        ss >> command;

        if (command == "exit")
        {
            break;
        }
        else if (command == "echo")
        {
            std::string word;
            while (ss >> word)
            {
                std::cout << word << " ";
            }
            std::cout << std::endl;
        }
        else if (command == "type")
        {
            std::string builtin[3] = {"echo", "exit", "type"};
            std::string argument;
            ss >> argument;
            bool found = false;

            for (int i = 0; i < builtin->length(); i++)
            {
                if (builtin[i] == argument)
                {
                    std::cout << argument << " is a shell builtin\n";
                    found = true;
                }
            }
            if (!found)
            {
                std::string path_env = std::getenv("PATH");
                std::stringstream ss_path(path_env);
                std::string path;
                while (std::getline(ss_path, path, ':'))
                {
                    std::string full_path = path + "/" + argument;
                    if (access(full_path.c_str(), X_OK) == 0)
                    {
                        std::cout << argument << " is " << full_path << "\n";
                        found = true;
                        break;
                    }
                }
            }
            if (!found)
            {
                std::cout << argument << ": not found\n";
            }
        }
        else
        {
            std::cout << line + ": command not found\n";
        }
    }
    return 0;
}
