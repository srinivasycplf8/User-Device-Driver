////////////////////////////////////////////////////////////////////////////////
//
//  File          : lcloud_client.c
//  Description   : This is the client side of the Lion Clound network
//                  communication protocol.
//
//  Author        : Naga Sai Srinivas
//  Last Modified : Sat 28 Mar 2020 09:43:05 AM EDT
//

// Include Files
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>

// Project Include Files
#include <lcloud_network.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>
//
// Functions
int socket_handle=1;
uint64_t b0,b1,c0,c1,c2,d0,d1;
void extract_nlcloud_registers(LCloudRegisterFrame rbus) {
   uint64_t temp1,tempc,tempc1,tempc2,tempd,tempd1;
   uint64_t temp_rbus,temp_rbus2,temp_rbus3,temp_rbus4,temp_rbus5,temp_rbus6,temp_rbus7;
   temp_rbus=rbus;
   b0=temp_rbus>>60;
   temp_rbus2=rbus;
   temp1=temp_rbus2<<4;
   b1=temp1>>60;
   temp_rbus3=rbus;
   tempc=temp_rbus3<<8;
   c0=tempc>>56;
   temp_rbus4=rbus;
   tempc1=temp_rbus4<<16;
   c1=tempc1>>56;
   temp_rbus5=rbus;
   tempc2=temp_rbus5<<24;
   c2=tempc2>>56;
   temp_rbus6=rbus;
   tempd=temp_rbus6<<32;
   d0=tempd>>48;
   temp_rbus7=rbus;
   tempd1=rbus<<48;
   d1=tempd1>>48;
}
////////////////////////////////////////////////////////////////////////////////
//
// Function     : client_lcloud_bus_request
// Description  : This the client regstateeration that sends a request to the 
//                lion client server.   It will:
//
//                1) if INIT make a connection to the server
//                2) send any request to the server, returning results
//                3) if CLOSE, will close the connection
//
// Inputs       : reg - the request reqisters for the command
//                buf - the block to be read/written from (READ/WRITE)
// Outputs      : the response structure encoded as needed
int socket_fd;
char *ip=LCLOUD_DEFAULT_IP;
unsigned short port = LCLOUD_DEFAULT_PORT;
struct sockaddr_in caddr;
int poweron;
LCloudRegisterFrame client_lcloud_bus_request( LCloudRegisterFrame reg, void *buf ) {

    if(socket_handle==1){
        

        caddr.sin_family=AF_INET;
        caddr.sin_port=htons(port);
        inet_aton(ip,&caddr.sin_addr);
        socket_fd=socket(PF_INET,SOCK_STREAM,0);
        connect(socket_fd,(const struct sockaddr *)&caddr,sizeof(caddr));
        socket_handle=0;

    }
    else{
        //IT is open conection
    }
    extract_nlcloud_registers(reg);
    
    if ((c0==LC_BLOCK_XFER)&&(c2==LC_XFER_WRITE)){
      //  logMessage(LOG_INFO_LEVEL,"c1 ::: %d",c1);
        uint64_t value;
        value=htonll64(reg);
        uint64_t *pointer;
        pointer=&value;
        write(socket_fd,pointer,sizeof(uint64_t));
        write(socket_fd,buf,LC_DEVICE_BLOCK_SIZE);
        read(socket_fd,pointer,sizeof(uint64_t));
        return ntohll64(*pointer);

   
    }
    if ((c0==LC_BLOCK_XFER)&&(c2==LC_XFER_READ)){
        uint64_t value;
        value=htonll64(reg);
        uint64_t *pointer;
        pointer=&value;
        write(socket_fd,pointer,sizeof(uint64_t));
        read(socket_fd,pointer,sizeof(uint64_t));
        read(socket_fd,buf,LC_DEVICE_BLOCK_SIZE);
        return ntohll64(*pointer);
    }
    if (c0==LC_POWER_OFF){
        uint64_t value;
        value=htonll64(reg);
        uint64_t *pointer;
        pointer=&value;
        write(socket_fd,pointer,sizeof(uint64_t));
        read(socket_fd,pointer,sizeof(uint64_t));
        close(socket_fd);
        socket_handle=1;
        return ntohll64(*pointer);
    }
    else{
         uint64_t value;
        value=htonll64(reg);
         uint64_t *pointer;
        pointer=&value;
        write(socket_fd,pointer,sizeof(uint64_t));
        read(socket_fd,pointer,sizeof(uint64_t));
        return ntohll64(*pointer);
    }


    
    
    
    
    
}

