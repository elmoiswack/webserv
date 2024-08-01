#include "../includes/Server.hpp"

void Server::EventsPollin(int fd, Client *client)
{
	logger("POLLIN");
	if (this->_donereading == false)
	{
		this->InitRequest(fd, client);
	}
	if (this->_donereading == true)
	{
		this->BuildResponse(client);
	}
}

void Server::InitRequest(int fd, Client *client)
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
	while (itfirst != this->_request.end() && !std::isspace(*itfirst) && index < 7) {
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
	int ret = this->RecieveMessage(fd, client);
	if (ret == -1)
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
	else if (ret == 0)
	{
		this->_recvzero = true;
		this->_donereading = false;
		this->_request.clear();
		this->_iffirstread = true;
		return ;
	}
}

int Server::RecieveMessage(int fd, Client *client)
{
	logger("Ready to recieve...");
	char buff[client->Getrecvmax()];
	int rbytes = recv(fd, &buff, client->Getrecvmax(), 0);
	std::cout << "Bytes recv = " << rbytes << std::endl;
	if (rbytes == -1)
	{
		logger("ERROR: RECV returned -1!");
		return (-1);
	}
	else if (rbytes == 0)
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

std::string listDirectoryContents(const std::string &directoryPath, const Server &server) {
	std::filesystem::path rootPath = server.GetRoot(); // get root directory
    std::filesystem::path fullPath = rootPath / directoryPath; // concatenate paths

	std::string message_body; // empty string to store 

	// Check if the index file exists and use it if available
	std::string indexFileName = server.GetServerIndex();
	if (!indexFileName.empty()) {
		std::ifstream indexFile(server.GetRoot() + indexFileName);
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
	if (tmp == "EMPTY") {
		if (itloc->GetAutoIndex() == true) {
			std::string dirPath = client->GetRoot() + path;
			return (listDirectoryContents(dirPath, *this));
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
		logger("RECV returned 0, connection closed!");
		return (0);
	}
	
	this->IsFirstRead(client, buff);
	this->IsDoneRead(client, rbytes);
	logger("message recieved!");
	return (1);
}

void Server::IsFirstRead(Client *client, char *buff)
{
	if (this->_iffirstread == true)
	{
		this->_iffirstread = false;
		if (this->GetContentLenght(buff) != -1)
		{
			this->_isbody = true;
			client->SetContentLenght(this->GetContentLenght(buff));
			std::cout << "CLIENT CONTENT LEN = " << client->GetContentLenght() << std::endl;
		}
		else
		{
			this->_isbody = false;
		}
	}
}

long Server::GetContentLenght(char *buff)
{
	std::string tmp(buff);

	int begin = tmp.find("Content-Length:", 0);
	if ((size_t)begin == tmp.npos)
		return (-1);
	if (begin == -1)
		return (0);
	while (!std::isspace(tmp[begin]))
		begin++;
	begin++;
	int end = begin;
	while (std::isdigit(tmp[end]))
		end++;

	std::string numb = tmp.substr(begin, end - begin);
	long body = std::stol(numb);
	begin = tmp.find("Priority:", 0);
	while (tmp[begin] && tmp[begin] != '-')
		begin++;
	
	if ((size_t)begin == tmp.size())
	{
		logger("FOR FUCK SAKE!!!!!!!!!!!!!");
		exit(EXIT_FAILURE);
	}
	
	numb = tmp.substr(0, begin);
	std::string strhead = std::to_string(numb.size());
	long head = std::stol(strhead);
	return (head + body);
}

void Server::IsDoneRead(Client *client, int rbytes)
{
	if (this->_isbody == true && this->_request.size() == (size_t)client->GetContentLenght())
	{
		this->_donereading = true;
		this->_request.push_back('\0');
		logger("Done reading post");
	}
	else if (this->_isbody == false && rbytes < client->Getrecvmax())
	{
		this->_donereading = true;
		this->_request.push_back('\0');
		logger("Done reading get");
	}
}

