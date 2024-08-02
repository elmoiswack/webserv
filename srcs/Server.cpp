#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"
#include "../includes/Client.hpp"

Server::Server(const std::string& ip, const std::string& port, const std::string& server_name,
               const std::string& client_max, const std::string& root, const std::unordered_map<int, std::string>& error_page, const std::string& serverindex, int allow_methods)
    : _allow_methods(allow_methods), _port(port), _ip(ip), _server_name(server_name), _client_max(client_max),  _root(root),  _serverindex(serverindex), _error_page(error_page){
		this->_ammount_sock = 0;
		this->_client = NULL;
		this->_donereading = false;
		this->_iscgi = false;
		this->_recvzero = false;
		this->_isbody = true;
		this->_listensock = 0;
		this->_iffirstread = true;
		this->_autoinfile = false;
		this->InitHardcodedError();
}



Server::Server(Parser &in)
{
	this->_serverblocks = in.GetServerBlocks();
	this->InitHardcodedError();
	this->_ammount_sock = 0;
	this->_client = NULL;
	this->_donereading = false;
	this->_iscgi = false;
	this->_recvzero = false;
	this->_isbody = true;
	this->_listensock = 0;
	this->_iffirstread = true;
	this->_autoinfile = false;
	this->_request.clear();
	this->_response.clear();
}

Server::~Server()
{
	this->CloseAllFds();
	this->_whatsockvec.clear();
	this->_error_page.clear();
	this->_locationblocks.clear();
	this->_locations.clear();
	this->_request.clear();
	delete this->_client;
	this->_sockvec.clear();
	this->_serverblocks.clear();
	this->_hcerr_page.clear();
}

void Server::InitHardcodedError()
{
	this->_hcerr_page[400] = "./var/www/status_codes/400.html";
	this->_hcerr_page[403] = "./var/www/status_codes/403.html";
	this->_hcerr_page[404] = "./var/www/status_codes/404.html";
	this->_hcerr_page[405] = "./var/www/status_codes/405.html";
	this->_hcerr_page[500] = "./var/www/status_codes/500.html";
	this->_hcerr_page[501] = "./var/www/status_codes/501.html";
}

void Server::AddSocket(int fd, bool is_client)
{
	pollfd temp;
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "ERROR FCNTL" << std::endl;
		throw(Server::FcntlErrorException());
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
	std::cout << "Ammount serverblocks = " << this->_serverblocks.size() << std::endl;
	int index = 0;
	for (std::vector<Server>::iterator it = this->_serverblocks.begin(); it != this->_serverblocks.end(); it++)
	{
		this->InitSocket(it);
		this->BindSockets(it, index);
		this->ListenSockets(index);
		index++;
	}
	logger("Server is initialized!");
	this->RunPoll();
	this->CloseAllFds();
}

void Server::InitSocket(std::vector<Server>::iterator it)
{
	int websock = socket(AF_INET, SOCK_STREAM, 0);
	if (websock < 0)
	{
		std::cout << "ERROR SOCKET" << std::endl;
		throw(Server::InitErrorException());
	}
	it->_listensock = websock;
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
		std::cout << "ERROR BIND" << std::endl;
		throw(Server::BindErrorException());
	}
}

void Server::ListenSockets(int index)
{
	if (listen(this->_sockvec[index].fd, 5) == -1)
	{
		std::cout << "ERROR LISTEN" << std::endl;
		throw(Server::ListenErrorException());	
	}
}

void Server::RunPoll()
{
	std::cout << "Ammount of listening sockets ready: " << this->_ammount_sock << std::endl;
	while (1)
	{
		int ret = poll(this->_sockvec.data(), this->_sockvec.size(), -1);
		if (ret < 0)
		{
			std::cout << "ERROR POLL" << std::endl;
			throw(Server::PollErrorException());
		}
		if (ret > 0)
			this->PollEvents();
	}	
}

void Server::InitClient(int socket, std::vector<Server>::iterator serverblock)
{
	this->_client = new Client(socket, serverblock);
	logger("New client allocated!");
	std::cout << this->_client << std::endl;
}

void Server::PollEvents()
{
	std::cout << "Ammount of sockets ready: " << this->_ammount_sock << std::endl;
	for (int index = 0; index < this->_ammount_sock; index++)
	{
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
				std::vector<Server>::iterator itter = this->_serverblocks.begin();
				while (itter != this->_serverblocks.end())
				{
					if (itter->_listensock == this->_sockvec[index].fd)
						break ;
					itter++;
				}
				if (itter == this->_serverblocks.end())
				{
					logger("Serverblock has not been found! Something went wrong.");
					throw(Server::ServerblockErrorException());
				}
				this->InitClient(this->_sockvec[index].fd, itter);
			}
			else
			{
				this->EventsPollin(temp.fd, this->_client);
				if (this->_recvzero == true)
				{
					logger("Client closed connection!");
					close(temp.fd);
					this->RmvSocket(index);
					if (this->_client != nullptr) 
					{
       	 				delete this->_client;
        				this->_client = nullptr;
   					}
					logger("client is deleted!");
					this->_recvzero = false;
				}
			}
		}
		else if (temp.revents & POLLOUT)
		{
			if (this->_donereading == true)
			{
				this->EventsPollout(temp.fd, this->_client);
				this->RmvSocket(index);
				if (this->_client != nullptr) 
				{
       	 			delete this->_client;
        			this->_client = nullptr;
   				}
				logger("client is deleted!");
			}
		}
		else if (temp.revents & POLLHUP)
		{
			logger("POLLHUP: Connection hung up!");
			close(temp.fd);
			this->RmvSocket(index);
		}
		else if (temp.revents & POLLERR)
		{
			logger("POLLERR: Connection already has been closed!");
			close(temp.fd);
			this->RmvSocket(index);
		}
		else if (temp.revents & POLLNVAL)
		{
			logger("POLLNVAL: invalid request, closing connection!");
			close(temp.fd);
			this->RmvSocket(index);
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
		throw(Server::AcceptErrorException());
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

const char *Server::FcntlErrorException::what() const throw()
{
	return ("function fctnl in Addsocket failed! Shutting down server!");
}

const char *Server::InitErrorException::what() const throw()
{
	return ("function socket inside initsocket failed! Shutting down server!");
}

const char *Server::BindErrorException::what() const throw()
{
	return ("function bind failed! Shutting down server!");
}

const char *Server::ListenErrorException::what() const throw()
{
	return ("function listen failed! Shutting down server!");
}

const char *Server::PollErrorException::what() const throw()
{
	return ("function poll failed! Shutting down server!");
}

const char *Server::ServerblockErrorException::what() const throw()
{
	return ("server block not found while it should be there in poll! Shutting down server!");
}

const char *Server::AcceptErrorException::what() const throw()
{
	return ("function accept failed! Shutting down server!");
}

const char *Server::WriteErrorException::what() const throw()
{
	return ("function write failed for the second time! Shutting down server!");
}