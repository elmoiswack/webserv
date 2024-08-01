#include "../includes/Server.hpp"
#include "../includes/Cgi.hpp"

std::string Server::WhichMethod(Client *client, std::string method, std::vector<char>::iterator itfirst)
{
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
	this->_iscgi = false;
	if (path == "/" || path == itloc->GetIndex())
		return (this->HtmlToString(client->GetRoot() + itloc->GetIndex(), client));
	else if (path.find("/status_codes/", 0) != path.npos)
		return (this->GetSatusCodeFile(path, client));
	else
		return (this->HtmlToString(this->GetHardCPathCode(404), client));
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