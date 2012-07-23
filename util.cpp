//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <fcntl.h>
//#include <unistd.h>
#include <stdio.h>
#include <errno.h>

bool setnonblocking( int sock )
{
	int opts=fcntl(sock,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(sock,GETFL)");
        return false;
    }

    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
        perror("fcntl(sock,SETFL,opts)");
		return false;
    }   
	return true;
}
