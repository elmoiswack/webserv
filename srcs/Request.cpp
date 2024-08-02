#include "../includes/Server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <chrono>
#include "../includes/Cgi.hpp"

void Server::EventsPollin(int fd, Client *client)
{
	logger("POLLIN");
	if (this->_donereading == false)
	{
		this->InitRequest(fd, client);
	}
	if (this->_donereading == true)
	{
		this->BuildResponse(client);
	}
}

void Server::InitRequest(int fd, Client *client)
{
	int ret = this->RecieveMessage(fd, client);
	if (ret == -1)
	{
		std::string errfile = this->HtmlToString(this->GetHardCPathCode(400), client);
		this->_response = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(errfile.length()) + "\r\n"
		"\r\n"
		+ errfile;
		this->_request.clear();
		return ;
	}
	else if (ret == 0)
	{
		this->_recvzero = true;
		this->_donereading = false;
		this->_request.clear();
		this->_iffirstread = true;
		return ;
	}
}

int Server::RecieveMessage(int fd, Client *client)
{
	logger("Ready to recieve...");
	char buff[client->Getrecvmax() + 1];
	
	int rbytes = recv(fd, &buff, client->Getrecvmax(), 0);
	std::cout << "Bytes recv = " << rbytes << std::endl;
	if (rbytes == -1)
	{
		logger("ERROR: RECV returned -1!");
		return (-1);
	}
	else if (rbytes == 0)
	{
		logger("RECV returned 0, connection closed!");
		return (0);
	}
	int index = 0;
	while (index < rbytes)
	{
		this->_request.push_back(buff[index]);
		index++;
	}
	buff[index] = '\0';
	this->IsFirstRead(client, buff);
	this->IsDoneRead(client, rbytes);
	logger("message recieved!");
	return (1);
}

void Server::IsFirstRead(Client *client, char *buff)
{
	if (this->_iffirstread == true)
	{
		this->_iffirstread = false;
		if (this->GetContentLenght(buff) != -1)
		{
			this->_isbody = true;
			client->SetContentLenght(this->GetContentLenght(buff));
			std::cout << "CLIENT CONTENT LEN = " << client->GetContentLenght() << std::endl;
		}
		else
		{
			this->_isbody = false;
		}
	}
}

long Server::GetContentLenght(char *buff)
{
	std::string tmp(buff);

	int begin = tmp.find("Content-Length:", 0);
	if ((size_t)begin == tmp.npos)
		return (-1);
	if (begin == -1)
		return (0);
	while (!std::isspace(tmp[begin]))
		begin++;
	begin++;
	int end = begin;
	while (std::isdigit(tmp[end]))
		end++;

	std::string numb = tmp.substr(begin, end - begin);
	long body = std::stol(numb);
	begin = tmp.find("Priority:", 0);
	while (tmp[begin] && tmp[begin] != '-')
		begin++;
	
	if ((size_t)begin == tmp.size())
	{
		logger("FOR FUCK SAKE!!!!!!!!!!!!!");
		exit(EXIT_FAILURE);
	}
	
	numb = tmp.substr(0, begin);
	std::string strhead = std::to_string(numb.size());
	long head = std::stol(strhead);
	return (head + body);
}

void Server::IsDoneRead(Client *client, int rbytes)
{
	if (this->_isbody == true && this->_request.size() == (size_t)client->GetContentLenght())
	{
		this->_donereading = true;
		this->_request.push_back('\0');
		logger("Done reading post");
	}
	else if (this->_isbody == false && rbytes < client->Getrecvmax())
	{
		this->_donereading = true;
		this->_request.push_back('\0');
		logger("Done reading get");
	}
}

std::string Server::ParseRequest(Client *client)
{
	std::vector<char>::iterator itfirst = this->_request.begin();
	logger("\n\nRequest after reading is done =");
	for (std::vector<char>::iterator print = this->_request.begin(); print != this->_request.end(); print++)
	{
		std::cout << *print;
	}
	std::cout << std::endl;
	char arr[7];
	int index = 0;
	if (std::isspace(*itfirst))
	{
		while (std::isspace(*itfirst))
			itfirst++;
	}
	if (itfirst == this->_request.end())
		return (this->HtmlToString(this->GetHardCPathCode(400), client));
	while ((itfirst != this->_request.end()) && (!std::isspace(*itfirst)) && (index < 7))
	{
		arr[index] = *itfirst;
		index++;
		itfirst++;
	}
	arr[index] = '\0';
	std::string method(arr);
	
	return (this->WhichMethod(client, method, itfirst));
}