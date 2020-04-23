

<h1 align="center">
<br>
  <a href="https://en.wikipedia.org/wiki/Client%E2%80%93server_model"><img src="https://entreunosyceros.net/wp-content/uploads/2013/10/clienteservidor-1024x538.jpg" alt="ClientServer" width=432 length=512"></a>
<br>
<br>
Proyecto de Chat
</h1>

<p align="center">Chat basico con implementacion de modelo cliente-servidor</p>


<hr />

## Descripción
Con este trabajo se reforzarán los conocimientos sobre procesos, threads, concurrencia y comunicación entre procesos. El objetivo es desarrollar una aplicación de chat en C/C++.

## Documentación
Documentación detallada sobre la esctrructura completa del proyecto.
- :point_right: [LINK](https://chchew.github.io/DOC-ProyectoChat/index.html)

## Tabla de contenido
En este proyecto se utilizan:

- :milky_way: **Google Protobuf** — Protocolo de serializacion de mensajes
- 🛠 **Sockets** — Para comunicacion en la red
- :balloon: **Pthreads** — Para manejo de concurrencia y comunicacion entre procesos

## Compilación y ejecución
1. Clonar el repo usando `https://github.com/sev17238/ProyectoChat.git`
2. Ir al directorio deseado: `cd ProyectoChat`.<br />
3. Correr `protoc -I=. --cpp_out=. mensaje.proto` para compilar el .proto y generar: mensaje.pb.cc y mensaje.pb.h
4. Y para compilar: `g++ file.cpp mensaje.pb.cc -lpthreads -lprotobuf -o filetest`
5. Correr `./filetest`

## Referencias

### Implementación PROTOBUF
 - https://vorbrodt.blog/2019/03/10/protocol-buffers-or-how-to-serialize-data/
 - https://developers.google.com/protocol-buffers/docs/cpptutorial
 - https://developers.google.com/protocol-buffers/docs/downloads?hl=es-419
- 
### Multithreading in C
 - https://www.geeksforgeeks.org/multithreading-c-2/
 - https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
 - http://profesores.elo.utfsm.cl/~agv/elo330/2s06/lectures/POSIX_threads/POSIX_Threads_Synchronization.html

### Client Server Model
 - https://github.com/lytsing/protobuf-demo
 - https://www.bogotobogo.com/cplusplus/sockets_server_client.php

### Sockets Tutorial
 - http://www.linuxhowtos.org/C_C++/socket.htm
 
## Instructivo para correr el chat
 - https://docs.google.com/document/d/1Q7Wml_oZrhG3A67V8cyEnkWPQP2rzPjwsfn9-CMHlQI/edit?usp=sharing