# port-forwarder
#port forwarder using c++ , using sockets , pthread, select and macros related to select.
#We will us telnet as client to connect terminals.

sudo apt-get install g++
sudo apt-get install telnet
g++ -std=c++11 -I include/ src/server.cpp main.cpp -lpthread -o server.cpp
./server  #or you can configure it to listening in another port running : ./server --local-port 5465
#The server should be ready and listening

#open several terminals and run :
telnet 127.0.0.1 2706 ( or replace 2706 by the port that you chose )

#You should be able to interect between different terminals as a chat room.
