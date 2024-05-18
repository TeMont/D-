#include "../headers/filesys.hpp"

std::string ReadSource(std::string Path)
{
	std::fstream file;
	file.open(Path);
	std::stringstream sstr;
	sstr << file.rdbuf();
	return sstr.str();
}

bool CheckFileExtension(std::string Path, std::string Extension)
{
	size_t dotPos = Path.find_last_of('.');
	if (dotPos == std::string::npos)
	{
		return false;
	}

	std::string fileExtension = Path.substr(dotPos + Extension.size() - 1);

	std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), [](unsigned char c)
	{
		return std::tolower(c);
	});

	return (fileExtension == Extension);
}