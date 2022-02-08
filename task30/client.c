#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define BUF_SIZE 256

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Client need text to send it to server\n");
        return -1;
    }
    struct sockaddr_un address;
    int socket_fd = 0, bytes_written = 0;

    char *msg = argv[1];
    char *path = "tmp_file";

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket(): ");
        return -1;
    }

    memset(&address, 0, sizeof(address));

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, path);

    int addrlen = sizeof(address);
    if (connect(socket_fd, (struct sockaddr *) &address, (socklen_t) addrlen) < 0) {
        perror("connect(): ");
        return -1;
    }

    send(socket_fd, msg, strlen(msg), 0);

    if (close(socket_fd) < 0) {
        perror("close(): ");
        return -1;
    }
    return 0;
}