/*******************************************************
Protocolos de Transporte


Fichero: cliente.c
Versión: 2.0
Autores: José Manuel Gómez Muñoz
		 Javier Almodóvar Villacañas
Descripción:
	Cliente sencillo SMTP para IPv4

*******************************************************/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ws2tcpip.h>//Necesaria para las funciones IPv6
#include <conio.h>
#include "protocol.h"
#include <string.h>



#pragma comment(lib, "Ws2_32.lib") // Añade la librería actualizada para IPV6

int main(int *argc, char *argv[])
{
	SOCKET sockfd; //Crea un nuevo socket
	struct sockaddr *server_in=NULL; //Estructura para los  parametros del socket
	struct sockaddr_in server_in4;
	struct sockaddr_in6 server_in6;
	int address_size = sizeof(server_in4);
	char buffer_in[1024], buffer_out[1024],input[1024]; //Inicializa los parámetros cadena para el funcionamiento correcto de la entrada y salida
	int recibidos=0,enviados=0;
	int estado=S_HELO; //Inicializa el estado y le da el valor de bienvenida
	char option;
	int ipversion=AF_INET;//IPv4 por defecto
	char ipdest[256];
	char default_ip4[16]="127.0.0.1";//Ip por defecto
	char default_ip6[64]="::1";
	char emisor[2054] = " ";//nombre del remitente
	char receptor[2054] = " ";//nombre del destinatario
	int i = 0;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
   
	//Inicialización Windows sockets - SOLO WINDOWS
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1){
		WSACleanup();
		return(0);
	}
	//Fin: Inicialización Windows sockets
	
	printf("**************\r\nCLIENTE SMTP SENCILLO SOBRE IPv4 \r\n*************\r\n");
	

	do{

		
			ipversion = AF_INET;//Para IPV4
		

		sockfd=socket(ipversion,SOCK_STREAM,0); //Crea un socket con los datos introducidos
		if(sockfd==INVALID_SOCKET){
			printf("CLIENTE> ERROR\r\n"); //Si el socket no es valido cierra el programa
			exit(-1);
		}
		else{
			printf("CLIENTE> Introduzca la IP destino o el dominio (pulsar enter para IP por defecto): ");
			gets_s(ipdest,sizeof(ipdest)); //Obtiene el valor IP introducido
			//printf(getaddrinfo(ipdest));
			struct hostent* servidor; //Estructura de datos de host
			servidor = gethostbyname(ipdest); //Resolucion de dominios
			if (servidor == NULL)
			{
				printf("La direccion IP del host es erronea, el programa se va a cerrar...\n"); //Comprueba que el host sea valido
				return 0;
			}
			struct in_addr ipadd; //Estructura de datos ip
			ipadd = *(struct in_addr*)servidor->h_addr_list[0];//Obtiene la direccion ip de un dominio
			printf("Direccion IP: %s\n", inet_ntoa(ipadd)); //Direccion ip del dominio dado
			

			
			//Dirección por defecto según la familia
			if(strcmp(ipdest,"")==0 && ipversion==AF_INET)
				strcpy_s(ipdest,sizeof(ipdest),default_ip4);

			if(strcmp(ipdest,"")==0 && ipversion==AF_INET6)
				strcpy_s(ipdest, sizeof(ipdest),default_ip6);

			if(ipversion==AF_INET){
				server_in4.sin_family=AF_INET;
				server_in4.sin_port=htons(TCP_SERVICE_PORT);
				server_in4.sin_addr.s_addr=inet_addr(inet_ntoa(ipadd));
				inet_pton(ipversion, inet_ntoa(ipadd),&server_in4.sin_addr.s_addr); //Convierte una cadena en una direccion ip
				server_in=(struct sockaddr*)&server_in4;
				address_size = sizeof(server_in4);
			}

			if(ipversion==AF_INET6){
				memset(&server_in6, 0, sizeof(server_in6));
				server_in6.sin6_family=AF_INET6;
				server_in6.sin6_port=htons(TCP_SERVICE_PORT);
				inet_pton(ipversion,ipdest,&server_in6.sin6_addr);
				server_in=(struct sockaddr*)&server_in6;
				address_size = sizeof(server_in6);
			}

			estado=S_HELO; //Estado que indica que la conexión se ha realizado

			if(connect(sockfd, server_in, address_size)==0)//Si la conexion es correcta
			{
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
				

				//Inicio de la máquina de estados
				do{
					switch(estado){
					case S_HELO:
						// Se recibe el mensaje de bienvenidaa
						printf("CLIENTE> Introduce tu nombre de equipo (enter para salir): ");
						gets_s(input, sizeof(input));
						if (strlen(input) == 0) {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", SD, CRLF);
							estado = S_QUIT; //Sale del programa al introducion enter
							enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0);//Envia todo lo del buffer de salida al socket
							recibidos = recv(sockfd, buffer_in, 512, 0);
						}
						sprintf_s(buffer_out, sizeof(buffer_out), "%s %s\n",HELO,input);//Manda la bienvenida
						enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0);//Envia todo lo del buffer de salida al socket
						recibidos = recv(sockfd, buffer_in, 512, 0);
						buffer_in[recibidos] = 0x00;
						printf(buffer_in);
																			
						break;
					case S_REMI:
						// establece la conexion de aplicacion 
						
						printf("CLIENTE> Introduzca el remitente del correo (enter para salir): ");
						gets_s(input,sizeof(input));
						if(strlen(input)==0){
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);//Si llega el input sin contenido se ejecuta esta linea de código y se para el programa
							estado=S_QUIT;
						}
						else
							
						sprintf_s (buffer_out, sizeof(buffer_out), "%s%s\n",FROM,input); //Manda al buffer de salida el usuario

						unirCadenas(emisor, input); //Inserta el nombre del remitente en la variable
						
						
						break;
					case S_DEST:
						
						printf("CLIENTE> Introduzca el destinatario del correo (enter para salir): ");
						gets_s(input, sizeof(input));
						
						if(strlen(input)==0){
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);
							estado=S_QUIT;
						}
						
						else {


							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s\n",RCPT, input);
							unirCadenas(receptor, input); //Inserta el nombre del destinatario en la variable
						}
						break;
				
					case S_AUTH: //Comprobación de la existencia de usuario, si  recibe ese codigo significa que no existe
						
						
						if (strncmp(buffer_in,"554",3)==0) {
							
							estado=4;//Manda al estado de RSET al recibir el codigo 554 error de usuario
						}

						else {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s\n",DATA);
						}
						break;
					
					
					case S_RSET:  //Apartado reset
						sprintf_s(buffer_out, sizeof(buffer_out), "%s %s",RSET, CRLF);
						enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0);//Envia
						estado = -1; //Reinicia la sesión
						break;

					case S_DATA:
						
						
						int msg = 10000;
						char mensaje[2054] = "";
						char espacio[2054]=" ";
						char salto[2054] = "\n";

						printf("CLIENTE> Introduzca el asunto (enter para dejar vacio): ");
						gets_s(input, sizeof(input));
						char body[2054] = " ";
						char asunto[2054] = "subject: ";
						unirCadenas(asunto, input);
						unirCadenas(asunto, salto);

					


						char from[2054] = "from: ";
						unirCadenas(asunto, from);
						unirCadenas(asunto, emisor);
						unirCadenas(asunto, salto);



						char to[2054] = "to: ";
						unirCadenas(asunto, to);
						unirCadenas(asunto, receptor);
						unirCadenas(asunto, salto);



						char asunto3[2054] = "\n\n";
						
						
						unirCadenas(asunto, asunto3);
						unirCadenas(mensaje, asunto);//Añade las cabeceras al mensaje

						printf("CLIENTE> Introduzca el mensaje (enter para salir): ");
						for (i = 0; i<msg; i++)
						{
							gets_s(input, sizeof(input));
							
							if (strcmp(input, ".") == 0) {
								i = msg;

								sprintf_s(buffer_out, sizeof(buffer_out), "%s %s.%s", mensaje, CRLF, CRLF);


							}
							
							unirCadenas(mensaje, input);
							unirCadenas(mensaje, espacio);//Une todo el mensaje hasta que se introduce un punto


							
							
						
						}
						emisor[2053] = " ";// vacia nombre del remitente
						receptor[2053] = " ";//vacia nombre del destinatario
						printf("CLIENTE> ¿Desea resetear la conexion? (Pulse N para no): ");
						gets_s(input, sizeof(input));
						if (strncmp(input,"n", 1) == 0|| strncmp(input, "N", 1) == 0) {
							estado=S_REMI;
						}
					
						break;
				
					
					}

					if(estado!=S_HELO){
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);//Envia todo lo del buffer de salida al socket
						if(enviados==SOCKET_ERROR){
							 estado=S_QUIT;
							 continue;
						}
					}
						
					recibidos=recv(sockfd,buffer_in,512,0); //Recibe el mensaje del servidor
					if(recibidos<=0){
						DWORD error=GetLastError();
						if(recibidos<0){
							printf("CLIENTE> Error %d en la recepción de datos\r\n",error);
							estado=S_QUIT;
						}
						else{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;
						}
					}else{
						buffer_in[recibidos]=0x00;
						printf(buffer_in);
						 //Solucionar
							estado++;  
					}

				}while(estado!=S_QUIT);		
			}
			else{
				int error_code=GetLastError();
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			}		
			// fin de la conexion de transporte
			closesocket(sockfd);//Cierra el socket
			
		}	
		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
		option=_getche();

	}while(option!='n' && option!='N');

	return(0);
	
}

int unirCadenas(char c1[], char c2[])
{
	int i, x;

	for (i = 0; c1[i] != '\0'; i++);
	for (x = 0; c2[x] != '\0'; x++, i++)
		c1[i] = c2[x];
	c1[i + 1] = '\0';
	return 0;
}