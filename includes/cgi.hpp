#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

class Cgi
{
	private:
		std::vector<std::string> 	m_cgi_env_vars;
		std::vector<char *> 		m_cgi_env_vars_cstyle;
	public:
		Cgi(char *client_resp);
		// bool						isCgi(const std::string &url);
		std::string					extractCgiPath(const std::string &url);
		std::string					readPipe(int fd);
		void						runCgi(const std::string &cgi_path);
		std::vector<std::string>	initCgiEnvVariables(const char *buf);
		std::vector<char *>  		initCgiEnvVariablesCstyle();
};
