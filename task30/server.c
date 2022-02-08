#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>

#define BUF_SIZE 256

int main() {
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket(): ");
        return -1;
    }
    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    char *path = "tmp_file";
    strcpy(address.sun_path, path);
    if (bind(socket_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind(): ");
        unlink(path);
        return -1;
    }
    if (listen(socket_fd, 1) < 0) {
        perror("listen(): ");
        unlink(path);
        return -1;
    }
    struct sockaddr_un clientAddress;
    int addrlen = sizeof(clientAddress);
    int client_fd = accept(socket_fd,
                           (struct sockaddr *) &clientAddress, (socklen_t * ) & addrlen);
    if (client_fd < 0) {
        perror("accept(): ");
        unlink(path);
        return -1;
    }
    int size = 0;
    char buffer[BUF_SIZE] = {0};
    while ((size = read(client_fd, buffer, BUF_SIZE)) == -1) {
        if (errno != EINTR) {
            perror("read(): ");
            unlink(path);
            return -1;
        }
    }
    size_t i;
    for (i = 0; i < size; ++i)
        buffer[i] = toupper(buffer[i]);
    printf("%s\n", buffer);
    unlink(path);
    return 0;
}