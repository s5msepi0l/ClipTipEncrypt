#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include "rc4.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef NETWORKING_T
#define NETWORKING_T
#define BACKLOG 256

typedef unsigned long long int Uint64_t;

static Uint64_t primeNum = 0xC0CFD797;
static int optval = 1;
//ska finnas TCP_client å
class TCP_server
 {
 	public:
		

		
		void accept_n();
		Uint64_t keyExchange(int socket_fd);
protected:
		int socket_fd;
		struct sockaddr_in serv;
};

//för allmänt skit
int TCP_server::init(int port) {
	srand((unsigned)time(NULL));
	
	this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int)) < 0){
		fprintf(stderr, "[*]Couldn't reuse port\n");
		return -1;
	}
	this->serv.sin_family = AF_INET;
	this->serv.sin_port = htons(port);
	this->serv.sin_addr.s_addr = INADDR_ANY;

	int addrlen = sizeof(this->serv);
	if (bind(this->socket_fd, (struct sockaddr*)&this->serv, addrlen) < 0) {
		fprintf(stderr, "[*]Invalid address");
	}
	listen(this->socket_fd, BACKLOG);
	return 0;
}
//janky asf
Uint64_t TCP_server::keyExchange(int socket_fd) {
	Uint64_t *P = &primeNum, key; 
	int PG = 5, private_key = rand();
	Uint64_t public_key = static_cast<Uint64_t>(pow(PG, private_key)) % (Uint64_t)P;

	send(socket_fd, (const void*)&P, sizeof(Uint64_t), 0);
	send(socket_fd, (const void*)&PG, sizeof(Uint64_t), 0);
	send(socket_fd, (const void*)&public_key, sizeof(Uint64_t), 0);

	recv(socket_fd, (void*)&key, sizeof(Uint64_t), 0);
	return key;
}
#endif
