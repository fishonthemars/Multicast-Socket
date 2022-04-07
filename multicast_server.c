/* Send Multicast Datagram code example. */
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
struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
char databuf[BUFFER_SIZE_MAX];
int datalen = sizeof(databuf);
 
int main (int argc, char *argv[ ])
{
/* Create a datagram socket on which to send. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
	  perror("Opening datagram socket error");
	  exit(1);
	}
	else
	  printf("Opening the datagram socket...OK.\n");
	 
	/* Initialize the group sockaddr structure with a */
	/* group address of 226.1.1.1 and port 4321. */
	memset((char *) &groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
	groupSock.sin_port = htons(4321);
	 
	/* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/* multicast capable interface. */
	localInterface.s_addr = inet_addr("192.168.2.196");
	
	/* IP_MULTICAST_IF:  Sets the interface over which outgoing multicast datagrams are sent. */
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
	{
	  perror("Setting local interface error");
	  exit(1);
	}
	else
	  printf("Setting the local interface...OK\n");


	/* Send a message to the multicast group specified by the*/
	/* groupSock sockaddr structure. */
	/*int datalen = 1024;*/
	if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
	{
		perror("Sending datagram message error");
	}
	else
	  printf("Sending datagram message...OK\n");

	
	//get filename
	char file_name[FILE_NAME_MAX_SIZE + 1];
	bzero(file_name, sizeof(file_name));

	
	if(strlen(argv[1]) <= FILE_NAME_MAX_SIZE) {
	    strncpy(file_name, argv[1],strlen(argv[1]));
	} else {
	    printf("File name too long");
	}
	
	if(sendto(sd, file_name, sizeof(file_name), 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0) {
		perror("Sending filename error");
	}
	FILE *fp = fopen(file_name, "r");
	if(fp==NULL) {
		printf("Fopen Fail.");
	} else {
		//get file size
		float fileNum; 
		fseek(fp, 0, SEEK_END);
		fileNum = ftell(fp);
		printf("file size: %.3fKB\n", fileNum/1024.0);
		fseek(fp, 0, SEEK_SET);

		bzero(databuf,BUFFER_SIZE_MAX);
		int file_block_length = 0;  
		while( (file_block_length = fread(databuf, sizeof(char), BUFFER_SIZE_MAX, fp)) > 0) {
			// printf("file_block_length = %d\n", file_block_length);  
    		if (sendto(sd, databuf, file_block_length, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0) {  
		        printf("Send File:\t%s Failed!\n", file_name);  
		        break;  
		    } 
		    bzero(databuf, sizeof(databuf));   
		}
		strncpy(databuf, "transfer_finish", 15);
			    	
		for(int i=0;i<20;i++) {
			sendto(sd, databuf, sizeof(databuf), 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
		}
			    	
		fclose(fp);
		 
	} 
	
	close(sd);
	return 0;
}
