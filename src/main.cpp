#include "../head/lib.h"


int main(int argc, char *argv[]) 
{
    if (argc < 2)
    {
        std::cerr << "Incorrect Usage, Use XComp --help" << std::endl;
        return EXIT_FAILURE;
    }
    else if (argc == 2)
    {
        if (std::strcmp(argv[1], "--help") == 0)
        {
            std::cout << "Build:\n\tXComp <filename>.x";
        }
        else
        {
            std::string entireFile = ReadEntireFile(argv[1]);     
            std::vector<Token> TokenVec = getTokens(entireFile);    
            for (int i = 0; i < TokenVec.size(); ++i)
            {
                std::cout << "Index: " << TokenVec[i].type << "\tValue: " << TokenVec[i].value << std::endl;
            }
        }
    }
    else
    {

    }
    return EXIT_SUCCESS;
}