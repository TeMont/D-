#pragma once
#include "../Token/token.hpp"
#include <thread>
#include <chrono>
#include <iostream>

std::stringstream TokensToAsm(std::vector<Token> TokensVec);

bool CreateObjectFile(std::string Path);

bool LinkObjectFiles(std::string Path);