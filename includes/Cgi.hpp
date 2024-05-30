#pragma once

#include <iostream>
#include <string>
#include <unistd.h>

bool			isCgi(const std::string &url);
std::string		extractCgiPath(const std::string &url);
void			runCgi(const std::string &cgi_path);
