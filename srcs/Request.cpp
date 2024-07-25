#include "../includes/Server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../includes/Cgi.hpp"

void Server::EventsPollin(int fd, Client *client)
{
	logger("POLLIN");
	this->GetResponse(fd, client);
}
void Server::GetResponse(int fd, Client *client)
{
	if (this->_donereading == false)
	{
		if (this->RecieveMessage(fd, client) == -1)
		{
			std::string errfile = this->HtmlToString(this->GetHardCPathCode(400), client);
			this->_response = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " + std::to_string(errfile.length()) + "\r\n"
			"\r\n"
			+ errfile;
			this->_request.clear();
			return ;
		}
	}
	if (this->_donereading == true)
	{
		std::string htmlfile = this->ParseRequest(client);
		if (this->_iscgi == false)
		{
			this->_response = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " + std::to_string(htmlfile.length()) + "\r\n"
			"\r\n"
			+ htmlfile;
			this->_request.clear();
		}
		else if (this->_iscgi == true)
		{
			this->_response = htmlfile;
			this->_iscgi = false;
			this->_request.clear();
		}
		this->_donereading = false;
	}
}

std::string Server::ParseRequest(Client *client)
{
	std::vector<char>::iterator itfirst = this->_request.begin();
	logger("\n\nRequest after reading is done =");
	for (std::vector<char>::iterator print = this->_request.begin(); print != this->_request.end(); print++)
	{
		std::cout << *print;
	}
	std::cout << std::endl;
	logger("\n\n");
	char arr[7];
	int index = 0;
	if (std::isspace(*itfirst))
	{
		while (std::isspace(*itfirst))
			itfirst++;
	}
	if (itfirst == this->_request.end())
		return (this->HtmlToString(this->GetHardCPathCode(400), client));
	while (!std::isspace(*itfirst) && index < 7)
	{
		arr[index] = *itfirst;
		index++;
		itfirst++;
	}
	arr[index] = '\0';
	std::string method(arr);

	if (method == "GET")
	{
		if (this->IsMethodAllowed(method, client) == -1)
			return (this->HtmlToString(this->GetHardCPathCode(405), client));
		this->_method = "GET";
		return (this->MethodGet(itfirst, client));
	}
	else if (method == "POST")
	{
		if (this->IsMethodAllowed(method, client) == -1)
			return (this->HtmlToString(this->GetHardCPathCode(405), client));
		this->_method = "POST";
		std::string response = this->MethodPost(itfirst);
		return (response);
	}
	else if (method == "DELETE")
	{
		if (this->IsMethodAllowed(method, client) == -1)
			return (this->HtmlToString(this->GetHardCPathCode(405), client));
		this->_method = "DELETE";
	}
	logger("\nCURRENT METHOD DOENS'T EXIST!\n");
	return (this->HtmlToString(this->GetHardCPathCode(501), client));
}

int Server::IsMethodAllowed(std::string method, Client *client)
{
	auto begin = client->GetMethodsBegin();
	auto end = client->GetMethodsEnd();
	while (begin != end)
	{
		std::cout <<"ALLOWED METHODS: " << *begin << std::endl;
		if (*begin == method)
			break ;
		begin++;
	}
	if (begin == end)
	{
		std::cout << "Method: " << method << " isn't allowed!" << std::endl;
		return (-1);
	}
	return (1);
}

std::string Server::ExtractBoundary(const std::string &content) {

	std::string boundary_prefix = "boundary=";
    size_t boundary_start = content.find(boundary_prefix);
    if (boundary_start == std::string::npos) {
        return "";
    }

    boundary_start += boundary_prefix.length(); // adjust start to point of start boundary
    
    size_t boundary_end = content.find("\n", boundary_start);
    // if (boundary_end == std::string::npos) { 
    //     boundary_end = content.length();
    // }

    // trim white space
    std::string boundary = content.substr(boundary_start, boundary_end - boundary_start);
    boundary.erase(boundary.find_last_not_of(" \t\n\r\f\v") + 1);
    
    return (boundary);
}

std::string Server::ParsePost(const std::string &content) {
	std::string content_type_header = "Content-Type: multipart/form-data; boundary=";
    size_t content_type_start = content.find(content_type_header);
    if (content_type_start == std::string::npos) {
		std::cout << "Content-Type not found" << std::endl;
        return ("");
    }
    content_type_start += content_type_header.length();
    size_t content_type_end = content.find("\n", content_type_start);
    std::string content_type = content.substr(content_type_start, content_type_end - content_type_start);

	//   std::cout << "Content-Type: " << content_type << std::endl;

    std::string boundary = ExtractBoundary(content);
    if (boundary.empty()) {
		std::cout << "Boundary not found" << std::endl;
        return ("");
    }

	// std::cout << "Boundary: " << boundary << std::endl;

    std::string boundary_start = "--" + boundary;
    std::string boundary_end = boundary_start + "--";
	
    size_t start_pos = content.find(boundary_start);
    size_t end_pos = content.find(boundary_end);
    if (start_pos == std::string::npos || end_pos == std::string::npos || start_pos >= end_pos) {
		std::cout << "Boundary positions not found" << std::endl;
        return ("");
    }
    
    // Include boundary_start in the extracted data
	// start_pos += boundary_start.length();
    std::string post_data = content.substr(start_pos, end_pos - start_pos + boundary_end.length());
    
    return post_data;
}

std::string Server::MethodPost(std::vector<char>::iterator itreq)
{
	while (std::isspace(*itreq))
		itreq++;
	std::vector<char>::iterator itend = itreq;
	while (!std::isspace(*itend))
		itend++;
	std::string path;
	path.assign(itreq, itend);
	logger(path);
	if (isCgi(path))
	{
		std::string tmp(this->_request.begin(), this->_request.end());
		std::string post_data = ParsePost(tmp);
		Cgi cgi(_method, post_data, path, tmp);
		this->_iscgi = true;
		if (this->_response.size() > 0)
			this->_response.clear();
		std::string cgi_path = cgi.constructCgiPath(path);
		this->_response = cgi.runCgi(cgi_path);
		// std::cout << "\nPOST DATA:\n" << post_data << "\n\n";
		// std::cout << "RESPONSE: \n\n" << this->_response;
		return (this->_response);
	}
	return ("");
}

std::string listDirectoryContents(const std::string &directoryPath) {

}

std::string Server::MethodGet(std::vector<char>::iterator itreq, Client *client)
{
	while (std::isspace(*itreq))
		itreq++;
	std::vector<char>::iterator itend = itreq;
	while (!std::isspace(*itend))
		itend++;
	std::string path;
	path.assign(itreq, itend);
	logger(path);

	if (isCgi(path))
	{
		std::string tmp(this->_request.begin(), this->_request.end());
		Cgi cgi(_method, path, tmp);
		this->_iscgi = true;
		if (this->_response.size() > 0)
			this->_response.clear();
		std::string cgi_path = cgi.constructCgiPath(path);
		this->_response = cgi.runCgi(cgi_path);
		// std::cout << "RESPONSE: \n\n" << this->_response;
		return (this->_response);
	}
	std::vector<Location>::iterator itloc = client->GetLocationblockBegin();

	std::string tmp;

	tmp = itloc->GetIndex();
	if (tmp.size() == 0) {
		if (itloc->GetAutoIndex() == true) {
			std::string dirPath = client->GetRoot() + path;
			return (listDirectoryContents(dirPath));
		}
		if (itloc->GetAutoIndex() == false)
			return (this->HtmlToString(this->GetHardCPathCode(403), client));
	}
	this->_iscgi = false;
	if (path == "/" || path == itloc->GetIndex())
		return (this->HtmlToString(client->GetRoot() + itloc->GetIndex(), client));
	else if (path.find("/status_codes/", 0) != path.npos)
		return (this->GetSatusCodeFile(path, client));
	else
		return (this->HtmlToString(this->GetHardCPathCode(404), client));
}

std::string Server::GetHardCPathCode(int code)
{
	logger("GETTING HARDCODED PATH TO ERRORFILE!");
	std::cout << "Getting " << code << " html file!" << std::endl;
	std::unordered_map<int, std::string>::iterator it = this->_hcerr_page.begin();
	while (it != this->_hcerr_page.end())
	{
		if (it->first == code)
			break ;
		it++;
	}
	if (it == this->_hcerr_page.end())
	{
		logger("Code passed isn't valid! Internal server error!");
		for (auto iterr = this->_hcerr_page.begin(); iterr != this->_hcerr_page.end(); iterr++)
		{
			if (iterr->first == 500)
				return (iterr->second);
		}
	}
	return (it->second);
}

std::string Server::GetSatusCodeFile(std::string path, Client *client)
{
	std::string::iterator begin = path.begin();
	while (begin != path.end() && !std::isdigit(*begin))
		begin++;
	if (begin == path.end())
	{
		logger("jdksa\n");
		return (this->HtmlToString(this->GetHardCPathCode(404), client));
	}
	auto end = begin;
	while (std::isdigit(*end))
		end++;
	std::string strcode(begin, end);
	int code = std::stoi(strcode);
	std::cout << "CODE = " << code << std::endl;

	std::unordered_map<int, std::string>::iterator iterr = client->GetErrorpageBegin();
	while (iterr != client->GetErrorpageEnd() && iterr->first != code)
		iterr++;
	if (iterr == client->GetErrorpageEnd())
	{
		return (this->HtmlToString(this->GetHardCPathCode(404), client));
	}
	
	std::string statuscode = client->GetRoot() + iterr->second;
	std::cout << "Statuscode = " << statuscode << std::endl;
	return (this->HtmlToString(statuscode, client));
}

std::string Server::HtmlToString(std::string path, Client *client)
{
	if (access(path.c_str(), F_OK) == -1)
		return (this->HtmlToString(this->GetHardCPathCode(404), client));
	if (access(path.c_str(), R_OK) == -1)
		return (this->HtmlToString(this->GetHardCPathCode(403), client));
	
	std::ifstream file(path, std::ios::binary);
	if (!file.good())
	{
		std::cout << "Failed to read file!\n" << std::endl;
		return (this->HtmlToString(this->GetHardCPathCode(404), client));
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

int Server::RecieveMessage(int fd, Client *client)
{
	logger("Ready to recieve...");
	std::cout << "maxrecv = " << client->Getrecvmax() << std::endl;
	char buff[client->Getrecvmax()];
	int rbytes = recv(fd, &buff, client->Getrecvmax(), 0);
	if (rbytes == -1)
	{
		logger("ERROR: RECV returned -1!");
		return (-1);
	}
	logger("request:");
	for (int i = 0; i < rbytes; i++)
	{
		std::cout << buff[i];
		this->_request.push_back(buff[i]);
	}
	std::cout << std::endl;
	std::cout << "Bytes recv = " << rbytes << std::endl;
	if (rbytes < client->Getrecvmax())
	{
		this->_donereading = true;
		this->_request.push_back('\0');
	}
	logger("message recieved!");
	return (1);
}
