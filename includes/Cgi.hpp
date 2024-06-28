#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


/**
 * uploadPipe:
 * - used to send data (POST) from parent to child (CGI)
 * - parent writes to pipe to send data to cgi
 * - child reads from pipe, the STDIN is redirected to read end of pipe
 * ----------------------------------------------------------------
 * responsePipe:
 * - used to capture the output of the child and send it back to parent
 * - child writes to pipe, the STDOUT is redirected to write end of pipe
 * - parent reads the CGI output from read end of this pipe
 */
class Cgi
{
	private:
		std::vector<std::string> 	_cgiEnvVars;
		std::vector<char *> 		_cgiEnvVarsCstyle;
		int 						_uploadPipe[2];
		int 						_responsePipe[2];	
		pid_t						_pid;
		void						_initPipes();
	public:
		Cgi();
		// Cgi(char *client_resp, const std::string &url);
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
