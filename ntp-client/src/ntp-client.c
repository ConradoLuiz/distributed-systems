#include "stdio.h"
#include "ntp-packet.h"
#include "string.h"
#include "winsock2.h"
#include "time.h" 

#define NTP_TIMESTAMP_DELTA 2208988800ull

SOCKET* create_socket(int type) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 0), &wsa);

    SOCKET* _socket = (SOCKET *) malloc(sizeof(SOCKET));
    *_socket  = socket(PF_INET, type, 0);

    if (*_socket < 0){
        perror("Error in socket creation");
        exit(1);
    }

    return _socket;
}

int connect_socket(SOCKET* _socket, char* server, int port) {
    struct hostent* host = gethostbyname(server);

    struct sockaddr_in sadd = {
        .sin_family         = AF_INET,
        .sin_port           = htons(port),
    };
    bcopy(host->h_addr, &sadd.sin_addr.s_addr, host->h_length);
    
    int result = connect(*_socket, (struct sockaddr *)&sadd, sizeof(sadd));
    
    return result;
}

time_t get_current_time(char* ntp_server, int port) {
    // Creating the ntp packet to send to the server
    ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    memset(&packet, 0, sizeof(ntp_packet));

    packet.li_vn_mode = 0b00011011;

    // Creating and connecting the socket to the ntp server
    SOCKET* _socket = create_socket(SOCK_DGRAM);

    int result = connect_socket(_socket, ntp_server, port);
    if (result < 0) {
        perror("Error in connect");
        return 1;
    }

    // Sending the packet requesting the time from the ntp server
    int n = send(*_socket, (char *) &packet, sizeof (ntp_packet), 0);
    if (n < 0) {
        perror("Error in writing to socket");
        return 1;
    }

    // Waiting to receive the packet with the current time
    n = recv(*_socket, (char *) &packet, sizeof(ntp_packet), 0);
    if (n < 0) {
        perror("Error in reading from socket");
        return 1;
    }

    closesocket(*_socket);
    WSACleanup();

    free(_socket);

    // Converting from net long to host long byte order
    packet.txTm_s = ntohl(packet.txTm_s);

    // Converting to unix timestamp
    time_t timestamp = (time_t) (packet.txTm_s - NTP_TIMESTAMP_DELTA);

    return timestamp;
}

int main(int argc, char* argv[]){
    char*   ntp_server  = "0.south-america.pool.ntp.org";
    int     port        = 123;
    int     minutes     = 5;

    if (argc > 1){
        minutes = atoi(argv[1]);
    }
    else {
        printf("\nPara escolher o intervalo, rodar o programa passando a quantidade de minutos de intervalo para ser utilizado entre as chamadas:\nntp-client.exe 5\n");
    }

    printf("\nIntervalo:\t%d minuto(s)\n", minutes);

    while(1){
        time_t timestamp = get_current_time(ntp_server, port);

        printf("\nHora agora:\t%s\n", ctime(&timestamp));

        Sleep(minutes * 60 * 1000);
    }
    
    
    return 0;
}