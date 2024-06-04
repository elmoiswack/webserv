#include "../includes/Cgi.hpp"


Cgi::Cgi(char *client_resp, const std::string &url) : 
	m_cgi_env_vars(this->initCgiEnvVariables(client_resp, url)),
	m_cgi_env_vars_cstyle(initCgiEnvVariablesCstyle())
{

}

Cgi::~Cgi()
{
	for (char *env : m_cgi_env_vars_cstyle)
		delete[] env;
	std::cout << "CGI DESCTRUCTED\n";
}


// bool Cgi::isCgi(const std::string &url)
// {
// 	std::string extension;

// 	size_t i = url.find("."); 
// 	while (url[i] != ' ')
// 		extension.push_back(url[i++]);
// 	if (extension != ".cgi")
// 		return (false);
// 	return (true);
// }

std::string Cgi::constructCgiPath(const std::string &url)
{
	std::string path;
	// path.append("/Users/rares/Documents/CODING/Codam/GitHub/webserv/var/www");
	path.append("./var/www/");
	size_t pos = url.find_first_of('?');
	if (pos == std::string::npos)
		path.append(url);
	else
	{
		// std::cout << "++++++++\n";
		path.insert(path.size(), url, 0, pos);
	}
	return (path);
}

std::string Cgi::extractReqUrl(const std::string &url)
{
	std::string path;
	// NEED TO MAKE IT WORK WITH RELATIVE PATH, USE ABLOSUTE FOR NOW!!!
	// path.append("/Users/rares/Documents/CODING/Codam/GitHub/webserv/var/www");
	size_t i = url.find("/");
	while (url[i] != ' ')
		path.push_back(url[i++]);
	return (path);
}

// std::string Cgi::extractCgiPath(const std::string &url)
// {
// 	std::string path;
// 	// NEED TO MAKE IT WORK WITH RELATIVE PATH, USE ABLOSUTE FOR NOW!!!
// 	path.append("/Users/rares/Documents/CODING/Codam/GitHub/webserv/var/www");
// 	size_t i = url.find("/");
// 	while (url[i] != ' ')
// 		path.push_back(url[i++]);
// 	return (path);
// }

std::string	Cgi::readPipe(int fd)
{
	std::ostringstream oss;
	char buffer[128];
	ssize_t bytes_read = 0;
	while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
		oss.write(buffer, bytes_read); // -> append read data to the output string stream
	close(fd);
	// std::cout << oss.str() << "\n";
	return (oss.str());
}

std::vector<std::string>Cgi::initCgiEnvVariables(const char *client_resp, const std::string &url)
{
	std::vector<std::string> env_vars = 
	{
    	"CONTENT_LENGTH=" + std::to_string(strlen(client_resp)),
   		"CONTENT_TYPE=text/plain",
		"GATEWAY_INTERFACE=CGI/1.1",
		"QUERY_STRING=" + this->extractQueryString(url),
		"REQUEST_METHOD=GET",
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


std::vector<char *> Cgi::initCgiEnvVariablesCstyle()
{
	std::vector<char *> env_vars;

	for (const std::string &env_var : m_cgi_env_vars)
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
	// std::cout << "-----URL: " << url << "\n\n\n";
	std::string querry_str;
	size_t pos = url.find('?');
	size_t end = url.find_last_of(' ');
	if (pos != std::string::npos)
	{

		querry_str = (end != std::string::npos)
			? url.substr(pos + 1, end - pos - 1)
			: url.substr(pos + 1, url.length() - pos - 1);
	}
	// std::cout << "+++++++QUERRY: " << querry_str << "\n\n\n";
	return (querry_str);
}

std::string Cgi::runCgi(const std::string &cgi_path)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		std::cout << "ERROR PIPE\n";
		std::exit(EXIT_FAILURE);
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		std::cout << "ERROR CREATING CHILD PROCESS\n";
        exit(EXIT_FAILURE);			
	}
	else if (pid == 0)	//-> child process
	{
		close(pipefd[0]); // -> close read end of pipe, only need to write
		dup2(pipefd[1], STDOUT_FILENO); // -> redirect stdout to write end of pipe
		const char *args[] = {"/usr/bin/python3", cgi_path.c_str(), NULL};
		if (execve("/usr/bin/python3", const_cast<char**>(args), m_cgi_env_vars_cstyle.data()) == -1)
		{
			std::cout << "ERROR EXECUTING CGI SCRIPT\n";
        	std::exit(EXIT_FAILURE);			
		}
	}
	else				//-> parent process
	{
		close(pipefd[1]); // -> close write end of pipe, only need to read
		int status;
		pid_t result = waitpid(pid, &status, 0);
		if (result == -1)
		{
			std::cout << "ERROR PARENT PROCESS\n";
            exit(EXIT_FAILURE);			

		}
		if (WIFEXITED(status))
		{
			// std::cout << "CGI script output:\n\n" << readPipe(pipefd[0]);
			std::cout << "Child process exited with status: " << WEXITSTATUS(status) << "\n";
			return (readPipe(pipefd[0]));
		}
		else
		{
            std::cout << "Child process exited abnormally" << "\n";
		}
	}
	return (nullptr);
}


