#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>


using namespace std;

void perror_and_exit(const char * szErrorInfo){
	printf("errno=%d,", errno);
	perror(szErrorInfo);
	exit(1);
}

int main( int argc, char *argv[] ){
	int epfd = epoll_create(256);
	if( epfd == -1 ){
		perror_and_exit("epoll_create failed");
	}

	int listenfd = socket( AF_INET, SOCK_STREAM, 0 );
	if( listenfd == -1 ){
		perror_and_exit("socket() failed");
	}

	{
		struct epoll_event ev;
		ev.events = EPOLLIN|EPOLLET;
		ev.data.fd = listenfd;
		epoll_ctl( epfd, EPOLL_CTL_ADD, listenfd, &ev );
	}

	// listening on the port
	{
		struct sockaddr_in serveraddr;
		bzero( &serveraddr, sizeof(serveraddr) );
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(8888);
		const char *local_ip = "0.0.0.0";
		inet_aton( local_ip, &serveraddr.sin_addr );
		bind( listenfd, (sockaddr*)&serveraddr, sizeof(serveraddr) );
		listen( listenfd, 10 );
	}

	static const int MAX_READY_EVENTS = 256;
	struct epoll_event events[MAX_READY_EVENTS];
	while (true){
		printf("working...\n");
		int nfds = epoll_wait( epfd, events, MAX_READY_EVENTS, 3000 );
		printf("ntds=%d\n", nfds);
		for( int nIndex = 0; nIndex<nfds; ++nIndex ){
			struct epoll_event & active_event = events[nIndex];

			if( active_event.data.fd == listenfd ) {
				cout<<"accepting new client..."<<endl;

				struct sockaddr_in clientaddr;
				socklen_t clilen = sizeof(clientaddr);
				int connfd = accept( listenfd, (sockaddr*)&clientaddr, &clilen );
				if( connfd<0 ){
					perror_and_exit("connfd<0");
				}

				char * remote_ip = inet_ntoa( clientaddr.sin_addr);
				cout<<"accept a connection from " << remote_ip<<endl;

				epoll_event clientEv;
				clientEv.data.fd = connfd;
				clientEv.events = EPOLLIN|EPOLLET;
				int add_result = epoll_ctl( epfd, EPOLL_CTL_ADD, connfd, &clientEv);
				if ( -1==add_result ){
					perror_and_exit( "EPOLL_CTL_ADD failed" );
				}



			}
			else if( active_event.events & EPOLLIN ){
				cout<<"EPOLLIN"<<endl;

				char line[1024]={0};
				int nReaded = read( active_event.data.fd, line, sizeof(line)-1 );
				if( nReaded <= 0 ){
					perror("nReaded<=0");

					if ( nReaded == 0 ){
						printf("client closed gracefully!\n");
					}
					
					epoll_ctl( epfd, EPOLL_CTL_DEL, active_event.data.fd, 0 );
					close( active_event.data.fd );
				}
				else{
					cout<<"Readed: ["<<line<<"]"<<endl;
					int nSended = send( active_event.data.fd, line, nReaded, 0 );
					if( -1==nSended ){
						perror( "send" );
						printf("errno=%d\n", errno );
						if( errno == EAGAIN ){
							printf("It's time to do something. the buffer is not sent.\n");
						}
					}
				}

			}
			else{
				printf("errno=%d, unkown event type!\n", errno );
			}
			
		}
	}


	return 0;
}

