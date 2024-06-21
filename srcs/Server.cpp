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
	int opt = 1;
	setsockopt(this->_websock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
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
		 
		std::string request;
        char buffer[1024];
        int bytes_received;
        bool headers_received = false;
        size_t content_length = 0;
        std::string headers;

        while ((bytes_received = recv(newsock, buffer, sizeof(buffer), 0)) > 0)
        {
            request.append(buffer, bytes_received);

            // Check if headers have been received
            if (!headers_received)
            {
                size_t header_end = request.find("\r\n\r\n");
                if (header_end != std::string::npos)
                {
                    headers = request.substr(0, header_end + 4);
                    headers_received = true;

                    // Parse headers to find Content-Length
                    size_t content_length_pos = headers.find("Content-Length: ");
                    if (content_length_pos != std::string::npos)
                    {
                        size_t content_length_end = headers.find("\r\n", content_length_pos);
                        std::string content_length_str = headers.substr(content_length_pos + 16, content_length_end - content_length_pos - 16);
                        content_length = std::stoul(content_length_str);
                    }
                }
            }

            // Check if we have received the entire body
            if (headers_received && request.size() >= headers.size() + content_length)
                break;
        }
		
		std::cout << "\n\n--------REQUEST: \n\n" << request;

		std::string response;		
		std::string html_file = readFile("./var/www/index.html");
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
		if (isCgi(std::string(request)))
		{
			Cgi cgi;
			response.clear();
			std::string req_url = cgi.extractReqUrl(request);
			std::string cgi_path = cgi.constructCgiPath(req_url);
			cgi.setCgiEnvVars(cgi.initCgiEnvVars(request, req_url));
			cgi.setCgiEnvVarsCstyle(cgi.initCgiEnvVarsCstyle());
			//std::cout << "\nREQUEST URL: " << req_url << "\n";
			//std::cout << "\nCGI PATH: " << cgi_path << "\n\n";
			// std::cout << "\n---QUERY_STRING: " << cgi.extractQueryString(req_url) << "\n\n\n";
			response = cgi.runCgi(cgi_path);
			std::cout << "\n--------------------------\n";
			std::cout << "RESPONSE: \n\n" << response;
			std::cout << "--------------------------\n";
		}

    	write(newsock, response.c_str(), response.size());
        close(newsock);
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
