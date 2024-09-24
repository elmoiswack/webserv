#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../includes/Location.hpp"
#include "../includes/Server.hpp"

class Client
{
private:
	int		_id;
	int		_listensocket;
	std::vector<Location> _locationblocks;
	int		_recvmax;
	std::unordered_map<int, std::string> _error_page;
	std::string		_root;
	std::string 	_index;
	std::vector<std::string> _allow_methods;
	long		_contentlenght;
	std::string _servername;
	std::string _port;
	std::vector<char> _request;
	std::string _response;
	std::string _method;
	std::string _returnstring;
	std::string _returncode;
	bool		_autoindex;
	bool 		_donereading;
	bool		_isstatuscode;
	bool		_returnstate;
	int			_code;


public:
	Client();
	Client(int socket, std::vector<Server>::iterator serverblock);
	~Client();

	int GetListensock();
	
	std::vector<Location>::iterator GetLocationblockBegin();
	std::vector<Location>::iterator GetLocationblockEnd();
	
	int Getrecvmax();
	
	std::unordered_map<int, std::string>::iterator GetErrorpageBegin();
	std::unordered_map<int, std::string>::iterator GetErrorpageEnd();
	
	std::string GetRoot();
	void SetRoot(std::string root);

	std::string GetIndex();
	void SetIndex(std::string index);

	void SetMethodVec(std::vector<std::string> methods);
	std::vector<std::string>::iterator GetMethodsBegin();
	std::vector<std::string>::iterator GetMethodsEnd();
	
	void SetContentLenght(long contentlen);
	long GetContentLenght();
	
	std::string GetServerName();
	
	std::string GetPort();
	
	int GetId();
	void SetId(int id);

	void PushToRequest(char buff);
	void ClearRequest();
	
	std::vector<char>::iterator GetBeginRequest();
	std::vector<char>::iterator GetEndRequest();
	ssize_t GetRequestSize();
	
	void SetCurrentMethod(std::string method);
	std::string GetCurrentMethod();
	
	void ClearResponse();
	void SetResponse(std::string tmp);
	std::string GetResponse();
	ssize_t GetResponseSize();
	
	void SetDonereading(bool state);
	bool GetDonereading();
	
	void SetStatusCode(int code);
	void SetStatusCodeState(bool state);
	
	int GetStatusCode();
	bool GetStatusCodeState();

	void SetAutoindex(bool state);
	bool GetAutoindex();

	void SetReturnState(bool state);
	bool GetReturnstate();
	void SetReturnCode(std::string code);
	std::string GetReturnCode();
	void Setreturn(std::string path);
	std::string GetReturn();

};

std::ostream& operator<<(std::ostream &out, Client *in);

#endif

