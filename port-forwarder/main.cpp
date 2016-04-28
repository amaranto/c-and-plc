#include <iostream>
#include <regex>
#include <string>
#include "color.h"
#include "server.h"

int main ( int argc, char **argv ){
	
	int local_port = 2706;

	/* ###########################
		Set up environment
	############################*/
	
	std::cout << CYAN << "\n[ ACTION ]" << RESET << " Starting \n";

	if ( argc <= 1 )
		std::cout << YELLOW << "[ WARNING ]"<< RESET << " Using environment by default\n";
	
	else if (  ( (std::string)argv[1] ).compare ("--local-port") == 0 ) {
			
			std::cout << CYAN << "[ ACTION ]" << RESET << " assigning local port : ";
			local_port = atoi ( argv[2] );
			
			if ( local_port == 0 ) {
                                std::cout << RED << " [ FAIL ] \n" << RESET;
				local_port = 2706;
			}
			else
				std::cout << GREEN <<"[ OK ]\n" << RESET;
	}
	
	std::cout << YELLOW << "[ INFO ]" << RESET << " listen on : " << local_port << std::endl << RESET;
		
	Server* server = new Server( local_port );
	server->on();
	server->wait();

	return 0;
}
