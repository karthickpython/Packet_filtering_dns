#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h>       
#include<stdlib.h>      
#include<string.h>     

#include<netinet/ip_icmp.h>     
#include<netinet/udp.h> 
#include<netinet/tcp.h> 
#include<netinet/ip.h>  
#include<netinet/if_ether.h>   
#include<net/ethernet.h>        
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

void ProcessPacket(unsigned char* buffer, int size);
void PrintData (unsigned char* , int);

FILE *logfile;
struct sockaddr_in source,dest;
int udp=0, others=0,i,j;

int main()
{
        int saddr_size , data_size;
        struct sockaddr saddr;

        unsigned char *buffer = (unsigned char *) malloc(65536); 


        int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;

        if(sock_raw < 0)
        {
              
                perror("Socket Error");
                return 1;
        }
        while(1)
        {
                saddr_size = sizeof saddr;
                //Receive a packet
                data_size = recvfrom(sock_raw , buffer , 65536 , 0 , &saddr , (socklen_t*)&saddr_size);
                if(data_size <0 )
                {
                        printf("Recvfrom error , failed to get packets\n");
                        return 1;
                }
        
                ProcessPacket(buffer , data_size);
        }
        close(sock_raw);
        printf("Finished");
        return 0;
}

void ProcessPacket(unsigned char* buffer, int size)
{
        struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
        switch (iph->protocol) 
        {
                
                case 17: 
                        ++udp;
                        break;

                default: 
                        ++others;
                        break;
        }
        printf("UDP : %d  others: %d \r", udp , others);
}


