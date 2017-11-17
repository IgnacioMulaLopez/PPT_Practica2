/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: cliente.c
Versión: 1.0
Fecha: 11/2017
Descripción:
		Cliente SMTP sencillo.

En esta práctica desarrollaremos a partir del codigo del cliente TCP, utilizado en la práctica anterior, un cliente de SMTP sencillo.
Para ello cambiaremos la máquina de estados, lo cual incluye:
 - Los distintos estados que forman parte de ella.
 - Los comandos de aplicación utilizados por el protocolo.
 - Las respuesta a dichos comandos, por parte del servidor.

El objetivo, es conseguir desarrollar un cliente de correo electronico, con el quye poder enviar correos de longitud ilimitada entre usuarios,
y que además responda correctamente a las respuestas del servidor, y que soporte el comando RSET.
Vamos a desarrollar este cliente de manera que oculte al usuario los comandos del protocolo, de manera que solo se le solicite la información
necesaria para rellenar el correo electrónico.

Autores: Ignacio Mula Lopez
	     Maria Josefa Fernández Guillén.

*******************************************************/
#include <stdio.h>
#include <winsock.h>
#include <time.h>
#include <conio.h>
#include "protocol.h"

int main(int *argc, char *argv[])
{   // Para iniciar un bloque
	//La declaración de variables, se hará al inicio de cada bloque.
	SOCKET sockfd;
	struct sockaddr_in server_in;
	char buffer_in[1024], buffer_out[1024], input[1024], response[4], opcion[2], asunto[50], remitente[20], destinat[20], mensaje[1024];
	int recibidos = 0, enviados = 0, control =1;
	char intentos[2];
	int fallo_len = 0, fin = 0;
	int estado = S_WLCM;
	char option;

	// Vamos a crear la estructura que almacenará la fecha y la hora, para la cabecera correspondiente.
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char timestamp[128];

	struct in_addr sin_addr;

	typedef struct addrinfo	{
		int	ai_flags;
		int	ai_family;
		int	ai_socktype;
		int	ai_protocol;
		size_t	ai_addrlen;
		char *ai_canonname;
		struct	sockaddr *ai_addr;
		struct	addrinfo *ai_next;
	} ADDRINFOA, *PADDRINFOA;

	struct in_addr address;


	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	char ipdest[16];
	char default_ip[16]="127.0.0.1";

	//Inicialización Windows sockets -                 ¡¡SOLO WINDOWS!!
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1){
		WSACleanup();
		return(0);
	}

	//Fin: Inicialización Windows sockets

																										// Aquí empieza nuestra aplicación
	printf("***********************\r\nCLIENTE SMTP SENCILLO\r\n***********************\r\n");
	do{																									//Va a hacer lo que hay dentro antes de comprobar, a diferencia del while.
		system("cls");
		
		sockfd=socket(AF_INET,SOCK_STREAM,0);															//SOCKET es la primitiva socket() que crea la memoria.

		if(sockfd==INVALID_SOCKET){																		//Debe ir siempre despues de todas las funciones de comunicación, es la comprobacion de errores.			
			printf("CLIENTE> ERROR\r\n");																//En caso de que haya un error, se mostrará el mensaje "cliente Error"
			exit(-1);																					// El programa termina con el codigo menos uno.
		}
		else{
			printf("CLIENTE> SOCKET CREADO CORRECTAMENTE\r\n");											//En el caso de que todo haya salido bien, informamos al usuario por pantalla.

			do{
			printf("Introduzca la direccion IP o el dominio destino: ");
			gets(ipdest);
			server_in.sin_addr.s_addr = inet_addr(ipdest);
			if (server_in.sin_addr.s_addr == INADDR_NONE) {
				struct hostent *host;

				host = gethostbyname(ipdest);
				if (host != NULL) { //Si la dirección introducida por teclado no es correcta o no corresponde con un dominio.
					memcpy(&address, host->h_addr_list[0], 4);
					printf("\nDireccion%s\n", inet_ntoa(address));
					strcpy(ipdest, inet_ntoa(address));
					control = 1;
				}
				else {
					printf("No se ha introducido ninguna direccion correcta, o no correponde con un dominio existente.\r\n");
					printf("Por favor, introduzca una direccion, o nombre valido.\r\n");
					control = 0;
				}
			}
			} while (control !=1);


			/*printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");			//Solicitamos al cliente que introduzca la dirección IP del servidor al que se quiere conectar.
			gets(ipdest);																				//Dicha dirección IP, se alamacenará en la variable "ipdest".

			if(strcmp(ipdest,"")==0)																	//Caso por defecto.
				strcpy(ipdest,default_ip);																//En el caso de introducir un dirección ip nueva, se copiará en la variable "default_ip".
				*/

			server_in.sin_family=AF_INET;																//Familia IP
			server_in.sin_port=htons(TCP_SERVICE_PORT);													//Puerto Que vamos a usar
			server_in.sin_addr.s_addr=inet_addr(ipdest);												//IP del servidor
			
			enviados=0;																					//Inicializamos la variable "enviados" a cero.
			estado=S_WLCM;																				//Estado inicial de saludo
		

			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0){						//SOCKET, es la primitiva connect(), que inicia la conexión con un servidor remoto.
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
				printf("**************************************************\r\n");
				printf("\r\n");
			
		
																										//Inicio de la primera máquina de estados.
				do{																						//Realizaremos el contenido del do, antes de comprobar la condición del mismo.
					switch(estado)																		//Iremos avanzando en la máquina, a través de la variable "estado".
					{

					case S_WLCM:																		//Caso de Bienvenida.
																										//Se da la bienvenida al servidor.
						break;																			//Fin del caso de Bienvenida.
						
					case S_HELO:																		//Caso de Saludo inicial.
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", HELO, CRLF);				//Preparamos un mensaje con el comando "HELO", que enviaremos al servidor.
						break;																			//Fin del caso Saludo.

					case S_MAIL:																		//Caso "MAIL FROM".
						printf("\r\n");																	//Introducimos un CRLF, para estilizar la información que aparece por pantalla.
						printf("CLIENTE> Introduzca el remitente: ");									//Solicitamos al cliente, que introduzca el remitente del correo electrónico.
						gets(input);																	//Dicho remitente se almacena en la variable "input".
						if (strcmp(input, QUIT) == 0 || (strcmp(input, "quit") == 0)) {					//En el caso de que se introduzca el comando "QUIT".
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", QUIT, CRLF);				//Preparamos un mensaje con el comando "QUIT", que se enviará al servidor.
							estado = S_QUIT;															//Cambiamos el estado a "S_QUIT".
						}
						else if (strcmp(input, RSET) == 0 || (strcmp(input, "rset") == 0)) {			//En el caso de que se introduzca el comando "RSET".
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", RSET, CRLF);				//Preparamos un mensaje con el comando "RSET", que se enviará al servidor.
							estado = S_RSET;															//Cambiamos el estado a "S_REST".
						}
						else																			//En el caso de que se introduzca un remitente.
							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", MAIL, input, CRLF);	//Preparamos un mensaje con el comando "MAIL", que se enviará al servidor, junto con el remitente.
						strcpy(remitente, input);														//Copiamos en la variable "remitente" el contenido de "input". Esto se utilizará más adelante en las cabeceras del correo.
						break;																			//Fin del caso "MAIL FROM".

					case S_RCPT:																		//Caso "RCPT TO".
						printf("CLIENTE> Introduzca el destinatario: ");								//Solicitamos al usuario que introduzca un destinatario.
						gets(input);																	//Dicha información se almacenará en la variable "input".
						if (strcmp(input, QUIT) == 0 || (strcmp(input, "quit") == 0)) {					//En el caso de que se introduzca el comando "QUIT".
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", QUIT, CRLF);				//Preparamos un mensaje con el comando "QUIT", que se enviará al servidor.
							estado = S_QUIT;															//Cambiamos el estado a "S_QUIT".
						}
						else if (strcmp(input, RSET) == 0 || (strcmp(input, "rset") == 0)) {			//En el caso de que se introduzca el comando "RSET".
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", RSET, CRLF);				//Preparamos un mensaje con el comando "RSET", que se enviará al servidor.
							estado = S_RSET;															//Cambiamos el estado a "S_REST".
						}
						else																			//En el caso de que se introduzca un destinatario. 
							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", RCPT, input, CRLF);	//Preparamos un mensaje con el comando "RCPT", que se enviará al servidor, junto con el destinatario.
						strcpy(destinat, input);														//Copiamos en la variable "destinat" el contenido de "input". Esto se utilizará más adelante en las cabeceras del correo.
						break;																			//Fin del caso "RCPT TO".

					case S_DATA: //Caso "DATA".
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", DATA, CRLF);				//Preparamos un mensaje con el comando "DATA", que se enviará al servidor.
						break; //Fin del caso "DATA".

					case S_MSEG:																		//Caso "MENSAJE".
																										//Primero rellenamos la cabeceras del correo electrónico.
						strftime(timestamp, 128, "%d/%m/%y %H:%M:%S", tlocal);							//Se alamacenará en la variable "timestamp" la fecha y la hora, con el formato establecido.
						printf("Asunto: ");																//Solicitamos al usuario el asunto del correo.
						gets(asunto);																	//Almacenamos dicho asunto en la variable "asunto".
						sprintf_s(mensaje, sizeof(mensaje), "Date: %s%sSubject: %s%sTo: %s%sFrom:%s%s", timestamp, CRLF, asunto, CRLF, destinat, CRLF, remitente, CRLF); //Preparamos un mensaje con las cabeceras, que se enviará al servidor.
						printf("\nDate: %s%sSubject: %s%sTo: %s%sFrom:%s%s", timestamp, CRLF, asunto, CRLF, destinat, CRLF, remitente, CRLF); //Mostramos las cabeceras por pantalla.
																										//Para rellenar el cuerpo del mensaje, vamos a realizar lo siguiente.
						do {
							gets(input);																//Almacenamos en la variable "input", la informacion que vaya proporcionando el usuario, hasta que presione "enter".
							sprintf_s(mensaje, sizeof(mensaje), "%s%s%s", mensaje, CRLF, input);		//La información introducida por el usuario se irá concatenando en la variable "mensaje".
						} while (strcmp(input, ".", 1) != 0); {											//Iremos almacenando información, hasta que el usuario solo introduzca un punto '.'.
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", mensaje, CRLF);			//En cuyo caso,preparamos el mensaje que vamos a enviar al servidor, con el contenido del mensaje.
						}
						break;																			//Fin del caso "MENSAJE".

					case S_RSET:																		//Caso "RESET".
						sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", RSET, CRLF);					//Preparamos un mensaje con el comando "RSET", que se enviará al servidor.
						break;																			//Fin del caso "RESET".

					case S_QUIT:																		//Caso "QUIT".
						sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", QUIT, CRLF);					//Preparamos un mensaje con el comando "QUIT", que se enviará al servidor.
						break;																			//Fin del caso "QUIT".
					}

																										//Aquí realizamos el envio de mensajes.
					if(estado!=S_WLCM){																	//Para enviar mensajes comprobamos que no nos encontramos en el caso de bienvenida.
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);						//SOCKET, es la primitiva send(), que envia el mensaje.
					}
					if (enviados<0){																	//En el caso de que haya ocurrido algún error, y la variable recibidos sea menor que cero.
						DWORD error=GetLastError();														//Obtenemos cual ha sido el error que se ha producido.
							printf("CLIENTE> Error %d en el envio de datos%s",error,CRLF);				//Informamos al usuario de que se ha producido tal error.
							break; //?
					}

																										//Aquí implementamos la parte de recibo de mensajes del servidor.
					recibidos=recv(sockfd,buffer_in,512,0);												//SOCKET, es la primitiva recv(), que recibe el mensaje.

					if(recibidos<=0){																	//Comprobamos la variable "recibidos" por si  se ha producido algún error.
						DWORD error=GetLastError();														//En el caso de que así sea, almacenamos dicho error.
						if(recibidos<0 && strncmp(input,QUIT,4)!=0 && strncmp(input,QUIT,4)!=0) {		//En el caso de que haya ocurrido un error, y no nos encontremos en el caso "S_QUIT".
							printf("CLIENTE> Error %d en la recepción de datos\r\n",error);				//Informamos al usuario de que se ha producido tal error.
							estado=S_QUIT;																//Cambiamos el estado a "S_QUIT".
						}
						else {
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;					
						}
					}
					else {																				//En el caso de que no se haya producido ningún error.
						buffer_in[recibidos]=0x00;														//Formateamos el contenido del buffer de entrada.
						sscanf_s(buffer_in, "%s %[^\r]s %s\r\n", response, sizeof(response), input, sizeof(input)); //Extraemos del mensaje recibido, la respuesta del servidor, así como el mensaje.
						printf("%s\r\n", input);														//Imprimimos por pantalla el mensaje recibido.
						
																										//Segundo máquina de estados.
						switch (estado) {																//Avanzaremos por la máquina a través de la variable "estado".
						case S_WLCM:																	//En el caso de bienvenida.
							estado++;																	//Pasamos al caso siguiente.
							break;																		//Fin del estado Bienvenida.

						case S_HELO:																	//En el caso de saludo.
						case S_MAIL:																	//Y en el caso de "MAIL".
							if (strncmp(response, "2", 1) == 0)											//Si recibimos una respuesta positiva del servidor (2xx).
								estado++; //Avanzamos al siguiente estado.
							if (strncmp(response, "5", 1) == 0)
								estado = S_QUIT;
							break;																		//Fin de los casos saludo y mail.

						case S_RCPT:																	//En el caso RCPT
							if (strncmp(response, "2", 1) == 0) {										//Si recibimos una respuesta positiva del servidor (2xx).
								printf("Desea introducir mas destinatrios? (S/N): ");					//Le preguntamos al usuario si quiere añadir más destinatarios.
								gets(opcion);															//Almacenamos la opcion elegida en la variable "opcion".
								if ((strncmp(opcion, "n", 1) == 0) || (strncmp(opcion, "N", 1) == 0))	//En el caso de que no se quieran añadir más destinatarios.
									estado++;															//Pasamos al estado siguiente.
								else																	//En caso de querer añadir más.
									estado = 3;															//Permanecemos en el estado RCPT.
							}
							else if (strncmp(response, "5", 1) == 0)									//En el caso de recibir una respuesta negativa del servidor (5xx).
								estado = 3;																//Permancemos en el estado RCPT.
						break;																			//Fin del caso RCPT.
						
						case S_DATA:																	//En el caso DATA
							if (strncmp(response, "3", 1) == 0) {										//Si recibimos del servidor una respuesta positiva (3xx).
								estado++;																//Avanzamos al estado siguiente.
							}
							break;																		//Fin del caso DATA.

						case S_MSEG:																	//En el caso del mensaje.
							if (strncmp(response, "2", 1) == 0) {										//Si recibimos del servidor una respuesta positiva (2xx).
								printf("Desea enviar otro correo? (S/N): ");							//Preguntamos al usuario si quiere enviar más correos.
								gets(opcion);															//Almacenamos la opcion elegida, en la variable "opcion".
								if ((strncmp(opcion, "n", 1) == 0) || (strncmp(opcion, "N", 1) == 0))	//En el caso de que no se quieran enviar más correos.
									estado = S_QUIT;													//Cambiamos al estado "S_QUIT".
								else																	//En el caso de querer enviar más correos.
									estado = S_RSET;													//Cambiamos al estado "S_RSET".
							}
							break;																		//Fin del caso mensaje.

						case S_RSET:																	//En el caso RSET.
							estado = S_MAIL;															//Pasamos al estado MAIL.
							break;																		//Fin del estado RSET.

						case S_QUIT:
							break;
						}
					}
				}while(estado!=S_QUIT);																	//Realizaremos el do principal, hasta que nos encontremos en el caso S_QUIT.		
			}
			else{																						//Error al Conectar
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
				exit(-1);																				//El programa termina con el codigo menos uno.
			}		
																										//Fin de la conexion de transporte
			closesocket(sockfd);																		//SOCKET la primitiva close() que cierra la memoria.
		}
		estado =S_WLCM;
		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
		option=_getche();																				//Realizar otra conexión.
	}while(option!='n' && option!='N');
	return(0);
}																										//Fin del main.