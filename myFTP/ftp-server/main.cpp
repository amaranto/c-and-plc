#include <iostream>
#include <stdlib.h>
#include "Color.h"
#include "Server.h"

int main ( int argc, char **argv )
{

    std::cout << BOLDRED;
    std::cout << "\n#############" << std::endl;
    std::cout << "#  Welcome  #"<< std::endl;
    std::cout << "#############" << std::endl;
    std::cout << RESET;

    std::cout << "\nSetting environment: " << std::endl;

    /** ################
        Setting port
    ###################*/

    int port = 2706,
        return_value = 0;

    std::cout << "\tWorking on: [ " << MAGENTA << argv[0] << RESET << " ]" << std::endl;

    if ( argc == 2 )
        port = atoi ( argv[1] );
    else
        std::cout << YELLOW << "\tWARNING: "<< RESET << "Bad arguments given. Using port 2706 by default." << std::endl;

    Server* server = new Server ( port );


    /** #####################
        Starting server
    ########################*/

    std::cout << "Starting Server.";

    return_value = server->on();
    if ( return_value > 0 )
            std::cout <<"Status:"<< GREEN << "[ OK ]" << RESET << std::endl;
    else {
            std::cout <<"Status:"<< RED << "[ FAIL ]" << RESET << std::endl;
            return -1;
    }

    server->wait();
    return 0;
}
