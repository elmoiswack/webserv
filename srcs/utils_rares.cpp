/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils_rares.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: raanghel <raanghel@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/15 18:17:10 by raanghel      #+#    #+#                 */
/*   Updated: 2024/05/15 18:49:48 by raanghel      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include"../includes/utils_rares.hpp"

std::string readFile(const std::string &path)
{
	std::ifstream file(path);
	if (!file.good())
	{
		std::cout << "Failed to read file!\n";
		std::exit(EXIT_FAILURE);
		// throw std::runtime_error("File not found");
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}
