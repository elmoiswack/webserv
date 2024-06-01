#ifndef SERVER_HPP
#define SERVER_HPP

#include <list>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "../includes/Parser.hpp"

class Parser;

class Server
{
private:

	std::string _port;
	std::string _ip;
	std::string _server_name;
	std::string _client_max;
	std::string _root;

	int		_websock;

public:
    Server(const std::string& ip, const std::string& port, const std::string& server_name,
           const std::string& client_max, const std::string& root);
	~Server();
	
	std::string GetPort() const;
    std::string GetIp() const;
    std::string GetServName() const;
    std::string GetClientMax() const;
    std::string GetRoot() const;

	void StartServer();

	void   ValidateListen(std::vector<std::string>& tokens);
	void   ValidatePort(std::vector<std::string>& tokens);
	void   ValidateServerName(std::vector<std::string>& tokens);
	void   ValidateClientMaxBodySize(std::vector<std::string>& tokens);
	void   ValidateRoot(std::vector<std::string>& tokens);
};

#endif