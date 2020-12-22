#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 8080



struct sockaddr_in serv_addr = {AF_INET, htons(PORT)};
int inet = inet_pton ( AF_INET, "127.0.0.1", &serv_addr.sin_addr);
int obj_socket = socket (AF_INET, SOCK_STREAM, 0 );
auto connection_obj = connect( obj_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr ));

int socket_send(std::string message)
{
    char buffer[1024] = {0};

    if (obj_socket < 0)
    {
        printf ( "Socket creation error !" );
        return -1;
    }

    // Converting IPv4 and IPv6 addresses from text to binary form
    if(inet <= 0)
        {
        printf ( "\nInvalid address ! This IP Address is not supported !\n" );
        return -1;
    }

    if (connection_obj  < 0)
    {
        printf ( "Connection Failed : Can't establish a connection over this socket !" );
        return -1;
    }

    send ( obj_socket , message.c_str(), message.size() , 0);
    return 0;
}
