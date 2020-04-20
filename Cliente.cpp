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
#define PORT 8080
#define MAXDATASIZE 4096

int fd;
int numbytes;
char buffer[MAXDATASIZE];
struct hostent *he;
struct sockaddr_in servidor;
/* {
    short   sin_family; must be AF_INET 
    u_short sin_port;
    struct  in_addr sin_addr;
    char    sin_zero[8]; Not used, must be zero 
};*/

string username;
string IPbuf;
int id;
int key;



//Funcion para errores y salida inmediata
static void err(const char* s){
    perror(s);
    exit(EXIT_FAILURE);
}

string *INFOusername;

int NumberClient;

void ThreeWayHandShake(string IPbuf,int fd,char *buffer){

        MyInfoSynchronize *miInfo(new MyInfoSynchronize);
        cout << "\nIngrese su nombre de cliente: " << endl;
        cin >> username;
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


void BroadCasting(string message,int fd,char *buffer){
    /*BroadcastRequest * broad (new BroadcastRequest);
    broad->set_message(message);

    ClientMessage * c_message(new ClientMessage);
    c_message->set_option(4);
    c_message->set_userid(id);
    c_message->set_allocated_broadcast(broad);
    //cout << "\nstatusprueba3\n" << endl;
    cout << "id: " << id<< endl;

    // Se serializa el message a string
    string binary;
    message->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

    send(fd , cstr , strlen(cstr) , 0 );       //Se manda el mensaje */


}

void sendDirectMessage(string receiver,string message,int fd,char *buffer){
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


}

void changeStatus(string status,int fd,char *buffer){
    // Se hace el request para mabiar el status.
    cout << "\nstatusprueba1\n" << endl;
    cout << "Este el estatus elejido: " << status << endl;
    ChangeStatusRequest * changereq(new ChangeStatusRequest);
    changereq->set_status(status);
    //changereq->set_userid(id);

    // Se crea instancia de Mensaje, se setea los valores deseados
    ClientMessage * message(new ClientMessage);
    message->set_option(3);
    message->set_userid(id);
    message->set_allocated_changestatus(changereq);
    cout << "\nstatusprueba3\n" << endl;
    cout << "id: " << id<< endl;

    cout << "\nSE acaba de mandar el status deseado al servidor: " << changereq->status()<< endl;
    cout << "\nSE acaba de mandar el status deseado al servidor: " << message->changestatus().status()<< endl;

    // Se serializa el message a string
    string binary;
    message->SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());
    send(fd , cstr , strlen(cstr) , 0);           //Se manda el mensaje con el request

    //delay()
    //Se recibe la respuesta del servidor
    read( fd , buffer, PORT);
    //string ret(buffer, PORT);

    ServerMessage * s_message(new ServerMessage);
    //s_message->ParseFromString(ret);
    s_message->ParseFromString(buffer);

    cout << "Su estatus se actualizo a: " << s_message->changestatusresponse().status() << endl;

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


int main(){
    //Cequeo de las versiones de la libreria con los headers compilados
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    key = 0; 

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
    servidor.sin_port = htons(PORT);
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
        ThreeWayHandShake(IPbuf,fd,buffer);

        //-------------------------------------------------------------------------------------------------------------------------

        string choice;
        while(choice != "7"){
            cout << "\n1. Chatear con todos los usuarios" << endl;
            cout << "2. Enviar mensaje privado." << endl;
            cout << "3. Cambiar de Status" << endl;
            cout << "4. Desplegar informacion de un usuario en particular" << endl;			
            cout << "5. Listar usuarios conectados" << endl;
            cout << "6. Ayuda" << endl;
            cout << "7. Exit" << endl;
            cin >> choice;

            int i;
	    string NombreUser;
	    string UserIp;
	    int lola;

            if(sscanf(choice.c_str(), "%d", &i) == 1){ //Revision de entrada
                i = std::stoi(choice);
                switch(i){
                    case 1:
                    {
                        string newbroadcast_message;
                        cout << "Escriba el mensaje que desea enviar al chat: ";
                        cin >> newbroadcast_message;
                        BroadCasting(newbroadcast_message,fd,buffer);
                        break;
                    }
                    case 2:
                    {
                        string receiver;
                        cout << "Escriba el nombre del usurio a quien desea enviarle el mensaje: ";
                        cin >> receiver;
                        string direct_message;
                        cout << "Escriba el mensaje que desea enviarle: ";
                        cin >> direct_message;                        
                        sendDirectMessage(receiver,direct_message,fd,buffer);
                        break;
                    }
                    case 3:
                    {   
                        string statuschoice;

                        cout << "Cambio de status.\n";
                        cout << "1. ACTIVO" << endl;
                        cout << "2. OCUPADO" << endl;
                        cout << "3. INACTIVO" << endl;
                        cin >> statuschoice;
                        string newstatus;
                        if(statuschoice == "1"){
                            newstatus = "ACTIVO";
                            changeStatus(newstatus,fd,buffer);
                        }else if(statuschoice == "2"){
                            newstatus = "OCUPADO";
                            changeStatus(newstatus,fd,buffer);
                        }else if(statuschoice == "3"){
                            newstatus = "INACTIVO";
                            changeStatus(newstatus,fd,buffer);
                            //changeTexas(fd,buffer);
                        }else{
                            cout << "Ingrese una opcion de estado valida.\n";
                        }
                        
                        break;
                    }
                    case 4:
		    {
			string search;
                        cout << "¿Sobre que usuario quieres saber informaciòn? 				\n";
			cin >> search;
			if (search == username){
				cout << "\nInformaciòn del usuario: \n";
				cout << "Este usuario eres tù, hola " + search + 					"\n";
				void devolver_INFO(string search);
			}else{
				cout << "\nInformaciòn del usuario: \n";
		                void devolver_INFO(string search);
			}
                        break;
		    }
                    case 5:
			{
                        cout << "Los usuario conectados son:  \n";
			key = 1;
                        break;
			}
                    case 6:
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
                        break;
                    case 7:
                        exitChat();
                        cout << "Gracias por usar el chat! Adios!!\n\n";  
                          
                        return 0;        
                        break;
                }
            }else {
                cout << "Ingrese un numero o se chinga!! \n\n";
            } 

/*
            bzero(buffer,sizeof(buffer));
            //Se recibe la respuesta del servidor
            read( fd , buffer, PORT);
            //string ret(buffer, PORT);

            ServerMessage * s_message(new ServerMessage);
            //s_message->ParseFromString(ret);
            s_message->ParseFromString(buffer);
*/
            //CODIGO QUE DEBERIA DE USARSE PARA RECIBIR LOS MENSAJES DEL CHAT ETC.
            /*if(s_message->option() == 1){
                cout << "Mensaje broadcast \n" << endl;
                cout << s_message->broadcast().message() << endl; 
            }else if(s_message->option() == 2){
                cout << "Mensaje directo \n" << endl;
                cout << s_message->message().message() << endl; 
            }else if(s_message->option() == 8){
                cout << "BroadcastResponse \n" << endl;
                cout << s_message->broadcastresponse().messagestatus() << endl; 
            }else if(s_message->option() == 9){
                cout << "direct message response \n" << endl;
                cout << s_message->directmessageresponse().messagestatus() << endl; 
            }*/
/*
            cout << "direct message: " << s_message->message().message() << endl; 
            cout << "messagestatus: " << s_message->directmessageresponse().messagestatus() << endl; 
            */
        }  
    }
    close(fd);
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
