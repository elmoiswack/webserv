#include "../includes/Server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

void Server::EventsPollin(int fd, std::vector<Server>::iterator it)
{
	logger("POLLIN");
	this->GetResponse(fd, it);
}
void Server::GetResponse(int fd, std::vector<Server>::iterator it)
{
	if (it->_donereading == false)
		this->RecieveMessage(fd, it);
	if (it->_donereading == true)
	{
		try
		{
			std::string htmlfile = this->ParseRequest(it);
			if (it->_iscgi == false)
			{
				it->_response = 
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " + std::to_string(htmlfile.length()) + "\r\n"
				"\r\n"
				+ htmlfile;
				it->_request.clear();
			}
			else if (it->_iscgi == true)
			{

				it->_response = htmlfile;
				it->_iscgi = false;
				it->_request.clear();
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

std::string Server::ParseRequest(std::vector<Server>::iterator it)
{
	std::vector<char>::iterator itfirst = it->_request.begin();
	logger("\n\nRequest after reading is done =");
	for (std::vector<char>::iterator bruh = it->_request.begin(); bruh != it->_request.end(); bruh++)
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
	
	std::vector<Location> bruh = it->GetLocations();
	std::vector<Location>::iterator itbruh = bruh.begin();
	std::cout << "hello bruh index = " << itbruh->GetIndex() << std::endl;

	if (method == "GET")
	{
		it->_method = "GET";
		return (this->MethodGet(itfirst, it));
	}
	else if (method == "POST")
	{
		it->_method = "POST";
		std::string bvruhg = this->MethodPost(itfirst, it);
		if (bvruhg.size() == 0)
		{
			logger("BRUH FAILED POST");
			exit(EXIT_FAILURE);
		}
		return (bvruhg);
	}
	else if (method == "DELETE")
	{
		it->_method = "DELETE";
	}
	logger("\nMETHOD IS NOT ACCEPTED OR DOENS'T EXIST!\n");
	logger("sending client back to index.html\n");
	return (it->HtmlToString("./var/www/index.html"));
}

std::string Server::MethodPost(std::vector<char>::iterator itreq, std::vector<Server>::iterator it)
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
		it->_iscgi = true;
		if (it->_response.size() > 0)
			it->_response.clear();
		//std::string req_url = cgi.extractReqUrl(path);
		std::string cgi_path = cgi.constructCgiPath(path);
		std::string tmp(it->_request.begin(), it->_request.end());
		cgi.setCgiEnvVars(cgi.initCgiEnvVars(tmp, path));
		cgi.setCgiEnvVarsCstyle(cgi.initCgiEnvVarsCstyle());
		//std::cout << "\nREQUEST URL: " << req_url << "\n";
		//std::cout << "\nCGI PATH: " << cgi_path << "\n\n";
		// std::cout << "\n---QUERY_STRING: " << cgi.extractQueryString(req_url) << "\n\n\n";
		it->_response = cgi.runCgi(cgi_path);
			
		std::cout << "\n--------------------------\n";
		std::cout << "RESPONSE: \n\n" << it->_response;
		std::cout << "--------------------------\n";
		return (it->_response);
	}
	return ("");
}

std::string Server::MethodGet(std::vector<char>::iterator itreq, std::vector<Server>::iterator it)
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
			it->_iscgi = true;
			if (it->_response.size() > 0)
				it->_response.clear();
			//std::string req_url = cgi.extractReqUrl(path);
			std::string cgi_path = cgi.constructCgiPath(path);
			std::string tmp(it->_request.begin(), it->_request.end());
			cgi.setCgiEnvVars(cgi.initCgiEnvVars(tmp, path));
			cgi.setCgiEnvVarsCstyle(cgi.initCgiEnvVarsCstyle());
			//std::cout << "\nREQUEST URL: " << req_url << "\n";
			//std::cout << "\nCGI PATH: " << cgi_path << "\n\n";
			// std::cout << "\n---QUERY_STRING: " << cgi.extractQueryString(req_url) << "\n\n\n";
			it->_response = cgi.runCgi(cgi_path);

			std::cout << "\n--------------------------\n";
			std::cout << "RESPONSE: \n\n" << it->_response;
			std::cout << "--------------------------\n";
			return (it->_response);
		}

	else if (path == "/" || path == "/index.html")
		return (it->HtmlToString("./var/www/index.html"));
	else if (path.find("/status_codes/", 0) != path.npos)		
		return (it->GetSatusCodeFile(path));
	else
		return (it->HtmlToString("./var/www/status_codes/404.html"));
}

std::string Server::GetSatusCodeFile(std::string code)
{
	std::string statuscode = "./var/www" + code;
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

void Server::RecieveMessage(int fd, std::vector<Server>::iterator it)
{
	logger("Ready to recieve...");
	char buff[it->_recvmax];
	int rbytes = recv(fd, &buff, it->_recvmax, 0);
	if (rbytes == -1)
	{
		std::cout << "ERROR read" << std::endl;
		exit(EXIT_FAILURE);
	}
	logger("request:");
	for (int i = 0; i < rbytes; i++)
	{
		std::cout << buff[i];
		it->_request.push_back(buff[i]);
	}
	std::cout << std::endl;
	std::cout << "Bytes recv = " << rbytes << std::endl;
	if (rbytes < it->_recvmax)
	{
		it->_donereading = true;
		it->_request.push_back('\0');
	}
	logger("message recieved!");
}