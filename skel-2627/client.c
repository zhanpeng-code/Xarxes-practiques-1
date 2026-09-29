// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEFAULT_PORT 8080
#define DEFAULT_DOMAIN "localhost"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	int sock = 0;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE] = {0};
	char cadena[BUFFER_SIZE] = "";

	char *domini;
	int port;
	char *endptr;

	if (argc == 1) {
		// Cap argument -> valors per defecte
		domini = DEFAULT_DOMAIN;
		port = DEFAULT_PORT;
		printf("Cap argument indicat. Utilitzant valors per defecte: %s %d\n", domini, port);
	} 
	else if (argc == 3) {
		domini = argv[1];

		errno = 0;
		port = strtol(argv[2], &endptr, 10);

		// Comprovacions de validesa
		if (errno != 0 || *endptr != '\0' || port < 1 || port > 65535) {
			fprintf(stderr, "Error: el port ha de ser un enter entre 1 i 65535.\n");
			exit(EXIT_FAILURE);
		}
	} 
	else {
		fprintf(stderr, "Ús: %s [nom_de_domini port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}



	// Crear el socket
	// Afegiu comentari explicant els arguments, i de quines altres opcions hi ha
	// AF_INET: indica que utilitzarem adreces IPv4.
	// SOCK_STREAM: indica que utilitzarem un socket orientat a connexió (TCP).
	// Una altra opció seria SOCK_DGRAM, que correspon a UDP.
	// 0: deixa que el sistema seleccioni automàticament el protocol adequat
	//    per a la combinació AF_INET + SOCK_STREAM.
	// per al segon d'ells (ara SOCK_STREAM)
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\nError en crear el socket\n");
		return -1;
	}

	// Afegiu comentari sobre què són aquests 3 paràmetres,
	// per què s'utilitzen els valors que hi ha,
	// i per què s'utilitza htons()

	// Configurar l'adreça del servidor.
	// sin_family: família d'adreces utilitzada
	// sin_port: port del servidor en format de xarxa (network byte order).
	// htons() converteix el port del format de l'ordinador al format de xarxa.
	// sin_addr: adreça IPv4 del servidor, que es converteix a format binari amb inet_pton().

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	inet_pton(AF_INET, domini, &serv_addr.sin_addr);	// Convertir adreça IPv4 a binari
   

	// Connectar amb el servidor
	// Afegiu comentari explicant els arguments
	// sock: descriptor del socket que volem utilitzar.
	// (struct sockaddr *)&serv_addr: adreça del servidor,
	// convertida al tipus genèric struct sockaddr *.
	// sizeof(serv_addr): mida de l'estructura que conté l'adreça.
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnexio fallida\n");
		return -1;
	}

	printf("Connectat al servidor. Podeu començar a enviar missatges.\n");

	int option;

	while (1) {
		memset(buffer, 0, sizeof(buffer)); // Netegem el buffer

		// Menú principal
		// Cal que implementeu un petit servei remot amb almenys 4 funcionalitats noves
		// Definiu vosaltres mateixos les dades a enviar (demanar en el client, i analitzar al servidor) i rebre
		// Si per donar més sentit al servei cal alguna opció més, la podeu afegir
		// Deixeu l'opció 1 com a Enviar missatge i l'última per Sortir

		printf("Menú principal:\n");
		printf("1. Enviar missatge\n");
		printf("2. Afegir tasca\n");
		printf("3. Eliminar tasca\n");
		printf("4. Veure tasques\n");
		printf("5. Modificar estat de tasca\n");
		printf("6. Sortir\n");
		printf("Opció: ");

		scanf("%d", &option);
		while (getchar() != '\n');  // buida el buffer fins al salt de línia

		switch (option)
		{
		case 1:
		case 6:
			if (option==1){
				printf("Introdueix el missatge a enviar ('EXIT' per tancar el servidor i sortir): ");
				fgets(cadena, BUFFER_SIZE, stdin);
				cadena[strcspn(cadena, "\n")] = '\0';  // Eliminar \n final
			} else {
				strcpy(cadena, "EXIT");
			}

			// Enviar missatge al servidor
			// Afegiu control d'errors
			// Afegiu comentari explicant els arguments
			
			// sock: socket pel qual enviem les dades.
			// cadena: dades que volem enviar.
			// strlen(cadena): nombre de bytes que volem enviar.
			// 0: flags per defecte.


			if (send(sock, cadena, strlen(cadena), 0) < 0) {
    			perror("Error en enviar el missatge");
    			close(sock);
    			return -1;
			}

			// Llegir resposta del servidor
			// Afegiu un control d'errors al recv()
			// Afegiu comentari explicant què fa i per què s'utilitza memset
			//		Netejar el buffer abans de rebre una nova resposta.
			//		Això posa tots els bytes del buffer a 0 i evita conservar dades de la resposta anterior.
			// Afegiu comentari explicant els arguments de la crida a recv()

			memset(buffer, 0, BUFFER_SIZE);

			// Rebre la resposta del servidor.
			// sock: socket pel qual rebem les dades.
			// buffer: memòria on guardarem les dades rebudes.
			// BUFFER_SIZE: nombre màxim de bytes que podem rebre.
			// 0: flags per defecte.

			int bytes_received;

		bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);

		if (bytes_received < 0) {
		    	perror("Error en rebre la resposta");
		   		close(sock);
		    	return -1;
		}

		if (bytes_received == 0) {
		    printf("El servidor ha tancat la connexió.\n");
		    close(sock);
		    return 0;
		}

			buffer[bytes_received] = '\0';
		
			printf("Resposta del servidor: %s\n", buffer);

			if (strcmp(cadena, "EXIT") == 0) {
				close(sock);
				return 0;
			}
			break;

	case 2: {

    printf("Introdueix la tasca: ");

    fgets(cadena, BUFFER_SIZE, stdin);
    cadena[strcspn(cadena, "\n")] = '\0';

    snprintf(buffer, BUFFER_SIZE, "ADD|%s", cadena);

    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        perror("Error en enviar");
        break;
    }

    memset(buffer, 0, BUFFER_SIZE);

    if (recv(sock, buffer, BUFFER_SIZE - 1, 0) <= 0) {
        printf("Error en rebre resposta\n");
        break;
    }

    printf("Resposta del servidor: %s", buffer);

    break;
}

		case 3: {

    int id;

    printf("Introdueix l'ID de la tasca: ");
    scanf("%d", &id);
    while (getchar() != '\n');

    snprintf(buffer, BUFFER_SIZE, "DELETE|%d", id);

    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        perror("Error en enviar");
        break;
    }

    memset(buffer, 0, BUFFER_SIZE);

    if (recv(sock, buffer, BUFFER_SIZE - 1, 0) <= 0) {
        printf("Error en rebre resposta\n");
        break;
    }

    printf("Resposta del servidor: %s", buffer);

    break;
}

		case 4: {

    strcpy(buffer, "LIST");

    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        perror("Error en enviar");
        break;
    }

    memset(buffer, 0, BUFFER_SIZE);

    if (recv(sock, buffer, BUFFER_SIZE - 1, 0) <= 0) {
        printf("Error en rebre resposta\n");
        break;
    }

    printf("\n===== TODO LIST =====\n");
    printf("%s", buffer);
    printf("=====================\n");

    break;
}

		case 5: {

    int id;

    printf("Introdueix l'ID de la tasca: ");
    scanf("%d", &id);
    while (getchar() != '\n');

    snprintf(buffer, BUFFER_SIZE, "DONE|%d", id);

    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        perror("Error en enviar");
        break;
    }

    memset(buffer, 0, BUFFER_SIZE);

    if (recv(sock, buffer, BUFFER_SIZE - 1, 0) <= 0) {
        printf("Error en rebre resposta\n");
        break;
    }

    printf("Resposta del servidor: %s", buffer);

    break;
}

		default:
			printf("Opció invàlida\n");
			break;
		}
	}

	close(sock);
	return 0;
}
