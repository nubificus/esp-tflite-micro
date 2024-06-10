#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PORT 1234
#define BUFFER_SIZE 640

void send_file_chunk(int socket, FILE *file) {
    char buffer[BUFFER_SIZE];
    size_t n;
    size_t total;

start:
    total = 0;
    n = fread(buffer, 1, BUFFER_SIZE, file);
    total += n;
    if (n > 0) {
	if (n < BUFFER_SIZE) {
	    rewind(file);
	    size_t idx = n;
	    n = fread(&buffer[idx], 1, BUFFER_SIZE - idx, file);
	    total += n;
    	}

    	if (n > 0) {
            if (send(socket, buffer, n, 0) < 0)
                perror("send");
        }
    }
    else {
	rewind(file);
        goto start;
    }
}

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *file_path = argv[1];
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        char request_buffer[1];
        while (read(new_socket, request_buffer, 1) > 0) {
            send_file_chunk(new_socket, file);
        }

        close(new_socket);
        rewind(file); 
    }

    fclose(file);
    return 0;
}

