#include "Server.h"

/** ################
    Constructors
###################*/
Server::Server( int port )
{
    s_addr.sin_port        = htons( port );
    s_addr.sin_family      = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    addr_len = sizeof(struct sockaddr_in);

    t_accept = 0;
    t_receive = 0;

    this->is_on = false;
    this->with_log = true;
    this->display_output = true;
    this->port = port;
}

Server::~Server()
{
    //dtor
}

/** ###################################################
 Statics functions to call pthreads void* functions
 ###################################################**/

static void* t_accept_connections ( void* args ){
    return ( (Server*)args )->accept_conn();
}


/** ############################
    Creating master socket
###############################*/

int Server::on(){

    if ( is_on ) return -1;

    write_output( "Configure");

    master_sock = socket( AF_INET, SOCK_STREAM, 0);
    if (master_sock <=0 ){
         write_output ("[ ERROR ] Master sock can't be created");
        return  -2;
    }

    if ( bind( master_sock, (struct sockaddr*)&s_addr, sizeof(s_addr) ) != 0 ){
          write_output( "[ ERROR ] Can't linked sock");
          return  -3;
    }

    if ( listen( master_sock, MAX_CLIENT ) != 0 ){
        write_output( "[ ERROR ] Listen failed");
        return -4;
    }

    is_on = true;

    if ( pthread_create ( &t_accept, NULL, &t_accept_connections, this )  != 0 ){
          write_output("[ ERROR ]Thread to accept incomming connections failed");
          return -6;
    }
    write_output( "[ OK ] Configure");
    return master_sock;
}

/** ################################# */


void* Server::accept_conn() {

    int client_sock = 0;
    struct sockaddr_in addr_client;
    std::string error_msg;

    while ( is_on ) {

        Client* client;

        bzero ( &addr_client, sizeof (addr_client) ); // cleaning struct sockaddr
        client_sock = accept( master_sock, (sockaddr*)&addr_client, (socklen_t*)&addr_len );

        if ( client_sock <= 0 ) {
            error_msg = "Incomming connection failed. IP=" + (std::string)inet_ntoa(addr_client.sin_addr);
            this->write_output ( error_msg );
            continue;
        }
        else if ( MAX_CLIENT == clients.size() ) {
            error_msg = "MAX_CLIENT reached. Connection refuse to" + (std::string)inet_ntoa( addr_client.sin_addr );
            this->write_output( error_msg );
            close ( client_sock );
            continue;
        }

        client = new Client( client_sock, addr_client);
        client->run();
        clients.push_back ( client ); // adding the new client into client container

        this->write_output( "New client connected, IP=" + (std::string)inet_ntoa(addr_client.sin_addr) );
    }

    return (void*) 0;
}

/** ##########################
#        Pthread join        #
#############################*/
int Server::wait (){
    if ( pthread_join ( t_accept, NULL) != 0 ) return -1;
    else return 0;
}

int Server::wait ( int sec ){
    sleep ( sec );
    if ( pthread_detach ( t_accept) != 0 ) return -1;
    else return 0;
}

/** #####################################
#        Server log Configuration       #
#######################################*/

int Server::log ( bool status ){
    with_log = status;
    return 0;
}

int Server::write_output( std::string buffer ){

    if ( display_output ) std::cout << buffer << std::endl;
    if ( with_log == false ) return -1;

    time_t log_time;
    time ( &log_time );

    std::ofstream log_file ("logs/server.log", std::ios::app);

    if ( log_file.is_open() ) {
        log_file << buffer << " " << ctime ( &log_time ) ;
        log_file.close();
        return 0;
    }
    else
        return -2;
}
