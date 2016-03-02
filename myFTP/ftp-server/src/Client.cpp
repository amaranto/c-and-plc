#include "Client.h"

/*******************************************
*               THREAD CLASS               *
*******************************************/

Thread::Thread() : m_tid (0), m_running (0), m_detached (0) { /*constructor*/ }

Thread::~Thread()
{
    if ( m_running == 1 && m_detached == 0 )
            pthread_detach( m_tid );
    else if ( m_running == 1 )
            pthread_cancel ( m_tid );
}

static void* runThread ( void* args );

int Thread::start() {

    if ( pthread_create ( &m_tid, NULL, &runThread, this ) == 0 ) {
        m_running = 1;
        return 0;
    }
    else
        return -1;
}

static void* runThread( void* args ){
    return ( (Thread*)args )->run ();
}

int Thread::join() {

   int success = -1;

   if ( m_running == 1 ) {
         success = pthread_join( m_tid, NULL );
         if ( success == 0 ) m_detached = 1;
   }

    return success;
}

int Thread::detach() {

    int success = -1;

    if ( m_running == 1 && m_detached == 0 ){
        success = pthread_detach ( m_tid );
        if ( success == 0 ) m_detached = 1;
    }

    return success;
}

pthread_t Thread::self() {
    return m_tid;
}


/********************************************************
*                       CLIENT CLASS                    *
********************************************************/
//fcntl ( client_sock, F_SETFL, O_NONBLOCK ); //non-blocking sock

Client::Client ( int sock, struct sockaddr_in s_addr){
    this->with_log = true;
    this->keep_alive = true;
    this->display_output = true;
    this->username = "Anon";
    this->sock = sock;
    this-> s_addr = s_addr;
    this->ip = inet_ntoa(s_addr.sin_addr);
}

Client::~Client() {}

/** ##################################
#  Starts protocols in a new thread  #
####################################*/

void* Client::run(){

    int result = 0;
    char* buffer = (char*)malloc ( sizeof(char*)*1024 );
    std::string value,
                protocol;
//--------------------------------------------------------------
    this->write_output( "Presentation");

    std::string response = "<220> srvFtp version 1.0";
    result = send ( sock, response.c_str(), response.size(), 0);

    if (result <= 0 ) {
        this->error_handler( "[ ERROR ] Presentation failed");
        return (void*) -20;
    }
    this->write_output( "[ OK ] Presentation");
//------------------------------------------------------

    this->write_output( "Authentication");

    memset( buffer, 0, 1024);
    result = recv ( sock, buffer, 1024, 0 ); // Waiting for username
    username = get_value( (std::string)buffer );
    protocol = get_protocol( (std::string)buffer );

    if ( result <= 0 || protocol.compare("<USER>") != 0 ) { // Check response
        this->error_handler( "[ ERROR ] Authentication : USER <username> was expected | " + username);
        return (void*) -21;
    }
//--------------------------------------------------------

    response = "<331> Password required for " + username; // Asking password
    result = send ( sock, response.c_str(), response.size(), 0 );

    if ( result <= 0 ) {
        this->error_handler( "[ ERROR ] Authentication: Can't send 331 code");
        return (void*)-22;
    }

//-------------------------------------------------------------------

    memset( buffer, 0, 1024); // cleaning buffer
    result = recv ( sock, buffer, 1024, 0 ); //Waiting password
    response = (std::string) buffer;
    protocol = get_protocol( response );
    std::string password = get_value ( response );

    if ( result <= 0 || protocol.compare ("<PASS>") != 0 ){
        this->error_handler ( "[ ERROR ] Authentication : PASS <password> was expected | " + response);
        return (void*) -23;
    }

//------------------------------------------------------------------------

    if ( validate( username, password ) == 0 ){

        response = "<230> User " + username + " logged in"; // Asking password
        result = send ( sock, response.c_str(), response.size(), 0 );
        if ( result <= 0 ) {
            this->error_handler( "[ ERROR ] Confirmation failed");
            return (void*)-24;
        }

    }
    else {
        response = "<530> User " + username + " loggin incorrect"; // Asking password
        result = send ( sock, response.c_str(), response.size(), 0 );
        this->error_handler( "[ ERROR ] Authentication failed, unknow user or pass.");
        return (void*)-25;
    }

    this->write_output( "[ OK ] Authentication"); // Authentication success

//------------------------------------------------------------------------------------
        /**###################################
        #     Starting FTP action protocols  #
        ####################################*/

    while ( keep_alive ) {

        memset( buffer, 0, 1024); // cleaning buffer
        result = recv ( sock, buffer, 1024, 0 ); //Waiting password
        response = (std::string) buffer;
        protocol = get_protocol( response );

        if ( result <= 0 ){
            this->error_handler( "[ ERROR ] Protocol : client disconnected ");
            return (void*) -23;
        }
		
		/** TRANSFER PROTOCOL **/
        if ( protocol.compare ("<RETR>") == 0 ){
		
		std::string output;	
		std::string path = get_value( response ); // Saving password
           	result = this->send_file ( path );
			
		if ( result < 0 ) output = "[ ERROR ] Transfer failed " + std::to_string (result) ;
		else output = "[ SUCCESS ] Transfer done";
		
		write_output ( output );
        }
    }
    
    //Server stopped
    return (void*) 0;
}

/** ############################
#           Functions          #
##############################*/
std::string Client::get_protocol( std::string buffer ){

    std::regex re("^<[1-600]+>|<[A-Z]+>", std::regex::extended );
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

int Client::create_user( std::string nick , std::string password, std::string group ){

    time_t session_time;
    time ( &session_time );

    std::ofstream users_file ( "ftpusers" , std::ios::app);

    if ( users_file.is_open() ) {
        users_file << nick << "|" << password << "|" << group << "|"<< ctime ( &session_time ) ;
        users_file.close();
        return 0;
    }
    else
        return -2;
    return 0;
}

int Client::validate( std::string nick , std::string password ){

    time_t session_time;
    time ( &session_time );

    std::string line,
                query = nick + ":" + password;

    std::ifstream users_file ( "ftpusers" , std::ios::app);
		
    if ( users_file.is_open() ) {

        while ( getline (users_file,line) )
            if ( line.find(query) == 0) return 0;

        users_file.close();
    }

    return -1;
}

int Client::send_file( std::string path ){

    int   transfer_sock = -1,
          transfer_port = 0,
          result = 0,
          b_size =  sizeof (char*)*34;

    char* buffer = (char*) malloc ( b_size );

    std::string protocol,
                value,
                response;

    struct sockaddr_in transfer_addr;
    //-----------------------------------------------------
    // Get transfer port from client using master sock    |
    //----------------------------------------------------- 
    memset( buffer, 0, b_size);
    result = recv ( sock, buffer, b_size, 0 );
    protocol = get_protocol( (std::string)buffer );
    value = get_value( (std::string)buffer );

    if ( result <= 0 || protocol.compare ("<PORT>") != 0 ){
            this->write_output( "[ ERROR ] Protocol : can't get port");
            return -23;
    }
  //-----------------------------------------------		
    transfer_port = atoi ( value.c_str() );     //| Port where the client is listening
  //-----------------------------------------------
    memset ( buffer, 0 , b_size );
    result = recv ( sock, buffer, b_size, 0);
    protocol = get_protocol ( (std::string)buffer );
    value = get_value ( (std::string)buffer );
    
    if ( result <= 0 || protocol.compare ("<READY>") != 0 ) {
        this->write_output ("[ ERROR ] Transfer protocol: Error protocol, <READY> is expected");
        close ( transfer_sock );
        return -31;
    }
    
//--------------------------------------------------
//	Creating new connection and start transfer |
//--------------------------------------------------

    transfer_sock = socket ( AF_INET, SOCK_STREAM, 0);

    if ( transfer_sock <= 0) {
       this->write_output ("[ ERROR ] Transfer protocol: Can't create sock");
       return transfer_sock;
    }

    transfer_addr.sin_family = AF_INET;
    transfer_addr.sin_addr.s_addr = inet_addr( ip.c_str() );
    transfer_addr.sin_port =  htons ( transfer_port );

    if ( connect ( transfer_sock, (struct sockaddr*)&transfer_addr, sizeof( transfer_addr ) )  < 0 ) {
       this->write_output ("[ ERROR ] Transfer protocol: Can't connect to client. ");
       close( transfer_sock );
       return -30;
    }
    
    std::ifstream file ( path.c_str() , std::ios::in | std::ios::binary );
    
    if ( file.is_open() ) {
		
		long transfer_size = 0,
		     cursor_pos    = 0;
                //------------------------------------------------------------
		file.seekg ( 0, std::ios::end );   // getting the size	     |
		long f_size =  file.tellg();	   //  of the file	     |
		file.seekg ( 0, std::ios::beg );   // to read by blocks	     |
		//------------------------------------------------------------
	
		std::regex re_filename("[a-zA-Z0-9-_. +]+$", std::regex::extended );
    		std::smatch match_filename;

    		std::regex_search(path, match_filename, re_filename);
		
		if ( match_filename.empty () ) {
			close ( transfer_sock );
			return -49;
		}

		std::string filename = match_filename[0];
		response = "<229> " + filename + "\0";

		result = send ( transfer_sock, response.c_str(), response.length(),0);
		
		if ( result <= 0 ){
			close ( transfer_sock );
			return -50;
		}
			
        	do{
			
	    		memset ( buffer, 0, sizeof (buffer) );

			cursor_pos = file.tellg();
			//if ( cursor_pos < 0 ) break;

			file.seekg ( cursor_pos, std::ios::beg );
            		file.read ( buffer, b_size );

            		result = send ( transfer_sock, buffer, 1024 ,0 );
            		if ( result <= 0 ){
				close ( transfer_sock );
				return -51;
			}
			
            		transfer_size = transfer_size + result;
			std::string status = "Transfer " + std::to_string(transfer_size) + "/" + std::to_string ( f_size ) + "\r";
			write_output ( status );
			
        	} while ( transfer_size < f_size );	
        
        response = "<226> Transfer complete\0";
        result = send ( transfer_sock, response.c_str(), response.length(), 0);
        write_output (" [ OK ] ");

        std::string info_transaction = "To: " + ip + ", Port:" + std::to_string (transfer_port) + ", File : " + path + ", Size: " + std::to_string (transfer_size);
        write_output ( info_transaction );
        
        file.close();
	close ( transfer_sock );
	return 0;
    }
    else {
		response = "<550> " + path + " : no such file or directory";
		result = send ( transfer_sock, response.c_str(), response.length(), 0);
		write_output ( response );
		return -53;
	}
}


/** #####################################
#        Client log Configuration       #
#######################################*/

int Client::error_handler ( std::string error_msg ){
	    this->write_output( error_msg );
        close( sock );
        keep_alive=false;
}

int Client::log ( bool status ){
    with_log = status;
    return 0;
}

int Client::display ( bool status ){
    display_output = status;
    return 0;
}

int Client::write_output( std::string buffer ){

    if ( display_output ) std::cout << "\n" << buffer << std::endl;
    if ( with_log == false ) return -1;

    time_t log_time;
    time ( &log_time );

    std::ofstream log_file ( "logs/clients.log" , std::ios::app);
	buffer = ip + "-> " + buffer; //adding IP information to the log 
	
    if ( log_file.is_open() ) {
        log_file << buffer << " " << ctime ( &log_time ) ;
        log_file.close();
        return 0;
    }
    else
        return -2;
}
