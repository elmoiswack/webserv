#include "../includes/Client.hpp"

Client::Client()
{

}

Client::Client(int socket, std::vector<Server>::iterator serverblock)
{
	this->_listensocket = socket;
	this->_locationblocks = serverblock->GetLocations();
	this->_servername = serverblock->GetServName();
	this->_port = serverblock->GetPort();
	this->_recvmax = std::stoi(serverblock->GetClientMax());
	this->_root = serverblock->GetRoot();
	this->_error_page = serverblock->GetErrorPage();
	this->_allow_methods = serverblock->GetAllowMethods();
	std::vector<std::string> tmp = serverblock->GetAllowMethods();
	this->_request.clear();
	this->_response.clear();
	this->_contentlenght = 0;
	this->_id = 0;
	this->_method = "EMPTY";
	std::cout << "CLIENT CREATED AND VARS SET!" << std::endl;
}

Client::~Client()
{
	this->_allow_methods.clear();
	this->_error_page.clear();
	this->_locationblocks.clear();
	this->_recvmax = 0;
	this->_listensocket = 0;
	this->_request.clear();
	this->_response.clear();
	this->_root.clear();
	std::cout << "client is about to get deleted!" << std::endl;
}

int Client::GetListensock()
{
	return (this->_listensocket);
}

std::string Client::GetServerName()
{
	return (this->_servername);
}

std::string Client::GetPort()
{
	return (this->_port);
}

int Client::GetId()
{
	return (this->_id);
}

void Client::SetId(int id)
{
	this->_id = id;
}

std::vector<Location>::iterator Client::GetLocationblockBegin()
{
	return (this->_locationblocks.begin());
}

std::vector<Location>::iterator Client::GetLocationblockEnd()
{
	return (this->_locationblocks.end());
}

int Client::Getrecvmax()
{
	return (this->_recvmax);
}

std::unordered_map<int, std::string>::iterator Client::GetErrorpageBegin()
{
	return (this->_error_page.begin());
}

std::unordered_map<int, std::string>::iterator Client::GetErrorpageEnd()
{
	return (this->_error_page.end());
}

std::string Client::GetRoot()
{
	return (this->_root);
}

std::vector<std::string>::iterator Client::GetMethodsBegin()
{
	return (this->_allow_methods.begin());
}

std::vector<std::string>::iterator Client::GetMethodsEnd()
{
	return (this->_allow_methods.end());
}

std::vector<std::string> Client::GetMethods()
{
	return (this->_allow_methods);
}

void Client::SetContentLenght(long contentlen)
{
	this->_contentlenght = contentlen;
}

long Client::GetContentLenght()
{
	return (this->_contentlenght);
}

void Client::PushToRequest(char buff)
{
	this->_request.push_back(buff);
}

std::vector<char>::iterator Client::GetBeginRequest()
{
	return (this->_request.begin());
}

std::vector<char>::iterator Client::GetEndRequest()
{
	return (this->_request.end());
}

void Client::SetCurrentMethod(std::string method)
{
	this->_method = method;
}

std::string Client::GetCurrentMethod()
{
	return (this->_method);
}

void Client::SetResponse(std::string tmp)
{
	this->_response = tmp;
}

std::string Client::GetResponse()
{
	return (this->_response);
}

ssize_t Client::GetRequestSize()
{
	return (this->_request.size());
}

ssize_t Client::GetResponseSize()
{
	return (this->_response.size());
}

void Client::ClearRequest()
{
	this->_request.clear();
}

void Client::ClearResponse()
{
	this->_response.clear();
}

std::ostream& operator<<(std::ostream &out, Client *in)
{
	out << "Client listen sock = " << in->GetListensock() << std::endl;
	out << "Client recvmax = " << in->Getrecvmax() << std::endl;
	out << "Client root = " << in->GetRoot() << std::endl;
	for (auto it = in->GetErrorpageBegin(); it != in->GetErrorpageEnd(); it++)
	{
		out << it->first << " ";
	}
	out << std::endl;
	for (auto it = in->GetMethodsBegin(); it != in->GetMethodsEnd(); it++)
	{
		out << *it << " ";
	}
	out << std::endl;
	out << "Client id = " << in->GetId() << std::endl;
	return (out);
}