#include "stdio.h"
#include "ntp-packet.h"
#include "string.h"
#include "winsock2.h"
#include "time.h" 

#define NTP_TIMESTAMP_DELTA 2208988800ull

WSADATA*    startup_socket();

int main(int argc, char* argv[]){
    char* ntp_server = "0.south-america.pool.ntp.org";
    int port = 123;
    

    // ------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------

    // Criando socket
    WSADATA* wsa = startup_socket();

    SOCKET _socket  = socket(PF_INET, SOCK_DGRAM, 0);
    if (_socket < 0){
        perror("Error in socket creation");
        return 1;
    }

    struct hostent* host = gethostbyname(ntp_server);

    struct sockaddr_in sadd;
    sadd.sin_family = AF_INET;
    sadd.sin_port   = htons(port);
 
    bcopy(
        host->h_addr,
        &sadd.sin_addr.s_addr,
        host->h_length
    );

    int result = connect(_socket, (struct sockaddr *)&sadd, sizeof(sadd));
get_time:
    if (result < 0) {
        perror("Error in connect");
        return 1;
    }


    ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    memset(&packet, 0, sizeof(ntp_packet));

    packet.li_vn_mode = 0b00011011;

    int n = send(_socket, (char *) &packet, sizeof (ntp_packet), 0);
    if (n < 0) {
        perror("Error in writing to socket");
        return 1;
    }

    n = recv(_socket, (char *) &packet, sizeof(ntp_packet), 0);
    if (n < 0) {
        perror("Error in reading from socket");
        return 1;
    }

    packet.txTm_f = ntohl(packet.txTm_f);
    packet.txTm_s = ntohl(packet.txTm_s);

    time_t txTm = (time_t) (packet.txTm_s - NTP_TIMESTAMP_DELTA);
    
    printf("\nHora agora: %s\n", ctime(&txTm) );

    Sleep(5 * 60 * 1000);
    goto get_time;

    
    return 0;
}

WSADATA* startup_socket() {
    WSADATA* wsa = (WSADATA*) malloc(sizeof(WSADATA));
    WSAStartup(MAKEWORD(2, 0), wsa);

    return wsa;
}