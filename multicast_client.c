/* Receiver/client multicast Datagram example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define FILE_NAME_MAX_SIZE  512 
#define BUFFER_SIZE_MAX 1024
struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[BUFFER_SIZE_MAX];
 
int main(int argc, char *argv[])
{
/* Create a datagram socket on which to receive. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
		perror("Opening datagram socket error");
		exit(1);
	}
	else
	printf("Opening datagram socket....OK.\n");
		 
	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */
	
	int reuse = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("Setting SO_REUSEADDR error");
		close(sd);
		exit(1);
	}
	else
		printf("Setting SO_REUSEADDR...OK.\n");
	
	 
	/* Bind to the proper port number with the IP address */
	/* specified as INADDR_ANY. */
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(4321);
	localSock.sin_addr.s_addr = INADDR_ANY;
	if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
	{
		perror("Binding datagram socket error");
		close(sd);
		exit(1);
	}
	else
		printf("Binding datagram socket...OK.\n");
	 
	/* Join the multicast group 226.1.1.1 on the local address*/
	/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
	group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
	/* your ip address */ 
	group.imr_interface.s_addr = inet_addr("192.168.2.196"); 
	/* IP_ADD_MEMBERSHIP:  Joins the multicast group specified */ 
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
	{
		perror("Adding multicast group error");
		close(sd);
		exit(1);
	}
	else
		printf("Adding multicast group...OK.\n");

	/* Read from the socket. */
	datalen = sizeof(databuf);
	if(recvfrom(sd, databuf, datalen, 0, NULL, NULL) < 0) {
		perror("Reading datagram message error");
	}
	else {
		printf("Reading datagram message...OK.\n");
	}
	 
	//get file name
    char file_name[FILE_NAME_MAX_SIZE + 1];  
    bzero(file_name, sizeof(file_name)); 

    bzero(databuf, sizeof(databuf));
    int message_length;
    datalen = sizeof(databuf);
    if((message_length = recvfrom(sd, databuf, datalen, 0, NULL, NULL))) {
    	strncpy(file_name, databuf, strlen(databuf));
        file_name[strlen(databuf)] = '\0';
        
    }

    FILE *fp = fopen(file_name, "w");  
	if (fp==NULL) {  
			printf("ERROR openning file.");  
	}
	bzero(databuf, sizeof(databuf)); 

	int n=0;
	while(1) {  
		datalen = sizeof(databuf);
        n=recvfrom(sd, databuf, datalen, 0, NULL, NULL);

		if(n < 0) {  
		    printf("Recieve Data From Server Failed!\n");  
		    break;  
		}
		//check transfer ened
 		if(strncmp(databuf, "transfer_finish", 15)==0) {
		    break;
		}
		  
		int write_length = fwrite(databuf, sizeof(char), n, fp);  
		if (write_length < n) {  
		    printf("File:\t%s Write Failed!\n", file_name);  
		    break;  
		}  
		bzero(databuf, datalen);
	}

	//get file size
	float recvFileNum;
	fseek(fp, 0, SEEK_END);
	recvFileNum = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("receive file size: %.3fKB\n",recvFileNum/1024.0); 

	close(sd); 
	
	return 0;
}
