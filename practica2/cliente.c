/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: cliente.c
Versión: 1.0
Fecha: 09/2017
Descripción:
		Cliente sencillo TCP.

Autor: Juan Carlos Cuevas Martínez


Comandos :

- SC "USER" ; Se utiliza para la solicitud de conexión del usuario.
- PW "PASS" ; Se utiliza para solicitar la contraseña, para establecer la conexión.
- SD  "QUIT" ; Se utiliza para terminar la conexión.
- SD2 "EXIT" ; Se utiliza para terminar la conexión.
- ECHO "ECHO" ; Esta es la definicion del comando "ECHO" para el servicio de eco.
- SUM "SUM" ; Esta es la definicion del comnado SUM, para hacer la suma.

*******************************************************/
#include <stdio.h>
#include <winsock.h>
#include <time.h>
#include <conio.h>
#include "protocol.h"


// La diferencia entre <> y comillas es donde busque las librerias, Comillas es para la ruta del proyecto 
// mientras que <> es para otra cosa.


int main(int *argc, char *argv[])
{   // Para iniciar un bloque
	//La declaración de variables, se hará al inicio de cada bloque.
	SOCKET sockfd;
	struct sockaddr_in server_in;
	char buffer_in[1024], buffer_out[1024],input[1024],response[4],opcion[1],asunto[50],remitente[20],destinat[20],mensaje[1024];
	int recibidos=0,enviados=0;
	char intentos[2];
	int fallo_len=0, fin=0;
	int estado=S_WLCM;
	char option;

	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char timestamp[128];

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

    char ipdest[16];
	char default_ip[16]="127.0.0.1";

	/*if (argc != NULL) {
	if (*argc >= 2)
	printf("Parametro %s:", *argv[1]);
	}

	strcpy_s(default_ip, 63, argv[1]); // Aquí vamos a copiar en default ip, la información de argv
	/* Habría qu ecomprobar que es una direccion correcta, ya que no tenemos servicio de DNS,
	pero en este caso no lo vamos a hacer. Cabe mencionar que como estamos modificando la ip por defecto,
	deberemos de poner la indicada anteriormente 127.0.0.1
	*/

	/* si ponemos un punto de ruptura, cuando la ejecución del porgrama llegue a dicho punto parará. para que
	observemos paso a paso, y más opciones lo que pasa a partir de dicho punto.
	*/

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
	printf("***********************\r\nCLIENTE TCP SENCILLO\r\n***********************\r\n");
	do{ // Va a hacer lo que hay dentro antes de comprobar, a diferencia del while.
		system("cls");
		
		sockfd=socket(AF_INET,SOCK_STREAM,0); //SOCKET es la primitiva socket() que crea la memoria.

		if(sockfd==INVALID_SOCKET){ //Debe ir siempre despues de todas las funciones de comunicación,
									//es la comprobacion de errores.			
			printf("CLIENTE> ERROR\r\n");//En caso de que haya un error, se mostrará el mensaje "cliente Error"
			exit(-1);// El programa termina con el codigo menos uno.
		}
		else{
			printf("CLIENTE> SOCKET CREADO CORRECTAMENTE\r\n");

			printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			gets(ipdest);

			if(strcmp(ipdest,"")==0)	//Caso por defecto
				strcpy(ipdest,default_ip);


			server_in.sin_family=AF_INET;					//Familia IP
			server_in.sin_port=htons(TCP_SERVICE_PORT);		//Puerto Que vamos a usar
			server_in.sin_addr.s_addr=inet_addr(ipdest);	//IP del servidor
			
			enviados=0;
			estado=S_WLCM;	//Estado inicial de saludo
		

			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0){//SOCKET, es la primitiva connect(), que inicia la conexxión con u nservidor remoto.
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			
		
				//Inicio de la máquina de estados
				do{
					switch(estado)
					{

					case S_WLCM:
						//Se da la bienvenida al servidor.
						break;
						
					case S_HELO:
						// establece la conexion de aplicacion 
						printf("CLIENTE> Introduzca su host: ");
						gets(input);
						if(strcmp(input, QUIT)==0){	//Enter para salir
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s%s",QUIT,CRLF); //Formato para Cerrar la Conexión
							estado=S_QUIT;
						}
						else if((strcmp(input, HELO) == 0) || (strcmp(input, "helo") == 0) ) {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", HELO, input, CRLF);
						}
						break;

					case S_MAIL:
						printf("CLIENTE> Introduzca el remitente: ");
						gets(input);
						if (strcmp(input, QUIT) == 0 || (strcmp(input, "quit") == 0)) {	//Enter para salir
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s", QUIT, CRLF); //Formato para Cerrar la Conexión
							estado = S_QUIT;
						}
						else if (strcmp(input, RSET) == 0 || (strcmp(input, "rset") == 0)) {	//Enter para salir
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s", RSET, CRLF); //Formato para Cerrar la Conexión
							estado = S_RSET;
						}
						else
							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", MAIL, input, CRLF);
						strcpy(remitente, input);
						break;

					case S_RCPT:
						printf("CLIENTE> Introduzca el destinatario: ");
						gets(input);
						if (strcmp(input, QUIT) == 0 || (strcmp(input, "quit") == 0)) {	//Enter para salir
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s", QUIT, CRLF); //Formato para Cerrar la Conexión
							estado = S_QUIT;
						}
						else if (strcmp(input, RSET) == 0 || (strcmp(input, "rset") == 0)) {	//Enter para salir
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s", RSET, CRLF); //Formato para Cerrar la Conexión
							estado = S_RSET;
						}
						else
							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", RCPT, input, CRLF);
						strcpy(destinat, input);
						break;
					case S_DATA:
						printf("CLIENTE> Introduzca el mensaje (ha de terminar con un '.'): ");
						gets(input);
						if (strcmp(input, QUIT) == 0 || (strcmp(input, "quit") == 0)) {	//Enter para salir
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s", QUIT, CRLF); //Formato para Cerrar la Conexión
							estado = S_QUIT;
						}
						else if (strcmp(input, RSET) == 0 || (strcmp(input, "rset") == 0)) {	//Enter para salir
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s", RSET, CRLF); //Formato para Cerrar la Conexión
							estado = S_RSET;
						}
						else
							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", DATA, input, CRLF);
						break;

					case S_MSEG:

						strftime(timestamp, 128, "%d/%m/%y %H:%M:%S", tlocal); //timestamp contiene la hora.
						printf("\nAsunto: ");
						gets(asunto);
						sprintf_s(mensaje, sizeof(mensaje), "Date: %s%sSubject: %s%sTo: %s%sFrom:%s%s", tlocal, CRLF, asunto, CRLF, destinat, CRLF, remitente, CRLF);
						printf("Date: %s%sSubject: %s%sTo: %s%sFrom:%s%s", tlocal, CRLF, asunto, CRLF, destinat, CRLF, remitente, CRLF);

						do {
							gets(input);
							sprintf_s(mensaje, sizeof(mensaje), "%s%s%s", mensaje, CRLF, input);
						} while (strcmp(input, ".", 1) != 0); {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", mensaje, CRLF);
						}
						/*
						gets(input);
						if (strcmp(input, ".") == 0) {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s", input);
							fin = 1;
						}
						else {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s", input);
							fin = 0;
						}*/


						break;

					}

					//Envio
					if(estado!=S_WLCM){ //Comprobar el estado de envio
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);//SOCKET, es la primitiva send(), que envia el mensaje.
					}
					if (enviados<0){
						DWORD error=GetLastError();
						
							printf("CLIENTE> Error %d en el envio de datos%s",error,CRLF);
							break;
					}


					//Recibo
					recibidos=recv(sockfd,buffer_in,512,0);

					if(recibidos<=0){
						DWORD error=GetLastError();
						if(recibidos<0 && strncmp(input,QUIT,4)!=0 && strncmp(input,QUIT,4)!=0)
						{
							printf("CLIENTE> Error %d en la recepción de datos\r\n",error);
							estado=S_QUIT;
						}
						else
						{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;					
						}
					}

					else
					{
						buffer_in[recibidos]=0x00;
						sscanf_s(buffer_in, "%s %[^\r]s %s\r\n", response, sizeof(response), input, sizeof(input));
						printf("%s\r\n", input);
						
						//Avance de la maquina de estados
						switch (estado) {
						case S_WLCM:
							estado++;
							break;
						case S_HELO:
						case S_MAIL:
							if (strncmp(response, "2", 1) == 0)
								estado++;
							break;

						case S_RCPT:
							printf("Desea introducir mas destinatrios? (S/N)\r\n");
							gets(opcion);
							if((strncmp(opcion, "n", 1)== 0) || (strncmp(opcion, "N", 1)==0)){
								estado++;
							}
							else
								estado = 3;
						break;
						case S_DATA:
							if (strncmp(response, "3", 1) == 0) {
								estado++;
							}
							break;
						case S_MSEG:
							if (fin == 1) {
								estado++;
							}
							else
								estado = 5;
							break;
						case S_RSET:
							estado = 2;
							break;
						case S_QUIT:
							break;

						}
						/*if(estado!=S_DATA && strncmp(response,"2",1)==0){
							estado++;
						}
						else if(estado == S_DATA && strcmp(response,"2",1)==0) {
							estado++;
						}*/
					}
				}while(estado!=S_QUIT);			
			}
			else{	//Error al Conectar
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
				exit(-1); // El programa termina con el codigo menos uno.
			}		
			// fin de la conexion de transporte
			closesocket(sockfd); //SOCKET la primitiva close() que cierra la memoria.
			
		}
		estado =S_WLCM;
		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
		option=_getche();	//realizar otra conexión

	}while(option!='n' && option!='N');

	
	
	return(0);

}
