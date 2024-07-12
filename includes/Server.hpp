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

	std::vector<Server> _serverblocks;
	std::vector<Location> _locationblocks;
	std::vector<struct pollfd> _sockvec;
	std::vector<std::string> _whatsockvec;
 	int			_ammount_sock;
	std::vector<char> _request;
	std::string _response;
	std::string _method;
	bool		_donereading;
	int			_recvmax;
	int 		_allow_methods;
	bool		_iscgi;


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

public:
 	Server(const std::string& ip, const std::string& port, const std::string& server_name,
           const std::string& client_max, const std::string& root, const std::unordered_map<int, std::string>& error_page, const std::string& serverindex, int allow_methods);
    Server(Parser &in);
	~Server();

	std::string GetMethod() const;
	std::string GetPort() const;
    std::string GetIp() const;
    std::string GetServName() const;
    std::string GetClientMax() const;
    std::string GetRoot() const;
	std::unordered_map<int, std::string> GetErrorPage() const;
	std::string GetServerIndex() const;
	std::vector<std::string> GetServerNames() const;
	int GetAllowMethods() const; 

	void   ValidateListen(std::vector<std::string>& tokens);
	void   ValidatePort(std::vector<std::string>& tokens);
	void   ValidateServerName(std::vector<std::string>& tokens);
	void   ValidateClientMaxBodySize(std::vector<std::string>& tokens);
	void   ValidateRoot(std::vector<std::string>& tokens);
	void   ValidateErrorPage(std::vector<std::string>& tokens);
	void   ValidateServerIndex(std::vector<std::string> &tokens);
	void 	ValidateAllowMethods(std::vector<std::string>& tokens);

	void ParseLocationBlock(std::vector<std::string>& tokens);

	std::vector<Location> GetLocations() const;

	///SERVER.CPP
	void SetUpServer();
	void InitSocket();
	void BindSockets(std::vector<Server>::iterator it, int index);
	void ListenSockets(int index);

	void RunPoll();
	void PollEvents();
	
	void AcceptClient(int index);
	void AddSocket(int fd, bool is_client);
	void RmvSocket(int index);
	void CloseAllFds();
	std::string ExtractBoundary(const std::string &content);
	std::string ParsePost(const std::string &content);
	
	///REQUEST.CPP
	void EventsPollin(int fd);
	void RecieveMessage(int fd);
	void GetResponse(int fd);
	std::string ParseRequest();
	std::string MethodGet(std::vector<char>::iterator itreq);
	std::string MethodPost(std::vector<char>::iterator itreq);
	std::string HtmlToString(std::string path);
	std::string GetSatusCodeFile(std::string code);

	///RESPONSE.CPP
	void EventsPollout(int fd, int index);


	

};

void logger(std::string input);
