/*
Developer: Abdulrahman Alandijani
Email: AbdulramanAlandijani@my.unt.edu
Student ID: 11118717
Due date: 11-25-2020
Program Discription: Simulating 4-way handshake DHCP server-client architecture.
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <math.h>

#define SERVER "127.0.0.1" //IP adress for cse03.cse.unt.edu or 129.120.151.96
#define BUFLEN 512              //Max length of buffer

//this structure represesnt the packet.
struct dhcp
{
    char siadder[100];
    char yiadder[100];
    int yiaddr_last;
    unsigned int trans_ID;
    unsigned short int lifetime;
} pkt; //structure tag

//functions declaration.
void die(char *s);
void display1(struct dhcp pkt);
void display2(struct dhcp pkt);

int main(int argc, char **argv)
{
    struct sockaddr_in si_other;
    int sockfd, i = 0, slen = sizeof(si_other);
    int port_number = atoi(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    //zero out the structure
    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port_number);

    //establish socket connection
    if (inet_aton(SERVER, &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    //-------------------------------------------------------------------------------------------------
    printf("\nEnter start to connect to DHCP server: ");
    char buff[10];
    while(1)
    {
        //-------------------------------------------------------------------------------------------------

        /*
            send discovery packet to server
        */

        scanf("%s", buff);
        srand(time(NULL));
        strcpy(pkt.siadder, SERVER);
        strcpy(pkt.yiadder, "0.0.0.0");
        pkt.trans_ID = rand() % 999 + 1;
        pkt.lifetime = 0;
        //send discover packet
        //Prepare the packet before transmitting.
        printf("\nSending discovery packet to server: \n");
        display1(pkt);
        if(sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sendto()");
        }

        //-------------------------------------------------------------------------------------------------

        /*
            receive offer packet from client
        */

        //if error while receiving packet
        if (recvfrom(sockfd, (struct dhcp *)&pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, &slen) == -1)
        {
            die("recvfrom()");
        }
        printf("\nreceived offer packet from server: \n");
        display2(pkt);

        //-------------------------------------------------------------------------------------------------

        /*
            send request packet to server
        */

        pkt.trans_ID++;
        printf("\nSending request packet to server: \n");
        display2(pkt);
        if (sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sendto()");
        }

        //-------------------------------------------------------------------------------------------------

        /*
            receive acknowledgment packet from server
        */

        //if error while receiving packet
        if (recvfrom(sockfd, (struct dhcp *)&pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, &slen) == -1)
        {
            die("recvfrom()");
        }
        printf("\nreceived acknowledgment packet from server: \n");
        display2(pkt);

        printf("\n----------------------------------------------------------------------\n");

        //-------------------------------------------------------------------------------------------------

        char check[100];
        if (recvfrom(sockfd, check, sizeof(check), 0, (struct sockaddr *)&si_other, &slen) == -1)
        {
            die("recvfrom()");
        }
        if(strcmp(check, "Full") == 0)
        {
            printf("Server has no available hosts.\n");
            break;
        }
        else if(strcmp(check, "not_full") == 0)
        {
            printf("\nEnter start to connect another client: ");
        }
    }
    //close socket and files
    close(sockfd);
    return 0;
}

//function used to catch errors
void die(char *s)
{
    perror(s);
    exit(1);
}

//functions used to display results
void display1(struct dhcp pkt)
{
    //display the reults to the user
    printf("siadder: \t\t\t%s\n", pkt.siadder);
    printf("yiadder: \t\t\t%s\n", pkt.yiadder);
    printf("transaction ID: \t\t%d\n", pkt.trans_ID);
    printf("lifetime: \t\t\t%d\n", pkt.lifetime);
}
void display2(struct dhcp pkt)
{
    //display the reults to the user
    printf("siadder: \t\t\t%s\n", pkt.siadder);
    printf("yiadder: \t\t\t%s%d\n", pkt.yiadder, pkt.yiaddr_last);
    printf("transaction ID: \t\t%d\n", pkt.trans_ID);
    printf("lifetime: \t\t\t%d\n", pkt.lifetime);
}