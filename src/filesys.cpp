#include "../head/filesys.h"

std::string ReadEntireFile(std::string Path)
{
    std::fstream file;
    file.open(Path);
    std::stringstream sstr;
    sstr << file.rdbuf();
    return sstr.str();
}