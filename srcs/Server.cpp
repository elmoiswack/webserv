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
		this->_locationblocks = it->GetLocations();
		this->_ammount_sock = 0;
		this->_recvmax = std::atoi(it->_client_max.c_str());
		this->_donereading = false;
		this->_iscgi = false;
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
		it->_whatsockvec.clear();
		it->_allow_methods.clear();
	}
	this->_sockvec.clear();
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
	this->_ammount_sock--;
}

void Server::SetUpServer()
{
	std::cout << "FKSAHFKSAHFA = " << this->_serverblocks.size() << std::endl;
	int index = 0;
	for (std::vector<Server>::iterator it = this->_serverblocks.begin(); it != this->_serverblocks.end(); it++)
	{
		this->InitSocket();
		this->BindSockets(it, index);
		this->ListenSockets(index);
		index++;
	}
	logger("Server is initialized!");
	this->RunPoll();
	this->CloseAllFds();
}

void Server::InitSocket()
{
	int websock = socket(AF_INET, SOCK_STREAM, 0);
	if (websock < 0)
	{
		std::cout << "ERROR SOCKET" << std::endl;
		exit(EXIT_FAILURE);
	}
	this->AddSocket(websock, false);
}

void Server::BindSockets(std::vector<Server>::iterator it, int index)
{
	struct sockaddr_in infoaddr;
	std::string port = it->GetPort();
	infoaddr.sin_family = AF_INET;
	infoaddr.sin_addr.s_addr = INADDR_ANY;
	infoaddr.sin_port = htons(std::atoi(port.c_str()));
	logger("port = " + port);
	int opt = 1;
	setsockopt(this->_sockvec[index].fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	if (bind(this->_sockvec[index].fd, (struct sockaddr *)&infoaddr, sizeof(infoaddr)) == -1)
	{
		std::cout << "ERROR bruh" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Server::ListenSockets(int index)
{
	if (listen(this->_sockvec[index].fd, 5) == -1)
	{
		std::cout << "ERROR LISTEN" << std::endl;
		exit(EXIT_FAILURE);		
	}
}

void Server::RunPoll()
{
	std::cout << "dsadsadsageagqw: " << this->_ammount_sock << std::endl;
	while (1)
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
	for (int index = 0; index < this->_ammount_sock; index++)
	{
		std::cout << "dsada: " << this->_ammount_sock << std::endl;
		pollfd temp;
		temp.fd = this->_sockvec[index].fd;
		temp.events = this->_sockvec[index].events;
		temp.revents = this->_sockvec[index].revents;

		std::cout << "index = " << index << " = ";
		logger(this->_whatsockvec[index]);
		
		if (temp.revents & POLLIN)
		{
			if (this->_whatsockvec[index] == "SERVER")
			{
				this->AcceptClient(index);
			}
			else
			{
				this->EventsPollin(temp.fd);
			}
		}
		else if (temp.revents & POLLOUT)
		{
			this->EventsPollout(temp.fd, index);
		}
		else if (temp.revents & POLLHUP)
		{
			logger("Connection hung up!");
			close(temp.fd);
			this->RmvSocket(index);
		}
		else if (temp.revents & POLLERR)
		{
			logger("Fuck error poll");
			exit(EXIT_FAILURE);
		}
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
	this->AddSocket(newsock, true);
	logger("Connection is accepted!");
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