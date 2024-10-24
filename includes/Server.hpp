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
#include <thread> 

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
	std::vector<Client*> _clientvec;
	std::vector<struct pollfd> _sockvec;
	std::vector<std::string> _whatsockvec;
 	int			_amount_sock;
	int			_amount_listen;
	int			_amount_client;
	std::vector<char> _cgi_response;
	std::string _post_data;
	std::vector<char> _post_data_v;
	std::string _method;
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
	std::string _recvsize;
	std::string _root;
	std::string _serverindex;
	std::unordered_map<int, std::string> _error_page;
	std::unordered_map<int, std::string> _hcerr_page;
	std::string _index;

	std::vector<Location> _locations;

	int		_listensock;

	int		_websock;
	Cgi*		_cgi;
	// std::chrono::time_point<std::chrono::system_clock> _start;
	std::chrono::time_point<std::chrono::steady_clock> _start;
	bool	_cgi_running;

	ssize_t _bytes_written;
	

	// std::unique_ptr<Cgi> _current_cgi;
	std::string stopCgiProcess(int index, Client *client);
	std::string CgiPipeFullyRead(int index, Client *client);
public:
	//SERVER.CPP
 	Server(const std::string& ip, const std::string& port, const std::string& server_name,
           const std::string& client_max, const std::string& root, const std::unordered_map<int, std::string>& error_page, const std::string& serverindex, int allow_methods);
    Server(Parser &in);
	~Server();

	//VALIDATE.CPP
	void ValidateListen(std::vector<std::string>& tokens);
	void ValidatePort(std::vector<std::string>& tokens);
	void ValidateServerName(std::vector<std::string>& tokens);
	void ValidateClientMaxBodySize(std::vector<std::string>& tokens);
	void ValidateRecvSize(std::vector<std::string>& tokens);
	void ValidateRoot(std::vector<std::string>& tokens);
	void ValidateErrorPage(std::vector<std::string>& tokens);
	void ValidateServerIndex(std::vector<std::string> &tokens);
	void ValidateAllowMethods(std::vector<std::string>& tokens);
	std::string GetMethod() const;
	std::string GetIp() const;
	std::string GetPort() const;	
	std::string GetServName() const;
	std::string GetClientMax() const;
	std::string GetRecvSize() const;
	std::string GetRoot() const;
	std::string GetServerIndex() const;
	std::vector<std::string> GetServerNames() const;
	std::vector<std::string> GetAllowMethods() const;
	std::vector<Location> GetLocations() const;
	std::unordered_map<int, std::string> GetErrorPage() const;

	//PARSER.CPP
	void ParseLocationBlock(std::vector<std::string>& tokens);


	///SERVER.CPP
	void InitHardcodedError();
	void SetUpServer();
	void InitSocket(std::vector<Server>::iterator it);
	void BindSockets(std::vector<Server>::iterator it, int index);
	void ListenSockets(int index);
	void RunPoll();
	void PollEvents();
	void AcceptClient(int index);
	void AddSocket(int fd, bool is_client);
	void AddSocket(int fd, const std::string& type);
	void InitClient(int socket, std::vector<Server>::iterator serverblock);
	void DeleteClient(int index, int fd);
	void RmvSocket(int index);
	void CloseAllFds();
	void writeToCgi(int fd, int index);
	void setStartTime(std::chrono::time_point<std::chrono::steady_clock> start);
	bool checkCgiTimer(pollfd temp, int index);
	std::vector<Client*>::iterator GetClient(int index);
	std::string readCgiResponse(int fd, int index, int recvmax, Client *client);
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
	class ClientErrorException : public std::exception
	{
		const char *what() const throw();
	};

	//REQUEST.CPP
	void EventsPollin(int fd, Client *client);
	void InitRequest(int fd, Client *client);
	void IsDoneRead(Client *client);
	int RecieveMessage(int fd, Client *client);
	long GetContentLenght(std::string buff);
	std::string WhichMethod(std::string buff);
	std::string GetHost(std::string tmp);
	std::string ParseRequest(Client *client);

	//RESPONSE.CPP
	void WriteToClient(int fd, Client *client);
	void BuildResponse(Client *client);
	void BuildResponseCode(Client *client, std::string htmlfile);
	void BuildResponseRedirect(Client *client, std::string htmlfile);


	//METHOD.CPP
	void SetClientVars(Client *client, std::vector<Location>::iterator it);
	int IsMethodAllowed(std::string method, Client *client);
	std::string WhichMethod(Client *client);
	std::string GetPath(Client *client);
	std::string listDirectoryContents(const std::string &directoryPath);
	std::string MethodDelete(std::string path, Client *client);
	std::string MethodGet(std::string path, Client *client);
	std::string GetAutoindex(std::string path, Client *client);
	std::string ExtractBoundary(const std::string &content);
	std::string ParsePost(const std::string &content);
	std::string MethodPost(std::string path, Client *client);
	std::vector<char> ParsePostV(const std::string &content);
	std::vector<Location>::iterator GetLocationBlock(Client *client, std::string path);

	//HTML.CPP
	int GetHardCCode(std::string path, Client *client);
	std::string GetHardCPathCode(int code, Client *client);
	std::string WhichMessageCode(int code);
	std::string GetSatusCodeFile(std::string path, Client *client);
	std::string HtmlToString(std::string path, Client *client);
};

void logger(std::string input);
