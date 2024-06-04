#include "../includes/Server.hpp"
#include "../includes/utils_rares.hpp"
#include "../includes/Cgi.hpp"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

Server::Server(Parser &in)
{
	this->_ip = in.GetIp();
	this->_port = in.GetPort();
	this->_server_name = in.GetServName();
}

Server::~Server()
{
	this->_ip.clear();
	this->_port.clear();
	this->_server_name.clear();
	close(this->_websock);
}

bool isCgi(const std::string &url)
{
	std::string extension;

	size_t i = url.find("."); 
	while (url[i] != ' ' && url[i] != '?')
		extension.push_back(url[i++]);
	if (extension != ".cgi")
	{
		// std::cout << "===\n\nNOT CGI======\n\n";

		return (false);
	}
	// std::cout << "===\n\nIS CGI======\n\n";
	return (true);
}


//https://localhost:8080/ our address
void Server::StartServer()
{
	this->_websock = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_websock < 0)
	{
		std::cout << "ERROR" << std::endl;
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in infoaddr;
	memset(&infoaddr, '\0', sizeof(infoaddr));
	infoaddr.sin_family = AF_INET;
	infoaddr.sin_addr.s_addr = INADDR_ANY;
	infoaddr.sin_port = htons(std::atoi(this->_port.c_str()));	
	if (bind(this->_websock, (struct sockaddr *)&infoaddr, sizeof(infoaddr)) == -1)
	{
		std::cout << "ERROR BIND" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (listen(this->_websock, 5) == -1)
	{
		std::cout << "ERROR LISTEN" << std::endl;
		exit(EXIT_FAILURE);		
	}

	// keeps it running
	long newsock;
	// Cgi cgi;
	while (true)
    {
		struct sockaddr_in client_addr;
  		int client_addr_len = sizeof(client_addr);

       	newsock = accept(this->_websock, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
        if (newsock == -1)
        {
            std::cout << "ERROR ACCEPT" << std::endl;
            exit(EXIT_FAILURE);			
        }
		 
		char buffer[1024] = { 0 }; 
		recv(newsock, buffer, sizeof(buffer), 0); 
		//std::cout << "Message from client:\n" << buffer << std::endl;
		std::string response;
		
		
		// else
		// {
			// NEED TO MAKE IT WORK WITH RELATIVE PATH, USE ABLOSUTE FOR NOW!!!
			// std::string html_file = readFile("/home/coxer/Documents/GitHub/webserv/var/www/index.html");
			std::string html_file = readFile("./var/www/index.html");
			// std::cout << html_file << "\n";
			response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " + std::to_string(html_file.length()) + "\r\n"
			"\r\n"
			+ html_file;
       	 	// Send the blank HTML page response again for new connections
    		// write(newsock, response.c_str(), response.size());
		// }
    	//write(newsock, response.c_str(), response.size());
		if (isCgi(std::string(buffer)))
		{
			response.clear();
			Cgi cgi(buffer, cgi.extractReqUrl(buffer));
			// std::cout << "RUNNING CGI SCRIPT...\n";
			std::string req_url = cgi.extractReqUrl(buffer);
			std::string cgi_path = cgi.constructCgiPath(req_url);
			// std::cout << "\nREQUEST URL: " << req_url << "\n";
			// std::cout << "\nCGI PATH: " << cgi_path << "\n\n";
			// std::cout << "\n---QUERY_STRING: " << cgi.extractQueryString(req_url) << "\n\n\n";
			response = cgi.runCgi(cgi_path);
			std::cout << "\n--------------------------\n";
			std::cout << "RESPONSE: \n\n" << response;
			std::cout << "--------------------------\n";
			// response.erase();
			// continue ;
		}
    	write(newsock, response.c_str(), response.size());
		
        close(newsock);
		// std::cout << "\n=============================\n";
    }
}	


// char buffer[1024] = { 0 }; 
// recv(newsock, buffer, sizeof(buffer), 0); 
// std::cout << "Message from client: " << buffer << std::endl; 
// void Server::StartClient(int fd)
// {

// 	struct sockaddr_in infoaddr;
// 	memset(&infoaddr, '\0', sizeof(infoaddr));
// 	infoaddr.sin_family = AF_INET;
// 	infoaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
// 	infoaddr.sin_port = htons(std::atoi(this->_port.c_str()));	
// 	if (connect(fd, (struct sockaddr *)&infoaddr, sizeof(infoaddr)) == -1)
// 	{
// 		std::cout << "ERROR CONNECT" << std::endl;
// 		exit(EXIT_FAILURE);
// 	}
// 	fcntl(fd, F_SETFL, O_NONBLOCK);
// 	std::string lmao = "hello world";
// 	int bret = send(fd, lmao.c_str(), lmao.size(), 0);
// 	if (bret == -1)
// 	{
// 		std::cout << "ERROR SEND" << std::endl;
// 		exit(EXIT_FAILURE);			
// 	}
// }
