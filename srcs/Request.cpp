#include "../includes/Server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../includes/Cgi.hpp"

void Server::EventsPollin(int fd)
{
	logger("POLLIN");
	this->GetResponse(fd);
}
void Server::GetResponse(int fd)
{
	if (this->_donereading == false)
		this->RecieveMessage(fd);
	if (this->_donereading == true)
	{
		std::string htmlfile = this->ParseRequest();
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

std::string Server::ParseRequest()
{
	std::vector<char>::iterator itfirst = this->_request.begin();
	logger("\n\nRequest after reading is done =");
	for (std::vector<char>::iterator bruh = this->_request.begin(); bruh != this->_request.end(); bruh++)
	{
		std::cout << *bruh;
	}
	std::cout << std::endl;
	logger("\n\n");
	char arr[7];
	int index = 0;
	while (!std::isspace(*itfirst))
	{
		arr[index] = *itfirst;
		index++;
		itfirst++;
	}
	arr[index] = '\0';
	std::string method(arr);

	auto it = this->GetClientLocationblockIt();
	std::vector<std::string>::iterator itmethod = it->_allow_methods.begin();
	
	if (method == "GET")
	{
		if (this->IsMethodAllowed(method, itmethod, it) == -1)
			return (this->GetSatusCodeFile(405));
		this->_method = "GET";
		return (this->MethodGet(itfirst));
	}
	else if (method == "POST")
	{
		if (this->IsMethodAllowed(method, itmethod, it) == -1)
			return (this->GetSatusCodeFile(405));
		this->_method = "POST";
		std::string bvruhg = this->MethodPost(itfirst);
		if (bvruhg.size() == 0)
		{
			logger("BRUH FAILED POST");
			exit(EXIT_FAILURE);
		}
		return (bvruhg);
	}
	else if (method == "DELETE")
	{
		if (this->IsMethodAllowed(method, itmethod, it) == -1)
			return (this->GetSatusCodeFile(405));
		this->_method = "DELETE";
	}
	logger("\nCURRENT METHOD DOENS'T EXIST!\n");
	return (this->GetSatusCodeFile(501));
}

int Server::IsMethodAllowed(std::string method, std::vector<std::string>::iterator itmethod, std::vector<Server>::iterator it)
{
	while (itmethod != it->_allow_methods.end())
	{
		if (*itmethod == method)
			break ;
		itmethod++;
	}
	if (itmethod == it->_allow_methods.end())
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

std::string Server::MethodGet(std::vector<char>::iterator itreq)
{
	auto it = this->GetClientLocationblockIt();
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
	
	std::vector<Location>::iterator itloc = it->_locationblocks.begin();	
	this->_iscgi = false;
	if (path == "/" || path == itloc->GetIndex())
		return (this->HtmlToString(it->_root + itloc->GetIndex()));
	else if (path.find("/status_codes/", 0) != path.npos)
		return (this->GetSatusCodeFile(path));
	else
		return (this->GetSatusCodeFile(404));
}

std::string Server::GetSatusCodeFile(int code)
{
	auto it = this->GetClientLocationblockIt();
	std::unordered_map<int, std::string>::iterator iterr = it->_error_page.begin();
	while (iterr != it->_error_page.end() && iterr->first != code)
		iterr++;

	if (iterr == it->_error_page.end())
		return (this->GetSatusCodeFile(404));

	std::string statuscode = it->_root + iterr->second;
	std::cout << "Statuscode = " << statuscode << std::endl;
	return (this->HtmlToString(statuscode));
}

std::string Server::GetSatusCodeFile(std::string path)
{
	auto it = this->GetClientLocationblockIt();
	std::string::iterator begin = path.begin();
	while (!std::isdigit(*begin))
		begin++;
	auto end = begin;
	while (std::isdigit(*end))
		end++;
	std::string strcode(begin, end);
	int code = std::stoi(strcode);
	std::cout << "CODE = " << code << std::endl;
	
	std::unordered_map<int, std::string>::iterator iterr = it->_error_page.begin();
	while (iterr != it->_error_page.end() && iterr->first != code)
		iterr++;
	if (iterr == it->_error_page.end())
		return (this->GetSatusCodeFile(404));
	
	std::string statuscode = it->_root + iterr->second;
	std::cout << "Statuscode = " << statuscode << std::endl;
	return (this->HtmlToString(statuscode));
}

std::string Server::HtmlToString(std::string path)
{
	if (access(path.c_str(), F_OK) == -1)
		return (this->GetSatusCodeFile(404));
	if (access(path.c_str(), R_OK) == -1)
		return (this->GetSatusCodeFile(403));
	
	std::ifstream file(path, std::ios::binary);
	if (!file.good())
	{
		std::cout << "Failed to read file!\n" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

void Server::RecieveMessage(int fd)
{
	auto it = this->GetClientLocationblockIt();
	logger("Ready to recieve...");
	std::cout << "maxrecv = " << it->_recvmax << std::endl;
	char buff[it->_recvmax];
	int rbytes = recv(fd, &buff, it->_recvmax, 0);
	if (rbytes == -1)
	{
		std::cout << "ERROR read" << std::endl;
		exit(EXIT_FAILURE);
	}
	logger("request:");
	for (int i = 0; i < rbytes; i++)
	{
		std::cout << buff[i];
		this->_request.push_back(buff[i]);
	}
	std::cout << std::endl;
	std::cout << "Bytes recv = " << rbytes << std::endl;
	if (rbytes < it->_recvmax && rbytes != 0)
	{
		this->_donereading = true;
		this->_request.push_back('\0');
	}
	logger("message recieved!");
}