#ifndef SERVER_H
#define SERVER_H


#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>
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
#include <fstream>
#include <string>
#include "Client.h"
#include "Color.h"

#define MAX_CLIENT 100


class Server
{
    public:
        Server( int port = 2706 );
        virtual ~Server();

        int connect_to ( int clt_port = 2706, std::string host = "127.0.0.1");

        int on ();
        int off();
        void* accept_conn();

        int wait ();
        int wait ( int sec );

        int log    ( bool status );
        int display( bool status );
        int write_output ( std::string buffer );

    protected:

    private:
            int master_sock,
                port,
                addr_len;

            bool is_on,
                 with_log,
                 display_output;

            struct sockaddr_in s_addr;

            fd_set fds_clients;

            pthread_t t_accept,
                      t_receive;

            std::vector <Client*> clients;
};


#endif // SERVER_H
