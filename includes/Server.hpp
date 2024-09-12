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
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fcntl.h>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <poll.h>
#include <chrono>

#include "../includes/Parser.hpp"
#include "../includes/Location.hpp"
#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp"

class Parser;
class Location;
class Client;

// class Cgi;

class Server
{
private:

	std::vector<Server> _serverblocks;
	std::vector<Location> _locationblocks;
	std::vector<struct pollfd> _sockvec;
	std::vector<std::string> _whatsockvec;
 	int			_ammount_sock;
	std::vector<char> _request;
	std::vector<char> _cgi_response;
	std::string _response;
	std::string _post_data;
	std::string _method;
	bool		_donereading;
	std::vector<std::string> _allow_methods;
	bool		_cgi_donereading;
	bool		_iscgi;
	bool		_recvzero;
	bool		_isbody;
	int			_statuscode;
	bool		_isstatuscode;
	long		_totalread;

	int			_recvmax;
	std::string _port;
	std::string _ip;
	std::string _server_name;
	std::string _client_max;
	std::string _root;
	std::string _serverindex;
	std::unordered_map<int, std::string> _error_page;
	std::unordered_map<int, std::string> _hcerr_page;
	std::string _index;

	std::vector<Location> _locations;

	int		_listensock;
	Client *_client;

	int		_websock;
	Cgi*		_cgi;
	std::chrono::time_point<std::chrono::system_clock> _start;
	bool	_cgi_running;

	ssize_t _bytes_written;
	

	// std::unique_ptr<Cgi> _current_cgi;
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
	void AddSocket(int fd, const std::string& type);
	void RmvSocket(int index);
	void CloseAllFds();
	std::string ExtractBoundary(const std::string &content);
	std::string ParsePost(const std::string &content);
	void checkCgiTimer();
	void setStartTime (std::chrono::time_point<std::chrono::system_clock> start);
	
	
	///REQUEST.CPP
	void EventsPollin(int fd, Client *client);
	int  RecieveMessage(int fd, Client *client);
	std::string ParseRequest(Client *client);
	std::string MethodGet(std::vector<char>::iterator itreq, Client *client);
	std::string MethodPost(std::vector<char>::iterator itreq);
	std::string HtmlToString(std::string path, Client *client);
	std::string HtmlToString(std::string path);
	std::string GetSatusCodeFile(std::string code, Client *client);
	long	GetContentLenght(std::string buff);
	void InitRequest(int fd, Client *client);
	void BuildResponse(Client *client);
	void IsDoneRead(Client *client);
	std::string WhichMethod(std::string);
	std::string WhichMethod(Client *client, std::vector<char>::iterator itfirst);
	std::string MethodDelete(std::vector<char>::iterator itreq);

	///RESPONSE.CPP
	void EventsPollout(int fd, Client *client);

	void InitHardcodedError();
	std::string GetHardCPathCode(int code);
	void InitClient(int socket, std::vector<Server>::iterator serverblock);
	int IsMethodAllowed(std::string method, Client *client);
	void writeToCgi(int fd, int index);
	std::string	readCgiResponse(int fd, int index, int recvmax);

	std::string listDirectoryContents(const std::string &directoryPath);

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

	class WriteErrorException : public std::exception
	{
		const char *what() const throw();
	};
};

void logger(std::string input);
