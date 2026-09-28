// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	int sock, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};

	int port;
	char *endptr;

	if (argc == 1) {
		// Cap argument -> port per defecte
		port = DEFAULT_PORT;
		printf("Cap port indicat. Utilitzant valor per defecte: %d\n", port);
	}
	else if (argc == 2) {
		errno = 0;
		port = strtol(argv[1], &endptr, 10);

		// Comprovacions
		if (errno != 0 || *endptr != '\0' || port < 1 || port > 65535) {
			fprintf(stderr, "Error: el port ha de ser un enter entre 1 i 65535.\n");
			exit(EXIT_FAILURE);
		}
	}
	else {
		fprintf(stderr, "Ús: %s [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	// Crear el socket
	// Afegiu comentari explicant els arguments

	// AF_INET: família d'adreces IPv4.
	// SOCK_STREAM: socket orientat a connexió, utilitzat per TCP.
	// Una altra opció és SOCK_DGRAM, utilitzat per UDP.
	// 0: indica que volem utilitzar el protocol per defecte associat a AF_INET i SOCK_STREAM.
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Error en crear el socket");
		exit(EXIT_FAILURE);
	}


	// Afegiu comentari explicant què són aquests 3 paràmetres,
	// per què s'utilitzen htons() i htonl(),
	// i per què s'utilitzen els valors que hi ha

	// sin_family:
	// Indica la família d'adreces. AF_INET correspon a IPv4.
	//
	// sin_addr.s_addr:
	// Indica l'adreça IP local on el servidor acceptarà connexions.
	// INADDR_ANY permet acceptar connexions per qualsevol interfície
	// de xarxa disponible.	
	// htonl() converteix el valor de 32 bits al network byte order.
	//
	// htonl() converteix el valor de 32 bits al network byte order.
	//
	// sin_port:
	// Indica el port TCP on escoltarà el servidor.
	// htons() converteix el número de port al network byte order.

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);	// Per a INADDR_ANY no faria falta htonl(), però li posem per coherència
	address.sin_port = htons(port);

	// Enllaçar el socket al port especificat
	// Afegiu comentari explicant els arguments
	// Enllaçar el socket amb l'adreça i el port especificats.
	//
	// sock:
	// Descriptor del socket que volem associar.
	//
	// (struct sockaddr *)&address:
	// Adreça local del servidor, convertida al tipus genèric
	// struct sockaddr *.
	//
	// sizeof(address):
	// Mida de l'estructura que conté l'adreça.

	if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("Error en fer el bind");
		close(sock);
		exit(EXIT_FAILURE);
	}

	// Escoltar connexions entrants
	// Afegiu comentari explicant els arguments
	// Posar el socket en mode d'escolta.
	//
	// sock:
	// Socket que utilitzarem per escoltar noves connexions.
	//
	// 3:
	// Nombre màxim de connexions pendents que poden esperar
	// a la cua abans de ser acceptades.
	
	if (listen(sock, 3) < 0) {
		perror("Error en escoltar");
		close(sock);
		exit(EXIT_FAILURE);
	}

	printf("Servidor en funcionament, esperant connexions...\n");

	while (1) {
		// Acceptar connexions de clients
		// Afegiu comentari explicant els arguments i per què hi ha i cal new_socket si ja tenim sock
		// Acceptar una connexió entrant.
	//
	// sock:
	// Socket d'escolta que utilitzem per acceptar noves connexions.
	//
	// (struct sockaddr *)&address:
	// Estructura on accept() guardarà l'adreça del client que es connecta.
	//
	// (socklen_t *)&addrlen:
	// Indica la mida de l'estructura address.
	//
	// accept() retorna un nou descriptor de socket, que guardem a
	// new_socket. Aquest nou socket s'utilitza per comunicar-nos amb
	// el client.
	//
	// sock continua sent el socket d'escolta i es manté disponible
	// per acceptar noves connexions.
		
		if ((new_socket = accept(sock, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
			perror("Error en acceptar la connexió");
			close(sock);
			exit(EXIT_FAILURE);
		}

		 while (1) {
			memset(buffer, 0, BUFFER_SIZE);
			
			// Llegir missatge del client
			if (recv(new_socket, buffer, BUFFER_SIZE, 0) <= 0) {
				printf("Client desconnectat (PID: %d)\n", getpid());
				break;
			}

			// Aquí haureu d'implementar l'anàlisi de la cadena rebuda per saber l'operació,
			// i si n'hi ha els arguments, executar-la i tornar el(s) resultat(s)

			// A continuació hi ha el codi corresponent a l'opció d'Enviar missatge, amb el retorn d'una cadena fixa,
			// i la de Sortida de la connexió/bucle si és el cas.
			// Modifiqueu la cadena de retorn per tal que sigui la que diu l'enunciat.

			printf("Missatge rebut del client (PID: %d): %s\n", getpid(), buffer);

			// Comprovar si el client vol tancar la connexió
			if (strcmp(buffer, "EXIT") == 0) {
				printf("Tancant connexió amb el client (PID: %d)...\n", getpid());
				send(new_socket, "Connexió tancada\n", strlen("Connexió tancada\n"), 0);
				break;	// Sortir del bucle
			} else {
				send(new_socket, "Missatge rebut\n", strlen("Missatge rebut\n"), 0);
			}
		}

		close(new_socket); // Tancar la connexió amb el client
	}

	close(sock);
	return 0;
}
