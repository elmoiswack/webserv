#include "../includes/Cgi.hpp"
#include "../includes/Server.hpp"

bool isCgi(const std::string &url)
{
	if (url.find(".cgi") != url.npos)
		return (true);
	return (false);
}

Cgi::Cgi()
{
	std::cout << "--CGI CONSTRUCTED\n";
	_initPipes();
}

Cgi::Cgi(const std::string &method, const std::string &post_data, const std::string &path, const std::string &request) :
    _method(method),
    _postData(post_data),
	_cgiEnvVars(initCgiEnvVars(request, path)),
	_cgiEnvVarsCstyle(initCgiEnvVarsCstyle())
{
	_initPipes();
}

Cgi::Cgi(const std::string &method, const std::string &path, const std::string &request) :
    _method(method),
	_cgiEnvVars(initCgiEnvVars(request, path)),
	_cgiEnvVarsCstyle(initCgiEnvVarsCstyle())
{
	_initPipes();
}

Cgi::Cgi(const Cgi& obj) :
	_method(obj._method),
	_postData(obj._postData),
	_cgiEnvVars(obj._cgiEnvVars),
	_pid(obj._pid)
{
	// _initPipes();
	// std::copy(std::begin(obj._uploadPipe), std::end(obj._uploadPipe), _uploadPipe);
	// std::copy(std::begin(obj._responsePipe), std::end(obj._responsePipe), _responsePipe);
	_cgiEnvVarsCstyle = this->initCgiEnvVarsCstyle();
}

Cgi &Cgi::operator=(const Cgi& obj)
{
	// _initPipes();
	if (this != &obj)
	{
    	_method = obj._method;
    	_postData = obj._postData;
    	_cgiEnvVars = obj._cgiEnvVars;
    	_pid = obj._pid;
    	// std::copy(std::begin(obj._uploadPipe), std::end(obj._uploadPipe), _uploadPipe);
    	// std::copy(std::begin(obj._responsePipe), std::end(obj._responsePipe), _responsePipe);
    	_cgiEnvVarsCstyle = this->initCgiEnvVarsCstyle();
    }
    return (*this);
}


// Cgi::Cgi(char *client_resp, const std::string &url) : 
// 	_cgiEnvVars(this->initCgiEnvVars(client_resp, url)),
// 	_cgiEnvVarsCstyle(initCgiEnvVarsCstyle())
// {

// }



Cgi::~Cgi()
{
	for (const char* env : _cgiEnvVarsCstyle)
		delete[] env;
	std::cout << "--CGI DESCTRUCTED\n";
}


void	Cgi::setCgiEnvVars(const std::vector<std::string> &vars)
{
	_cgiEnvVars = vars;
}

void	Cgi::setCgiEnvVarsCstyle(const std::vector<char *> &vars_cstyle)
{
	_cgiEnvVarsCstyle = vars_cstyle;
}

void	Cgi::setPostData(const std::string &post_data)
{
	_postData = post_data;
}

void	Cgi::setMethod(const std::string &method)
{
	_method = method;
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

// std::string	Cgi::readCgiResponse(int fd)
// {
// 	std::ostringstream oss;
// 	char buffer[1200];
// 	ssize_t bytes_read = 0;
// 	while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
// 		oss.write(buffer, bytes_read); // -> append read data to the output string stream
// 	close(fd);
// 	// std::cout << oss.str() << "\n";
// 	return (oss.str());
// }

// std::string	Cgi::readCgiResponse(int fd)
// {
// 	std::ostringstream oss;
// 	char buffer[1200];
// 	ssize_t bytes_read = -1;
// 	while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
// 		oss.write(buffer, bytes_read); // -> append read data to the output string stream
// 	close(fd);
// 	// std::cout << oss.str() << "\n";
// 	return (oss.str());
// }

// void Cgi::writeToCgi(int fd)
// {
// 	close(this->_uploadPipe[0]);
// 	logger("----CGI POLLOUT\n\n");
// 	write(temp.fd, this->_post_data.c_str(), this->_post_data.size());
// 	this->RmvSocket(index);
// 	this->_response.clear();
// }

std::string extractBoundary(const std::string &content) {

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

std::vector<std::string>Cgi::initCgiEnvVars(const std::string &client_resp, const std::string &url)
{
	std::vector<std::string> env_vars = 
	{	
    	"CONTENT_LENGTH=",
   		// "CONTENT_TYPE=multipart/form-data; boundary=" + extractBoundary(client_resp),
   		"CONTENT_TYPE=" + this->extractContentType(client_resp),
		"GATEWAY_INTERFACE=CGI/1.1",
		"QUERY_STRING=" + this->extractQueryString(url),
		"UPLOAD_FILENAME=test.txt",
		"REQUEST_METHOD=" + this->_method,
		"REMOTE_ADDR=",
		"SCRIPT_NAME=",
		"SCRIPT_FILENAME=",
		"SERVER_NAME=",
		"SERVER_PORT=",
		"SERVER_PROTOCOL=HTTP/1.1",
    	"HTTP_COOKIE=",
    	// "REMOTE_ADDR", "192.168.1.100"
	};
	logger("CGI ENV VARS: \n\n");
	for (const std::string &env : env_vars)
		std::cout << env << "\n";
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

		querry_str = end != std::string::npos
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


// bool Cgi::waitForChild() const
// {
// 	int exit_code = 0;
//     pid_t result = waitpid(this->_pid, &exit_code, WNOHANG);
//     if (result == -1) {
//         std::cout << "ERROR PARENT PROCESS\n";
// 		return (false);          
//     }
// 	// else if (result == 0)
// 	// {
//     //     std::cout << "NO CHILD HAS EXITED YET\n";
// 	// 	return (false);
// 	// }
//     if (WIFEXITED(exit_code))
// 	{
//         std::cout << "Child process exited with status: " << WEXITSTATUS(exit_code) << "\n";
//         return (WEXITSTATUS(exit_code) == EXIT_SUCCESS);
//     }
// 	else
// 	{
//         std::cout << "Child process exited abnormally" << "\n";
// 		return (false);
// 	}
// }

// bool Cgi::waitForChild() const
// {
//     int exit_code = 0;  // Initialize exit_code to 0
//     pid_t result;

//     while (true)
// 	{
//         result = waitpid(this->_pid, &exit_code, WNOHANG);
        
//         std::cout << "waitpid result: " << result << ", exit_code: " << exit_code << "\n";
        
//         if (result == -1)
// 		{
//             std::cout << "ERROR PARENT PROCESS\n";
//             return false;
//         }
// 		else if (result == 0)
// 		{
//             // No child process has exited yet, sleep for a short period
//             std::cout << "NO CHILD HAS EXITED YET\n";
//           	// usleep(10000);
//             continue;
//         }
//         if (WIFEXITED(exit_code))
// 		{
//             std::cout << "Child process exited with status: " << WEXITSTATUS(exit_code) << "\n";
//             return WEXITSTATUS(exit_code) == EXIT_SUCCESS;
//         } 
// 		else
// 		{
//             std::cout << "Child process exited abnormally\n";
//             return false;
//         }
//     }
// }


// bool Cgi::waitForChild() const
// {
// 	int cgiExitCode = -1;
// 	waitpid(this->_pid, &cgiExitCode, WNOHANG);
//     if (cgiExitCode == EXIT_SUCCESS)
//         return true;
//     else
//         return false;
// }

// #include <chrono>
// #include <thread>

bool Cgi::waitForChild() const
{
    int exit_code = 0;
	// std::cout << "\n\nCOPIED PID: " << this->_pid << "\n\n";

    pid_t result = waitpid(this->_pid, &exit_code, WNOHANG);
    
    if (result == -1) {
        std::cerr << "ERROR: waitpid failed\n";
        return false;
    }
    if (WIFEXITED(exit_code)) {
        std::cout << "Child process exited with status: " << WEXITSTATUS(exit_code) << "\n";
        return (WEXITSTATUS(exit_code) == EXIT_SUCCESS);
    } 
    else if (WIFSIGNALED(exit_code)) {
        std::cerr << "Child process was terminated by signal: " << WTERMSIG(exit_code) << "\n";
        return false;
    }
    else if (WIFSTOPPED(exit_code)) {
        std::cerr << "Child process is stopped by signal: " << WSTOPSIG(exit_code) << "\n";
        return false;
    }
    else {
        std::cerr << "Child process terminated abnormally\n";
        return false;
    }
}


std::string Cgi::runCgi(const std::string &cgi_path, Server *server)
{
	(void)server;
    pid_t pid = fork();
    if (pid == -1)
	{
        std::cout << "ERROR CREATING CHILD PROCESS\n";
        exit(EXIT_FAILURE);            
    }
	else if (pid == 0) 								// Child process
	{
        close(_responsePipe[0]); 					// Close read end of response pipe
        dup2(_responsePipe[1], STDOUT_FILENO); 		// Redirect cgi stdout to write end of response pipe
        close(_uploadPipe[1]); 						// Close write end of upload pipe
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
		// std::cout << "\n\nORIGINAL PID: " << pid << "\n\n";
		this->_pid = pid; 								// save pid for further processing if needed
        close(_responsePipe[1]);	
    }
	return ("");
}


void Cgi::_initPipes()
{
    if (pipe(_responsePipe) == -1 || pipe(_uploadPipe) == -1)
	{
        std::cout << "ERROR PIPE\n";
        std::exit(EXIT_FAILURE);
    }
}

int Cgi::getReadEndResponsePipe() const
{
	return (this->_responsePipe[0]);
}

int Cgi::getWriteEndUploadPipe() const
{
	return (this->_uploadPipe[1]);
}

int Cgi::getReadEndUploadPipe() const
{
	return (this->_uploadPipe[0]);
}

std::string Cgi::getMethod()
{
	return (this->_method);
}

void Cgi::appendResponse(std::string responseToAdd)
{
	this->_response += responseToAdd;
}




		// !!HAS TO BE RAN THROUGH POLL, CAN BE DONE OUTSIDE OF THIS SCOPE!!
        // write(_uploadPipe[1], this->_postData.c_str(), this->_postData.size()); // Write POST data to CGI via upload pipe
        // close(_uploadPipe[1]);						// Close write end of upload pipe after writing to cgi
        
		// int status;
        // pid_t result = waitpid(pid, &status, 0);
        // if (result == -1) {
        //     std::cout << "ERROR PARENT PROCESS\n";
        //     exit(EXIT_FAILURE);            
        // }
        // if (WIFEXITED(status)) {
        //     std::cout << "Child process exited with status: " << WEXITSTATUS(status) << "\n";
        //     return (readCgiResponse(_responsePipe[0])); 	// !!HAS TO BE RAN THROUGH POLL!!
        // } else {
        //     std::cout << "Child process exited abnormally" << "\n";
        // }
