/*
Instituto tecnológico de Costa Rica
Administración de Tecnologías de Información
TI 3404, Lenguajes de rogramación
Prof.: Andréi Fuentes leiva

	Esrudiantes:					Carné:
	Melissa Gutiérrez Vega			2010
	Fernanda Fernández Ocampo		2010
	Jean Carlo Alfaro Campos		201025382

Tarea Programada #1

	MESSENGER EN LENGUAJE C

-> Uso del paradigma imperativo
-> Implementación de Sockets
-> Redes con SO Linux
-> Bifurcación de Procesos
*/

//Inclusión de archivos objeto (.h)//
#include <unistd.h>     // Symbolic Constants //
#include <sys/types.h>  // Primitive System Data Types //
#include <errno.h>      // Errors //
#include <stdio.h>      // Input/Output //
#include <sys/wait.h>   // Wait for Process Termination //
#include <stdlib.h>     // General Utilities //
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/un.h>
#include <netdb.h>

int main(int argc, char *argv[])	// Entrada: IP_Remota, Puerto_Remoto, Puerto_Local, Nick //
{

	if(argc != 5)
	{
		// Se muetsra el uso correcto en caso de invocación errónea //
		printf("\n¡ups!\nEl uso correcto del programa es el siguiente:\n\t%s IP_Remota Puerto_Remoto Puerto_Local Nickname\n\n", argv[0]);
	}

	// Nombramiento de los parámetros recibidos //
	char* programa = argv[0];
	char* ip_remota = argv[1];
	int puerto_remoto = atoi(argv[2]);
	int puerto_local = atoi(argv[3]);
	char* nick = argv[4];

	printf("\n\t¡Hola %s!\n\tBienvenid@ a ATI_Messenger_3404, Edición para Estudiantes\n\n", nick);
    
    // BIFURCACIÓN de procesos mediante el uso de fork() //
	pid_t childpid;	// process_ID del proceso hijo //
    childpid = fork();	// Llamada a fork() //
    
    if (childpid >= 0)	// si el process_ID del hijo es >= 0, el fork fue exitoso //
    {

		if (childpid == 0)	// fork() retorna 0 al process_ID del hijo //
        {

			// SERVIDOR //
		
			int fd, fd2;	// nombre que describe los sockets //
			int sin_size;	// Variable para el tamanno de conexiones //

			char buf[1024];	// bufer para las cadenas de texto recibidas //

			struct sockaddr_in server;	// dirección IP del servidor //
			struct sockaddr_in client;	// dirección IP del cliente //

			if ((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 )	// CREACION DEL SOCKET //
			{  
			  printf("error en socket()\n");	// ERROR en la creacion del socket //
			  exit(-1);
			}

			server.sin_family = AF_INET;	// Asignacion de la familia del socket //
			server.sin_port = htons(puerto_local);	// Asignacion del Puerto_Local //
			server.sin_addr.s_addr = INADDR_ANY;	// Dirección IP local de manera automatica //
			bzero(&(server.sin_zero),8);	// Se colocan ceros en el resto de la estructura //

			if(bind(fd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1)	// Intento de hacer bind con los puertos ingresados //
			{
				printf("ERROR Puerto ocupado -.- \n");	// ERROR en caso de que no se pueda completar el bind //
				exit(-1);
			}     

			if(listen(fd,1) == -1)	// llamada al proceso de escucha, 2do arg es num de conexiones permitidas //
			{  
				printf("error en listen()\n");	// ERROR en caso que falle el proceso de listen //
				exit(-1);
			}
 
			sin_size = sizeof(struct sockaddr_in);	// Se captura el tamanno para realizar conexiones //

			if ((fd2 = accept(fd,(struct sockaddr *)&client, &sin_size))==-1)	// Valida que la conexion sea aceptada //
			{
				printf("error en accept()\n");	// ERROR en caso que no se pueda aceptar la conexion entrante //
				exit(-1);
			}

			// Se muestra ip remota //
			printf("\tUn amigo se ha conectado desde: %s\n\tPara chatear solo teclea y presiona Enter\n\n", (char*)inet_ntoa(client.sin_addr));

			while (strcmp(buf,"Adios") != 0)	// revisa que el mensaje entrante no sea "Adios" //
			{ 
			 	if ((recv(fd2,buf,1024,0)) > 0)	// Recibe el mensaje y valida error //
				{  
					printf("Amig@: %s\n", buf);	// muestra el mensaje recibido //
				}
			}

			close(fd2);	// Se finaliza la conexion //
			close(fd);	// Se finaliza el socket //
			printf("\tTu Amig@ terminó la conversación,\n\tVuelve pronto...!\n\n");	// AVISO si el proceso remoto finaliza la charla //
			exit(-1);	// Sale del Programa //
			
		}	// FIN del proceso padre //
		else	//fork() retorna process_ID del hijo al padre//
        {

			// CLIENTE //
        
			int fd;	// nombre que describe los sockets //

			struct hostent *he;	// estructura para informacion del proceso remoto //
			struct sockaddr_in server;	// dirección IP del servidor //

			if ((he=gethostbyname(ip_remota))==NULL)	// se comprueba la direccion IP_Remota //
			{   
				printf("gethostbyname() error\n");	// ERROR en caso de no poder verificar la IP_Remota //
				exit(-1);
			}

			if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1)	// CREACION DEL SOCKET //
			{  
				printf("socket() error\n");	// ERROR en la creacion del socket //
				exit(-1);
			}

			server.sin_family = AF_INET;	// Asignacion de la familia del socket //
			server.sin_port = htons(puerto_remoto);	// Asignacion del Puerto_Remoto //
			server.sin_addr = *((struct in_addr *)he->h_addr);	// Dirección IP_Remota //
			bzero(&(server.sin_zero),8);	// Se colocan ceros en el resto de la estructura //

			while(connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr))==-1)	// Se intenta realizar la conexion //
			{ 
				// Tratando de Conectarse ////////////////////////////////////////
			}

			int conversacion = 1;	// booleano para indicar si termina la conversacion //
			while(conversacion == 1)	// bucle de conversacion activa  //
			{
				char mensaje[1024];	// bufer para las cadenas de texto recibidas //
				//printf("Tu: ");
				gets (mensaje);	// obtener el mensaje del teclado //

				if (strcmp(mensaje, "Adios") == 0)	// revisa que el mensaje entrante no sea "Adios" //
					conversacion = 0;	// en caso de "Adios" finaliza la conversacion //

				if (send(fd,&mensaje,1024,0) == -1)	// Envia el mensaje y valida error //
					printf("\n\tERROR en el envío :/\n\tVuelve a intentarlo\n");	// ERROR en case que falle el envio //

			}

			printf("\tTerminaste la conversación,\n\tVuelve pronto...!\n\n");	// AVISO si se finaliza la charla //
			close(fd);	// Se finaliza el socket //
			exit(0);		// Sale del Programa //
			
		}
    }	// FIN de proceso hijo //
	else	// fork() retorna un -1 en caso de fallo //
    {
        perror("Error en la bifurcacion de procesos fork()");	// Despliegue si hay ERROR en el fork() //
        exit(0);	// Sale del Programa //
    }  // FIN de error en el fork() //

} // FIN del main //
