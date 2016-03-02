#ifndef CLIENT_H
#define CLIENT_H

#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <regex>
#include "Color.h"

class Client
{
    public:
        Client( int port=2706, std::string host="127.0.0.1" , std::string username="Anon");
        virtual ~Client();

        int start ();
        int log ( bool status );
        int display ( bool status );
        int write_output( std::string buffer );

        int get_file (std::string path );

    protected:
    private:
        int sock,
            port;
        std::string host,
                    username;
        bool connected,
             display_output,
             with_log;
        struct sockaddr_in s_addr_clt;

        std::string get_protocol( std::string buffer );
        std::string get_value   ( std::string buffer );
};

#endif // CLIENT_H
