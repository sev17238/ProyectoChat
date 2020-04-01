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
//protobuf
#include "mensaje.pb.h"

using namespace std;
using namespace chat;


//Definicion de puerto y nombre de host
#define HOSTNAME "localhost"
#define PORT 8000
#define MAXDATASIZE 4096


//utiles
using std::cout;
using std::cin;
using std::endl;
using std::string;

//Funcion para errores y salida inmediata
static void err(const char* s){
    perror(s);
    exit(EXIT_FAILURE);
}

int main(){
    //Cequeo de las versiones de la libreria con los headers compilados
    GOOGLE_PROTOBUF_VERIFY_VERSION;

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
        err("connect");
        cout << "SERVER NOT FOUND 404. Intente de nuevo mas tarde.\n" << endl;
        connection = 0;
    }else {
        cout << "CONEXION CON SERVIDOR COMPLETADA EXITOSAMENTE.\n" << endl;
        connection = 1;
    }    


    if(connection == 1){

        //////-------------------------------------------3 WAY HAND SHAKE------------------------------------------------//////
        
        // Se crea instacia tipo MyInfoSynchronize y se setean los valores deseados
        MyInfoSynchronize *miInfo(new MyInfoSynchronize);
        cout << "\nIngrese su nombre de cliente: " << endl;
        string username;
        miInfo->set_username(username);
        miInfo->set_ip("127.0.0.1");

        // Se crea instancia de Mensaje, se setea los valores deseados
        ClientMessage * message(new ClientMessage);
        message->set_option("1");
        message->set_userid("2");
        message->set_allocated_synchronize(miInfo);
 
        // Se serializa el message a string
        string binary;
        message->SerializeToString(&binary);

        char cstr[binary.size() + 1];
        strcpy(cstr, binary.c_str());
        send(fd , cstr , strlen(cstr) , 0 );                       //Se manda el nuevo usuario con su respectivo id.

        //delay(4000);
        //________________________________________________
        //Se recibe la respuesta del servidor
        read( fd , buffer, PORT);
	    string ret(buffer, PORT);

        MyInfoResponse * response(new MyInfoResponse);
        response->ParseFromString(ret);
        cout << "response userid: " << response->userid() << endl;

        MyInfoAcknowledge * ack(new MyInfoAcknowledge);
        ack->set_userid(response->userid());

        string binary2;
        ack->SerializeToString(&binary2);

        char cstr2[binary2.size() + 1];
        strcpy(cstr2, binary2.c_str());
        send(fd , cstr2 , strlen(cstr2) , 0 );                    //Se manda el acknoledge al servidor.

        




        //-------------------------------------------------------------------------------------------------------------------------




        string choice;
        while(choice != "7"){
            cout << "\n1. Chatear con todos los usuarios" << endl;
            cout << "2. Enviar mensaje privado." << endl;
            cout << "3. Cambiar de Status" << endl;
            cout << "4. Listar usuarios conectados" << endl;
            cout << "5. Desplegar informacion de un usuario en particular" << endl;
            cout << "6. Ayuda" << endl;
            cout << "7. Exit" << endl;
            cin >> choice;

            int i;

            if(sscanf(choice.c_str(), "%d", &i) == 1){ //Revision de entrada
                i = std::stoi(choice);
                switch(i){
                    case 1:
                        cout << "1!!\n";
                        break;
                    case 2:
                        cout << "2!!\n";
                        break;
                    case 3:
                        cout << "3!!\n";
                        break;
                    case 4:
                        cout << "4!! \n";
                        break;
                    case 5:
                        cout << "5!! \n";
                        break;
                    case 6:
                        cout << "6!! \n";
                        break;
                    case 7:
                        cout << "Gracias por usar el chat! Adios!!\n\n";    
                        return 0;        
                        break;
                }
            }else {
                cout << "Ingrese un numero o se chinga!! \n\n";
            }           
        }  

    }/*else{   
        cout << "servidor NOT FOUND 404. \nTry again later. \n\n";

    }*/
    close(fd);
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}