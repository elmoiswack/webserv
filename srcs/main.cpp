#include <iostream>
#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"
#include "../includes/Cgi.hpp"
#include "../includes/Location.hpp"


void runTests(const Parser& parser) {
    std::vector<Server> servers = parser.GetServerBlocks();
    std::cout << "Number of server blocks: " << servers.size() << std::endl;
    for (const auto& server : servers) {
        std::cout << "Server IP: " << server.GetIp() << std::endl;
        std::cout << "Server Port: " << server.GetPort() << std::endl;
        std::cout << "Server Name: " << server.GetServName() << std::endl;
        std::cout << "Client Max Body Size: " << server.GetClientMax() << std::endl;
        std::cout << "Root: " << server.GetRoot() << std::endl;
        std::cout << "Server Index: " << server.GetServerIndex() << std::endl;
        std::cout << "--------------------------------------" << std::endl;

        std::unordered_map<int, std::string> errorPageMap = server.GetErrorPage();
        std::cout << "Error Pages:" << std::endl;
        for (const auto& errorPage : errorPageMap) {
            std::cout << "Error Code: " << errorPage.first << " -> URL: " << errorPage.second << std::endl;
        }

        std::cout << "--------------------------------------" << std::endl;
        std::cout << "Locations:" << std::endl;
        std::vector<Location> locations = server.GetLocations();
        for (const auto& location : locations) {
            std::vector<std::string> urls = location.GetURL();
            for (const auto& url : urls) {
                std::cout << "URL: " << url << std::endl;
            }
            std::cout << "Auto Index: " << (location.GetAutoIndex() ? "on" : "off") << std::endl;
            std::cout << "Allow Methods: " << location.GetAllowMethods() << std::endl;
            std::cout << "Index: " << location.GetIndex() << std::endl;
            std::cout << "Return Redirect: " << location.GetReturnRedirect() << std::endl;
            std::cout << "Alias: " << location.GetAlias() << std::endl;
            std::cout << "CGI Pass: " << location.GetCGIpass() << std::endl;
            std::cout << "CGI Index: " << location.GetCGIindex() << std::endl;
            std::cout << "CGI Param: ";
            std::vector<std::string> cgiPaths = location.GetCGIparam();
            for (const auto& path : cgiPaths) {
                std::cout << path << " ";
            }
            std::cout << std::endl;
            std::cout << "Root: " << location.GetLocRoot() << std::endl;
            std::cout << "--------------------------------------" << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "The program needs only 1 configuration file!\n";
        std::cout << "To run the program: ./webserv 'the configuration file'!" << std::endl;
        return 1;
    }

    try
    {
        Parser pars(argv[1], pars);
      	runCgi("./var/www/cgi-bin");
        runTests(pars);

        std::vector<Server> servers = pars.GetServerBlocks();

        if (!servers.empty()) {
            servers[0].StartServer();
            std::cout << "Started server on port: " << servers[0].GetPort() << std::endl;
        }

        // Test the getServer function
        try {
            Server& testServer = pars.getServer("test", 8001);
            std::cout << "Retrieved server with port: " << testServer.GetPort() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error retrieving server: " << e.what() << std::endl;
        }
        Server serv(pars);
      	serv.SetUpServer();
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

// int main(int argc, char *argv[])
// {
// 	if (argc != 2)
// 	{
// 		std::cout << "The program needs only 1 configuration file!\n";
// 		std::cout << "To run the program: ./webserv 'the configutation file'!" << std::endl;
// 	}
// 	try
// 	{
// 		Parser pars(argv[1]);
// 		Server serv(pars);
// 		runCgi("./var/www/cgi-bin");
// 		serv.SetUpServer();

// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << "ERROR: " << e.what() << std::endl;
// 	}
// }