#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define HTTP_PORT 80
#define HTTPS_PORT 443

void grab_banners(char* ip_address) {
    struct sockaddr_in server_address;
    int sockfd, bytes_received;
    char buffer[4096];

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // set server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(HTTP_PORT);
    if (inet_pton(AF_INET, ip_address, &server_address.sin_addr) != 1) {
        perror("inet_pton");
        exit(1);
    }

    // connect to server
    if (connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("connect");
        exit(1);
    }

    // receive HTTP banner
    bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("recv");
        exit(1);
    }
    printf("HTTP banner:\n%s\n", buffer);

    // close socket
    close(sockfd);

    // create SSL context
    SSL_CTX* ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!ssl_ctx) {
        perror("SSL_CTX_new");
        exit(1);
    }

    // create SSL connection
    SSL* ssl = SSL_new(ssl_ctx);
    if (!ssl) {
        perror("SSL_new");
        exit(1);
    }

    // set server name indication (SNI) extension
    if (SSL_set_tlsext_host_name(ssl, ip_address) != 1) {
        perror("SSL_set_tlsext_host_name");
        exit(1);
    }

    // set server address
    server_address.sin_port = htons(HTTPS_PORT);

    // connect to server
    if (connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("connect");
        exit(1);
    }

    // connect SSL connection to socket
    if (SSL_set_fd(ssl, sockfd) != 1) {
        perror("SSL_set_fd");
        exit(1);
    }

    // perform SSL handshake
    if (SSL_connect(ssl) != 1) {
        perror("SSL_connect");
        exit(1);
    }

    // receive HTTPS banner
    bytes_received = SSL_read(ssl, buffer, sizeof(buffer));
    if (bytes_received == -1) {
        perror("SSL_read");
        exit(1);
    }
    printf("HTTPS banner:\n%s\n", buffer);

    // close SSL connection
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ssl_ctx);

    // close socket
    close(sockfd);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s ip_address\n", argv[0]);
        return 1;
    }
    char* ip_address = argv[1];
    grab_banners(ip_address);
    return 0;
}
