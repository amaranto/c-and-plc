#include <iostream>
#include <regex>
#include <stdlib.h>
#include <string.h>
#include "Client.h"
#include "Color.h"

int main( int argc, char**argv)
{
    /** ####################
        Setting Environment
    #######################*/

    int result = 0,
		port = 2706;
    std::string username = "Anon",
                host = "127.0.0.1";

    std::cout << BOLDRED;
    std::cout << "\n#############" << std::endl;
    std::cout << "#  Welcome  #"<< std::endl;
    std::cout << "#############" << std::endl;
    std::cout << RESET;

    std::cout << "\nStarting client" << std::endl;
    std::cout << "\tWorking on: [ " << MAGENTA << argv[0] << RESET << " ]" << std::endl;

    if ( argc == 3 ){
        host = (std::string)argv[1];
        port = atoi ( argv[2] );
    }
    else
        std::cout << YELLOW << "\tWARNING: "<< RESET << \
                               "Bad arguments given. Using localhost:2706 by default.\n" << \
                               "\tUse: ./ftp-client "<< MAGENTA << "<PORT>" << YELLOW <<" <HOST>" << RESET << std::endl;

    std::cout << "\nNick: " << RED;
    std::cin >> username;
    std::cout << RESET;

    Client* client = new Client( port, host, username);

    //client->log(false); /** Comment out this line to disable logs **/
    //client->display ( false ); /** Comment out this line to display debug messages **/

    if ( client->start() <= 0 ){

        std::cout << RED << "[ ACCESS REFUSE ] " << RESET << std::endl;
        delete client;
        return -1;

    }else{
        std::cout << GREEN << "[ ACCESS SUCCESS ] " << RESET << std::endl;
    }

    std::string input;
    std::cin.get();
    do {
        std::cout << BOLDCYAN << "command: " << RESET;
	std::getline ( std::cin, input );
	
	std::size_t pos = input.find(" ");
	std::string command   = input.substr ( 0, pos );
	std::string arguments = input.substr ( pos + 1 );	
	arguments = arguments + "\0";
	
	if ( command.compare ("get") == 0 )		
		result = client->get_file( arguments );
	else{
		std::cout << "Command: " << command  << " arguments: " << arguments << RED << "[ NOT FOUND ]" << RESET << std::endl;
		continue;
	}
	// all commands return 0 or more than 0 in successfull execution
	if ( result >= 0 )
		std::cout << GREEN << "[ COMPLETE ]" << RESET << std::endl;
	else
		std::cout << RED   << "[ FAIL ] " << std::to_string ( result ) << RESET << std::endl;
			
    } while ( true );

    return 0;
}
