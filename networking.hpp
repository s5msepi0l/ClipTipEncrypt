#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdint>
#include <cstring>
#include <cmath>
#include "rc4.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#ifndef NETWORKING_T
#define NETWORKING_T
#define BACKLOG 8

typedef unsigned long long int u128;

u128 keyExchange(int socket_fd);
static u128 primeNum = 0xC0CFD797;
static int optval = 1;
static int default_port = 2222;
//ska finnas TCP_client å
class TCP_server
{
 	public:
        TCP_server(int port) {
            srand((unsigned)time(NULL));

            this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int)) < 0){
                fprintf(stderr, "[*]Couldn't reuse port\n");
            }
            this->serv.sin_family = AF_INET;
            if (port != 0) {
            this->serv.sin_port = htons(port);
            std::cout << "cut\n";
            } else {
            	this->serv.sin_port = htons(default_port);
            	std::cout << "def\n";
            }

            this->addrlen = sizeof(this->serv);

        }

		virtual int accept_n() {
            this->serv.sin_addr.s_addr = INADDR_ANY;
            if (bind(this->socket_fd, (struct sockaddr*)&this->serv, this->addrlen) < 0) {
                throw std::runtime_error("Error binding socket");
            }

            std::cout << "[*]Listening on" << this->serv.sin_port << "\n";
            listen(this->socket_fd, 8);
			accept(this->socket_fd, (struct sockaddr*)&this->serv, (socklen_t*)&this->addrlen);
			std::cout << "recv\n";
			if (this->encryption_key == 0) {
				throw std::runtime_error("He sheit ner se");
			}
    		return 0;
		}

        virtual void keyExchange(int socket_fd) {
			u128 *P = &primeNum;
			u128 key;
			int PG = 5, private_key = rand();
			u128 public_key = static_cast<u128>(pow(PG, private_key)) % (u128)P;

			send(socket_fd, (const void*)&P, sizeof(u128), 0);
			send(socket_fd, (const void*)&PG, sizeof(u128), 0);
			send(socket_fd, (const void*)&public_key, sizeof(u128), 0);

			recv(socket_fd, (void*)&key, sizeof(u128), 0);

			this->encryption_key = static_cast<u128>(pow(key, private_key)) % (u128)primeNum;
			close(this->socket_fd);
			}
        void send_n(std::string addr, int port, std::string msg) {
        	std::string key = std::to_string(this->encryption_key); 
        	unsigned char *string = RC4(key.c_str(), msg.c_str());
        	send(socket_fd, (const void*)&string, msg.length()+1, 0);
        }
        void recv_n();

	protected:
        int socket_fd;
		struct sockaddr_in serv;
        int addrlen;

        u128 encryption_key;
};

class TCP_client : public TCP_server
{
    public:
    	TCP_client() : TCP_server(0){
    		;;
    	}
    	//självförklarande
    	int accept_n() override {return 0;}

    	//kernel'ln väljer faktist port'in på random
    	int connect_n(std::string addr, int port) {
    		int socket_instance = this->socket_fd, return_value = 0;
    		const char *src = addr.c_str();
    		this->serv.sin_port = htons(port);
			return_value = inet_pton(AF_INET, src, (void*)&this->serv.sin_addr);
			std::cout << "rv: " << return_value << "Hello_world\n";
 			if (return_value = connect(socket_instance, (struct sockaddr*)&serv, sizeof(this->serv)) < 0) {
 				std::cout << return_value << "Hello";
 				return return_value;
 			}

 			keyExchange(socket_instance);
    		return 0;
    		}

    	void keyExchange(int socket_fd) override {
    		std::cout << "client keyExchange\n";
    		u128 P;
			u128 key;
			int PG, private_key = rand();
			u128 public_key = static_cast<u128>(pow(PG, private_key)) % (u128)P;

			recv(socket_fd, (void*)&P, sizeof(u128), 0);
			recv(socket_fd, (void*)&PG, sizeof(u128), 0);
			recv(socket_fd, (void*)&key, sizeof(u128), 0);
			
			send(socket_fd, (const void*)&public_key, sizeof(u128), 0);

			this->encryption_key = static_cast<u128>(pow(key, private_key)) % (u128)primeNum;
    		close(this->socket_fd);
    	}

    	~TCP_client() {
    		std::cout << "[*]Destructor executed\n";
    		close(this->socket_fd);
    	}
    private:
    	char *addr;
};

#endif
