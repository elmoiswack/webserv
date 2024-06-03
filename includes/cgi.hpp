#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

class Cgi
{
	public:
		bool			isCgi(const std::string &url);
		std::string		extractCgiPath(const std::string &url);
		void			runCgi(const std::string &cgi_path);
};
