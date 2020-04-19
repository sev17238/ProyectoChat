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
#include "a.h"
//#include <dos.h> //para delay()

using namespace std;
using namespace chat;
using std::cout;
using std::cin;
using std::endl;
using std::string;

string NombreUser;
string UserStatus;
int lola;

class Cliente
{
    public:
    struct sockaddr_in socket; //socket del cliente
    int fd; //file descriptor del socket
    int fdconn; //file descriptor generado por la connexion
    string username; //nombre de usuario
    int id = -1; //identificacion
    string ip; //direccion ip
    string status; //estado

};

#define PORT 8080          // el puerto a donde los usuarios se van a conectar
#define MAXDATASIZE 4096    // el maximo numero de bites que se pueden mandar a la vez
#define BACKLOG 5          // how many pending connections queue will hold

Cliente clientes_connectados[BACKLOG]; //almacenamiento de los clientes


int listenfd; //file descriptor para escuchar clientes
int connectfd; // file descriptor para conexion
int numbytes;
char buf[MAXDATASIZE];
struct sockaddr_in servidor;
struct sockaddr_in cliente;
int clients_count = 0;
socklen_t sin_size;
struct sigaction sa;
char hostbuf[256];
struct hostent *he;



//esta funcion estaba en un ejemplo, talvez la quitamos sino sirve.
void sigchld_handler(int s) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void devolver_INFO(string name){
	if (name == NombreUser){
	cout << "Nombre: " + NombreUser;
	cout << "Status: " + UserStatus;
	cout << "ID: " + lola;
	}else{
	cout << "informaciòn no disponible";
	}
}

string mensajeDevolver;
int mensajeNumero;
void devolver_todos(string something, int number){
	cout << "\n" << number << something;
}

//Funcion para errores y salida inmediata
static void err(const char* s) {
    perror(s);
    exit(EXIT_FAILURE);
}

void ThreeWayHandShake(int connectfd,char *buf,struct sockaddr_in socketcliente){
    /// REQUEST PARA NUEVOS USUARIOS -----------------------------------------------
    //Se recibe el MyInfoSynchronize del cliente
    read( connectfd , buf, PORT);

    //string ret1(buf, PORT); //se convierte el char a string

    ClientMessage * message(new ClientMessage);
    //message->ParseFromString(ret1);
    message->ParseFromString(buf);
    
    cout << "Client IP: " << message->synchronize().ip() << endl;
    cout << "ClientMessage id: " << message->userid() << endl;

    int currentid;
    int success = 0;
    int i = 0;
	
    for (i = 0; i < BACKLOG; i++)
    {
        Cliente c = clientes_connectados[i];
        cout << "Client name socket: " << message->synchronize().username() << endl;
        cout << "Client name obj: " << c.username << endl;
        cout << "True or false: " << (message->synchronize().username() != c.username) << endl;
        cout << "True or false length: " << (c.username.length() == 0) << endl;
        
        if(message->synchronize().username() != c.username && c.username.length() == 0 ){
            c.socket = socketcliente;
            c.fdconn = connectfd;
            c.id = i;
            currentid = i;
            c.status = "Activo";
            c.username = message->synchronize().username();
            c.ip = message->synchronize().ip();
            clientes_connectados[i] = c;

            cout << "El cliente con ip: " << message->synchronize().ip() << endl;
            cout << "y nombre: " << message->synchronize().username() << " Ha sido agregado al chat."<< endl;
            success = 1;
            break;
        }else{
            cout << "EL registro de un usuario fallo. Debido a: "<< endl;
            cout << "1. Id o nombre ya existente o 2. Numero de clientes conectados ha sobrepasado 10."<< endl;
            success = 0;
        }
    }
   
    if(success == 1){
        // Se crea instancia de respuesta para el cliente.
        MyInfoResponse * response(new MyInfoResponse);
        response->set_userid(currentid);
        cout << "Userid: " << currentid << endl;
        ServerMessage * server_res(new ServerMessage);
        server_res->set_option(4);
        server_res->set_allocated_myinforesponse(response);

        // Se serializa la respuesta a string
        string binary;
        server_res->SerializeToString(&binary);

        char cstr[binary.size() + 1];
        strcpy(cstr, binary.c_str());
        send(connectfd , cstr , strlen(cstr) , 0 );                               //Se manda el inforesponse devuelta al usuario o cliente

        //delay(5000);
        //_________________________________________________________
        //Se recive el acknowledge del cliente para comenzar con la comunicacion
        read( connectfd , buf, PORT);
        //string ret2(buf, PORT); //se convierte el char a string

        ClientMessage * message2(new ClientMessage);
        //message2->ParseFromString(ret2);
        message2->ParseFromString(buf);

        cout << "Acknowledge userid: " << message2->acknowledge().userid() << endl; //por alguna razon no jala el id correcto
        //cout << "Message userid: " << message2->userid() << endl;

    }else{
        ErrorResponse * errr(new ErrorResponse);
        errr->set_errormessage("Error Drastico!!");
    }
    
    
}

void clientsBroadCasting(){
    
}

void sendClientDirectMessage(){
    
}

void changeClientStatus(int connectfd,char *buf){

    read( connectfd , buf, PORT);
    //string ret1(buf, PORT); //se convierte el char a string

    ClientMessage * message(new ClientMessage);
    //message->ParseFromString(ret1);
    message->ParseFromString(buf);
    
    cout << "Change status request from Client id: " << message->changestatus().userid() << endl;
    cout << "status al que quiere cambiar: " << message->changestatus().status() << endl;
    cout << "\nstatusprueba6\n" << endl;
    int i;
    for (i = 0; i < BACKLOG; i++){
        Cliente c = clientes_connectados[i];
        cout << "\nc.id: "<< c.id << " userid(): " << message->changestatus().userid() << endl;
        if(c.id == message->userid()){
            c.status = message->changestatus().status();
            
        }
    }
    

    //ChangeStatusResponse * status_res(new ChangeStatusResponse);
    ChangeStatusResponse * status_res(new ChangeStatusResponse);
    status_res->set_status(message->changestatus().status());
    status_res->set_userid(message->changestatus().userid());

    ServerMessage * server_res(new ServerMessage);
    server_res->set_option(6);
    server_res->set_allocated_changestatusresponse(status_res);

    // Se serializa la respuesta a string
    string binary;
    server_res->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());
    send(connectfd , cstr , strlen(cstr) , 0 );       //se manda el response al cliente



}

void clientList(){

}

void clientInfo(){

}

void helpSection(){

}

void exitClient(int connectfd,char *buf){
    read( connectfd , buf, PORT);
    //string ret1(buf, PORT); //se convierte el char a string
    ClientMessage * message(new ClientMessage);
    //message->ParseFromString(ret1);
    message->ParseFromString(buf);
    
    cout << "El Client con id: " << message->exitchat().userid() << " desea salir." << endl;
    int i;
    for (i = 0; i < BACKLOG; i++){
        Cliente c = clientes_connectados[i];
        if(c.id == message->exitchat().userid()){
            Cliente clienteVacio;
            clientes_connectados[i] = clienteVacio;
            
        }
    }

    cout << "El Client con id: " << message->exitchat().userid() << " ha salido de la sesion." << endl;
}


int main(int argc, char** argv) {
    
    //Cequeo de las versiones de la libreria con los headers compilados
    GOOGLE_PROTOBUF_VERIFY_VERSION;
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
    //cout << "listenfd " << listenfd << endl;

    int opt = SO_REUSEADDR;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        err("setsockopt");
    }else
    {
        cout << "Todo bien con el puerto del socket... \n" << endl;
    }
    
    bzero(&servidor, sizeof(servidor));
    servidor.sin_family = AF_INET;                // orden de bytes del host
    servidor.sin_port = htons(PORT);              // orden de ytes de la red
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
        ThreeWayHandShake(connectfd,buf,cliente);    
        
        ////--------------------------------------------------------------------
        
        //PROBAR IMPRIMIR LOS CLIENTES EN LA LISTA. 
        //ENTONCES AL AGREGAR UN NUEVO CLIENTE REEMPLAZA EL QUE YA EXISTE.
        int e = 0;
        for (e = 0; e < BACKLOG; e++)
        {
            Cliente c = clientes_connectados[e];
            //if(c != NULL){
            cout << "\n"<< e << ". Cliente: " << c.username <<endl;
	    mensajeDevolver = ". Cliente: " + c.username;
	    mensajeNumero = e;
            cout << "estado: " << c.status << " id: " << c.id << "\n" <<endl;
	    NombreUser = "" + c.username;
	    UserStatus = c.status;
	    lola = c.id;
	    
            //}
        }
        
        devolver_INFO(NombreUser);
        exitClient(connectfd,buf); //Ya funciona


        //changeClientStatus(connectfd,buf); //prueba que no sirve ahorita :( basura asquerosa*/


    }

    close(listenfd);
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
