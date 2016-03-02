#include "server.h"

Server::Server ( int port ) {

	s_addr.sin_port 	= htons ( port );
	s_addr.sin_family	= AF_INET;
	s_addr.sin_addr.s_addr  = INADDR_ANY;
	addr_len = sizeof ( struct sockaddr_in );

	s_local_addr.sin_port   = htons ( port );
	s_local_addr.sin_family = AF_INET;
	s_local_addr.sin_addr.s_addr = inet_addr ( "127.0.0.1" );
   
	t_accept = 0;
	t_receive = 0;

	this->is_on = false;
}

Server::~Server () { }

static void* t_accept_connections ( void* args ){
	return ( (Server*)args )->accept_conn();
}

static void* t_get_msg ( void* args ){
	return ( (Server*)args )->get_msg();
}

int Server::on () {

   if ( is_on ) return -1;
   else is_on = true;

   master_sock = socket( AF_INET, SOCK_STREAM, 0);
   if (master_sock <=0 ){
        std::cout << RED << "\n[ ERROR ]" << RESET  << " Master sock can't be created\n";
        is_on = false;
        return  -2;
   }

   if ( bind( master_sock, (struct sockaddr*)&s_addr, sizeof(s_addr) ) != 0 ){
        std::cout << RED <<  "\n[ ERROR ]" << RESET << " Can't bind the file descriptor\n";
        is_on = false;
        return  -3;
   }

   if ( listen( master_sock, MAX_CLIENTS ) != 0 ){
       std::cout << RED <<  "\n[ ERROR ]" << RESET << " Listen failed\n";
       is_on = false;
       return -4;
   }

   if ( pthread_create ( &t_accept, NULL, &t_accept_connections, this )  != 0 ){
       std::cout << RED << "\n[ ERROR ]" << RESET << "Thread to accept incomming connections failed\n";
       is_on = false;
       return -6;
   }

	
   if ( pthread_create ( &t_receive, NULL, &t_get_msg, this ) != 0 ) {
        std::cout << RED << "\n[ ERROR ]" << RESET << "Thread to get incomming msg failed\n";
       	is_on = false;
        return -7;
   }


   //####### Creating local communication #############   


   local_sock = socket ( AF_INET, SOCK_STREAM, 0);

   if ( local_sock <= 0) {
      std::cout << RED << "[ ERROR ] Can't create socket \n";
      is_on = false;
      return local_sock;
   }

	
   if ( connect ( local_sock, (struct sockaddr*)&s_local_addr, sizeof( s_local_addr ) )  < 0 ) {
        std::cout << RED << "[ ERROR ] Can't reach to the server \n";
       	is_on = false;
        close( local_sock );
        return -10;
   }

   //######################################################

   std::cout << GREEN << "\n[ OK ]" << RESET  << "Server started succesfully !\n";
   return master_sock;
}

void* Server::accept_conn () {

    int index = 0,
	client_sock = 0;

    struct sockaddr_in addr_client;

    while ( is_on ) {
	
        bzero ( &addr_client, sizeof (addr_client) ); // cleaning struct sockaddr
        client_sock = accept( master_sock, (sockaddr*)&addr_client, (socklen_t*)&addr_len );

        if ( client_sock <= 0 ) {
		
            this->send_local_msg ( "{ msg : { type:error, text: error accepting client }  }\0" );   
            continue;
        }
	else{	

	   for ( int i = 0; i < MAX_CLIENTS; i++ )
		if ( clientes[i] == 0 )	{
			 clientes[i] = client_sock;
			 break;	   
		}

	   this->send_local_msg ( "{ msg : { type:notification, text: new connection started } } \0"  );
	}	
    }
}

int Server::wait (){

	if ( pthread_join ( t_accept, NULL) != 0 || pthread_join ( t_receive, NULL ) != 0 ) return -1;
	else return 0;
}

int Server::send_local_msg ( std::string buffer ){

        int buffer_len = -1;
        buffer_len = send ( local_sock, buffer.c_str(), 256, 0 );
	
		        
        return buffer_len;
}

void* Server::get_msg (){
	
	
	char* msg = (char*) malloc ( sizeof(char*) * 256 ); 
	int buffer_len = -1;

	fd_set readfds;
		
		
	sleep (2); // wait for local socket to avoid dead lock		
	while ( is_on ) {	
		
		bzero ( msg, sizeof (char*)*256 );
		FD_ZERO ( &readfds );

    		for ( int i = 0; i < MAX_CLIENTS; i++) 
			if ( clientes[i] > 2 ) // The 3 first fd are reserved for standard input, standard output and standard error 
	        		FD_SET( clientes[i], &readfds );

    		int returned = select( MAX_CLIENTS + 1, &readfds, NULL, NULL, NULL);

    		if (returned > 0 ) {

        		for ( int i = 0; i < MAX_CLIENTS; i++) {

				if ( clientes[i] <= 2 ) continue; // avoid read and configure standars i/o

            			if ( FD_ISSET( clientes[i] , &readfds) ) { // search the fd ready to receive

                			buffer_len = recv ( clientes[i], msg, 256, 0);
					if ( buffer_len <= 0 ) { // it is equal to 0 when the connection is closed or started via telnet

						std::cout << YELLOW << "[ INFO ]" << " Connection close !\n";
						close ( clientes[i] ); // closing socket
						clientes[i] = 0; // get out fd from the array
						break; 
					}
					else if ( clientes[i] == local_sock + 1 ) {
						std::cout << CYAN << "[ LOCAL ]" <<  RESET << "-> " << msg << "\n";
						break;
					}
					else {
						std::cout << GREEN << "[ IN ] "<< RESET << clientes[i] << " says -> " <<  msg ;

						for ( int j = 0; j < MAX_CLIENTS; j++ ){ //forward the msg received to all the rest except by internal server and sender
							if ( clientes [j] == clientes [i] || clientes[j] == 0 || clientes[j] == local_sock + 1 ) continue;
							std::cout << MAGENTA << "[ FOWARDING ]" << RESET << " Sending to " << clientes[j] << std::endl; 	
							send ( clientes[j], msg , 256 , 0 );
						}		

						break;
					}
            			}

        		}
    		}
	}	

	return (void*) buffer_len;
}
