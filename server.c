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
#include <ctype.h>

#define BUFLEN 512 //Max length of buffer

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
int gen3numbers(char net_adr[]);
void get_first_part(char net_addr[], char first_part[]);

int main(int argc, char **argv)
{
    struct sockaddr_in si_me, si_other;
    int sockfd, i, slen = sizeof(si_other), recv_len;
    char buff[100];
    int addr_counter = 0, available_addr;
    char net_addr[100], net_addr2[100];
    int subnet;

    if(argc != 2)
    {
        printf("Usage: %s <portnum>\n", argv[0]);
        die("port_number");
    }

    int port_number = atoi(argv[1]);

    printf("Network address: ");
    scanf("%s", net_addr);
    strcpy(net_addr2, net_addr);
    printf("Subnet number: ");
    scanf("%d", &subnet);

    //create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    // zero out the structure
    memset((char *)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port_number);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if (bind(sockfd, (struct sockaddr *)&si_me, sizeof(si_me)) == -1)
    {
        die("bind");
    }

    //calculate available addresses
    available_addr = (pow(2, 32 - subnet) - 2);
    int last3 = gen3numbers(net_addr);
    char first3_parts[100];

    //declare pointer addresses
    int *addresses = (int*)malloc(available_addr * sizeof(int));

    for(int i = 0; i < available_addr; i++)
    {
        last3++;
        addresses[i] = last3;
    }

    get_first_part(net_addr2, first3_parts);
    printf("\nStarting DHCP server...\n");

    // run clients requests untill out of addresses
    while(addr_counter < available_addr)
    {
        fflush(stdout);

        //-------------------------------------------------------------------------------------------------

        /*
            receive discovery packet from client
        */

        //if error while receiving packet
        if ((recv_len = recvfrom(sockfd, (struct dhcp *)&pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
        printf("\nRecieved Discovery packet from client: \n");
        display1(pkt);

        //-------------------------------------------------------------------------------------------------

        /*
            send offer packet to client
        */

        //sending offer packet to client
        strcpy(pkt.yiadder, first3_parts);
        pkt.yiaddr_last = addresses[addr_counter];
        pkt.lifetime = 3600;
        printf("\nSending offer packet to client: \n");
        display2(pkt);
        if (sendto(sockfd, (struct dhcp *)&pkt, recv_len, 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sendto()");
        }

        //-------------------------------------------------------------------------------------------------

        /*
            receive request packet from client
        */

        //if error while receiving packet
        if ((recv_len = recvfrom(sockfd, (struct dhcp *)&pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
        printf("\nReceived request packet from client: \n");
        display2(pkt);

        //-------------------------------------------------------------------------------------------------

        /*
            send acknowledgment packet to client
        */

        //sending acknowledgment packet
        printf("\nSending acknowledgment packet to client: \n");
        display2(pkt);
        if (sendto(sockfd, (struct dhcp *)&pkt, recv_len, 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sendto()");
        }

        printf("\n----------------------------------------------------------------------\n");

        //-------------------------------------------------------------------------------------------------

        //keep track if maximum number of hosts reached
        if(addr_counter == (available_addr-1))
        {
            strcpy(buff, "Full");
        }
        else
        {
            strcpy(buff, "not_full");
        }
        if (sendto(sockfd, buff, recv_len, 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sendto()");
        }
        addr_counter++;
        bzero(buff, sizeof(buff));


    }
    printf("Available hosts: %d\n", available_addr);
    printf("Ran out of hosts\n");
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


//function to get the last 3 number in the network address
int gen3numbers(char net_adr[])
{
    int n;
    char dot[3] = ".";
    char *token = strtok(net_adr, dot);
    for(int i = 0; i < 3; i++)
    {
        token = strtok(0, dot);
    }
    return atoi(token);
}

//function to get the first 3 parts of the network address
void get_first_part(char net_addr[], char first_part[])
{
    char dot[3] = ".";
    char *token;
    token = strtok(net_addr, dot);
    char second_part[100], third_part[100];
    //parse the first 3 parts in the network address into tokens
    strcpy(first_part, token);
    for (int i = 0; i < 3; i++)
    {
        token = strtok(0, dot);
        if (i == 0)
        {
            strcpy(second_part, token);
        }
        else if (i == 1)
        {
            strcpy(third_part, token);
        }
    }

    //rebuild the address without the last part
    strcat(first_part, ".");
    strcat(first_part, second_part);
    strcat(first_part, ".");
    strcat(first_part, third_part);
    strcat(first_part, ".");
}