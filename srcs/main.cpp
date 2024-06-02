#include <iostream>
#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"

// void runTest(const std::string& testName, const std::string& configFile, bool shouldPass) {
//     try {
//         Parser parser(configFile);
//         std::cout << testName << ": Passed" << std::endl;
//         if (!shouldPass) {
//             std::cerr << testName << " was expected to fail but passed." << std::endl;
//         }
//     } catch (const std::exception& e) {
//         if (shouldPass) {
//             std::cerr << testName << ": Failed with error: " << e.what() << std::endl;
//         } else {
//             std::cout << testName << ": Passed (correctly failed with error: " << e.what() << ")" << std::endl;
//         }
//     }
// }

// int main() {
//     runTest("Valid Configuration", "valid_config.conf", true);
//     runTest("Invalid Configuration", "invalid_config.conf", false);
//     return 0;
// }

// int main(int argc, char *argv[])
// {
// 	if (argc != 2)
// 	{
// 		std::cout << "The program needs only 1 configuration file!\n";
// 		std::cout << "To run the program: ./webserv 'the configutation file'!" << std::endl;
// 	}
// 	try
// 	{
// 		Parser pars(argv[1], pars);
// 		Server serv(pars);
// 		serv.StartServer();

// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << "ERROR: " << e.what() << std::endl;
// 	}
// }

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "The program needs only 1 configuration file!\n";
        std::cout << "To run the program: ./webserv 'the configuration file'!" << std::endl;
        return 1; // Return an error code
    }

    try
    {
        Parser pars(argv[1], pars);

        // Testing the dynamic usage of the serverblocks vector
        std::vector<Server>& servers = const_cast<std::vector<Server>&>(pars.GetServerBlocks());
        std::cout << "Number of server blocks: " << servers.size() << std::endl;
        for (const auto& server : servers)
        {
            std::cout << "Server IP: " << server.GetIp() << std::endl;
            std::cout << "Server Port: " << server.GetPort() << std::endl;
            std::cout << "Server Name: " << server.GetServName() << std::endl;
            std::cout << "Client Max Body Size: " << server.GetClientMax() << std::endl;
            std::cout << "Root: " << server.GetRoot() << std::endl;

            std::unordered_map<int, std::string> errorPageMap = server.GetErrorPage();
            std::cout << "Error Pages:" << std::endl;
            for (const auto& errorPage : errorPageMap)
            {
                std::cout << "Error Code: " << errorPage.first << " -> URL: " << errorPage.second << std::endl;
            }

            std::cout << "--------------------------------------" << std::endl;
        }

        // Start the first server in the list for demonstration
        if (!servers.empty())
        {
            servers[0].StartServer();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1; // Return an error code
    }

    return 0; // Indicate success
}
