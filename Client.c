#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>                                                // Additional functionality just to calculate the round trip time

#define PORT 8080
#define SIZE 32

void write_in_file(int sock_id) {                                // Function that reads the data from the socket and writes it in a new file
    int val;                                                     // This variable is use to check for end of file
    FILE* fp;
    const char* filename = "received_video.mp4";                 // Name of the new file that will be created
    char buffer[SIZE];

    fp = fopen(filename, "wb");                                  // New file created and opened in write byte mode
    while (1) {
        int val = read(sock_id, buffer, SIZE);                   // Data is being read from socket with id socket_id. The function read returns data if read correctly
        if (val < 0) {                                           // A non integer value indicates data read properly from the socket
            perror("Error occured while reading the socket\n"); 
        }
        else if (val == 0) {                                     // Used to ckeck for end of file
            break;
        }

        fwrite(buffer, 1, val, fp);                              // Data writen into the file whose pointer is stored in fp
    }

    return;
}

int main(int argc, char const *argv[]) {                        // This is the main client side socket code. It sends a request to the server and establishes a connection 

    clock_t start;

    int new_sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[SIZE] = {0};
    
    new_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (new_sock < 0) {
        perror("Socket creation error");
        return -1;
    }
    
    start = clock();                                            // Start variable stores the time when the client has sent a request to the server

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid IP address\n");
        return -1;
    }

    if ( connect(new_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) {
        printf("[-]Connection Failed");
        return -1;
    }

    write_in_file(new_sock);                                       // After connecting with the server, the data received from the server needs to be written into a new file
    printf("File has been received from the server.\n");
    printf("Closing The Connection...\n");
    close(new_sock);                                               // Socket is closed by the reciever or the client
    printf("Time taken = %f seconds\n\n", (float)(clock() -start)/CLOCKS_PER_SEC);           // Printing of the round trip time

    return 0;
}
