#include "../includes/Server.hpp"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

Server::Server(Parser &in)
{
	this->_ip = in.GetIp();
	this->_ports.push_back(in.GetPort());
	this->_server_name = in.GetServName();
	this->_ammount_sock = 0;
	this->_htmlstartsend = false;
}

Server::~Server()
{
	this->_ip.clear();
	this->_server_name.clear();
	this->_sockvec.clear();
	this->_whatsockvec.clear();
	this->_ports.clear();
}

void Server::AddSocket(int fd, bool is_client)
{
	pollfd temp;
	temp.fd = fd;
	temp.events = POLLIN | POLLOUT;
	temp.revents = 0;
	this->_sockvec.push_back(temp);
	if (is_client == true)
	{
		this->_htmlstartsend = false;
		this->_whatsockvec.push_back("CLIENT");
	}
	else
		this->_whatsockvec.push_back("SERVER");
	this->_ammount_sock += 1;
}

void Server::RmvSocket(int index)
{
	std::vector<struct pollfd>::iterator it = this->_sockvec.begin();
	for (int i = 0; i != index; i++)
	{
		it++;
	}
	close(this->_sockvec[index].fd);
	this->_sockvec.erase(it);
	std::vector<std::string>::iterator jt = this->_whatsockvec.begin();
	for (int j = 0; j != index; j++)
	{
		jt++;
	}
	this->_whatsockvec.erase(jt);
}

//https://localhost:8080/ our address
void Server::SetUpServer()
{
	this->InitSocket();
	this->BindSockets();
	this->ListenSockets();
	logger("Server is initialized!");
	this->_server_running = true;
	this->RunPoll();
	this->CloseAllFds();
}

void Server::InitSocket()
{
	std::vector<std::string>::iterator it;
	for (it = this->_ports.begin(); it != this->_ports.end(); it++)
	{
		int websock = socket(AF_INET, SOCK_STREAM, 0);
		if (websock < 0)
		{
			std::cout << "ERROR SOCKET" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (fcntl(websock, F_SETFL, O_NONBLOCK) == -1)
		{
			std::cout << "ERROR FCNTL" << std::endl;
			exit(EXIT_FAILURE);	
		}
		this->AddSocket(websock, false);
	}
}

void Server::BindSockets()
{
	std::vector<std::string>::iterator it;
	struct sockaddr_in infoaddr;
	int index = 0;
	for (it = this->_ports.begin(); it != this->_ports.end(); it++)
	{
		std::string tmp = *it;
		infoaddr.sin_family = AF_INET;
		infoaddr.sin_addr.s_addr = INADDR_ANY;
		infoaddr.sin_port = htons(std::atoi(tmp.c_str()));
		if (bind(this->_sockvec[index].fd, reinterpret_cast<struct sockaddr *>(&infoaddr), sizeof(infoaddr)) == -1)
		{
			std::cout << "ERROR bruh" << std::endl;
			exit(EXIT_FAILURE);
		}
		index++;
	}
}

void Server::ListenSockets()
{
	std::vector<std::string>::iterator it;
	int index = 0;
	for (it = this->_ports.begin(); it != this->_ports.end(); it++)
	{
		if (listen(this->_sockvec[index].fd, 5) == -1)
		{
			std::cout << "ERROR LISTEN" << std::endl;
			exit(EXIT_FAILURE);		
		}
		index++;
	}	
}

void Server::RunPoll()
{
	while (this->_server_running == true)
	{
		int ret = poll(this->_sockvec.data(), this->_sockvec.size(), -1);
		if (ret < 0)
		{
			std::cout << "ERROR POLL" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (ret > 0)
			this->PollEvents();
	}	
}

void Server::PollEvents()
{
	for (int index = 0; index != this->_ammount_sock; index++)
	{
		pollfd temp;
		temp.fd = this->_sockvec[index].fd;
		temp.events = this->_sockvec[index].events;
		temp.revents = this->_sockvec[index].revents;
		if (temp.revents == POLLIN)
		{
			this->EventsPollin(temp.fd, index);
		}
		if (temp.revents == POLLOUT)
		{
			this->EventsPollout(temp.fd, index);
		}
		if (temp.revents == POLLHUP)
		{
			logger("Connection hung up!");
			close(temp.fd);
			this->RmvSocket(index);
		}
		if (temp.revents == POLLERR)
		{
			logger("Fuck error poll");
			exit(EXIT_FAILURE);
		}
	}	
}

void Server::EventsPollin(int fd, int index)
{
	logger("POLLIN");
	logger("action pending...");
	if (this->_whatsockvec[index] == "SERVER")
	{
		this->AcceptClient(index);
	}
	else if (this->_whatsockvec[index] == "CLIENT")
	{
		this->RecieveMessage(fd, index);

	}
}

void Server::AcceptClient(int index)
{
	logger("attempting connection...");
	int newsock = accept(this->_sockvec[index].fd, NULL, NULL);
	if (newsock == -1)
	{
		std::cout << "ERROR ACCEPT" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (fcntl(newsock, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "ERROR FCNTL" << std::endl;
		exit(EXIT_FAILURE);	
	}
	this->AddSocket(newsock, true);
	logger("Connection is accepted!");
}

void Server::RecieveMessage(int fd, int index)
{
	logger("NOT WORKING");
	logger("Ready to recieve...");
	if (index != 0)
		index = 0;

	char buff[1024];
	int nbytes = read(fd, buff, sizeof(buff));
	if (nbytes == -1)
	{
		std::cout << "ERROR read" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string temp(buff);
	this->_response + temp;
	logger("message recieved!");
}

#include<fstream>
#include<string>
#include <sstream>

std::string Server::HtmlToString(std::string path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file.good())
	{
		std::cout << "Failed to read file!\n";
		std::exit(EXIT_FAILURE);
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

std::string Server::GetResponse()
{
	return ("empty");
}

void Server::EventsPollout(int fd, int index)
{
	logger("POLLOUT");
	if (this->_whatsockvec[index] == "CLIENT")
	{
		logger("sending response to client");
		if (this->_htmlstartsend == false)
		{
			this->_response = this->HtmlToString("./blankpage.html");
			this->_htmlstartsend = true;
		}
		else
		{
			this->_response = this->GetResponse();
		}
		logger(this->_response);
		write(fd, this->_response.c_str(), this->_response.size());
		logger("HTMLpage is sent to fd!");
		close(fd);
		logger("fd is closed");
		this->_response.clear();
	}
}

void Server::CloseAllFds()
{
	for (int i = 0;  i != this->_ammount_sock; i++)
	{
		close(this->_sockvec[i].fd);
	}
}

void logger(std::string input)
{
	std::cout << input << std::endl;
}