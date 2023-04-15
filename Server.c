#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080      // Port number over which the communication will take place is defined. 
#define SIZE 32        // Size of the buffer is 32. This means that the file to be sent will be divided into chunks of 32 bits and then sent.

void file_send(FILE* fp, int sock_id) {                 // This is the function that will send the file to the client over the connection. It takes the file to be sent and socket number as arguments.
    char buffer[SIZE];                     
    while (1) {                                        // Data is continously sent over the socket until the end of file
        size_t data = fread(buffer, 1, SIZE, fp);  // File data is read. fread function reads data from file fp, SIZE (32) bits at a time.
        if (data == 0) {                           // If the read data is 0, function will break. This means end of file. 
            break;
        }

        int val = write(sock_id, buffer, data);       // This is to write or send the data on the socket represented by the socket number. This will return a value if sent succesfully
        if (val < 0) {                        
            perror("An error has occured while writing to the socket\n");    // Error if data not written succesfully. 
            exit(EXIT_FAILURE);
        }
        else if (val == 0) {
            break;
        }

    }
}

int main(int argc, char const *argv[]) {            //This is the main server function. It includes the standard way of creating a socket and client communication.

    int server_id, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[SIZE] = {0};
    char filename[]="";

    server_id = socket(AF_INET, SOCK_STREAM, 0);       //This function activates the socket. The socket function creates a socket and returns the server id
    if (server_id == 0) {
        perror("[-]Socket Creation Failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_id, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("[-]Error in setsockopt");
        exit(EXIT_FAILURE);
    }
    
    printf("Socket Created Successfully\n");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");      //Includes IP address
    address.sin_port = htons(PORT);

    if ( bind(server_id, (struct sockaddr *)&address, sizeof(address)) < 0 ) {   // Binding function
        perror("[-]Binding Error");
        exit(EXIT_FAILURE);
    }
    
    printf("Connection binded successfully\n");

    if (listen(server_id, 10) < 0) {
        perror("[-]Error Listening");
        exit(EXIT_FAILURE);
    }
    
    else {printf("Listening......\n");}                // Listening to or waiting for the client request for the video
    
    printf("PLease enter the name of the file you want to send with extension: ");  // This is the file to be sent to the client when the request arrives.
    scanf("%s", filename);                                                          
    
     printf("Listening......\n");
 

    
    while (1) {                                                  
        new_socket = accept(server_id, (struct sockaddr *)&address, (socklen_t*)&addrlen);    //Socket is created 
        printf("Connected to Client: %s over Port: 8080\n", inet_ntoa(address.sin_addr));  //This message is printed when the client sends a request and is succesfully connected
        
        
        FILE* fp = fopen(filename, "rb");            // Since it is an video file, the file mode is 'read byte'. fopen returns a pointer of the file if succesfully opened
        if (fp == NULL) {                             
            perror("[-]Error in reading file\n");
            exit(EXIT_FAILURE);
        }

        file_send(fp, new_socket);                   // Send file function
        printf("File has been sent successfully to the client: %s\n\n", inet_ntoa(address.sin_addr));
        close(new_socket);                           // Socket closed by the sender once file has been succesfully sent
        
    }

    close(server_id);                                //Server closed once communication takes place.

    return 0;
}
