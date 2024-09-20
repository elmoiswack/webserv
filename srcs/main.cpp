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
		std::vector<std::string> test_methods = server.GetAllowMethods();
   			std::cout << "Allow Methods: ";
    		for (const auto& method : test_methods) {
       		 std::cout << method << " ";
   			}
		std::cout << std::endl;
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
            std::string urls = location.GetURL();
            std::cout << "URL: " << urls << std::endl;
            std::cout << "Auto Index: " << (location.GetAutoIndex() ? "on" : "off") << std::endl;
    		std::vector<std::string> _methods = location.Get_AllowMethods();	
   			std::cout << "Allow Methods: ";
    		for (const auto& method : _methods) {
       		 std::cout << method << " ";
   			}
    		std::cout << std::endl;
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
        runTests(pars);

        //std::vector<Server> servers = pars.GetServerBlocks();
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