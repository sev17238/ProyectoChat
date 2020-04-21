/*
    Universidad del Valle de Guatemala
    Sistemas operativos
    Alexander Trujillo
    Diego Sevilla
    Carlos Chew
    //////////////////////////////////
    Programa para el manejo de un cliente y su conexion al servidor
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
#include <pthread.h>
//#include <windows.h> //para delay()
//red
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
//protobuf
#include "mensaje.pb.h"
#include "a.h"

using namespace std;
using namespace chat;
//utiles
using std::cout;
using std::cin;
using std::endl;
using std::string;

//Definicion de puerto y nombre de host
#define HOSTNAME "localhost"

//#define HOSTNAME "18.222.175.74"
//#define PORT 8888
#define MAXDATASIZE 4096

int fd;
int numbytes;
char buffer[MAXDATASIZE];
//char bufferInter[MAXDATASIZE];
//char bufferMensajes[MAXDATASIZE];
struct hostent *he;
struct sockaddr_in servidor;
/* {
    short   sin_family; must be AF_INET 
    u_short sin_port;
    struct  in_addr sin_addr;
    char    sin_zero[8]; Not used, must be zero 
};*/

struct connection_data
{
	int fd;
	int numbytes;
	struct sockaddr_in server;
	struct hostent *he;
    string IPbuf;
    int port;
    string username;
};

//string username;
string IPbuf;
int id;
string statusG = "ACTIVO";
string choice; //para decisiones de cliente
int mensajellego = 0;

//Funcion para errores y salida inmediata
static void err(const char* s){
    perror(s);
    exit(EXIT_FAILURE);
}

string *INFOusername;

int NumberClient;

void ThreeWayHandShake(string username, string IPbuf,int fd,char *buffer,int PORT){

    MyInfoSynchronize *miInfo(new MyInfoSynchronize);
    //cout << "\nIngrese su nombre de cliente: " << endl;
    //cin >> username;
    miInfo->set_username(username); 
    miInfo->set_ip(IPbuf);
    //cout << "\nip: " << IPbuf << endl;

    // Se crea instancia de Mensaje, se setea los valores deseados
    ClientMessage * message(new ClientMessage);
    message->set_option(1);
    //message->set_userid('2');
    message->set_allocated_synchronize(miInfo);
    
    // Se serializa el message a string
    string binary;
    message->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

    send(fd , cstr , strlen(cstr) , 0 );       //Se manda el nuevo usuario con su respectivo id.

    //delay(4000);
    //________________________________________________
    //Se recibe la respuesta del servidor
    read( fd , buffer, PORT);
    //string ret(buffer, PORT); //se convierte el char a string

    ServerMessage * server_res(new ServerMessage);
    //server_res->ParseFromString(ret);
    server_res->ParseFromString(buffer);
    cout << "response userid: " << server_res->myinforesponse().userid() << endl;
    id = server_res->myinforesponse().userid(); //id que el servidor asigno al cliente

    MyInfoAcknowledge * ack(new MyInfoAcknowledge);
    ack->set_userid(server_res->myinforesponse().userid());
    cout << "ack userid: " << ack->userid() << endl;

    ClientMessage * message2(new ClientMessage);
    message2->set_option(7);
    message2->set_userid(id);                //id que el servidor asigno al cliente
    message2->set_allocated_acknowledge(ack);

    string binary2;
    message2->SerializeToString(&binary2);

    char cstr2[binary2.size() + 1];
    strcpy(cstr2, binary2.c_str());
    send(fd , cstr2 , strlen(cstr2) , 0 );     //Se manda el acknoledge al servidor.
}


void BroadCasting(string message,int fd,char *buffer,int PORT){
    BroadcastRequest * broad (new BroadcastRequest);
    broad->set_message(message);

    ClientMessage * c_message(new ClientMessage);
    c_message->set_option(4);
    c_message->set_userid(id);
    c_message->set_allocated_broadcast(broad);
    //cout << "\nstatusprueba3\n" << endl;
    cout << "id: " << id<< endl;

    // Se serializa el message a string
    string binary;
    c_message->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

    send(fd , cstr , strlen(cstr) , 0 );       //Se manda el mensaje */

    /*recv( fd , buffer, PORT,0);
    //string ret(buffer, PORT);
    //cout << "\nstatusprueba5\n" << endl;
    ServerMessage * s_message(new ServerMessage);
    //s_message->ParseFromString(ret);
    s_message->ParseFromString(buffer);

    cout << "estado de mensaje enviado: " << s_message->broadcastresponse().messagestatus()<< endl;*/



}


//chafa
void Mensajes(int fd,char *buffer,int PORT)
{
   
    recv( fd , buffer, PORT,0);
    //string ret(buffer, PORT);
    //cout << "\nstatusprueba5\n" << endl;
    ServerMessage * s_message(new ServerMessage);
    //s_message->ParseFromString(ret);
    s_message->ParseFromString(buffer);

    if(s_message->option()== 2){
        cout << "cliente con id: " << s_message->broadcast().userid()<< endl;
        cout << "mensaje broadcast: " << s_message->broadcast().message()<< endl;
    }else if(s_message->option()== 3){
        cout << "cliente con id: " << s_message->message().userid()<< endl;
        cout << "mensaje broadcast: " << s_message->message().message()<< endl;
    }
}


void sendDirectMessage(string receiver,string message,int fd,char *buffer,int PORT){
    DirectMessageRequest * direct (new DirectMessageRequest);
    direct->set_message(message);
    direct->set_userid(id);
    direct->set_username(receiver);

    ClientMessage * c_message(new ClientMessage);
    c_message->set_option(5);
    c_message->set_userid(id);
    c_message->set_allocated_directmessage(direct);
    //cout << "\nstatusprueba3\n" << endl;
    cout << "id: " << id<< endl;

    // Se serializa el message a string
    string binary;
    c_message->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

    send(fd , cstr , strlen(cstr) , 0 );       //Se manda el mensaje

    recv( fd , buffer, PORT,0);
    //string ret(buffer, PORT);
    //cout << "\nstatusprueba5\n" << endl;
    ServerMessage * s_message(new ServerMessage);
    //s_message->ParseFromString(ret);
    s_message->ParseFromString(buffer);

    cout << "estado de mensaje enviado: " << s_message->directmessageresponse().messagestatus()<< endl;
}

void changeStatus(string status,int fd,char *buffer,int PORT){
    // Se hace el request para mabiar el status.

    cout << "Este el estatus elejido: " << status << endl;
    ChangeStatusRequest * changereq(new ChangeStatusRequest);
    changereq->set_status(status);
    //changereq->set_userid(id);

    // Se crea instancia de Mensaje, se setea los valores deseados
    ClientMessage * message(new ClientMessage);
    message->set_option(3);
    message->set_userid(id);
    message->set_allocated_changestatus(changereq);

    cout << "id: " << id<< endl;

    cout << "\nSE acaba de mandar el status deseado al servidor: " << changereq->status()<< endl;
    cout << "\nSE acaba de mandar el status deseado al servidor: " << message->changestatus().status()<< endl;
    statusG = message->changestatus().status();
    // Se serializa el message a string
    string binary;
    message->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());
    send(fd , cstr , strlen(cstr) , 0);           //Se manda el mensaje con el request
    cout << "\nstatusprueba\n" << endl;

    //delay()
    //Se recibe la respuesta del servidor
    //read( fd , buffer, PORT);
    /*recv( fd , buffer, PORT,0);
    //string ret(buffer, PORT);
    cout << "\nstatusprueba5\n" << endl;
    ServerMessage * s_message(new ServerMessage);
    //s_message->ParseFromString(ret);
    s_message->ParseFromString(buffer);
    cout << "\nstatusprueba6\n" << endl;*/

    //cout << "Su estatus se actualizo a: " << s_message->changestatusresponse().status() << endl;
}



void exitChat(){
    ExitChat *exit(new ExitChat);
    exit->set_userid(id); 
    
    ClientMessage * message(new ClientMessage);
    message->set_option(7);
    message->set_userid(id);
    message->set_allocated_exitchat(exit);
    cout << "id: " << id << endl;
    // Se serializa el message a string
    string binary;
    message->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

    send(fd , cstr , strlen(cstr) , 0 );       //Se manda el nuevo usuario con su respectivo id.
}

void usersList(int fd,char *buffer,int PORT){
    connectedUserRequest * cureq(new connectedUserRequest);
    cureq->set_userid(-1);
    //changereq->set_userid(id);

    // Se crea instancia de Mensaje, se setea los valores deseados
    ClientMessage * message(new ClientMessage);
    message->set_option(2);
    message->set_userid(id);
    message->set_allocated_connectedusers(cureq);

    //cout << "id: " << id<< endl;

    // Se serializa el message a string
    string binary;
    message->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());
    send(fd , cstr , strlen(cstr) , 0);           //Se manda el mensaje con el request


    recv( fd , buffer, PORT,0);
    //string ret(buffer, PORT);
    //cout << "\nstatusprueba5\n" << endl;
    ServerMessage * s_message(new ServerMessage);
    //s_message->ParseFromString(ret);
    s_message->ParseFromString(buffer);

    if(s_message->option() == 5){
        if (s_message->connecteduserresponse().connectedusers_size() != 0){
            int i;
            for (i = 0; i < s_message->connecteduserresponse().connectedusers().size(); i++)
            {
                cout << "---------------------------------" << endl;
                cout << "Nombre: " << s_message->connecteduserresponse().connectedusers(i).username() << endl;
                cout << "Id: " << s_message->connecteduserresponse().connectedusers(i).userid() << endl;
                //cout << "Ip: " << s_message->connecteduserresponse().connectedusers(i).ip() << endl;
                cout << "Estado: " << s_message->connecteduserresponse().connectedusers(i).status() << endl;
            }            
        }
    }
}

void UserInfo(string otherclientname,int fd,char *buffer,int PORT){
    connectedUserRequest * cureq(new connectedUserRequest);
    cureq->set_userid(id);
    cureq->set_username(otherclientname);
    //changereq->set_userid(id);

    // Se crea instancia de Mensaje, se setea los valores deseados
    ClientMessage * message(new ClientMessage);
    message->set_option(2);
    message->set_userid(id);
    message->set_allocated_connectedusers(cureq);

    cout << "id: " << id<< endl;
    cout << "\nSolicito la informacion de: " << message->connectedusers().username()<< endl;

    // Se serializa el message a string
    string binary;
    message->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());
    send(fd , cstr , strlen(cstr) , 0);           //Se manda el mensaje con el request
    
    cout << "\nstatusprueba4\n" << endl;

    //read( fd , buffer, PORT);
    recv( fd , buffer, PORT,0);
    //string ret(buffer, PORT);
    cout << "\nstatusprueba5\n" << endl;
    ServerMessage * s_message(new ServerMessage);
    //s_message->ParseFromString(ret);
    s_message->ParseFromString(buffer);

    if(s_message->option() == 5){
        if (s_message->connecteduserresponse().connectedusers_size() != 0){
            cout << "----------Usuario consultado----------" << endl;
            cout << "Nombre: " << s_message->connecteduserresponse().connectedusers(0).username() << endl;
            cout << "Id: " << s_message->connecteduserresponse().connectedusers(0).userid() << endl;
            cout << "Ip: " << s_message->connecteduserresponse().connectedusers(0).ip() << endl;
            cout << "Estado: " << s_message->connecteduserresponse().connectedusers(0).status() << endl;
        }
    }else if(s_message->option() == 3){
        cout << "No existe el usuario. " << s_message->error().errormessage() << endl;
    }   
}


void *Mensajes_p(void *args)
{
    char bufferMensajes[MAXDATASIZE] = {0};
    bufferMensajes[MAXDATASIZE] = {0};

    struct connection_data *data;
	data = (struct connection_data *)args;

    int fd = data->fd;

    if(choice != "7"){

    
        //while(mensajellego != 1){

        //while(choice != "7"){
        //recv( fd , bufferMensajes, PORT,0);
        recv( fd , bufferMensajes, data->port ,0);
        cout << "\nwhilemensaje\n" << endl;
        if(bufferMensajes[0] != '\0'){
            ServerMessage * s_message(new ServerMessage);
            //s_message->ParseFromString(ret);
            s_message->ParseFromString(bufferMensajes);

            if(s_message->option()== 1){
                cout << "cliente con id: " << s_message->broadcast().userid()<< endl;
                cout << "mensaje broadcast: " << s_message->broadcast().message()<< endl;
                mensajellego = 1;
            }else if(s_message->option()== 2){
                cout << "cliente con id: " << s_message->message().userid()<< endl;
                cout << "mensaje directo: " << s_message->message().message()<< endl;
                mensajellego = 1;
            }
        }
    
        //}
        //mensajellego = 0;
    }
    
    //pthread_exit(NULL);
}


void *InteraccionGUI(void *args){

    char bufferInter[MAXDATASIZE] = {0};

    struct connection_data *data;
	data = (struct connection_data *)args;

    int fd = data->fd;
    string IPbuf = data->IPbuf;
    int PORT = data->port;

    while(choice != "7"){
        cout << "\nBienvenid@ " << data->username << "! Escoge una opcion porfavor: " << endl;
        cout << "Estatus: " << statusG << endl;
        cout << "1. Chatear con todos los usuarios" << endl;
        cout << "2. Enviar mensaje privado." << endl;
        cout << "3. Cambiar de Status" << endl;
        cout << "4. Desplegar informacion de un usuario en particular" << endl;			
        cout << "5. Listar usuarios conectados" << endl;
        cout << "6. Ayuda" << endl;
        cout << "7. Exit" << endl;
        cin >> choice;

        int i;

        if(sscanf(choice.c_str(), "%d", &i) == 1){ //Revision de entrada
            i = std::stoi(choice);
            
            if(i==1)
            {
                string newbroadcast_message;
                cout << "Escriba el mensaje que desea enviar al chat: ";
                cin >> newbroadcast_message;
                BroadCasting(newbroadcast_message,fd,bufferInter,PORT );
                
            }
            else if(i == 2)
            {
                string receiver;
                cout << "Escriba el nombre del usurio a quien desea enviarle el mensaje: ";
                cin >> receiver;
                string direct_message;
                cout << "Escriba el mensaje que desea enviarle: ";
                cin >> direct_message;                        
                sendDirectMessage(receiver,direct_message,fd,bufferInter,PORT);
                
            }
            else if(i == 3)
            {   
                string statuschoice;

                cout << "CAMBIO DE ESTATUS.\n";
                cout << "1. ACTIVO" << endl;
                cout << "2. OCUPADO" << endl;
                cout << "3. INACTIVO" << endl;
                cin >> statuschoice;
                string newstatus;
                if(statuschoice == "1"){
                    newstatus = "ACTIVO";
                    changeStatus(newstatus,fd,bufferInter,PORT);
                }else if(statuschoice == "2"){
                    newstatus = "OCUPADO";
                    changeStatus(newstatus,fd,bufferInter,PORT);
                }else if(statuschoice == "3"){
                    newstatus = "INACTIVO";
                    changeStatus(newstatus,fd,bufferInter,PORT);
                    //changeTexas(fd,buffer);
                }else{
                    cout << "Ingrese una opcion de estado valida.\n";
                }
                
                
            }
            else if(i == 4)
            {
                string username;
                cout << "Escriba el nombre del usurio de quien desea informacion: ";
                cin >> username;
                UserInfo(username,fd,bufferInter,PORT);                
            }		            
            else if(i == 5)
            {
                cout << "\nLISTA DE USUARIOS DEL CHAT" << endl;
                usersList(fd,bufferInter,PORT);	                
            }
            else if(i == 6)
            {
                string nada; 
                            cout << "\tBienvenido a la secciòn de ayuda \n";
                cout << "\n\tPuedes consultar como funciona todas las opciones del programa aquì, acontinuaciòn se iran desplegando el funcionamiento de cada una, escribe entendido cuando desees cambiar de instruciòn";

                cout << "\n\n\t1. Chatear con todos los usuarios: En secciòn puedes enviar mensajes, pero mucho ojo porque todos los usuarios conectados prodran verlos, cuando elijas esta opciòn solo escribe lo que deseas expresar, luego presiona ENTER y tu mensaje prodra ser visto por todos los demas usuarios. \n" << endl;
                cin >> nada;

                cout << "\n\t2. Enviar mensaje privado: Si no deseas que tu mensaje sea visto por todos sino solo por alguien en especifico, usa esta opciòn. Cuando entres escribe primero el nombre del usuario, asegurate de que sea el correcto, presiona ENTER y procede a escribir el mensaje que deseas enviarle al usuario, una vez mas presiona ENTER y listo.\n" << endl;	
                cin >> nada;

                cout << "\n\t3. Cambiar de Status: Mientras estes en el chat puedes tener 3 posibles status, ACTIVO(Significa que estas conectado y disponible para chatear), INACTIVO(No te encuentras disponible para nada dentro del chat) y OCUPADO(No estas disponible en el momento, pero pronto podrias estarlo), cuando entres a esta opciòn solo escribe el nùmero de la opciòn que deseas presiona ENTER y cambiars a ese estado.\n" << endl;
                cin >> nada;

                cout << "\n\t4. Desplegar informacion de un usuario en particular: Todos los usuarios en chat al igual que tù, tienen una serie de informaciòn la cual puedes ver, por ejemplo su estatus, cuando entres en esta opciòn escribe el nombre del usuario del que quieres saber mas informaciòn, asegurate que el nombre este bien escirto, despuès esa informaciòn sera desplegada en pantalla.\n" << endl;
                cin >> nada;

                cout << "\n\t5. Listar usuarios conectados: Aquì podras ver cuantos y quienes estan conectados al chat, solo entra y la informaciòn saldra en pantalla.\n" << endl;
                cin >> nada;

                cout << "\n\t7. Exit: Cuando quieras salirte usa esa opcion y automaticamente saldras del chat.\n" << endl;
                cin >> nada;	
            }                    
            else if(i == 7)
            {
                exitChat();
                cout << "Gracias por usar el chat! Adios!!\n\n";  
            }
            
        }else {
            cout << "Ingrese un numero o se chinga!! \n\n";
        } 
    }
    //pthread_exit(NULL);
}


int main(int argc, char *argv[]){
    //Cequeo de las versiones de la libreria con los headers compilados
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    int p = std::stoi(argv[3]);

    //vector<pthread_t> threadVector;
	pthread_t threadMI[2];
	void *retvals[2];

    //Se chequea que el nombre de host local este correcto.
    if((he = gethostbyname(HOSTNAME)) == NULL){
        err("get host by name");
    }else{
        cout << "Todo bien con el localhost...\n" << endl;
    }
    
    //Usuario chequea que el puerto donde el servidor recibira conexiones este disponible.
    if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        err("socket");
    }else{
        cout << "El puerto del servidor esta activo...\n" << endl;
    }

    //El buffer es limpiado
    bzero(&servidor,sizeof(servidor));
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(p);
    servidor.sin_addr = *((struct in_addr *)he -> h_addr);

    int connection;
    //Se chequea la conexion con el servidor
    if(connect(fd, (struct sockaddr *)&servidor,sizeof(struct sockaddr)) == -1){
        //cout << "SERVER NOT FOUND 404.\n" << endl;
        err("connect.");
        connection = 0;
    }else {
        cout << "CONEXION CON SERVIDOR COMPLETADA EXITOSAMENTE.\n" << endl;
        connection = 1;
    }    


    if(connection == 1){
        IPbuf = inet_ntoa(*((struct in_addr*)he->h_addr_list));
        // Se crea instacia tipo MyInfoSynchronize y se setean los valores deseados
        //////-------------------------------------------3 WAY HAND SHAKE------------------------------------------------//////
        
        ThreeWayHandShake(argv[1],IPbuf,fd,buffer,p);

        //-------------------------------------------------------------------------------------------------------------------------
        while(choice != "7"){
            struct connection_data new_connection;
            new_connection.fd = fd;
            new_connection.he = he;
            new_connection.IPbuf = IPbuf;
            new_connection.server = servidor;
            new_connection.username = argv[1];
            new_connection.port = p;

            pthread_create(&threadMI[1],NULL,InteraccionGUI,(void *)&new_connection);
            pthread_create(&threadMI[2],NULL,Mensajes_p,(void *)&new_connection);	
            pthread_join(threadMI[1], &retvals[1]);            
            pthread_join(threadMI[2], &retvals[2]);
        }
       
        pthread_exit(NULL);
        /*for (int i = 0; i < 2; ++i){
            if (pthread_join(threadMI[i], &retvals[i]) < 0){
                err("ERROR, IMposible terminar con el Thread chat");
            }
        }*/


    }
    close(fd);
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
