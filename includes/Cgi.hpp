#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

class Cgi
{
	private:
		std::vector<std::string> 	m_cgi_env_vars;
		std::vector<char *> 		m_cgi_env_vars_cstyle;
	public:
		Cgi();
		Cgi(char *client_resp, const std::string &url);
		~Cgi();
		// bool						isCgi(const std::string &url);
		std::string					constructCgiPath(const std::string &url);
		std::string 				extractReqUrl(const std::string &url);
		std::string					extractQueryString(const std::string &url);
		std::string					extractContentType(const std::string &req);
		std::string					readPipe(int fd);
		std::string					runCgi(const std::string &cgi_path);
		std::vector<std::string>	initCgiEnvVars(const std::string &client_resp, const std::string &url);
		std::vector<char *>  		initCgiEnvVarsCstyle();
		void						setCgiEnvVars(const std::vector<std::string> &vars);
		void						setCgiEnvVarsCstyle(const std::vector<char *> &vars_cstyle);
};


bool isCgi(const std::string &url);
