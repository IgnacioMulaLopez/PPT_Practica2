/*******************************************************
Protocolos de Transporte
Grado en Ingenier�a Telem�tica
Dpto. Ingen�er�a de Telecomunicaci�n
Univerisdad de Ja�n

Fichero: cliente.c
Versi�n: 1.0
Fecha: 09/2017
Descripci�n:
		Cliente sencillo TCP.

Autor: Juan Carlos Cuevas Mart�nez


Comandos :

- SC "USER" ; Se utiliza para la solicitud de conexi�n del usuario.
- PW "PASS" ; Se utiliza para solicitar la contrase�a, para establecer la conexi�n.
- SD  "QUIT" ; Se utiliza para terminar la conexi�n.
- SD2 "EXIT" ; Se utiliza para terminar la conexi�n.
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
	//La declaraci�n de variables, se har� al inicio de cada bloque.
	SOCKET sockfd;
	struct sockaddr_in server_in;
	char buffer_in[1024], buffer_out[1024],input[1024],NUM1[5],NUM2[5],SOL[6];
	int recibidos=0,enviados=0;
	char intentos[2];
	int fallo_len=0;
	int estado=S_WLCM;
	char option;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

    char ipdest[16];
	char default_ip[16]="127.0.0.1";

	/*if (argc != NULL) {
	if (*argc >= 2)
	printf("Parametro %s:", *argv[1]);
	}

	strcpy_s(default_ip, 63, argv[1]); // Aqu� vamos a copiar en default ip, la informaci�n de argv
	/* Habr�a qu ecomprobar que es una direccion correcta, ya que no tenemos servicio de DNS,
	pero en este caso no lo vamos a hacer. Cabe mencionar que como estamos modificando la ip por defecto,
	deberemos de poner la indicada anteriormente 127.0.0.1
	*/

	/* si ponemos un punto de ruptura, cuando la ejecuci�n del porgrama llegue a dicho punto parar�. para que
	observemos paso a paso, y m�s opciones lo que pasa a partir de dicho punto.
	*/

	//Inicializaci�n Windows sockets -                 ��SOLO WINDOWS!!
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1){
		WSACleanup();
		return(0);
	}

	//Fin: Inicializaci�n Windows sockets

	// Aqu� empieza nuestra aplicaci�n
	printf("***********************\r\nCLIENTE TCP SENCILLO\r\n***********************\r\n");
	do{ // Va a hacer lo que hay dentro antes de comprobar, a diferencia del while.
		system("cls");
		
		sockfd=socket(AF_INET,SOCK_STREAM,0); //SOCKET es la primitiva socket() que crea la memoria.

		if(sockfd==INVALID_SOCKET){ //Debe ir siempre despues de todas las funciones de comunicaci�n,
									//es la comprobacion de errores.			
			printf("CLIENTE> ERROR\r\n");//En caso de que haya un error, se mostrar� el mensaje "cliente Error"
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
		

			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0){//SOCKET, es la primitiva connect(), que inicia la conexxi�n con u nservidor remoto.
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			
		
				//Inicio de la m�quina de estados
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
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s%s",QUIT,CRLF); //Formato para Cerrar la Conexi�n
							estado=S_QUIT;
						}
						else if(strcmp(input, HELO) == 0) {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", HELO, input, CRLF);
						}
						break;

					/*case S_PASS:
						//Contrase�a
						printf("CLIENTE> Introduzca la clave (enter para salir): ");
						gets(input);
						if(strlen(input)==0)	//Enter para salir
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF); //Formato para Cerrar la Conexion
							estado=S_QUIT;
						}
						else
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",PW,input,CRLF);	//Formato de envio de contrase�a
						break;
						
					case S_DATA:
						//Funcionalidad del servidor
						printf("CLIENTE> Introduzca datos (enter o QUIT para salir): ");
						gets(input);
						if(strlen(input)==0){	//Enter para salir
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);		//Formato para Cerrar la Cnexion
							estado=S_QUIT;
						}
						else{
							if (strncmp(input,SUM,3)==0){//Comando Sum

								//Numero 1
								do{
									fallo_len=0;
									printf("CLIENTE> Introduce el primer numero: ");
									gets(NUM1);
									if(strlen(NUM1)>4){		//Comprobaci�n del tama�o
										printf("CLIENTE> %s MAXIMO 4 DIGITOS%s",ER,CRLF);
										fallo_len=1;
									}
								}while(fallo_len==1);

								//Numero 2
								do{
									fallo_len=0;
									printf("CLIENTE> Introduce el segundo numero: ");
									gets(NUM2);
									if(strlen(NUM2)>4){		//Comprobacion del tama�o
										printf("CLIENTE> %s MAXIMO 4 DIGITOS%s",ER,CRLF);
										fallo_len=1;
									}
								}while(fallo_len==1);

								//Formato de envio
								sprintf_s(buffer_out,sizeof(buffer_out),"SUM %s %s%s",NUM1,NUM2,CRLF);
							}
							
							//Comando EXIT
							else if(strncmp(input,SD2,4)==0){	//Correcion del Fallo del comando EXIT
								sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",input,CRLF);	//Formato de envio del EXIT
								estado = S_QUIT;
							}

							//Comando QUIT
							else if(strncmp(input,SD,4)==0){  //Correcion del Fallo del comando QUIT
								sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",input,CRLF);	//Formato de envio de QUIT
								estado = S_QUIT;
							}

							//Resto de comandos
							else sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",input,CRLF); //Formato del resto de comandos
						}
						break;
				*/
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
							printf("CLIENTE> Error %d en la recepci�n de datos\r\n",error);
							estado=S_QUIT;
						}
						else
						{
							printf("CLIENTE> Conexi�n con el servidor cerrada\r\n");
							estado=S_QUIT;
						
					
						}
					}

					else
					{
						buffer_in[recibidos]=0x00;
						
						//Presentacion de la Solucion de la suma
						/*if (strncmp(input,SUM,3)==0 && strncmp(buffer_in,"OK",2)==0){
							sscanf_s(buffer_in,"OK %s\r\n",SOL,sizeof(SOL));
							printf("CLIENTE> Resultado = %s%s",SOL,CRLF);
						}
						//Presentacion del resto de datos recibidos
						else printf(buffer_in);

						//Soluci�n a la autentificacion erronea
						sscanf_s(buffer_in,"ERROR Autenticacion erronea intentos %s\r\n",intentos,sizeof(intentos));	//formato de recepci�n de intentos
						if (strncmp(buffer_in,ER,5)==0 && estado==S_PASS){	//En caso de reccibir un error despues de enviar la contrase�a
							estado = S_USER;								//volvemos al estado usuario
							if (strncmp(intentos,"3",1)==0){				//Al tercer intento
								estado = S_QUIT;							//Se Cierra la conexi�n
							}	
						}
						*/
						//Avance de la maquina de estados
						/*else*/ if(estado!=S_DATA && strncmp(buffer_in,OK,2)==0){
							estado++;
						}
						
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
		option=_getche();	//realizar otra conexi�n

	}while(option!='n' && option!='N');

	
	
	return(0);

}
