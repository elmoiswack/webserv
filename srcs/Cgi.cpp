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
}

Cgi::Cgi(const std::string &method, const std::string &post_data, const std::string &path, const std::string &request) : _method(method),
																														 _postData(post_data),
																														 _cgiEnvVars(initCgiEnvVars(request, path)),
																														 _cgiEnvVarsCstyle(initCgiEnvVarsCstyle())
{}

Cgi::Cgi(const std::string &method, const std::string &path, const std::string &request) : _method(method),
																						   _cgiEnvVars(initCgiEnvVars(request, path)),
																						   _cgiEnvVarsCstyle(initCgiEnvVarsCstyle())
{}

Cgi::Cgi(const Cgi &obj) : _method(obj._method),
						   _postData(obj._postData),
						   _cgiEnvVars(obj._cgiEnvVars),
						   _pid(obj._pid)
{
	_cgiEnvVarsCstyle = this->initCgiEnvVarsCstyle();
}

Cgi &Cgi::operator=(const Cgi &obj)
{
	if (this != &obj)
	{
		_method = obj._method;
		_postData = obj._postData;
		_cgiEnvVars = obj._cgiEnvVars;
		_pid = obj._pid;
		_cgiEnvVarsCstyle = this->initCgiEnvVarsCstyle();
	}
	return (*this);
}

Cgi::~Cgi()
{
	for (const char *env : _cgiEnvVarsCstyle)
		delete[] env;
	std::cout << "--CGI DESCTRUCTED\n";
}

void Cgi::setCgiEnvVars(const std::vector<std::string> &vars)
{
	_cgiEnvVars = vars;
}

void Cgi::setCgiEnvVarsCstyle(const std::vector<char *> &vars_cstyle)
{
	_cgiEnvVarsCstyle = vars_cstyle;
}

void Cgi::setPostData(const std::string &post_data)
{
	_postData = post_data;
}

void Cgi::setMethod(const std::string &method)
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

std::string extractBoundary(const std::string &content)
{

	std::string boundary_prefix = "boundary=";
	size_t boundary_start = content.find(boundary_prefix);
	if (boundary_start == std::string::npos)
	{
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

std::vector<std::string> Cgi::initCgiEnvVars(const std::string &client_resp, const std::string &url)
{
	std::vector<std::string> env_vars =
		{
			"CONTENT_LENGTH=" + this-> extractContentLength(client_resp),
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
	// logger("CGI ENV VARS: \n\n");
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

std::string Cgi::extractQueryString(const std::string &url)
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

std::string Cgi::extractContentLength(const std::string &client_req)
{
	size_t pos = client_req.find("Content-Length:");
	if (pos != std::string::npos)
	{
		pos += std::string("Content-Length:").length();
		size_t end = client_req.find("\n", pos);
		while (std::isspace(client_req[pos]))
			++pos;
		if (end != std::string::npos)
			return (client_req.substr(pos, end - pos));
	}
	return ("");
}

std::string Cgi::extractContentType(const std::string &req)
{
	if (req.empty())
		return ("");
	std::string contentType;
	std::size_t pos = req.find("Content-Type:");
	if (pos != std::string::npos)
	{
		pos += sizeof("Content-Type:");
		std::size_t endPos = req.find("\r\n", pos);
		if (endPos != std::string::npos)
		{
			contentType = req.substr(pos, endPos - pos);
			// std::cout << "\n--CONTENT TYPE: " << contentType << "\n\n";
			return (contentType);
		}
	}
	return "";
}

bool Cgi::waitForChild() const
{
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int exit_code = 0;
    while (true)
	{
        pid_t result = waitpid(this->_pid, &exit_code, WNOHANG);
        if (result == 0)
		{
            // Child is still running, continue loop (non-blocking)
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }
		else if (result == -1)
            return false;
		if (result == this->_pid)
		{
			int exit_status = WEXITSTATUS(exit_code);
            // Child process finished
            if (exit_status == 0)
			{
                std::cout << "CGI script exited successfully" << std::endl;
				return true;
			}
			else
			{
                std::cout << "Child process exited with error code: " << exit_code << std::endl;
            	return false;
			}
        }
		else if (WIFSIGNALED(exit_code))
		{
	        std::cout << "Child process was terminated by a signal: " << WTERMSIG(exit_code) << std::endl;
			return (false);
		}

	}    
}

bool Cgi::runCgi(const std::string &cgi_path, Server *server)
{
	(void)server;
	(void)cgi_path;
	pid_t pid = fork();
	if (pid == -1)
	{
		std::cout << "ERROR CREATING CHILD PROCESS\n";
		return (false);
	}
	else if (pid == 0) // Child process
	{
		close(_responsePipe[0]);			   // Close read end of response pipe
		dup2(_responsePipe[1], STDOUT_FILENO); // Redirect cgi stdout to write end of response pipe
		close(_uploadPipe[1]);				   // Close write end of upload pipe
		dup2(_uploadPipe[0], STDIN_FILENO);	   // Redirect cgi stdin to read end of upload pipe
		// char *args[] = { NULL };
		// char *env[] = { NULL };
		// if (execve("/invalid/path", args, env) == -1)
		const char *args[] = {cgi_path.c_str(), NULL};
		if (execve(cgi_path.c_str(), const_cast<char **>(args), _cgiEnvVarsCstyle.data()) == -1)
		{
			close(_uploadPipe[0]);
			std::cout << "ERROR EXECUTING CGI SCRIPT\n" << std::flush;
			std::exit(EXIT_FAILURE);
		}
	}
	else // Parent process
	{
		this->_pid = pid;
		close(_responsePipe[1]);
	}
	return (true);
}

bool Cgi::_initPipes()
{
	if (pipe(_responsePipe) == -1 || pipe(_uploadPipe) == -1)
	{
		std::cout << "ERROR CREATING PIPES\n";
		return false;
	}
	return true;
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

pid_t Cgi::getPidCgi() const
{
	return (this->_pid);
}

void Cgi::appendResponse(std::string responseToAdd)
{
	this->_response += responseToAdd;
}

void Cgi::killCgi()
{
	// std::cout << "PID BEFORE KILL: " << _pid << "\n";
	if (this->_pid > 0)
	{
		if (kill(this->_pid, SIGKILL) < 0)
			std::cerr << "Failed to terminate child process: " << strerror(errno) << std::endl;
		else
			logger("Successfully terminated child process");
		waitpid(this->_pid, nullptr, 0);
	}
}
