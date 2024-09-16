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
		std::string tmp(client->GetBeginRequest(),client->GetEndRequest());
		std::cout << "HAHAHJFSAHKJFAHKFAKJFSAn\n" << tmp << std::endl; 
		this->BuildResponse(client);
	}
}

void Server::InitRequest(int fd, Client *client)
{
	int ret = this->RecieveMessage(fd, client);
	if (ret == -1)
	{
		// std::string errfile = this->HtmlToString(this->GetHardCPathCode(400), client);
		// this->_response = 
		// "HTTP/1.1 200 OK\r\n"
		// "Content-Type: text/html\r\n"
		// "Content-Length: " + std::to_string(errfile.length()) + "\r\n"
		// "\r\n"
		// + errfile;
		// this->_request.clear();
		// this->_donereading = true;
		this->_recvzero = true;
		this->_donereading = false;
		client->ClearRequest();
		return ;
	}
	else if (ret == 0)
	{
		this->_recvzero = true;
		this->_donereading = false;
		client->ClearRequest();
		return ;
	}
}

int Server::RecieveMessage(int fd, Client *client)
{
	logger("Ready to recieve...");
	std::cout << "recvmax = " << client->Getrecvmax() << std::endl;
	char* buff = new char[client->Getrecvmax()];
	int rbytes = recv(fd, buff, client->Getrecvmax(), 0);
	std::cout << "Bytes recv = " << rbytes << std::endl;
	this->_totalread += rbytes;
	if (rbytes == -1)
	{
		logger("ERROR: RECV returned -1!");
		delete[] buff;
		return (-1);
	}
	else if (rbytes == 0)
	{
		logger("RECV returned 0, connection closed!");
		delete[] buff;
		return (0);
	}
	int index = 0;
	logger("message recieved!");
	while (index < rbytes)
	{
		client->PushToRequest(buff[index]);
		index++;
	}
	std::cout << "TOTAL READ = " << client->GetRequestSize() << std::endl;
	this->IsDoneRead(client);
	delete[] buff;
	return (1);
}

long Server::GetContentLenght(std::string buff)
{
	int begin = buff.find("Content-Length:", 0);
	if ((size_t)begin == buff.npos || begin == -1)
		return (-1);
	while (!std::isspace(buff[begin]))
		begin++;
	begin++;
	int end = begin;
	while (std::isdigit(buff[end]))
		end++;
	std::string numb = buff.substr(begin, end - begin);
	long body = std::stol(numb);
	if (body == 0)
		return (-2);
	
	begin = buff.find("Priority:", 0);
	while (buff[begin] && buff[begin] != '-')
		begin++;
	numb = buff.substr(0, begin);
	std::string strhead = std::to_string(numb.size());
	long head = std::stol(strhead);

	return (head + body);
}

std::string Server::WhichMethod(std::string buff)
{
	if (buff.find("GET", 0) != buff.npos)
		return ("GET");
	else if (buff.find("POST", 0) != buff.npos)
		return ("POST");
	else if (buff.find("DELETE", 0) != buff.npos)
		return ("DELETE");
	return ("EMPTY");
}

void Server::IsDoneRead(Client *client)
{
	std::string tmp(client->GetBeginRequest(), client->GetEndRequest());
	
	if (client->GetCurrentMethod() == "EMPTY")
	{
		client->SetCurrentMethod(this->WhichMethod(tmp));
		if (client->GetCurrentMethod() == "EMPTY")
			return ;
	}
	if ((tmp.find("\r\n\r\n", 0) != tmp.npos))
	{
		if (client->GetCurrentMethod() == "POST")
		{
			if (this->GetContentLenght(tmp) == -1)
			{
				return ;
			}
			if (client->GetContentLenght() == 0)
			{
				this->_isbody = true;
				client->SetContentLenght(this->GetContentLenght(tmp));
				std::cout << "CLIENT CONTENT LEN = " << client->GetContentLenght() << std::endl;
			}
			if (this->_isbody == true && client->GetRequestSize() == (ssize_t)client->GetContentLenght())
			{
				this->_donereading = true;
				client->PushToRequest('\0');

				// std::cout << "\nFULL REQUEST:" << std::endl;
				// for (size_t i = 0; i < this->_request.size(); i++)
				// {
				// 	std::cout << this->_request[i];
				// }
				logger("Done reading post");
			}
		}
		else if ((client->GetCurrentMethod() == "GET") || (client->GetCurrentMethod() == "DELETE"))
		{
			this->_donereading = true;
			client->PushToRequest('\0');
			// std::cout << "\nFULL REQUEST:" << std::endl;
			// for (size_t i = 0; i < this->_request.size(); i++)
			// {
			// 	std::cout << this->_request[i];
			// }
			logger("Done reading get");
		}
	}
}

std::string GetHost(std::string tmp)
{
	auto it = tmp.find("Host: ", 0);

	while (!std::isspace(tmp[it]))
		it++;
	it++;
	auto end = it;
	std::string host;
	while (tmp[end] != '\r')
	{
		host.push_back(tmp[end]);
		end++;
	}
	return (host);
}

std::string Server::ParseRequest(Client *client)
{
	std::vector<char>::iterator itfirst = client->GetBeginRequest();
	while ((itfirst != client->GetEndRequest()) && (!std::isspace(*itfirst)))
	{
		itfirst++;
	}
	if (itfirst == client->GetEndRequest())
		return (this->HtmlToString(this->GetHardCPathCode(400), client));
	std::string tmp(client->GetBeginRequest(), client->GetEndRequest());
	std::string hostreq = GetHost(tmp);
	std::string hostserv = client->GetServerName() + ":" + client->GetPort();
	if (hostreq != hostserv && hostreq != ("localhost:" + client->GetPort()))
		return (this->HtmlToString(this->GetHardCPathCode(400), client));
	return (this->WhichMethod(client, itfirst));
}