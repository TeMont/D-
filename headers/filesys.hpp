#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

std::string ReadEntireFile(std::string Path);

bool CheckFileExtension(std::string Path, std::string Extension);