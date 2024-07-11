#include "../includes/Server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

void Server::EventsPollin(int fd)
{
	logger("POLLIN");
	this->GetResponse(fd);
}
void Server::GetResponse(int fd)
{
	if (this->_donereading == false)
		this->RecieveMessage(fd);
	if (this->_donereading == true)
	{
		try
		{
			std::string htmlfile = this->ParseRequest();
			if (this->_iscgi == false)
			{
				this->_response = 
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " + std::to_string(htmlfile.length()) + "\r\n"
				"\r\n"
				+ htmlfile;
				this->_request.clear();
			}
			else if (this->_iscgi == true)
			{
				this->_response = htmlfile;
				this->_iscgi = false;
				this->_request.clear();
			}
			this->_donereading = false;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

	}
}

#include "../includes/Cgi.hpp"

std::string Server::ParseRequest()
{
	std::vector<char>::iterator itfirst = this->_request.begin();
	logger("\n\nRequest after reading is done =");
	for (std::vector<char>::iterator bruh = this->_request.begin(); bruh != this->_request.end(); bruh++)
	{
		std::cout << *bruh;
	}
	std::cout << std::endl;
	logger("\n\n");
	char arr[7];
	int index = 0;
	while (!std::isspace(*itfirst))
	{
		arr[index] = *itfirst;
		index++;
		itfirst++;
	}
	arr[index] = '\0';
	std::string method(arr);


	if (method == "GET")
	{
		this->_method = "GET";
		return (this->MethodGet(itfirst));
	}
	else if (method == "POST")
	{
		this->_method = "POST";
		std::string bvruhg = this->MethodPost(itfirst);
		if (bvruhg.size() == 0)
		{
			logger("BRUH FAILED POST");
			exit(EXIT_FAILURE);
		}
		return (bvruhg);
	}
	else if (method == "DELETE")
	{
		this->_method = "DELETE";
	}
	logger("\nMETHOD IS NOT ACCEPTED OR DOENS'T EXIST!\n");
	logger("sending client back to index.html\n");
	return (this->HtmlToString("./var/www/index.html"));
}

std::string Server::MethodPost(std::vector<char>::iterator itreq)
{
	while (std::isspace(*itreq))
		itreq++;
	std::vector<char>::iterator itend = itreq;
	while (!std::isspace(*itend))
		itend++;
	std::string path;
	path.assign(itreq, itend);
	logger(path);

	if (isCgi(path))
	{
		Cgi cgi;
		this->_iscgi = true;
		if (this->_response.size() > 0)
			this->_response.clear();
		std::string cgi_path = cgi.constructCgiPath(path);
		std::string tmp(this->_request.begin(), this->_request.end());
		cgi.setCgiEnvVars(cgi.initCgiEnvVars(tmp, path));
		cgi.setCgiEnvVarsCstyle(cgi.initCgiEnvVarsCstyle());
		this->_response = cgi.runCgi(cgi_path);
		std::cout << "\n--------------------------\n";
		std::cout << "RESPONSE: \n\n" << this->_response;
		std::cout << "--------------------------\n";
		return (this->_response);
	}
	return ("");
}

std::string Server::MethodGet(std::vector<char>::iterator itreq)
{
	while (std::isspace(*itreq))
		itreq++;
	std::vector<char>::iterator itend = itreq;
	while (!std::isspace(*itend))
		itend++;
	std::string path;
	path.assign(itreq, itend);
	logger(path);

	std::vector<Location>::iterator itloc = this->_locationblocks.begin();
	if (isCgi(path))
	{
		Cgi cgi;
		this->_iscgi = true;
		if (this->_response.size() > 0)
			this->_response.clear();
		std::string cgi_path = cgi.constructCgiPath(path);
		std::string tmp(this->_request.begin(), this->_request.end());
		cgi.setCgiEnvVars(cgi.initCgiEnvVars(tmp, path));
		cgi.setCgiEnvVarsCstyle(cgi.initCgiEnvVarsCstyle());
		this->_response = cgi.runCgi(cgi_path);
		std::cout << "\n--------------------------\n";
		std::cout << "RESPONSE: \n\n" << this->_response;
		std::cout << "--------------------------\n";
		return (this->_response);
	}
	if (path == "/" || path == itloc->GetIndex())
		return (this->HtmlToString("./var/www" + itloc->GetIndex()));
	else if (path.find("/status_codes/", 0) != path.npos)
		return (this->GetSatusCodeFile(path));
	else
		return (this->HtmlToString("./var/www/status_codes/404.html"));
}

std::string Server::GetSatusCodeFile(std::string path)
{
	// std::string::iterator begin = path.begin();
	// while (!std::isdigit(*begin))
	// 	begin++;
	// auto end = begin;
	// while (std::isdigit(*end))
	// 	end++;
	// std::string strcode(begin, end);
	// int code = std::stoi(strcode);
	// std::cout << "CODE = " << code << std::endl;
	
	// std::unordered_map<int, std::string>::iterator iterr = it->_error_page.begin();
	// while (iterr != it->_error_page.end() && iterr->first != code)
	// {
	// 	std::cout << iterr->first << std::endl;
	// 	iterr++;
	// }
	// if (iterr == it->_error_page.end())
	// {
	// 	std::cout << "ficledsajda" << std::endl;
	// 	exit(1);
	// }
	// std::cout << "itersecond = " << iterr->second << std::endl;
	// std::string statuscode = "./var/www" + code;
	// return (it->HtmlToString(statuscode));

	std::string statuscode = "./var/www" + path;
	return (this->HtmlToString(statuscode));
}

std::string Server::HtmlToString(std::string path)
{
	if (access(path.c_str(), F_OK | R_OK) == -1)
	{
		logger("DENIED ACCES htmltostring");
		exit(EXIT_FAILURE);
	}	
	std::ifstream file(path, std::ios::binary);
	if (!file.good())
	{
		std::cout << "Failed to read file!\n" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

void Server::RecieveMessage(int fd)
{
	logger("Ready to recieve...");
	std::cout << "maxrecv = " << this->_recvmax << std::endl;
	char buff[this->_recvmax];
	int rbytes = recv(fd, &buff, this->_recvmax, 0);
	if (rbytes == -1)
	{
		std::cout << "ERROR read" << std::endl;
		exit(EXIT_FAILURE);
	}
	logger("request:");
	for (int i = 0; i < rbytes; i++)
	{
		std::cout << buff[i];
		this->_request.push_back(buff[i]);
	}
	std::cout << std::endl;
	std::cout << "Bytes recv = " << rbytes << std::endl;
	if (rbytes < this->_recvmax)
	{
		this->_donereading = true;
		this->_request.push_back('\0');
	}
	logger("message recieved!");
}