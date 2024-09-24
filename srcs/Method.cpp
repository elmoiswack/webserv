#include "../includes/Server.hpp"
#include "../includes/Cgi.hpp"
#include <filesystem> 
#include <fstream> 
#include <iostream> 

std::string Server::WhichMethod(Client *client, std::vector<char>::iterator itfirst)
{
	std::string method = client->GetCurrentMethod();
	std::string path = this->GetPath(itfirst);

	std::cout << "METHOD = " << method << std::endl;
	std::cout << "PATH = " << path << std::endl;

	auto it = this->GetLocationBlock(client, path);
	if (it == client->GetLocationblockEnd())
		return (this->HtmlToString(this->GetHardCPathCode(500, client), client));
	this->SetClientVars(client, it);
	
	if (method == "GET")
	{
		if (this->IsMethodAllowed(method, client) == -1)
			return (this->HtmlToString(this->GetHardCPathCode(405, client), client));
		return (this->MethodGet(path, client));
	}
	else if (method == "POST")
	{
		if (this->IsMethodAllowed(method, client) == -1)
			return (this->HtmlToString(this->GetHardCPathCode(405, client), client));
		return (this->MethodPost(path, client));
	}
	else if (method == "DELETE")
	{
		if (this->IsMethodAllowed(method, client) == -1)
			return (this->HtmlToString(this->GetHardCPathCode(405, client), client));
		return (this->MethodDelete(path, client));
	}
	logger("Current method isn't implemented!");
	return (this->HtmlToString(this->GetHardCPathCode(501, client), client));
}

std::string Server::GetPath(std::vector<char>::iterator itfirst)
{
	while (std::isspace(*itfirst))
		itfirst++;
	std::vector<char>::iterator itend = itfirst;
	while (!std::isspace(*itend))
		itend++;
	std::string path;
	path.assign(itfirst, itend);
	return (path);
}

void Server::SetClientVars(Client *client, std::vector<Location>::iterator it)
{
	client->SetLocationName(it->GetURL());
	client->SetMethodVec(it->Get_AllowMethods());
	client->SetAutoindex(it->GetAutoIndex());
	client->SetRoot(it->GetLocRoot());
	client->SetIndex(it->GetIndex());
	std::string tmp = it->GetReturnRedirect();
	if (tmp.size() > 0)
	{
		client->SetReturnState(true);
		client->Setreturn(tmp);
		client->SetReturnCode(it->GetReturnRedirectCode());
	}
	else
		client->SetReturnState(false);
}

std::vector<Location>::iterator Server::GetLocationBlock(Client *client, std::string path)
{
	auto it = client->GetLocationblockEnd();
	it--;
	while (it != client->GetLocationblockBegin())
	{
		if (path.find(it->GetURL()) != path.npos)
			break;
		it--;
	}
	if (path.find(it->GetURL()) != path.npos)
		return (it);
	return (client->GetLocationblockEnd());
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

std::string Server::MethodDelete(std::string path, Client *client)
{
	path.insert(0, "./var/www");
	logger("PATH: " + path);

	if (std::remove(path.c_str()) == 0)
	{
		logger("FILE DELETED!");
		return (this->HtmlToString("./var/www/file_deleted.html", client));
	}
	logger("ERROR DELETING FILE!");
	return (this->HtmlToString(this->GetHardCPathCode(404, client), client));
}

std::string Server::MethodGet(std::string path, Client *client)
{
	if (isCgi(path))
	{
		if (!Location::GetCGIstatus())
			return (this->HtmlToString(this->GetHardCPathCode(500, client), client));
		this->_cgi_donereading = false;
		std::string tmp(client->GetBeginRequest(), client->GetEndRequest());
		this->_cgi = new Cgi(client->GetCurrentMethod(), path, tmp);
		this->AddSocket(_cgi->getReadEndResponsePipe(), std::string("CGI_READ"));
		this->_iscgi = true;
		if (client->GetResponseSize() > 0)
			client->ClearResponse();
		std::string cgi_path = _cgi->constructCgiPath(path);
		std::chrono::time_point<std::chrono::steady_clock> cgiStartTime = std::chrono::steady_clock::now();
		this->setStartTime(cgiStartTime);
		client->SetResponse(_cgi->runCgi(cgi_path, this));
		this->_cgi_running = true;
		return (client->GetResponse());
	}
	
	this->_iscgi = false;
	if (client->GetIndex() == "EMPTY")
		return (this->GetAutoindex(path, client));
	
	auto pathend = path.back();
	if (pathend == '/' || path == client->GetIndex())
		return (this->HtmlToString(client->GetRoot() + client->GetIndex(), client));
	else if (path.find("/status_codes/", 0) != path.npos)
		return (this->GetSatusCodeFile(path, client));
	else if (client->GetReturnstate() == true)
	{
		if (path == client->GetLocationName())
			return ("");
	}
	logger("Path not found, sending 404!");
	return (this->HtmlToString(this->GetHardCPathCode(404, client), client));
}

std::string Server::listDirectoryContents(const std::string &directoryPath) {
	std::filesystem::path rootPath = this->GetRoot(); // get root directory
    std::filesystem::path fullPath = rootPath / directoryPath; // concatenate paths

	std::string message_body; // empty string to store 
	// Check if the index file exists and use it if available
	std::string indexFileName = this->GetServerIndex();
	if (!indexFileName.empty()) {
		std::ifstream indexFile(this->GetRoot() + indexFileName);
		if (indexFile.is_open()) // if it can be opened it is read
		{
			std::ostringstream ss;
			ss << indexFile.rdbuf();
			message_body = ss.str();
			return (message_body);
		}
		else
		{
			std::cerr << "Error opening index file: " << indexFileName << "\n";
		}
	}
    // Build directory listing if no index file is used
    message_body = "<html>\n<title>" + directoryPath + "</title>\n";
    message_body += "<body><h1>Index of " + directoryPath + "</h1>\n";
    message_body += "<table><tr><th>Name</th></tr>\n";

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(fullPath)) { // iterate through directory
            if (entry.path().filename() == ".") // current directory
                continue;
            std::string filePath = directoryPath;
            if (directoryPath.back() != '/') // if it doesn't have slash add one
                filePath += '/';
            filePath += entry.path().filename().string();
            message_body += "<tr><td><a href=\"" + filePath;
            if (std::filesystem::is_directory(entry.status()))
                message_body += "/";
			message_body += "\">" + entry.path().filename().string();
            if (std::filesystem::is_directory(entry.status()))
                message_body += "/";
            message_body += "</a></td></tr>\n";
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << "\n";
    }
    message_body += "</table></body>\n</html>\n"; // close html
	return (message_body);
}

std::string Server::GetAutoindex(std::string path, Client *client)
{
	if (client->GetAutoindex() == true) 
	{
		std::cout << "AUTOINDEX PATH = " << path << std::endl;
		if (path == "/" || path == "/index.html")
			return (this->listDirectoryContents(client->GetRoot()));
		else if (path.back() == '/')
		{
			path.insert(0, ".");
			return (this->listDirectoryContents(path));
		}
		else
			return(this->HtmlToString(this->GetHardCPathCode(403, client), client));
	}
	if (client->GetAutoindex() == false)
		return (this->HtmlToString(this->GetHardCPathCode(403, client), client));
	return ("");
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

std::vector<char> Server::ParsePostV(const std::string &content) {
	std::string content_type_header = "Content-Type: multipart/form-data; boundary=";
    size_t content_type_start = content.find(content_type_header);
    if (content_type_start == std::string::npos) {
		std::cout << "Content-Type not found" << std::endl;
        return {};
    }
    content_type_start += content_type_header.length();
    size_t content_type_end = content.find("\n", content_type_start);
    std::string content_type = content.substr(content_type_start, content_type_end - content_type_start);

	//   std::cout << "Content-Type: " << content_type << std::endl;

    std::string boundary = ExtractBoundary(content);
    if (boundary.empty()) {
		std::cout << "Boundary not found" << std::endl;
        return {};
    }

	// std::cout << "Boundary: " << boundary << std::endl;

    std::string boundary_start = "--" + boundary;
    std::string boundary_end = boundary_start + "--";
	
    size_t start_pos = content.find(boundary_start);
    size_t end_pos = content.find(boundary_end);
    if (start_pos == std::string::npos || end_pos == std::string::npos || start_pos >= end_pos) {
		std::cout << "Boundary positions not found" << std::endl;
        return {};
    }
	std::vector<char> post_data;
    for (size_t i = start_pos; i < end_pos + boundary_end.length(); ++i)
	{
		post_data.push_back(content[i]);
		// std::cout << content[i];
	}
    // Include boundary_start in the extracted data
	// start_pos += boundary_start.length();
    // std::string post_data = content.substr(start_pos, end_pos - start_pos + boundary_end.length());
    
    return post_data;
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

std::string Server::MethodPost(std::string path, Client *client)
{
	if (isCgi(path))
	{
		if (!Location::GetCGIstatus())
			return (this->HtmlToString(this->GetHardCPathCode(500, client), client));
		this->_cgi_donereading = false;
		std::string tmp(client->GetBeginRequest(), client->GetEndRequest());
		this->_post_data = ParsePost(tmp);
		this->_cgi = new Cgi(client->GetCurrentMethod(), this->_post_data, path, tmp);	
		this->AddSocket(_cgi->getReadEndResponsePipe(), std::string("CGI_READ"));
		this->AddSocket(_cgi->getWriteEndUploadPipe(), std::string("CGI_WRITE"));
		this->_iscgi = true;
		if (client->GetResponseSize() > 0)
			client->ClearResponse();
		std::string cgi_path = _cgi->constructCgiPath(path);
		std::chrono::time_point<std::chrono::steady_clock> cgiStartTime = std::chrono::steady_clock::now();
		this->setStartTime(cgiStartTime);
		_cgi->runCgi(cgi_path, this);
		this->_cgi_running = true;
		return (client->GetResponse());
	}
	return (this->HtmlToString(this->GetHardCPathCode(400, client), client));
}