#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <vector>

Server::Server(const std::string& ip, const std::string& port, const std::string& server_name,
               const std::string& client_max, const std::string& root, const std::unordered_map<int, std::string>& error_page, const std::string& serverindex, int allow_methods)
    : _allow_methods(allow_methods), _port(port), _ip(ip), _server_name(server_name), _client_max(client_max),  _root(root),  _serverindex(serverindex), _error_page(error_page), _websock(-1) {}



Server::Server(Parser &in)
{
	this->_serverblocks = in.GetServerBlocks();
	for (std::vector<Server>::iterator it = this->_serverblocks.begin(); it != this->_serverblocks.end(); it++)
	{
		it->_locationblocks = it->GetLocations();
		it->_ammount_sock = 0;
		it->_recvmax = std::atoi(it->_client_max.c_str());
		it->_donereading = false;
		it->_iscgi = false;
	}
}

Server::~Server()
{
	for (std::vector<Server>::iterator it = this->_serverblocks.begin(); it != this->_serverblocks.end(); it++)
	{
		it->_whatsockvec.clear();
		it->_client_max.clear();
		it->_error_page.clear();
		it->_index.clear();
		it->_ip.clear();
		it->_locationblocks.clear();
		it->_locations.clear();
		it->_port.clear();
		it->_request.clear();
		it->_response.clear();
		it->_root.clear();
		it->_server_name.clear();
		it->_serverindex.clear();
		it->_sockvec.clear();
		it->_whatsockvec.clear();
		it->_allow_methods.clear();
	}
	for (std::vector<Server>::iterator it = this->_serverblocks.begin(); it != this->_serverblocks.end(); it++)
	{
		this->_serverblocks.clear();
	}
}

void Server::AddSocket(int fd, bool is_client)
{
	pollfd temp;
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "ERROR FCNTL" << std::endl;
		exit(EXIT_FAILURE);	
	}
	temp.fd = fd;
	temp.events = POLLIN | POLLOUT;
	temp.revents = 0;
	this->_sockvec.push_back(temp);
	if (is_client == true)
	{
		this->_whatsockvec.push_back("CLIENT");
	}
	else
		this->_whatsockvec.push_back("SERVER");
	this->_ammount_sock += 1;
}

void Server::RmvSocket(int index, std::vector<Server>::iterator serv)
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
	serv->_ammount_sock--;
}

void Server::SetUpServer()
{
	for (unsigned long i = 0; i < this->_serverblocks.size(); i++)
	{
		std::vector<Server>::iterator it = this->_serverblocks.begin();
		this->InitSocket(it);
		this->BindSockets(it);
		this->ListenSockets(it);
	}
	logger("Server is initialized!");
	this->RunPoll();
	for (unsigned long i = 0; i < this->_serverblocks.size(); i++)
	{
		std::vector<Server>::iterator it = this->_serverblocks.begin();
		this->CloseAllFds(it);
	}
}

void Server::InitSocket(std::vector<Server>::iterator it)
{
	int websock = socket(AF_INET, SOCK_STREAM, 0);
	if (websock < 0)
	{
		std::cout << "ERROR SOCKET" << std::endl;
		exit(EXIT_FAILURE);
	}
	it->AddSocket(websock, false);
}

void Server::BindSockets(std::vector<Server>::iterator it)
{
	struct sockaddr_in infoaddr;
	std::string port = it->GetPort();
	infoaddr.sin_family = AF_INET;
	infoaddr.sin_addr.s_addr = INADDR_ANY;
	infoaddr.sin_port = htons(std::atoi(port.c_str()));
	logger("port = " + port);
	int opt = 1;
	setsockopt(it->_sockvec[0].fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	if (bind(it->_sockvec[0].fd, (struct sockaddr *)&infoaddr, sizeof(infoaddr)) == -1)
	{
		std::cout << "ERROR bruh" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Server::ListenSockets(std::vector<Server>::iterator it)
{
	if (listen(it->_sockvec[0].fd, 5) == -1)
	{
		std::cout << "ERROR LISTEN" << std::endl;
		exit(EXIT_FAILURE);		
	}
}

void Server::RunPoll()
{
	while (1)
	{
		for (std::vector<Server>::iterator it = this->_serverblocks.begin(); it != this->_serverblocks.end(); it++)
		{
			int ret = poll(it->_sockvec.data(), it->_sockvec.size(), -1);
			if (ret < 0)
			{
				std::cout << "ERROR POLL" << std::endl;
				exit(EXIT_FAILURE);
			}
			if (ret > 0)
				this->PollEvents(it);
		}
	}	
}

void Server::PollEvents(std::vector<Server>::iterator it)
{
	for (int index = 0; index < it->_ammount_sock; index++)
	{
		pollfd temp;
		temp.fd = it->_sockvec[index].fd;
		temp.events = it->_sockvec[index].events;
		temp.revents = it->_sockvec[index].revents;

		std::cout << "index = " << index << " = ";
		logger(it->_whatsockvec[index]);
		
		if (temp.revents & POLLIN)
		{
			if (it->_whatsockvec[index] == "SERVER")
			{
				it->AcceptClient(index, it);
			}
			else
			{
				it->EventsPollin(temp.fd, it);
			}
		}
		else if (temp.revents & POLLOUT)
		{
			it->EventsPollout(temp.fd, index, it);
		}
		else if (temp.revents & POLLHUP)
		{
			logger("Connection hung up!");
			close(temp.fd);
			it->RmvSocket(index, it);
		}
		else if (temp.revents & POLLERR)
		{
			logger("Fuck error poll");
			exit(EXIT_FAILURE);
		}
	}	
}

void Server::AcceptClient(int index, std::vector<Server>::iterator it)
{
	logger("attempting connection...");
	int newsock = accept(it->_sockvec[index].fd, NULL, NULL);
	if (newsock == -1)
	{
		std::cout << "ERROR ACCEPT" << std::endl;
		exit(EXIT_FAILURE);
	}
	it->AddSocket(newsock, true);
	logger("Connection is accepted!");
}

void Server::CloseAllFds(std::vector<Server>::iterator it)
{
	for (int i = 0;  i != it->_ammount_sock; i++)
	{
		close(it->_sockvec[i].fd);
	}
}

void logger(std::string input)
{
	std::cout << input << std::endl;
}