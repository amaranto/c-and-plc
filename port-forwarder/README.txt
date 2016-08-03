
#port forwarder using c++ , sockets , pthread, select and some macros.
#We will us telnet as client to connect terminals.

sudo apt-get install g++ telnet
g++ -std=c++11 -I include/ src/server.cpp main.cpp -lpthread -o server.cpp
./server  
# or ./server --local-port SOME_PORT
#The server should be ready and listening

#open few terminals and run :
telnet 127.0.0.1 2706 ( or replace 2706 by the port that you chose in the previous step)

#You should be able to interect between different terminals as a chat room.
