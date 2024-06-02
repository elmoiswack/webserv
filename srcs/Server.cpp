#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"

Server::Server(const std::string& ip, const std::string& port, const std::string& server_name,
               const std::string& client_max, const std::string& root, const std::unordered_map<int, std::string>& error_page, const std::string& serverindex)
    : _port(port), _ip(ip),_server_name(server_name), _client_max(client_max), _root(root), _serverindex(serverindex), _error_page(error_page), _websock(-1) {}

Server::~Server()
{
	this->_ip.clear();
	this->_port.clear();
	this->_server_name.clear();
	this->_client_max.clear();
	this->_root.clear();
	this->_error_page.clear();
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
    std::cout << "Socket is set up and listening on " << this->_ip << ":" << this->_port << std::endl;
    std::cout << "Socket file descriptor: " << this->_websock << std::endl;
	// keeps it running
	while (true)
    {
		std::cout << "Waiting for a connection..." << std::endl;
        int newsock = accept(this->_websock, NULL, NULL);
        if (newsock == -1)
        {
            std::cout << "ERROR ACCEPT" << std::endl;
            exit(EXIT_FAILURE);			
        }
 		// std::cout << "Connection accepted on socket " << newsock << std::endl;
		// blank HTML page
		const char *response =
	    "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "<html></html>";

        // Send the blank HTML page response again for new connections
        write(newsock, response, strlen(response));
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
