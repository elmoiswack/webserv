#pragma once

#include <list>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <unordered_map>

#include "Parser.hpp"
#include <vector>
#include <poll.h>

#include "../includes/Parser.hpp"
#include "../includes/Location.hpp"

class Parser;

class Location;

class Server
{
private:


	std::string _port;
	std::string _ip;
	std::string _server_name;
	std::string _client_max;
	std::string _root;
	std::string _serverindex;
	std::unordered_map<int, std::string> _error_page;
	std::string _index;

	std::vector<Location> _locations;

	int		_websock;

// 	std::string _ip;
// 	std::vector<std::string> _ports;
// 	std::string _server_name;
// 	bool		_server_running;
// 	int			_ammount_sock;
// 	std::vector<struct pollfd> _sockvec;
// 	std::vector<std::string> _whatsockvec;
// 	std::string _response;
// 	bool 		_htmlstartsend;

public:
    Server(const std::string& ip, const std::string& port, const std::string& server_name,
           const std::string& client_max, const std::string& root, const std::unordered_map<int, std::string>& error_page, const std::string& serverindex);
    ~Server();
	
	std::string GetPort() const;
    std::string GetIp() const;
    std::string GetServName() const;
    std::string GetClientMax() const;
    std::string GetRoot() const;
	std::unordered_map<int, std::string> GetErrorPage() const;
	std::string GetServerIndex() const;
	std::vector<std::string> GetServerNames() const;

	void StartServer();

	void   ValidateListen(std::vector<std::string>& tokens);
	void   ValidatePort(std::vector<std::string>& tokens);
	void   ValidateServerName(std::vector<std::string>& tokens);
	void   ValidateClientMaxBodySize(std::vector<std::string>& tokens);
	void   ValidateRoot(std::vector<std::string>& tokens);
	void   ValidateErrorPage(std::vector<std::string>& tokens);
	void   ValidateServerIndex(std::vector<std::string> &tokens);

	void ParseLocationBlock(std::vector<std::string>& tokens);

	std::vector<Location> GetLocations() const;

	void SetUpServer();
	void InitSocket();
	void BindSockets();
	void ListenSockets();

	void RunPoll();

	void PollEvents();
	void EventsPollin(int fd, int index);
	void EventsPollout(int fd, int index);
	
	void AcceptClient(int index);
	void RecieveMessage(int fd, int index);

	std::string HtmlToString(std::string path);
	std::string GetResponse(int fd, int index);

	void AddSocket(int fd, bool is_client);
	void RmvSocket(int index);
	void CloseAllFds();
};

void logger(std::string input);
