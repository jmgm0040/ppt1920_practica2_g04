#ifndef protocolostpte_practicas_headerfile
#define protocolostpte_practicas_headerfile
#endif

// COMANDOS DE APLICACION
#define SC "USER"  // SOLICITUD DE CONEXION USER usuario 
#define PW "PASS"  // Password del usuario  PASS password

#define SD  "QUIT"  // Finalizacion de la conexion de aplicacion
#define SD2 "EXIT"  // Finalizacion de la conexion de aplicacion 
#define ECHO "ECHO" // Definicion del comando "ECHO" para el servicio de eco
#define AUTH "AUTH" //Comando de autenticacion
#define SUM "SUM" //Suma de autentificación


// RESPUESTAS A COMANDOS DE APLICACION
#define OK  "OK"
#define ER  "ER"

//FIN DE RESPUESTA
#define CRLF "\r\n"

//ESTADOS
#define S_HELO 0
#define S_REMI 1
#define S_DEST 2
#define S_AUTH 3
#define S_DATA 4
#define S_RSET 5
#define S_QUIT 6
#define S_EXIT 7
//Comandos de protocolo
#define HELO "helo"
#define FROM "mail from:"
#define RCPT "rcpt to:"
#define DATA "data"
#define RSET "RSET"


//PUERTO DEL SERVICIO
#define TCP_SERVICE_PORT	25

// NOMBRE Y PASSWORD AUTORIZADOS
#define USER		"alumno"
#define PASSWORD	"123456"