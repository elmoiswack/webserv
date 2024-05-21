#include <iostream>
#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"

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