#ifndef protocolostpte_practicas_headerfile
#define protocolostpte_practicas_headerfile
#endif

// COMANDOS DE APLICACION
#define HELO "HELO"  // Mensaje de bienvenida del servidor.
#define MAIL "MAIL FROM:"  // Comando para introducir el remitente del correo.
#define RCPT "RCPT TO:"  // Comando para indicar el destinatario del correo.
#define DATA "DATA"  // Comando para introducir el mensaje del correo.
#define RSET "RSET"  // Comando para resetear el proceso.
#define QUIT "QUIT"  // Finalizacion de la conexion de aplicacion.

// RESPUESTAS A COMANDOS DE APLICACION
#define OK  "2"
#define OKDATA "3"
#define ER  "5"

//FIN DE RESPUESTA
#define CRLF "\r\n"
#define SP " "

//ESTADOS
#define S_WLCM 0
#define S_HELO 1
#define S_MAIL 2
#define S_RCPT 3
#define S_DATA 4
#define S_MSEG 5
#define S_RSET 6
#define S_QUIT 7

//PUERTO DEL SERVICIO
#define TCP_SERVICE_PORT	25

#define mod "mod"
