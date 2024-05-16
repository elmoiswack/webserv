#include "../includes/Server.hpp"
#include "../includes/utils_rares.hpp"
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

//https://localhost:8080/ our address
void Server::StartServer()
{

	_websock = socket(AF_INET, SOCK_STREAM, 0);
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

	while (true)
	{
		long newsock = accept(this->_websock, NULL, NULL);
		if (newsock == -1)
		{
			std::cout << "ERROR ACCEPT" << std::endl;
			exit(EXIT_FAILURE);			
		}
		std::cout << "connection accepted!" << std::endl;

		const int BUFFER_SIZE = 30720;
		char buffer[BUFFER_SIZE] = {0};
		int bytes_received = read(newsock, buffer, BUFFER_SIZE);
		if (bytes_received < 0)
		{
			std::cout << "ERROR READ" << std::endl;
			exit(EXIT_FAILURE);		
		}
		std::string response = readFile("/home/coxer/Documents/GitHub/webserv/var/www/index.html");
		std::cout << response << "\n";
		
	
		// std::string response =
		// "HTTP/1.1 200 OK\r\n"
		// "Content-Type: text/html\r\n"
		// "Content-Length: 25\r\n"
		// "\r\n"
		// "<html>"
		// "HELLO WORLD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1"
		// "</html>";


		size_t bytes_sent = write(newsock, response.c_str(), response.size());
		if (bytes_sent != response.size())	
		{
			std::cout << "ERROR WRITE" << std::endl;
			exit(EXIT_FAILURE);		
		}

		close(newsock);
	}




	// this->_websock = socket(AF_INET, SOCK_STREAM, 0);
	// if (this->_websock < 0)
	// {
	// 	std::cout << "ERROR" << std::endl;
	// 	exit(EXIT_FAILURE);
	// }

	// struct sockaddr_in infoaddr;
	// memset(&infoaddr, '\0', sizeof(infoaddr));
	// infoaddr.sin_family = AF_INET;
	// infoaddr.sin_addr.s_addr = INADDR_ANY;
	// infoaddr.sin_port = htons(std::atoi(this->_port.c_str()));	
	// if (bind(this->_websock, (struct sockaddr *)&infoaddr, sizeof(infoaddr)) == -1)
	// {
	// 	std::cout << "ERROR BIND" << std::endl;
	// 	exit(EXIT_FAILURE);
	// }
	// if (listen(this->_websock, 5) == -1)
	// {
	// 	std::cout << "ERROR LISTEN" << std::endl;
	// 	exit(EXIT_FAILURE);		
	// }
	// long newsock = accept(this->_websock, NULL, NULL);
	// if (newsock == -1)
	// {
	// 	std::cout << "ERROR ACCEPT" << std::endl;
	// 	exit(EXIT_FAILURE);			
	// }

	// std::cout << "connection accepted!" << std::endl;

	// //std::string html_file_content = readFile("/home/coxer/Documents/GitHub/webserv/var/www/index.html");
	// //std::cout << html_file_content << "\n";
	// // std::string response = ""
	// // keeps it running
	// while (true)
    // {
    //     newsock = accept(this->_websock, NULL, NULL);
    //     if (newsock == -1)
    //     {
    //         std::cout << "ERROR ACCEPT" << std::endl;
    //         exit(EXIT_FAILURE);			
    //     }

	// 	// blank HTML page
		
	// 	//const char *response = html_file_content.c_str();
	// 	std::string response =
	// 	"HTTP/1.1 200 OK\r\n"
	// 	"Content-Type: text/html\r\n"
	// 	"Content-Length: 25\r\n"
	// 	"\r\n"
	// 	"<html>"
	// 	"HELLO WORLD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1"
	// 	"</html>";

    //     // Send the blank HTML page response again for new connections
    //     write(newsock, response.c_str(), response.size());
    //     close(newsock);
    // }
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
