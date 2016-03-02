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
#include "color.h"

#define MAX_CLIENTS 10

class Server
{
    public:
        Server( int port = 2706 );
        virtual ~Server();

        int on ();
        int off();
        int wait ();
        int wait ( int sec );

        void* accept_conn();
	void* get_msg();

    protected:

    private:
        int master_sock,
	    local_sock,
            port,
            addr_len;

        bool is_on;

        struct sockaddr_in s_addr,
			   s_local_addr;

        pthread_t t_accept,
                  t_receive;

	int clientes [ MAX_CLIENTS ];
	int send_local_msg ( std::string msg );
};


#endif // SERVER_H
