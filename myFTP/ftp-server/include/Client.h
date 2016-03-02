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

class Thread
{
    public:
        Thread();
        virtual ~Thread();

        int start ();
        int join  ();
        int detach();
        pthread_t self ();
        virtual void* run()=0;

    protected:

    private:
        pthread_t m_tid;
        int      m_running;
        int      m_detached;
};

class Client:public Thread
{
    public:

        Client();
        Client (int sock, struct sockaddr_in s_addr );

        void* run();

        int get_port ();
        int get_socket ();
        std::string get_ip ();
        std::string get_nick();
        struct sockaddr_in get_addr_in ();

        int create_user( std::string nick , std::string password, std::string group = "default" );
        int validate ( std::string nick , std::string pass );
        int send_file ( std::string path );

        int log ( bool status );
        int display ( bool status);
        int write_output ( std::string buffer );
        virtual ~Client();

    protected:

    private:

        int sock;

        bool with_log,
             display_output,
             keep_alive;
        struct sockaddr_in s_addr;
        time_t connection_time;
        std::string ip,
                    username;
		
		int error_handler ( std::string error_msg );
        std::string get_protocol ( std::string buffer );
        std::string get_value    ( std::string buffer );
};


#endif // CLIENT_H
