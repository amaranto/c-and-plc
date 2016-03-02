#include "Client.h"

Client::Client( int port, std::string host, std::string username )
{
    this->port = port;
    this->host = host;
    this->username = username;
    this->with_log = true;
    this->display_output = true;
    this->connected = false;
}

Client::~Client()
{
    //dtor
}

/**##############################
#   connect to remote server    #
//#############################*/

int Client::start () {

     int result = 0;

     char* buffer = (char*) malloc( sizeof(char*)*1024 );
     std::string protocol,
                 value;
//-------------------------------------------------------------
     this->write_output ("Connection");

     sock = socket ( AF_INET, SOCK_STREAM, 0);
     if ( sock <= 0) {
        this->write_output ("[ ERROR ] Can't create socket.");
        return sock;
     }

     s_addr_clt.sin_family = AF_INET;
     s_addr_clt.sin_addr.s_addr = inet_addr( host.c_str() );
     s_addr_clt.sin_port =  htons ( port );

     if ( connect ( sock, (struct sockaddr*)&s_addr_clt, sizeof( s_addr_clt ) )  < 0 ) {
        this->write_output ("[ ERROR ] Can't reach to the server");
        close( sock );
        return -10;
     }

     this->write_output ("[ OK ] Connection");
//--------------------------------------------------------------------
     this->write_output ("Presentation");

     memset( buffer, 0, 1024); // clean buffer
     result = recv ( sock, buffer, 1024,0 );
     protocol = get_protocol( (std::string)buffer );
     value    = get_value ( (std::string)buffer );

     if ( result <= 0 || protocol.compare("<220>") !=0 ) {
        this->write_output ("[ ERROR ] Presentation: 220 is expected | Response:" + protocol + " " + value);
        close( sock );
        return -20;
     }

     this->write_output ("[ OK ] Presentation: " + protocol + value );
//------------------------------------------------------------------------------
     this->write_output ("Authentication" );

     std::string response = "<USER> " + username; // Sending username
     result = send ( sock, response.c_str(), response.size(),0 );
     if ( result <= 0 ){
        this->write_output ("[ ERROR ] Authentication: Can't send username" );
        close( sock );
        return -21;
     }

//-----------------------------------------------------------
     memset( buffer, 0, 1024);
     result = recv ( sock, buffer, 1024,0); // Waiting for an answer . 331 is expected.
     protocol = get_protocol( (std::string)buffer );
     value    = get_value ( (std::string)buffer );

     if ( result <= 0 || protocol.compare("<331>") !=0 ) {
        this->write_output ("[ ERROR ] Presentation: 331 is expected | Response:" + protocol + " " +value);
        close( sock );
        return -22;
     }

     std::cout << "-> " << buffer;
     std::cout << GREEN << " Password: " << RED;
     std::cin >> response;
     std::cout << RESET << std::endl;
//-------------------------------------------------------------------
     response = "<PASS> " + response;
     result = send ( sock, response.c_str(), 1024, 0);
     if ( result <=0 ){
        this->write_output ("[ ERROR ] Authentication: Can't send password");
        close( sock );
        return -23;
     }
//------------------------------------------------------------------

     memset( buffer, 0, 1024);
     result = recv ( sock, buffer, 1024,0); // Waiting for an answer . 331 is expected.
     protocol = get_protocol ( (std::string)buffer );
     value    = get_value ( (std::string)buffer );

     if ( result <= 0 || protocol.compare ("<230>") !=0 ) {
        this->write_output ("[ ERROR ] Presentation: 200 is expected | Response: " + protocol + " " + value );
        close( sock );
        return -22;
     }
     else 
        this->write_output ("[ OK ] Authentication: " + protocol + " " + value );
     
     return sock;
}

/** #####################################
#        Client FTP functions           #
#######################################*/

int Client::get_file( std::string path ) {
	
    int result = 0;
	
    std::string line = "<RETR> " + path;
    result = send ( sock, line.c_str(), line.length(), 0);
    sleep (5);	
    line = "<PORT> 11011";
    result = send ( sock, line.c_str(), line.length(), 0);

    if ( result <=0 ){
       this->write_output ("[ ERROR ] Transfer Protocol: Can't send start transfer protocol");
       close( sock );
       return -23;
    }

    /********************************************************************
                           Transfer connection Protocol					
    ********************************************************************/

    char* buffer = (char*)malloc ( sizeof (char*)*512 );
    struct sockaddr_in child_addr,
					   master_addr;
	       
    master_addr.sin_port        = htons( 11011 );
    master_addr.sin_family      = AF_INET;
    master_addr.sin_addr.s_addr = INADDR_ANY;
    int addr_len = sizeof ( struct sockaddr_in);
    int yes = 1; // set the socket as SO_REUSEADDR
	
    int tmp_sock = socket( AF_INET, SOCK_STREAM, 0);
    if (tmp_sock <=0 ){
        write_output ("[ ERROR ] Master sock can't be created");
        return  -2;
    }

    if ( setsockopt(tmp_sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) 
    	write_output ( "[ WARNING ] Can't set sock as SO_REUSEADDR.");

    if ( bind( tmp_sock, (struct sockaddr*)&master_addr, sizeof(master_addr) ) != 0 ){
          write_output( "[ ERROR ] Can't linked sock");
          return  -3;
    }

    if ( listen( tmp_sock, 1 ) != 0 ){
        write_output( "[ ERROR ] Listen failed");
        return -4;
    }
   	
    write_output( "[ OK ]  Transfer protocol: socket created ");

//-------------------------------------------------------
// 		Sending confirmation			|
//-------------------------------------------------------

    write_output ( "Transfer Protocol: sending confirmation");

    line = "<READY> Transfer Protoco: is ready";
    result = send ( sock, line.c_str(), line.length(),0);
    if ( result <= 0 ){
    	write_output ( "[ ERROR ] Treansfer Protocol: error sending confirmation");
	close ( tmp_sock);
	close ( sock );
	return -21;
    } 	
	
    write_output ("Waiting signal from server ");
    int transfer_sock = accept ( tmp_sock, (struct sockaddr*)&child_addr, (socklen_t*)&addr_len);
    shutdown ( tmp_sock, 2 );   
 
    if  ( transfer_sock <= 0){
		write_output ( "[ ERROR ] Transfer Protocol: Can't accept client");
		close ( tmp_sock);
		close ( transfer_sock);
		return -23;
    }
   write_output ( "[ OK ] Transfer protocol: Connection ready"); 
//-----------------------------------------------------
//		Evaluate protocol using transfer_sock  |
//-----------------------------------------------------

    memset( buffer, 0, 1024);
    result = recv ( transfer_sock, buffer, 1024,0);
    if ( result <= 0 ){
			write_output ("[ ERROR ] Transfer Protocol: Can't receive protocol");
			close ( transfer_sock );
			return -25;
    }
	
    std::string response = ( std::string ) buffer;
    std::string protocol = get_protocol ( response );
    std::string value = get_value ( response );
 	  
    if ( protocol.compare ("<229>") == 0 ){ // Server says: file exist in the server
		
		write_output ( "File found: " + response );		
		
		int total_transfer = 0;
		std::ofstream file;
		std::string filename = value;
		std::string filepath = "downloads/" + filename;

		file.open ( filepath.c_str(), std::ios::binary );
	
		do {
			memset( buffer, 0, 1024);
			result = recv ( transfer_sock, buffer, 1024,0);
			response = (std::string) buffer;
			if ( response.compare ( "<226> Transfer complete" ) == 0 ) break;
			else if ( result <= 0){ 
				close ( transfer_sock );
				return -26;
			}

			file << response;
			total_transfer = total_transfer + sizeof (result);
			
		} while ( true );
		
		write_output ( response );
		close ( transfer_sock);
		return total_transfer;
	}
	else if ( protocol.compare ("<550>") == 0 ){ // File not found
		write_output ( response );
		close ( transfer_sock );
		return -27;
	}
	else {  
		write_output ( "[ ERROR ] Transfer protocol : Internal server error. Server said : " + response ); 
		close ( transfer_sock );
		return -28;
    	}
    
    return -29;
}

/** ##############################################
#                   Functions                    #
################################################*/

std::string Client::get_protocol( std::string buffer ){

    std::regex re("^<.*>", std::regex::extended );
    std::smatch m;

    std::regex_search(buffer, m, re);

    if ( m.empty() ){
        write_output( "[ERROR 500]");
        return "";
    }
    else return m[0];
}

std::string Client::get_value   ( std::string buffer ) {

    std::regex re_del ("^<.*> " ,std::regex::extended );
    std::string tmp = ""; // split the line without spaces

    tmp = std::regex_replace ( buffer, re_del, ""); //verify that the value only has number and/or letters

    if ( tmp.empty() ) {
        write_output( "[ERROR 502]");
        return "";
    }
    else
        return tmp;

}

/** #####################################
#        Client log Configuration       #
#######################################*/

int Client::log ( bool status ){
    with_log = status;
    return 0;
}

int Client::display( bool status ){
    display_output = status;
    return 0;
}

int Client::write_output( std::string buffer ){

    if ( display_output ) std::cout << "\n" << buffer << std::endl;
    if ( with_log == false ) return -1;

    time_t log_time;
    time ( &log_time );
    
    std::string log_path = "logs/" + username;
    std::ofstream log_file ( log_path.c_str(), std::ios::app );

    if ( log_file.is_open() ) {
        log_file << buffer << " " << ctime ( &log_time ) ;
        log_file.close();
        return 0;
    }
    else
        return -2;
}
