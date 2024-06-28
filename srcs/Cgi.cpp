#include "../includes/Cgi.hpp"

bool isCgi(const std::string &url)
{
	// std::string extension;

	// std::cout << "URL = " << url << std::endl;
	// size_t i = url.find("."); 
	// while (url[i] != ' ' && url[i] != '?')
	// 	extension.push_back(url[i++]);
	// std::cout << "aEXTENSION = " << extension << std::endl;
	// if (extension != ".cgi")
	// 	return (false);
	// return (true);
	if (url.find(".cgi") != url.npos)
		return (true);
	return (false);
}

Cgi::Cgi()
{
	std::cout << "CGI CONSTRUCTED\n";
	_initPipes();
}

// Cgi::Cgi(char *client_resp, const std::string &url) : 
// 	_cgiEnvVars(this->initCgiEnvVars(client_resp, url)),
// 	_cgiEnvVarsCstyle(initCgiEnvVarsCstyle())
// {

// }

Cgi::~Cgi()
{
	for (char *env : _cgiEnvVarsCstyle)
		delete[] env;
	std::cout << "CGI DESCTRUCTED\n";
}


void	Cgi::setCgiEnvVars(const std::vector<std::string> &vars)
{
	_cgiEnvVars = vars;
}

void	Cgi::setCgiEnvVarsCstyle(const std::vector<char *> &vars_cstyle)
{
	_cgiEnvVarsCstyle = vars_cstyle;
}


std::string Cgi::constructCgiPath(const std::string &url)
{
	std::string path;
	path.append("./var/www");
	size_t pos = url.find_first_of('?');
	if (pos == std::string::npos)
		path.append(url);
	else
		path.insert(path.size(), url, 0, pos);
	return (path);
}

std::string Cgi::extractReqUrl(const std::string &url)
{
	std::string path;
	size_t i = url.find("/");
	while (url[i])
		path.push_back(url[i++]);
	return (path);
}

std::string	Cgi::readPipe(int fd)
{
	std::ostringstream oss;
	char buffer[1200];
	ssize_t bytes_read = 0;
	while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
		oss.write(buffer, bytes_read); // -> append read data to the output string stream
	close(fd);
	// std::cout << oss.str() << "\n";
	return (oss.str());
}

std::vector<std::string>Cgi::initCgiEnvVars(const std::string &client_resp, const std::string &url)
{
	(void) client_resp;
	std::vector<std::string> env_vars = 
	{	
    	"CONTENT_LENGTH=",
   		"CONTENT_TYPE=multipart/form-data; boundary=----WebKitFormBoundaryRSs5b6yEDT0Vouq9",
   		// "CONTENT_TYPE=" + this->extractContentType(client_resp),
		"GATEWAY_INTERFACE=CGI/1.1",
		"QUERY_STRING=" + this->extractQueryString(url),
		"UPLOAD_FILENAME=test.txt",
		"REQUEST_METHOD=POST",
		"REMOTE_ADDR=",
		"SCRIPT_NAME=",
		"SCRIPT_FILENAME=",
		"SERVER_NAME=",
		"SERVER_PORT=",
		"SERVER_PROTOCOL=HTTP/1.1",
    	"HTTP_COOKIE=",
    	// "REMOTE_ADDR", "192.168.1.100"
	};
	// for (const std::string &env : env_vars)
	// 	std::cout << env << "\n";
	return (env_vars);
}


std::vector<char *> Cgi::initCgiEnvVarsCstyle()
{
	std::vector<char *> env_vars;

	for (const std::string &env_var : _cgiEnvVars)
	{
		char *env_copy = new char[env_var.length() + 1];
		std::copy(env_var.begin(), env_var.end(), env_copy);
		env_copy[env_var.length()] = '\0';
		env_vars.push_back(env_copy);
	}
	env_vars.push_back(nullptr);
	return (env_vars);
}

std::string	Cgi::extractQueryString(const std::string &url)
{
	std::string querry_str;
	size_t pos = url.find('?');
	size_t end = url.find_last_of(' ');
	if (pos != std::string::npos)
	{

		querry_str = (end != std::string::npos)
			? url.substr(pos + 1, end - pos - 1)
			: url.substr(pos + 1, url.length() - pos - 1);
	}
	return (querry_str);
}

std::string	Cgi::extractContentType(const std::string &req)
{
	if (req.empty())
		return ("");
	std::string contentType;
	std::size_t pos = req.find("Content-Type:");
    if (pos != std::string::npos) {
        pos += sizeof("Content-Type:");
        std::size_t endPos = req.find("\r\n", pos);
        if (endPos != std::string::npos)
		{
			contentType = req.substr(pos, endPos - pos);
			//std::cout << "\n--CONTENT TYPE: " << contentType << "\n\n";
            return (contentType);
        }
    }
    return "";
}

std::string Cgi::runCgi(const std::string &cgi_path)
{
    pid_t pid = fork();
    if (pid == -1)
	{
        std::cout << "ERROR CREATING CHILD PROCESS\n";
        exit(EXIT_FAILURE);            
    }
	else if (pid == 0) 	// Child process
	{
        close(_responsePipe[0]); 					// Close read end of response pipe
        close(_uploadPipe[1]); 						// Close write end of upload pipe
        dup2(_responsePipe[1], STDOUT_FILENO); 		// Redirect cgi stdout to write end of response pipe
        dup2(_uploadPipe[0], STDIN_FILENO); 		// Redirect cgi stdin to read end of upload pipe
	    const char *args[] = {cgi_path.c_str(), NULL};
        if (execve(cgi_path.c_str(), const_cast<char**>(args), _cgiEnvVarsCstyle.data()) == -1)
		{
            std::cout << "ERROR EXECUTING CGI SCRIPT\n";
            std::exit(EXIT_FAILURE);            
        }
    }
	else  											// Parent process
	{
		_pid = pid; 								// save pid for further processing if needed
		std::string boundary = "----WebKitFormBoundaryRSs5b6yEDT0Vouq9";
    	std::string post_data = "--" + boundary + "\r\n"
                            	"Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"\r\n"
                            	"Content-Type: text/plain\r\n\r\n"
                            	"TEST\r\n"
                            	"TEST\r\n"
                            	"TEST\r\n"
                            	"--" + boundary + "--\r\n";
		std::cout << "\n\nPOST size: " << post_data.size() << "\n\n";
        close(_responsePipe[1]);					// Close write end of CGI response pipe
		// !!HAS TO BE RAN THROUGH POLL, CAN BE DONE OUTSIDE OF THIS SCOPE!!
        close(_uploadPipe[0]);						// Close read end of upload pipe
        write(_uploadPipe[1], post_data.c_str(), post_data.size()); // Write POST data to CGI via upload pipe
        close(_uploadPipe[1]);						// Close write end of upload pipe after writing to cgi

        int status;
        pid_t result = waitpid(pid, &status, 0);
        if (result == -1) {
            std::cout << "ERROR PARENT PROCESS\n";
            exit(EXIT_FAILURE);            
        }
        if (WIFEXITED(status)) {
            std::cout << "Child process exited with status: " << WEXITSTATUS(status) << "\n";
            return (readPipe(_responsePipe[0])); 	// !!HAS TO BE RAN THROUGH POLL!!
        } else {
            std::cout << "Child process exited abnormally" << "\n";
        }
    }
    return "";
}

void Cgi::_initPipes()
{
    if (pipe(_responsePipe) == -1 || pipe(_uploadPipe) == -1)
	{
        std::cout << "ERROR PIPE\n";
        std::exit(EXIT_FAILURE);
    }
}


// std::string Cgi::runCgi(const std::string &cgi_path)
// {
// 	int responsePipeFd[2];
//     int uploadPipeFd[2];

//     if (pipe(responsePipeFd) == -1 || pipe(uploadPipeFd) == -1) {
//         std::cout << "ERROR PIPE\n";
//         std::exit(EXIT_FAILURE);
//     }

//     pid_t pid = fork();
//     if (pid == -1)
// 	{
//         std::cout << "ERROR CREATING CHILD PROCESS\n";
//         exit(EXIT_FAILURE);            
//     }
// 	else if (pid == 0)
// 	{ // Child process
//         close(responsePipeFd[0]); // Close read end of response pipe
//         dup2(responsePipeFd[1], STDOUT_FILENO); // Redirect stdout to write end of response pipe

//         close(uploadPipeFd[1]); // Close write end of upload pipe
//         dup2(uploadPipeFd[0], STDIN_FILENO); // Redirect stdin to read end of upload pipe
	
//         const char *args[] = {"/usr/bin/python3", cgi_path.c_str(), NULL};
//         if (execve("/usr/bin/python3", const_cast<char**>(args), nullptr) == -1) {
//             std::cout << "ERROR EXECUTING CGI SCRIPT\n";
//             std::exit(EXIT_FAILURE);            
//         }
//     }
// 	else
// 	{ // Parent process
	
//         std::string post_data = "------WebKitFormBoundaryRSs5b6yEDT0Vouq9\r\n"
// 								"Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"\r\n"
// 								"Content-Type: text/plain\r\n\r\n"
// 								"TEST\r\n"
// 								"TEST\r\n"
// 								"TEST\r\n"
// 								"TEST\r\n"
//                                 "------WebKitFormBoundaryRSs5b6yEDT0Vouq9--\r\n";
// 		std::cout << "\n\nPOST size: " << post_data.size() << "\n\n";
//         close(responsePipeFd[1]); // Close write end of response pipe

//         close(uploadPipeFd[0]); // Close read end of upload pipe
//         write(uploadPipeFd[1], post_data.c_str(), post_data.size()); // Write POST data to upload pipe
//         close(uploadPipeFd[1]); // Close write end of upload pipe after writing

//         int status;
//         pid_t result = waitpid(pid, &status, 0);
//         if (result == -1) {
//             std::cout << "ERROR PARENT PROCESS\n";
//             exit(EXIT_FAILURE);            
//         }
//         if (WIFEXITED(status)) {
//             std::cout << "Child process exited with status: " << WEXITSTATUS(status) << "\n";
//             return (readPipe(responsePipeFd[0]));
//         } else {
//             std::cout << "Child process exited abnormally" << "\n";
//         }
//     }
//     return "";
// }


// std::string Cgi::runCgi(const std::string &cgi_path)
// {
// 	int	pipefd[2];

// 	if (pipe(pipefd) == -1)
// 	{
// 		std::cout << "ERROR PIPE\n";
// 		std::exit(EXIT_FAILURE);
// 	}
// 	pid_t pid = fork();
// 	if (pid == -1)
// 	{
// 		std::cout << "ERROR CREATING CHILD PROCESS\n";
//         exit(EXIT_FAILURE);			
// 	}
// 	else if (pid == 0)											//-> child process
// 	{
// 		//std::cout << "\n\n+++++++++++++++\n\n";
// 		close(pipefd[0]); 										// -> close read end of pipe, only need to write
// 		dup2(pipefd[1], STDOUT_FILENO); 						// -> redirect stdout to write end of pipe
// 		const char *args[] = {"/usr/bin/python3", cgi_path.c_str(), NULL};
// 		if (execve("/usr/bin/python3", const_cast<char**>(args), _cgiEnvVarsCstyle.data()) == -1)
// 		{
// 			std::cout << "ERROR EXECUTING CGI SCRIPT\n";
//         	std::exit(EXIT_FAILURE);			
// 		}
// 	}
// 	else														//-> parent process
// 	{
// 		close(pipefd[1]); 										// -> close write end of pipe, only need to read
// 		int status;
// 		pid_t result = waitpid(pid, &status, 0);
// 		if (result == -1)
// 		{
// 			std::cout << "ERROR PARENT PROCESS\n";
//             exit(EXIT_FAILURE);			

// 		}
// 		if (WIFEXITED(status))
// 		{
// 			std::cout << "Child process exited with status: " << WEXITSTATUS(status) << "\n";
// 			return (readPipe(pipefd[0]));
// 		}
// 		else
// 		{
//             std::cout << "Child process exited abnormally" << "\n";
// 		}
// 	}
// 	return (nullptr);
// }


// bool isCgi(const std::string &url)
// {
// 	std::string extension;

// 	size_t i = url.find("."); 
// 	while (url[i] != ' ' && url[i] != '?')
// 		extension.push_back(url[i++]);
// 	if (extension != ".cgi")
// 		return (false);
// 	return (true);
// }

// ------OLD SERVER------//
// void Server::StartServer()
// {
// 	this->_websock = socket(AF_INET, SOCK_STREAM, 0);
// 	if (this->_websock < 0)
// 	{
// 		std::cout << "ERROR" << std::endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	struct sockaddr_in infoaddr;
// 	memset(&infoaddr, '\0', sizeof(infoaddr));
// 	infoaddr.sin_family = AF_INET;
// 	infoaddr.sin_addr.s_addr = INADDR_ANY;
// 	infoaddr.sin_port = htons(std::atoi(this->_port.c_str()));	
// 	if (bind(this->_websock, (struct sockaddr *)&infoaddr, sizeof(infoaddr)) == -1)
// 	{
// 		std::cout << "ERROR BIND" << std::endl;
// 		exit(EXIT_FAILURE);
// 	}
// 	if (listen(this->_websock, 5) == -1)
// 	{
// 		std::cout << "ERROR LISTEN" << std::endl;
// 		exit(EXIT_FAILURE);		
// 	}

// 	// keeps it running
// 	long newsock;
// 	while (true)
//     {
// 		struct sockaddr_in client_addr;
//   		int client_addr_len = sizeof(client_addr);

//        	newsock = accept(this->_websock, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
//         if (newsock == -1)
//         {
//             std::cout << "ERROR ACCEPT" << std::endl;
//             exit(EXIT_FAILURE);			
//         }
		 
// 		char buffer[1024] = { 0 }; 
// 		recv(newsock, buffer, sizeof(buffer), 0); 
// 		std::string response;		
// 		std::string html_file = readFile("./var/www/index.html");
// 		response =
// 		"HTTP/1.1 200 OK\r\n"
// 		"Content-Type: text/html\r\n"
// 		"Content-Length: " + std::to_string(html_file.length()) + "\r\n"
// 		"\r\n"
// 		+ html_file;
//        	 	// Send the blank HTML page response again for new connections
//     		// write(newsock, response.c_str(), response.size());
// 		// }
//     	//write(newsock, response.c_str(), response.size());
// 		if (isCgi(std::string(buffer)))
// 		{
// 			Cgi cgi;
// 			response.clear();
// 			std::string req_url = cgi.extractReqUrl(buffer);
// 			std::string cgi_path = cgi.constructCgiPath(req_url);
// 			cgi.setCgiEnvVars(cgi.initCgiEnvVars(buffer, req_url));
// 			cgi.setCgiEnvVarsCstyle(cgi.initCgiEnvVarsCstyle());
// 			// std::cout << "\nREQUEST URL: " << req_url << "\n";
// 			// std::cout << "\nCGI PATH: " << cgi_path << "\n\n";
// 			// std::cout << "\n---QUERY_STRING: " << cgi.extractQueryString(req_url) << "\n\n\n";
// 			response = cgi.runCgi(cgi_path);
// 			std::cout << "\n--------------------------\n";
// 			std::cout << "RESPONSE: \n\n" << response;
// 			std::cout << "--------------------------\n";
// 		}
//     	write(newsock, response.c_str(), response.size());
//         close(newsock);
//     }
// }