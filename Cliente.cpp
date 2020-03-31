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
#include <stdio>
#include <unistd.h>
#include <strings.h>
#include <string>
#include <sstream>
#include <iostream>
//sistema
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;
using namespace chat;

//utiles
using std::cout;
using std::cin;
using std::endl;
using std::string;

int main(){
    //GOOGLE_PROTOBUF_VERIFY_VERSION;

    int connection;


    if(connection == 1){

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
    return 0;
}