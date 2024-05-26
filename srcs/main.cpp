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

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "The program needs only 1 configuration file!\n";
		std::cout << "To run the program: ./webserv 'the configutation file'!" << std::endl;
	}
	try
	{
		Parser pars(argv[1]);
		Server serv(pars);
		serv.StartServer();

	}
	catch(const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}