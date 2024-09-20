#include "../includes/Client.hpp"

Client::Client()
{

}

Client::Client(int socket, std::vector<Server>::iterator serverblock)
{
	this->_listensocket = socket;
	this->_locationblocks = serverblock->GetLocations();
	this->_servername = serverblock->GetServName();
	this->_error_page = serverblock->GetErrorPage();
	this->_port = serverblock->GetPort();
	this->_recvmax = std::stoi(serverblock->GetClientMax());
	this->_request.clear();
	this->_response.clear();
	this->_contentlenght = 0;
	this->_id = 0;
	this->_method = "EMPTY";
	
	//get from location
	this->_root = "";
	this->_allow_methods.clear();
	this->_redir_page.clear();
	this->_redirectstate = false;
	this->_index = "";
	
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

void Client::SetRoot(std::string root)
{
	this->_root = root;
}

void Client::SetIndex(std::string index)
{
	this->_index = index;
}

std::string Client::GetIndex()
{
	return (this->_index);
}

void Client::SetMethodVec(std::vector<std::string> method)
{
	this->_allow_methods = method;
}

std::vector<std::string>::iterator Client::GetMethodsBegin()
{
	return (this->_allow_methods.begin());
}

std::vector<std::string>::iterator Client::GetMethodsEnd()
{
	return (this->_allow_methods.end());
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

void Client::SetDonereading(bool state)
{
	this->_donereading = state;
}

bool Client::GetDonereading()
{
	return (this->_donereading);
}

void Client::SetStatusCode(int code)
{
	this->_code = code;
}

int Client::GetStatusCode()
{
	return (this->_code);
}

void Client::SetStatusCodeState(bool state)
{
	this->_isstatuscode = state;
}

bool Client::GetStatusCodeState()
{
	return (this->_isstatuscode);
}

void Client::SetRedirectPage(std::unordered_map<std::string, std::string> pages)
{
	this->_redir_page = pages;
}

void Client::SetRedirectState(bool state)
{
	this->_redirectstate = state;
}

std::unordered_map<std::string, std::string> Client::GetRedirectPage()
{
	return (this->_redir_page);
}

bool Client::GetRedirectState()
{
	return (this->_redirectstate);
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