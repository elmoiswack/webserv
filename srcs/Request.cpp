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
	if (client->GetDonereading() == false)
	{
		this->InitRequest(fd, client);
	}
	if (client->GetDonereading() == true && client->GetResponseSize() == 0)
	{
		this->BuildResponse(client);
	}
}

void Server::InitRequest(int fd, Client *client)
{
	int ret = this->RecieveMessage(fd, client);
	if (ret == -1 || ret == 0)
	{
		this->_recvzero = true;
		client->SetDonereading(false);
		client->ClearRequest();
		return ;
	}
}

int Server::RecieveMessage(int fd, Client *client)
{
	logger("Ready to recieve...");
	char* buff = new char[client->GetRecvSize()];
	int rbytes = recv(fd, buff, client->GetRecvSize(), 0);
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
	while (index < rbytes)
	{
		client->PushToRequest(buff[index]);
		index++;
	}
	std::cout << "Total recv = " << client->GetRequestSize() << std::endl;
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
	if ((std::size_t)begin == buff.npos)
	{
		begin = buff.find_last_of("\r\n");
		if ((std::size_t)begin == buff.npos)
			return (-1);
	}
	else
	{
		while (buff[begin] && buff[begin] != '-')
			begin++;
	}
	numb = buff.substr(0, begin);
	std::string strhead = std::to_string(numb.size());
	long head = std::stol(strhead);

	return (head + body);
}

void Server::IsDoneRead(Client *client)
{
	if (client->GetRecvMax() < client->GetRequestSize())
	{
		client->SetStatusCode(413);
		std::string html = (this->HtmlToString(this->GetHardCPathCode(413, client), client));
		this->BuildResponseCode(client, html);
		client->SetDonereading(true);
		return ;
	}
	std::string tmp(client->GetBeginRequest(), client->GetEndRequest());
	if (client->GetCurrentMethod() == "EMPTY")
	{
		if (client->GetRequestSize() > 10)
		{
			client->SetCurrentMethod(this->WhichMethod(tmp));
		}
	}
	if ((tmp.find("\r\n\r\n", 0) != tmp.npos))
	{
		if (client->GetCurrentMethod() == "POST")
		{
			if (this->GetContentLenght(tmp) == -1)
				return ;
			if (client->GetContentLenght() == 0)
			{
				this->_isbody = true;
				client->SetContentLenght(this->GetContentLenght(tmp));
			}
			if (this->_isbody == true && client->GetRequestSize() >= (ssize_t)client->GetContentLenght())
			{
				client->SetDonereading(true);
				client->PushToRequest('\0');
				logger("Done reading post!");
			}
		}
		else if ((client->GetCurrentMethod() == "GET") || (client->GetCurrentMethod() == "DELETE"))
		{
			client->SetDonereading(true);
			client->PushToRequest('\0');
			if (client->GetCurrentMethod() == "GET")
				logger("Done reading get!");
			else
				logger("Done reading delete!");
		}
		else if (client->GetCurrentMethod() == "NOTIMPLEMENTED")
		{
			client->SetDonereading(true);
			client->PushToRequest('\0');
		}
	}
}

std::string Server::WhichMethod(std::string buff)
{
	if (buff.find("GET", 0) != buff.npos)
		return ("GET");
	else if (buff.find("POST", 0) != buff.npos)
		return ("POST");
	else if (buff.find("DELETE", 0) != buff.npos)
		return ("DELETE");
	return ("NOTIMPLEMENTED");
}


std::string Server::GetHost(std::string tmp)
{
	auto it = tmp.find("Host: ", 0);
	if (it == tmp.npos)
		return ("EMPTY");
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
	if (client->GetCurrentMethod() == "NOTIMPLEMENTED")
	{
		logger("Current method isn't implemented!");
		return (this->HtmlToString(this->GetHardCPathCode(501, client), client));	
	}
	std::string tmp(client->GetBeginRequest(), client->GetEndRequest());
	std::string hostreq = this->GetHost(tmp);
	if (hostreq == "EMPTY")
		return (this->HtmlToString(this->GetHardCPathCode(400, client), client));
	std::string hostserv = client->GetServerName() + ":" + client->GetPort();
	if (hostreq != hostserv && hostreq != ("localhost:" + client->GetPort()) && hostreq != ("127.0.0.1:" + client->GetPort()))
		return (this->HtmlToString(this->GetHardCPathCode(400, client), client));
	return (this->WhichMethod(client));
}