// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 1024
#define MAX_TODOS 100
#define TODO_SIZE 256

typedef struct {
    int id;
    char text[TODO_SIZE];
    int completed;
} Todo;

Todo todos[MAX_TODOS];
int todo_count = 0;


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
			 if (strncmp(buffer, "ADD|", 4) == 0) {

    if (todo_count >= MAX_TODOS) {
        send(new_socket, "Error: massa tasques\n", 21, 0);
    } else {

        todos[todo_count].id = todo_count + 1;

        strcpy(todos[todo_count].text, buffer + 4);

        todos[todo_count].completed = 0;

        char resposta[BUFFER_SIZE];

        snprintf(resposta, BUFFER_SIZE,
                 "Tasca afegida: %d - %s\n",
                 todos[todo_count].id,
                 todos[todo_count].text);

        todo_count++;

        send(new_socket, resposta, strlen(resposta), 0);
    }
}else if (strncmp(buffer, "DELETE|", 7) == 0) {

    int id = atoi(buffer + 7);

    int found = 0;

    for (int i = 0; i < todo_count; i++) {

        if (todos[i].id == id) {

            found = 1;

            // 把后面的任务往前移动
            for (int j = i; j < todo_count - 1; j++) {
                todos[j] = todos[j + 1];
            }

            todo_count--;

            send(new_socket,
                 "Tasca eliminada\n",
                 strlen("Tasca eliminada\n"),
                 0);

            break;
        }
    }

    if (!found) {
        send(new_socket,
             "Error: tasca no trobada\n",
             strlen("Error: tasca no trobada\n"),
             0);
    }
}else if (strcmp(buffer, "LIST") == 0) {

    char resposta[BUFFER_SIZE];

    if (todo_count == 0) {

        strcpy(resposta, "No hi ha tasques.\n");

        send(new_socket,
             resposta,
             strlen(resposta),
             0);

    } else {

        resposta[0] = '\0';

        for (int i = 0; i < todo_count; i++) {

            char temp[TODO_SIZE + 100];

            snprintf(temp, sizeof(temp),
                     "%d. %s [%s]\n",
                     todos[i].id,
                     todos[i].text,
                     todos[i].completed ? "Fet" : "Pendent");

            strcat(resposta, temp);
        }

        send(new_socket,
             resposta,
             strlen(resposta),
             0);
    }
}else if (strncmp(buffer, "DONE|", 5) == 0) {

    int id = atoi(buffer + 5);

    int found = 0;

    for (int i = 0; i < todo_count; i++) {

        if (todos[i].id == id) {

            todos[i].completed = 1;
            found = 1;

            send(new_socket,
                 "Tasca marcada com a feta\n",
                 strlen("Tasca marcada com a feta\n"),
                 0);

            break;
        }
    }

    if (!found) {
        send(new_socket,
             "Error: tasca no trobada\n",
             strlen("Error: tasca no trobada\n"),
             0);
    }
}

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
