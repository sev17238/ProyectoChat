/*
    Universidad del Valle de Guatemala
    Sistemas operativos
    Alexander Trujillo
    Diego Sevilla
    Carlos Chew
    //////////////////////////////////
    Programa para el manejo del servidor y gestion de usuarios
*/

//Strings
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string>
#include <sstream>
#include <iostream>
//sistema
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
//#include <windows.h> //para delay()
//red
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//protobuf
#include "mensaje.pb.h"

//#include <dos.h> //para delay()


using namespace std;
using namespace chat;

#define PORT 8000           // el puerto a donde los usuarios se van a conectar
#define MAXDATASIZE 4096    // el maximo numero de bites que se pueden mandar a la vez
#define BACKLOG 10          // how many pending connections queue will hold


using std::cout;
using std::cin;
using std::endl;
using std::string;


void sigchld_handler(int s) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

//Funcion para errores y salida inmediata
static void err(const char* s) {
    perror(s);
    exit(EXIT_FAILURE);
}

void ThreeWayHandShake(int connectfd,char *buf){
     /// REQUEST DE USUARIOS -----------------------------------------------
    //Se recibe el MyInfoSynchronize del cliente
    read( connectfd , buf, PORT);
    string ret1(buf, PORT); //se convierte el char a string

    ClientMessage * message(new ClientMessage);
    message->ParseFromString(ret1);
    cout << "ClientMessage id: " << message->userid() << endl;

    // Se crea instancia de respuesta para el cliente.
    MyInfoResponse * response(new MyInfoResponse);
    response->set_userid('1');

    // Se serializa la respuesta a string
    string binary;
    response->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());
    send(connectfd , cstr , strlen(cstr) , 0 );                               //Se manda el id devuelta al usuario o cliente

    //delay(5000);
    //_________________________________________________________
    //Se recive el acknowledge del cliente para comenzar con la comunicacion
    read( connectfd , buf, PORT);
    string ret2(buf, PORT); //se convierte el char a string

    MyInfoAcknowledge * ack(new MyInfoAcknowledge);
    ack->ParseFromString(ret2);

    cout << "Acknowledge userid: " << ack->userid() << endl;
}


int main(int argc, char** argv) {
    //Cequeo de las versiones de la libreria con los headers compilados
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    int listenfd; //socket para escuchar
    int connectfd; // socket para conexion
    int numbytes;
    char buf[MAXDATASIZE];
    struct sockaddr_in servidor;
    struct sockaddr_in cliente;
    int clients_count = 0;
    socklen_t sin_size;
    struct sigaction sa;

    char hostbuf[256];
    struct hostent *he;
    /*char *IPbuf;

    he = gethostbyname(hostbuf);
    IPbuf = inet_ntoa(*((struct in_addr*)he->h_addr_list[0]));
    cout << "ip :" << IPbuf << "\n"<< endl;*/

    //Se chequea que el puerto donde se recibira las conexiones este disponible.
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        err("socket");
    }else
    {
        cout << "Todo bien con el socket... \n" << endl;
    }
    //cout << "ip " << AF_INET << endl;

    int opt = SO_REUSEADDR;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        err("setsockopt");
    }else
    {
        cout << "Todo bien con el puerto del socket... \n" << endl;
    }
    
    bzero(&servidor, sizeof(servidor));
    servidor.sin_family = AF_INET;                // orden de bytes del host
    servidor.sin_port = htons(PORT);              // orden de vytes de la red
    servidor.sin_addr.s_addr = htonl(INADDR_ANY); // se llena con la IP
    
    //Se emplea la llamada a sistema bind() para hacerle bind al socket a la ip del puerto del servidor
    if (bind(listenfd, (struct sockaddr *)&servidor, sizeof(struct sockaddr)) == -1) {
        err("bind");
    }else{
        cout << "Todo bien con el binding del socket y la direccion ip... \n" << endl;
    }

    //Se emplea la llamada de sistema listen() que permite al proceso chequear por conexiones en el socket. 
    if (listen(listenfd, BACKLOG) == -1) {
        err("listen");
    }else
    {
        cout << "Escuchando... \n" << endl;
    }

    sa.sa_handler = sigchld_handler;  // se eliminan calquier proceso existente
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        err("sigaction");
    }


    while(1){
        sin_size = sizeof(struct sockaddr_in);
        connectfd = accept(listenfd, (struct sockaddr *)&cliente, &sin_size);

        if (connectfd < 0){
            err("ERROR on accept");
            
        }



        /// REQUEST DE USUARIOS -----------------------------------------------
        ThreeWayHandShake(connectfd,buf);    

        ////--------------------------------------------------------------------







    }

    close(listenfd);
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}