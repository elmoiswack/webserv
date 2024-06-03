#include "../includes/Cgi.hpp"

bool Cgi::isCgi(const std::string &url)
{
	std::string extension;

	size_t i = url.find("."); 
	while (url[i] != ' ')
		extension.push_back(url[i++]);
	if (extension != ".cgi")
		return (false);
	return (true);
}


std::string Cgi::extractCgiPath(const std::string &url)
{
	std::string path;
	// NEED TO MAKE IT WORK WITH RELATIVE PATH, USE ABLOSUTE FOR NOW!!!
	path.append("/Users/rares/Documents/CODING/Codam/GitHub/webserv/var/www");
	size_t i = url.find("/");
	while (url[i] != ' ')
		path.push_back(url[i++]);
	return (path);
}

void Cgi::runCgi(const std::string &cgi_path)
{
	pid_t pid = fork();
	if (pid == -1)
	{
		std::cout << "ERROR CREATING CHILD PROCESS\n";
        exit(EXIT_FAILURE);			
	}
	else if (pid == 0)	//-> child process
	{
		const char *args[] = {"/usr/bin/python3", cgi_path.c_str(), NULL};
		if (execve("/usr/bin/python3", const_cast<char**>(args), NULL) == -1)
		{
			std::cout << "ERROR EXECUTING CGI SCRIPT\n";
        	exit(EXIT_FAILURE);			
		}
	}
	else				//-> parent process
	{
		int status;
		pid_t result = waitpid(pid, &status, 0);
		if (result == -1)
		{
			std::cout << "ERROR PARENT PROCESS\n";
            exit(EXIT_FAILURE);			

		}
		if (WIFEXITED(status))
			std::cout << "Child process exited with status: " << WEXITSTATUS(status) << "\n";
		else
            std::cout << "Child process exited abnormally" << "\n";
	}
}
