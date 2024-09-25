#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"
#include "../includes/Client.hpp"
#include <chrono>
#include <iostream>
#include <thread>

Server::Server(const std::string& ip, const std::string& port, const std::string& server_name,
               const std::string& client_max, const std::string& root, const std::unordered_map<int, std::string>& error_page, const std::string& serverindex, int allow_methods)
    : _allow_methods(allow_methods), _port(port), _ip(ip), _server_name(server_name), _client_max(client_max),  _root(root),  _serverindex(serverindex), _error_page(error_page), _cgi(nullptr){
		this->_amount_sock = 0;
		this->_amount_listen = 0;
		this->_amount_client = 0;
		this->_iscgi = false;
		this->_recvzero = false;
		this->_isbody = true;
		this->_listensock = 0;
		this->_statuscode = 0;
		this->_isstatuscode = false;
		this->_totalread = 0;
		this->_bytes_written = 0;
		this->InitHardcodedError();
		this->_bytes_written = 0;
}



Server::Server(Parser &in)
{
	this->_serverblocks = in.GetServerBlocks();
	this->InitHardcodedError();
	this->_amount_sock = 0;
	this->_amount_listen = 0;
	this->_amount_client = 0;
	this->_iscgi = false;
	this->_recvzero = false;
	this->_isbody = true;
	this->_listensock = 0;
	this->_statuscode = 0;
	this->_isstatuscode = false;
	this->_totalread = 0;
	this->_cgi = nullptr;
	this->_bytes_written = 0;
}

Server::~Server()
{
	logger("SERVER DESTRUCTED\n");
	this->CloseAllFds();
	this->_whatsockvec.clear();
	this->_error_page.clear();
	this->_locationblocks.clear();
	this->_locations.clear();
	this->_sockvec.clear();
	this->_serverblocks.clear();
	this->_hcerr_page.clear();
	for (auto it = this->_clientvec.begin(); it != this->_clientvec.end(); it++)
		delete *it;
}

void Server::InitHardcodedError()
{
	this->_hcerr_page[204] = "./var/www/status_codes/204.html";
	this->_hcerr_page[301] = "./var/www/status_codes/301.html";
	this->_hcerr_page[400] = "./var/www/status_codes/400.html";
	this->_hcerr_page[403] = "./var/www/status_codes/403.html";
	this->_hcerr_page[404] = "./var/www/status_codes/404.html";
	this->_hcerr_page[405] = "./var/www/status_codes/405.html";
	this->_hcerr_page[411] = "./var/www/status_codes/411.html";
	this->_hcerr_page[500] = "./var/www/status_codes/500.html";
	this->_hcerr_page[501] = "./var/www/status_codes/501.html";
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
		this->_amount_listen += 1;
		index++;
	}
	logger("Server is initialized!");
	this->_cgi_running = false;
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
	std::cout << "Ammount of listening sockets ready: " << this->_amount_sock << std::endl;
	while (1)
	{
		int ret = poll(this->_sockvec.data(), this->_sockvec.size(), -1);
		logger("\nWent through poll!\n");
		if (ret < 0)
		{
			std::cout << "ERROR POLL" << std::endl;
			throw(Server::PollErrorException());
		}
		if (ret > 0)
		{
			this->PollEvents();
		}
	}	
}

void Server::PollEvents()
{
	for (int index = 0; index < this->_amount_sock; index++)
	{
		pollfd temp;
		temp.fd = this->_sockvec[index].fd;
		temp.events = this->_sockvec[index].events;
		temp.revents = this->_sockvec[index].revents;
		std::cout << this->_sockvec.size() << std::endl;
		Client *client;
		if (this->_whatsockvec[index] == "CLIENT")
		{
			auto it = this->GetClient(index);
			if (it == this->_clientvec.end())
			{
				std::cout << "ERROR GetClient" << std::endl;
				throw(Server::ClientErrorException());
			}
			client = *it;
		}
		if (!(temp.revents & POLLIN) && this->_cgi_running && this->_whatsockvec[index] != "SERVER" && this->_whatsockvec[index] != "CLIENT" && !this->checkCgiTimer(temp, index))
		{
			std::string errfile = this->HtmlToString(this->GetHardCPathCode(500, client), client);
			std::string response = 
			"HTTP/1.1 500 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " + std::to_string(errfile.length()) + "\r\n"
			"\r\n"
			+ errfile;
			this->_cgi->killCgi();
			this->_cgi_running = false;
			delete this->_cgi;
			this->_cgi = nullptr;
			client->SetDonereading(true);
			client->SetResponse(response);
			client->ClearRequest();
			close(temp.fd);
			RmvSocket(index);
		}
		else if (temp.revents & POLLIN)
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
			else if (this->_whatsockvec[index] == "CLIENT")
			{
				if (client->GetStatusCodeState() == false)
				{
					this->EventsPollin(temp.fd, client);
					if (this->_recvzero == true)
					{
						logger("Client closed connection!");
						this->DeleteClient(index, temp.fd);
						this->_recvzero = false;
					}
				}
				else if ((client->GetDonereading() == true && client->GetResponseSize() > 0)) //this is bit weird, only used with siege but idk
				{
					this->WriteToClient(temp.fd, client);
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					if (client->GetResponse() == "EMPTY")
						this->DeleteClient(index, temp.fd);
				}
			}
			else if (this->_whatsockvec[index] == "CGI_READ")
			{
				logger("\n--CGI POLLIN\n");
				if (this->_cgi_donereading == false)
					client->SetResponse(this->readCgiResponse(temp.fd, index, client->Getrecvmax(), client));
			}
		}
		else if (temp.revents & POLLOUT)
		{
			if (this->_whatsockvec[index] == "CGI_WRITE")
			{
				logger("--CGI POLLOUT\n");
				this->writeToCgi(temp.fd, index);
			}
			else if (client->GetDonereading() == true && client->GetResponseSize() > 0)
			{
				this->WriteToClient(temp.fd, client);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				if (client->GetResponse() == "EMPTY")
					this->DeleteClient(index, temp.fd);
			}
		}
		else if (temp.revents & POLLHUP)
		{
			logger("Connection hung up!");
			if (this->_whatsockvec[index] == "CGI_READ" && this->_cgi_running)
			{
				logger("\nCGI ERROR (poll loop)!");
				this->_cgi_running = false;
				delete (this->_cgi);
				this->_cgi = nullptr;
				std::string errfile = this->HtmlToString(this->GetHardCPathCode(500, client), client);
				std::string response = 
				"HTTP/1.1 500 Internal Server Error\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " + std::to_string(errfile.length()) + "\r\n"
				"\r\n"
				+ errfile;
				client->SetResponse(response);
				client->SetDonereading(true);
				client->ClearRequest();

			}
			close(temp.fd);
			RmvSocket(index);
		}
		else if (temp.revents & POLLERR)
		{
			logger("POLLERR: Connection already has been closed!");
			this->DeleteClient(index, temp.fd);
		}
		else if (temp.revents & POLLNVAL)
		{
			logger("POLLNVAL: invalid request, closing connection!");
			this->DeleteClient(index, temp.fd);
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
	this->_amount_sock += 1;
}

void Server::AddSocket(int fd, const std::string& type) // for the cgi
{
	logger("\nCGI SOCKET ADDED -> " + type);
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
	this->_whatsockvec.push_back(type);
	this->_amount_sock += 1;
}

void Server::InitClient(int socket, std::vector<Server>::iterator serverblock)
{
	this->_clientvec.push_back(new Client(socket, serverblock));
	logger("New client allocated!");
	std::vector<Client*>::iterator it = this->_clientvec.end();
	it--;
	Client* client = *it;
	client->SetId(this->_amount_client);
	this->_amount_client += 1;
}

std::vector<Client*>::iterator Server::GetClient(int index)
{
	index -= this->_amount_listen;
	int count = 0;
	auto it = this->_clientvec.begin();
	while (count != index)
	{
		count++;
		it++;
	}
	if (it == this->_clientvec.end())
	{
		logger("FAILED GETTING CLIENT");
		return (this->_clientvec.end());
	}
	return (it);
}

void Server::DeleteClient(int index, int fd)
{
	close(fd);
	this->RmvSocket(index);
	int clientid = this->_amount_client - 1;
	if (this->_clientvec[clientid] != nullptr)
	{
		delete this->_clientvec[clientid];
		this->_clientvec.pop_back();
	}
	logger("client is deleted!\n");
	this->_amount_client -= 1;
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
	this->_amount_sock--;
}

void Server::CloseAllFds()
{
	for (int i = 0;  i != this->_amount_sock; i++)
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

const char *Server::ClientErrorException::what() const throw()
{
	return ("failed to get the clients. Shutting down server!");
}

void Server::writeToCgi(int fd, int index) 
{
    close(this->_cgi->getReadEndUploadPipe());
	std::cout << "post data size = " << this->_post_data.size() << std::endl;
	if (!this->_post_data.empty()) 
	{
		ssize_t total_bytes = this->_post_data.size();
		const char *data_ptr = this->_post_data.data();

        if (this->_bytes_written < total_bytes) 
		{
			int tmp = 10000;
			if ((this->_bytes_written + tmp) > total_bytes)
			{
				tmp = total_bytes - this->_bytes_written;
			}
            ssize_t written = write(fd, data_ptr + this->_bytes_written, tmp);
            if (written == -1) 
			{
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
				return ;
			}
            this->_bytes_written += written;
        }
		else
		{
			this->_bytes_written = 0;
			logger("REMOVING SOCKET IN writeToCgi:");
			this->RmvSocket(index);
		}
    }
}

std::string Server::readCgiResponse(int fd, int index, int recvmax, Client *client)
{
	char *buffer = new char[recvmax];
    if (this->_cgi->waitForChild() == false)
    {
        logger("\nERROR CGI PROCESS\n");
		delete this->_cgi;
		this->RmvSocket(index);
		std::string errfile = this->HtmlToString(this->GetHardCPathCode(500, client), client);
		std::string response = 
		"HTTP/1.1 500 Internal Server Error\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(errfile.length()) + "\r\n"
		"\r\n"
		+ errfile;
		this->_cgi_running = false;
		client->SetDonereading(true);
		client->ClearRequest();
		return(response);
    }
    ssize_t bytes_read = read(fd, buffer, recvmax);
	if (bytes_read == 0)
    {
        logger("REACHED EOF");
		this->RmvSocket(index);
		std::string errfile = this->HtmlToString(this->GetHardCPathCode(500, client), client);
		std::string response = 
		"HTTP/1.1 500 Internal Server Error\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(errfile.length()) + "\r\n"
		"\r\n"
		+ errfile;
		this->_cgi_running = false;
		client->SetResponse(response);
		client->SetDonereading(true);
		client->ClearRequest();
		return (client->GetResponse());
    }
    else if (bytes_read < 0)
	{
        logger("ERROR READING FROM CGI PIPE (read returned -1)");
		std::exit(EXIT_FAILURE);
	}
	for (int i = 0; i < bytes_read; ++i)
		this->_cgi_response.push_back(buffer[i]);
	if (bytes_read < recvmax)
    {
        logger("CGI PIPE FULLY READ");
		std::string tmp(this->_cgi_response.begin(), this->_cgi_response.end());
		client->SetResponse(tmp);
		client->ClearRequest();
		this->_cgi_response.clear();
		this->_cgi_donereading = true;
		this->_cgi_running = false;
		delete this->_cgi;
		logger("REMOVING SOCKET IN READ CGI RES");
		this->RmvSocket(index);
		return (client->GetResponse());
	}
	delete[] buffer;
    return "";
}

bool Server::checkCgiTimer(pollfd temp, int index)
{
	(void)temp;
	(void)index;
	std::chrono::time_point<std::chrono::steady_clock> currentTime;
	if (this->_cgi_running)
	{
		currentTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = currentTime - this->_start;
		// std::cout << elapsed_seconds.count() << "s\n";
		if (elapsed_seconds.count() > 3.0)
			return false;
	}
	return true;
}

void Server::setStartTime(std::chrono::time_point<std::chrono::steady_clock> start)
{
	this->_start = start;
}




















































// void Server::checkCgiTimer(pollfd temp, int index)
// {
// 	(void)temp;
// 	(void)index;
// 	// std::chrono::time_point<std::chrono::system_clock> now;
// 	std::chrono::time_point<std::chrono::steady_clock> currentTime;
// 	if (this->_cgi_running)
// 			{
// 				currentTime = std::chrono::steady_clock::now();
// 				std::chrono::duration<double> elapsed_seconds = currentTime - this->_start;
// 				// std::cout << elapsed_seconds.count() << "s\n";
// 				if (elapsed_seconds.count() > 3.0)
// 				{
// 					// this->_start = currentTime;
// 					logger("\nCGI EXECUTION EXCEEDED 3 SECONDS\n");
// 					this->_cgi->killCgi();
// 					this->_cgi_running = false;
// 					delete this->_cgi;
// 					this->_cgi = nullptr;
// 					this->_response.clear();
// 					this->_response = 
// 					"HTTP/1.1 500 OK\r\n"
// 					"Content-Type: text/html\r\n"
// 					"Content-Length: " + std::to_string(this->HtmlToString("./var/www/status_codes/500.html").length()) + "\r\n"
// 					"\r\n"
// 					+ this->HtmlToString("./var/www/status_codes/500.html");
// 					// logger("REMOVING SOCKET IN CHECK CGI TIMER");
// 					// close(temp.fd);
// 					// RmvSocket(index);
// 				}
				
// 				// else
// 				// 	logger("\n\nREQUEST OK\n");
// 			}
// }




