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
#include "../includes/Client.hpp"

class Parser;
class Location;
class Client;

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
	std::vector<std::string> _allow_methods;
	bool		_iscgi;

	int			_recvmax;
	std::string _port;
	std::string _ip;
	std::string _server_name;
	std::string _client_max;
	std::string _root;
	std::string _serverindex;
	std::unordered_map<int, std::string> _error_page;
	std::string _index;

	std::vector<Location> _locations;

	int		_listensock;
	Client *_client;

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
	std::vector<std::string> GetAllowMethods() const; 

	void   ValidateListen(std::vector<std::string>& tokens);
	void   ValidatePort(std::vector<std::string>& tokens);
	void   ValidateServerName(std::vector<std::string>& tokens);
	void   ValidateClientMaxBodySize(std::vector<std::string>& tokens);
	void   ValidateRoot(std::vector<std::string>& tokens);
	void   ValidateErrorPage(std::vector<std::string>& tokens);
	void   ValidateServerIndex(std::vector<std::string> &tokens);
	void   ValidateAllowMethods(std::vector<std::string>& tokens);

	void ParseLocationBlock(std::vector<std::string>& tokens);

	std::vector<Location> GetLocations() const;

	///SERVER.CPP
	void SetUpServer();
	void InitSocket(std::vector<Server>::iterator it);
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
	void EventsPollin(int fd, Client *client);
	int  RecieveMessage(int fd, Client *client);
	void GetResponse(int fd, Client *client);
	std::string ParseRequest(Client *client);
	std::string MethodGet(std::vector<char>::iterator itreq, Client *client);
	std::string MethodPost(std::vector<char>::iterator itreq);
	std::string HtmlToString(std::string path, Client *clien);
	std::string GetSatusCodeFile(std::string code, Client *client);
	std::string GetSatusCodeFile(int code, Client *client);

	///RESPONSE.CPP
	void EventsPollout(int fd);


	void InitClient(int socket, std::vector<Server>::iterator serverblock);
	int IsMethodAllowed(std::string method, Client *client);
	void CheckUnusedClients();

	class BindErrorException : public std::exception
	{
		const char *what() const throw();
	};

	class InitErrorException : public std::exception
	{
		const char *what() const throw();
	};

	class ListenErrorException : public std::exception
	{
		const char *what() const throw();
	};

	class PollErrorException : public std::exception
	{
		const char *what() const throw();
	};

	class AcceptErrorException : public std::exception
	{
		const char *what() const throw();
	};

	class FcntlErrorException : public std::exception
	{
		const char *what() const throw();
	};

	class ServerblockErrorException : public std::exception
	{
		const char *what() const throw();
	};
};

void logger(std::string input);
