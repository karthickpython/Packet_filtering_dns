#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h>	//For standard things
#include<stdlib.h>	//malloc
#include<string.h>	//strlen

#include<netinet/ip_icmp.h>	//Provides declarations for icmp header
#include<netinet/udp.h>	//Provides declarations for udp header
#include<netinet/tcp.h>	//Provides declarations for tcp header
#include<netinet/ip.h>	//Provides declarations for ip header
#include<netinet/if_ether.h>	//For ETH_P_ALL
#include<net/ethernet.h>	//For ether_header
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>



void print_dns_packet(unsigned char*, int);
void ProcessPacket(unsigned char* , int);
void print_ip_header(unsigned char* , int);
void print_tcp_packet(unsigned char * , int );
void print_udp_packet(unsigned char * , int );
void print_icmp_packet(unsigned char* , int );
void PrintData (unsigned char* , int);

FILE *logfile;
struct sockaddr_in source,dest;
int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j;	

int main()
{
	int saddr_size , data_size;
	struct sockaddr saddr;
		
	unsigned char *buffer = (unsigned char *) malloc(65536); //Its Big!
	
	logfile=fopen("log.txt","w");
	if(logfile==NULL) 
	{
		printf("Unable to create log.txt file.");
	}
	printf("Starting...\n");
	
	int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;
	//setsockopt(sock_raw , SOL_SOCKET , SO_BINDTODEVICE , "eth0" , strlen("eth0")+ 1 );
	
	if(sock_raw < 0)
	{
		//Print the error with proper message
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
		//Now process the packet
		ProcessPacket(buffer , data_size);
	}
	close(sock_raw);
	printf("Finished");
	return 0;
}

void ProcessPacket(unsigned char* buffer, int size)
{

       struct ifreq ifr;
       char *if_name = "wlp0s20f3";
       struct sockaddr_in sa;
       struct sockaddr_in sa_d1;
       struct sockaddr_in sa_d2;
       struct sockaddr_in sa_d3;

       char str[INET_ADDRSTRLEN];
       char str1[INET_ADDRSTRLEN];
       char str2[INET_ADDRSTRLEN];
       char str3[INET_ADDRSTRLEN];

       //converting string IP to IP structure
       // store this IP address in sa:
       inet_pton(AF_INET, "192.168.100.11", &(sa.sin_addr));
       inet_ntop(AF_INET, &(sa.sin_addr), str, INET_ADDRSTRLEN);
   

       inet_pton(AF_INET, "192.168.100.52", &(sa_d1.sin_addr));
       inet_ntop(AF_INET, &(sa_d1.sin_addr), str, INET_ADDRSTRLEN);

       inet_pton(AF_INET, "192.168.100.53", &(sa_d2.sin_addr));
       inet_ntop(AF_INET, &(sa_d2.sin_addr), str, INET_ADDRSTRLEN);

       inet_pton(AF_INET, "192.168.100.30", &(sa_d3.sin_addr));
       inet_ntop(AF_INET, &(sa_d3.sin_addr), str, INET_ADDRSTRLEN);

       size_t if_name_len=strlen((const char *)if_name);
       if (if_name_len<sizeof(ifr.ifr_name)) {
           memcpy(ifr.ifr_name,if_name,if_name_len);
           ifr.ifr_name[if_name_len]=0;
        } else {
           printf("interface name is too long");
        }

        int fd=socket(AF_INET,SOCK_DGRAM,0);
        if (fd==-1) {
          printf("%s",strerror(errno));
        }

	if (ioctl(fd,SIOCGIFADDR,&ifr)==-1) {
           int temp_errno=errno;
           close(fd);
           printf("%s",strerror(temp_errno));
          }
         close(fd);

	struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
       // printf("IP address: %s : DNS IP %s \n",inet_ntoa(ipaddr->sin_addr), inet_ntoa(sa.sin_addr));
	//Get the IP Header part of this packet , excluding the ethernet header
	struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
	++total;
	struct sockaddr_in source_ip,dest_ip;
	memset(&source_ip, 0, sizeof(source_ip));
        dest_ip.sin_addr.s_addr = iph->daddr;
       // printf("%s  %s \n", inet_ntoa(dest_ip.sin_addr), inet_ntoa(sa.sin_addr));


	if((inet_ntoa(dest_ip.sin_addr) == inet_ntoa(sa.sin_addr)) || (inet_ntoa(dest_ip.sin_addr) == inet_ntoa(sa_d1.sin_addr)) || (inet_ntoa(dest_ip.sin_addr) == inet_ntoa(sa_d2.sin_addr)) || (inet_ntoa(dest_ip.sin_addr) == inet_ntoa(sa_d3.sin_addr)) ) {
             //   printf("Printing source IP \n");
		print_dns_packet(buffer, size);
		return;
	}
//	int ip = iph->saddr;
//	printf("ip %d \n", ip);
//	printf("traignt ip %s \n", inet_ntoa(source_ip.sin_addr));
//	printf("my ip %s \n", inet_ntoa("192.168.0.11"));
        if((inet_ntoa(source_ip.sin_addr) == str)){
		printf("DNS IP \n");
		return;
	}
	if((inet_ntoa(source_ip.sin_addr) == "192.168.100.11") || (inet_ntoa(source_ip.sin_addr) == "192.168.100.52") || (inet_ntoa(source_ip.sin_addr) == "192.168.100.53") || (inet_ntoa(source_ip.sin_addr) == "192.168.100.30"))
	{
             printf("DNS Query \n");
	     return;
	}
//	switch (iph->protocol) //Check the Protocol and do accordingly...
//	{
//		case 1:  //ICMP Protocol
//			++icmp;
//			print_icmp_packet( buffer , size);
//			break;
//		
//		case 2:  //IGMP Protocol
//			++igmp;
//			break;
//		
//		case 6:  //TCP Protocol
//			++tcp;
//			print_tcp_packet(buffer , size);
//			break;
//		
//		case 17: //UDP Protocol
//			++udp;
//			print_udp_packet(buffer , size);
//			break;
//		
//		default: //Some Other Protocol like ARP etc.
//			++others;
//			break;
//	}
	printf("TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\r", tcp , udp , icmp , igmp , others , total);
}

void print_ethernet_header(unsigned char* Buffer, int Size)
{
	struct ethhdr *eth = (struct ethhdr *)Buffer;
	
	fprintf(logfile , "\n");
	fprintf(logfile , "Ethernet Header\n");
	fprintf(logfile , "   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
	fprintf(logfile , "   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
	fprintf(logfile , "   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
}

void print_ip_header(unsigned char* Buffer, int Size)
{
	print_ethernet_header(Buffer , Size);
  
	unsigned short iphdrlen;
		
	struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr) );
	iphdrlen =iph->ihl*4;
	
	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;
	
	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iph->daddr;
	
	fprintf(logfile , "\n");
	fprintf(logfile , "IP Header\n");
	fprintf(logfile , "   |-IP Version        : %d\n",(unsigned int)iph->version);
	fprintf(logfile , "   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
	fprintf(logfile , "   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
	fprintf(logfile , "   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
	fprintf(logfile , "   |-Identification    : %d\n",ntohs(iph->id));
	//fprintf(logfile , "   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
	//fprintf(logfile , "   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
	//fprintf(logfile , "   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
	fprintf(logfile , "   |-TTL      : %d\n",(unsigned int)iph->ttl);
	fprintf(logfile , "   |-Protocol : %d\n",(unsigned int)iph->protocol);
	fprintf(logfile , "   |-Checksum : %d\n",ntohs(iph->check));
	fprintf(logfile , "   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
	fprintf(logfile , "   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
}

void print_tcp_packet(unsigned char* Buffer, int Size)
{
	unsigned short iphdrlen;
	
	struct iphdr *iph = (struct iphdr *)( Buffer  + sizeof(struct ethhdr) );
	iphdrlen = iph->ihl*4;
	
	struct tcphdr *tcph=(struct tcphdr*)(Buffer + iphdrlen + sizeof(struct ethhdr));
			
	int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;
	
	fprintf(logfile , "\n\n***********************TCP Packet*************************\n");	
		
	print_ip_header(Buffer,Size);
		
	fprintf(logfile , "\n");
	fprintf(logfile , "TCP Header\n");
	fprintf(logfile , "   |-Source Port      : %u\n",ntohs(tcph->source));
	fprintf(logfile , "   |-Destination Port : %u\n",ntohs(tcph->dest));
	fprintf(logfile , "   |-Sequence Number    : %u\n",ntohl(tcph->seq));
	fprintf(logfile , "   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
	fprintf(logfile , "   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
	//fprintf(logfile , "   |-CWR Flag : %d\n",(unsigned int)tcph->cwr);
	//fprintf(logfile , "   |-ECN Flag : %d\n",(unsigned int)tcph->ece);
	fprintf(logfile , "   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
	fprintf(logfile , "   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
	fprintf(logfile , "   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
	fprintf(logfile , "   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
	fprintf(logfile , "   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
	fprintf(logfile , "   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
	fprintf(logfile , "   |-Window         : %d\n",ntohs(tcph->window));
	fprintf(logfile , "   |-Checksum       : %d\n",ntohs(tcph->check));
	fprintf(logfile , "   |-Urgent Pointer : %d\n",tcph->urg_ptr);
	fprintf(logfile , "\n");
	fprintf(logfile , "                        DATA Dump                         ");
	fprintf(logfile , "\n");
		
	fprintf(logfile , "IP Header\n");
	PrintData(Buffer,iphdrlen);
		
	fprintf(logfile , "TCP Header\n");
	PrintData(Buffer+iphdrlen,tcph->doff*4);
		
	fprintf(logfile , "Data Payload\n");	
	PrintData(Buffer + header_size , Size - header_size );
						
	fprintf(logfile , "\n###########################################################");
}


void print_dns_packet(unsigned char *Buffer, int Size){

	unsigned short iphdrlen;
        struct iphdr *iph = (struct iphdr *)(Buffer +  sizeof(struct ethhdr));
        iphdrlen = iph->ihl*4;
        //printf("inside dns packet \n");
        struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen  + sizeof(struct ethhdr));

        int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof udph;
       // printf("INside DNS function \n");
	if(ntohs(udph->dest) == 53){
	       PrintData(Buffer + header_size+ 12 , Size - header_size-12);
	      // printf("\n");
	}

}


void PrintData (unsigned char* data , int Size)
{
	int i , j,s=0;
	char dns_name[100];
	for(i=0 ; i < Size ; i++)
	{
		if( i!=0 && i%16==0)   //if one line of hex printing is complete...
		{
			fprintf(logfile , "         ");
			for(j=i-16 ; j<i ; j++)
			{
				if(data[j]>=32 && data[j]<=128){
					fprintf(logfile , "%c",(unsigned char)data[j]); //if its a number or alphabet
				      //  printf("%c",(unsigned char)data[j]);
					dns_name[s] = (unsigned char) data[j];
					s++;
				}	
				
				else fprintf(logfile , "."); //otherwise print a dot
			}
			fprintf(logfile , "\n");
		} 
		
		if(i%16==0) fprintf(logfile , "   ");
			fprintf(logfile , " %02X",(unsigned int)data[i]);
				
		if( i==Size-1)  //print the last spaces
		{
			for(j=0;j<15-i%16;j++) 
			{
			  fprintf(logfile , "   "); //extra spaces
			}
			
			fprintf(logfile , "         ");
			
			for(j=i-i%16 ; j<=i ; j++)
			{
				if(data[j]>=32 && data[j]<=128) 
				{
				  fprintf(logfile , "%c",(unsigned char)data[j]);
				}
				else 
				{
				  fprintf(logfile , ".");
				}
			}
			
			fprintf(logfile ,  "\n \n \n \n" );

		}
	}
	if((dns_name[0] == 'w') && (dns_name[1] == 'w') && (dns_name[2] == 'w')){
	 printf("%s\n",dns_name);
	}

       fprintf(logfile , "\n");	
}
