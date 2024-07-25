#pragma once

// #include "../includes/Server.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

class Server;


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
		std::string 				_method;
		std::string 				_response;
		std::string 				_postData;
		std::vector<std::string> 	_cgiEnvVars;
		pid_t						_pid;
		int 						_uploadPipe[2];
		int 						_responsePipe[2];	
		void						_initPipes();
	public:
		std::vector<char *> 		_cgiEnvVarsCstyle;
		Cgi();
		Cgi(const std::string &method, const std::string &post_data, const std::string &path, const std::string &request);
        Cgi(const std::string &method, const std::string &path, const std::string &request);
		Cgi(const Cgi& obj);
		Cgi& operator=(const Cgi& obj);
		// Cgi(char *client_resp, const std::string &url);
		~Cgi();
		// bool						isCgi(const std::string &url);
		std::string					constructCgiPath(const std::string &url);
		std::string 				extractReqUrl(const std::string &url);
		std::string					extractQueryString(const std::string &url);
		std::string					extractContentType(const std::string &req);
		// std::string					readCgiResponse(int fd);
		// void 						writeToCgi(int fd);
		std::string					runCgi(const std::string &cgi_path, Server *self);
		// void						runCgi(const std::string &cgi_path, Server *self);
		std::vector<std::string>	initCgiEnvVars(const std::string &client_resp, const std::string &url);
		std::vector<char *>  		initCgiEnvVarsCstyle();
		void						setPostData(const std::string &post_data);
		void						setCgiEnvVars(const std::vector<std::string> &vars);
		void						setCgiEnvVarsCstyle(const std::vector<char *> &vars_cstyle);
		void						setMethod(const std::string &method);
		std::string					getMethod();
		std::string					getPos(const std::string &url);
		int 						getReadEndResponsePipe() const;
		int 						getWriteEndUploadPipe() const;
		int 						getReadEndUploadPipe() const;
		bool 						waitForChild() const;
		void 						appendResponse(std::string responseToAdd);


};


bool isCgi(const std::string &url);
