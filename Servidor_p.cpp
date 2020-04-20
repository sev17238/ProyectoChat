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
#include <pthread.h>
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

int INFONE;
string INFOTWO;
int INFOTHREE;

int mensajeD_prueba = 0;
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

//define composite data to connect with socket and client
struct connection_data
{
	int socket_cliente;
	int tid;
	struct sockaddr_in cli_addr;
	socklen_t cli_len;
	int opcionServer;
};

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

    ClientMessage * message(new ClientMessage);

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
        mensajeD_prueba++;
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

    }
}

void *conneccionPthreads(void *args){


}

void clientsBroadCasting(int listenfd, int connectfd,char *buf){
    /*read( connectfd , buf, PORT);

    ClientMessage * message(new ClientMessage);

    message->ParseFromString(buf);
    
    cout << "Client message: " << message->broadcast().message() << endl;
    cout << "ClientMessage id: " << message->userid() << endl;

    ServerMessage * serverm(new ServerMessage);
    serverm->set_allocated_broadcast(message);

    // Se serializa la respuesta a string
    string binary;
    serverm->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());  

    int i = 0;
    for (i = 0; i < BACKLOG; i++){
        Cliente c = clientes_connectados[i];
        //cout << "\nc.id: "<< c.id << " userid(): " << message->userid() << endl;
        sin_size = sizeof(struct sockaddr_in);
        connectfd = accept(listenfd, (struct sockaddr *)&c.socket, &sin_size);
        send(connectfd, cstr , strlen(cstr) , 0 );    
    }*/

}

void sendClientDirectMessage(int connectfd,char *buf){
    cout << "pruebadirectmessage1" << endl;
    //read( connectfd, buf, PORT);

    ClientMessage * message(new ClientMessage);

    message->ParseFromString(buf);
    
    cout << "Client message: " << message->directmessage().message() << endl;
    cout << "La id del cliente que desea enviar el mensaje: " << message->userid() << endl;

    int i = 0;
    int id_receiver;
    for (i = 0; i < BACKLOG; i++){
        Cliente c = clientes_connectados[i];
        cout << "\nc.id: "<< c.id << " userid(): " << message->userid() << endl;
        cout << "\nc.username: "<< c.username<< " username(): " << message->directmessage().username() << endl;
        if(c.username == message->directmessage().username()){
            id_receiver = c.id;    
        }
    }

    cout << "La id del cliente a quien se le enviara el mensaje: " << id_receiver << endl;

    DirectMessage * dm(new DirectMessage);
    dm->set_message(message->directmessage().message());
    dm->set_userid(id_receiver);

    DirectMessageResponse * dmr(new DirectMessageResponse);
    dmr->set_messagestatus("<Mensaje entregado>");

    ServerMessage * serverm(new ServerMessage);
    serverm->set_option(8);
    serverm->set_allocated_message(dm);
    serverm->set_allocated_directmessageresponse(dmr);

    // Se serializa la respuesta a string
    string binary;
    serverm->SerializeToString(&binary);
    cout << "pruebadirectmessage2" << endl;
    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());
    //send(connectfd , cstr , strlen(cstr) , 0 );
    cout << "pruebadirectmessage3" << endl;
    i = 0;
    for (i = 0; i < BACKLOG; i++){
        Cliente c = clientes_connectados[i];
        cout << "\nc.id: "<< c.id << " userid(): " << id_receiver << endl;
        if(c.id == id_receiver){
            //sin_size = sizeof(struct sockaddr_in);
            //connectfd = accept(listenfd, (struct sockaddr *)&c.socket, &sin_size);
            send(connectfd , cstr , strlen(cstr) , 0 );  

        }
    }
}

void changeClientStatus(int connectfd,char *buf){
    //read( connectfd , buf, PORT);
    ClientMessage * message(new ClientMessage);

    message->ParseFromString(buf);
    
    cout << "optin: " << message->option() << endl;
    cout << "Change status request from Client id: " << message->userid() << endl;
    cout << "status al que quiere cambiar: " << message->changestatus().status() << endl;
    cout << "\nstatusprueba6\n" << endl;
    int i;
    for (i = 0; i < BACKLOG; i++){
        Cliente c = clientes_connectados[i];
        cout << "\nc.id: "<< c.id << " userid(): " << message->userid() << endl;
        if(c.id == message->userid()){
            c.status = message->changestatus().status();
            clientes_connectados[i].status = c.status;
            
        }
    }
    
    cout << "\nstatusprueba7\n" << endl;
    //ChangeStatusResponse * status_res(new ChangeStatusResponse);
    ChangeStatusResponse * status_res(new ChangeStatusResponse);
    status_res->set_status(message->changestatus().status());
    //status_res->set_userid(message->changestatus().userid());

    ServerMessage * server_res(new ServerMessage);
    server_res->set_option(6);
    server_res->set_allocated_changestatusresponse(status_res);

    cout << "\nstatusprueba8\n" << endl;
    // Se serializa la respuesta a string
    /*string binary;
    server_res->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());
    send(connectfd , cstr , strlen(cstr) , 0 );       //se manda el response al cliente

    cout << "\nstatusprueba9\n" << endl;*/

}

void clientList(){

}

void clientInfo(){

}


void exitClient(int connectfd,char *buf){
    //read( connectfd , buf, PORT);
    ClientMessage * message(new ClientMessage);

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

//define connection with client
void *conexionConClientes(void *args)
{
	char buf[MAXDATASIZE];
	bool out_of_chat_room = false;

	struct connection_data *data;
	data = (struct connection_data *)args;

	struct sockaddr_in serv_addr;
	int socket_cliente = data->socket_cliente;
	int tid = data->tid;
	struct sockaddr_in cli_addr = data->cli_addr;
	socklen_t cli_len = data->cli_len;

	if (socket_cliente > 0)
	{

		/// REQUEST PARA NUEVOS USUARIOS -----------------------------------------------
        //Se recibe el MyInfoSynchronize del cliente
        read( connectfd , buf, PORT);

        ClientMessage * message(new ClientMessage);

        message->ParseFromString(buf);
        
        //cout << "Client IP: " << message->synchronize().ip() << endl;
        cout << "ClientMessage id: " << message->userid() << endl;
		// Se puede accesar a los valores de la siguiente manera:
		//cout << "Opcion general enviada.... : " << m->option() << endl;
		//cout << "Username: " << m->synchronize().username() << endl;
		//cout << "Client IP: " << m->synchronize().ip() << endl;

		int optionn = message->option();

		if (optionn == 1) //opcion 1 es que se quiere conectar, entonces mandamos de vuelta
		{
			// Se crea instancia de respuesta para el cliente.
            MyInfoResponse * response(new MyInfoResponse);
            response->set_userid(tid);
            cout << "Userid: " << tid << endl;
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
			int currentid;
			//Le creamos un usuario en nuestra clase
			Cliente c = clientes_connectados[tid];

			if (message->synchronize().username() != c.username && message2->userid() != c.id)
			{
				c.socket = data->cli_addr;
				c.fdconn = socket_cliente;
				c.id = message2->userid();
				currentid = message2->userid();
				c.status = "Activo";
				c.username = message->synchronize().username();
				c.ip = message->synchronize().ip();
				clientes_connectados[message2->userid()] = c;

                
				cout << "Usted ha sido agregao al server con los siguientes datos: " << endl;
				cout << "ID: " << clientes_connectados[message2->userid()].id << endl;
				cout << "Nombre: " << clientes_connectados[message2->userid()].username << endl;
				cout << "IP: " << clientes_connectados[message2->userid()].ip << endl;
				cout << "Status: " << clientes_connectados[message2->userid()].status << endl;
			}
			else
			{
				cout << "ERROR de registo cliente. Chequear lo siguiente:" << endl;
				cout << "1. Id o nombre ya existe" << endl;
				cout << "2. Numero de usuarios maximo revasado" << endl;
			}
			bzero(buf, MAXDATASIZE);
            int m;
            for (m = 0; m < BACKLOG; m++)
            {
                Cliente c = clientes_connectados[m];
                cout << "\n"<< m << "\nid: " << c.id << "\nnombre: "<< c.username<<"\nStatus:" <<c.status<< endl;

            }
            

		}

		int option_chat;
		do
		{
			printf("\nCLIENTE: ");
			recv(socket_cliente, buf, MAXDATASIZE, 0);

			//ecibimos el mensaje del cliente
			ClientMessage *message_chat(new ClientMessage);
			//message2->ParseFromString(ret2);
			message_chat->ParseFromString(buf);
			cout << "Opcion: " << message_chat->option() << endl;
			option_chat = message_chat->option();
			if (option_chat == 2){
				//clientInfo(socket_cliente, buf);
			}else if (option_chat == 3){				
                changeClientStatus(socket_cliente, buf);
			}else if (option_chat == 4){				
                //clientsBroadCasting(socket_cliente, buf);
			}else if (option_chat == 5){				
                sendClientDirectMessage(socket_cliente, buf);
			}else if (option_chat == 7){				
                exitClient(socket_cliente, buf);
			}else{
				printf("%s", buf);
			}

			/*if (*buf == '#')
			{
				printf("\nSERVER: El cliente ha abandonado la sala\nF principal\n");
				out_of_chat_room = true;
			}*/
		//} while (*buf != '#');
        } while (option_chat != 7);

		printf("Se ha finalizado la conexión con: %s\n", inet_ntoa(cli_addr.sin_addr));
		close(socket_cliente);
		out_of_chat_room = false;

	}

	pthread_exit(NULL);
}



int main(int argc, char** argv) {
    
    //Cequeo de las versiones de la libreria con los headers compilados
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    /*char *IPbuf;
    he = gethostbyname(hostbuf);
    IPbuf = inet_ntoa(*((struct in_addr*)he->h_addr_list[0]));
    cout << "ip :" << IPbuf << "\n"<< endl;*/

    //int sockfd, portno;
	//struct sockaddr_in serv_addr;
	vector<pthread_t> threadVector;
	pthread_t threadChat[BACKLOG];
	void *retvals[BACKLOG];

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

    int client_in_chat = 1;

    while(client_in_chat < BACKLOG){
        
        sin_size = sizeof(struct sockaddr_in);
        connectfd = accept(listenfd, (struct sockaddr *)&cliente, &sin_size);

        if (connectfd < 0){
            err("ERROR on accept");            
        }else{
			printf("NUEVO CLIENTE HA INGRESADO!\n");
		}


        //define composite data to get data and conection
		struct connection_data new_connection;
		new_connection.socket_cliente = connectfd;
		new_connection.cli_addr = cliente;
		new_connection.cli_len = sin_size;
		new_connection.tid = client_in_chat;

        pthread_create(&threadChat[client_in_chat],NULL,conexionConClientes,(void *)&new_connection);
		client_in_chat++;



        //CODIGO QUE DEBERIA DE USARSE PARA RECIBIR LOS REQUESTS DE LOS CLIENTES
        //Hay que volarse el read() de los metodos para hacerlo y probar, porque a mi
        //no me funciono bien, algo con el ciclo y el connectfd no deja, sinceramente nose :(
        /*
        bzero(buf,sizeof(buf)); //se limpia buf
        read( connectfd , buf, PORT);
        //recv(connectfd,buf,sizeof(buf),0);
        ClientMessage * messagee(new ClientMessage);    
        messagee->ParseFromString(buf);
        
        cout << "option message: " << messagee->option() << "\n"<< endl;
        /// REQUEST DE USUARIOS -----------------------------------------------

        if(messagee->option() == 1){
            ThreeWayHandShake(messagee,connectfd,buf,cliente); //funciona
        }else if(messagee->option() == 2){

        }else if(messagee->option() == 3){
            changeClientStatus(messagee,connectfd,buf); //prueba que no sirve ahorita
        }else if(messagee->option() == 4){
            clientsBroadCasting(messagee,connectfd,buf);
        }else if(messagee->option() == 5){
            sendClientDirectMessage(messagee,connectfd,buf);
        }else if(messagee->option() == 6){

        }else if(messagee->option() == 7){
            exitClient(messagee,connectfd,buf); //Ya funciona
        }
        */
        
        ////--------------------------------------------------------------------
        
        /// REQUEST DE USUARIOS -----------------------------------------------
        //ThreeWayHandShake(connectfd,buf,cliente); //funciona
        //cout << "hola" <<endl;
        ///-----------------------------------------------------------


        //PROBAR IMPRIMIR LOS CLIENTES EN LA LISTA. 
        //ENTONCES AL AGREGAR UN NUEVO CLIENTE REEMPLAZA EL QUE YA EXISTE.
        /*int e = 0;
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
        }*/


        //changeClientStatus(connectfd,buf); //prueba que no sirve ahorita
        
        //clientsBroadCasting(connectfd,buf); //en pruebas luego de que funcione el mensaje directo
        //devolver_INFO(NombreUser);



        //exitClient(connectfd,buf); //Ya funciona

        //if(mensajeD_prueba == 2){
        //    sendClientDirectMessage(listenfd,connectfd,buf); //aun no funciona, el mensaje del cliente se lo manda a si mismo
        //}
        

    }

    for (int i = 0; i < BACKLOG; ++i)
	{
		if (pthread_join(threadChat[i], &retvals[i]) < 0)
			err("ERROR, IMposible terminar con el Thread chat");
	}

    close(listenfd);
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}