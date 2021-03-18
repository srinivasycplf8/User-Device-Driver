////////////////////////////////////////////////////////////////////////////////
//
//  File           : lcloud_filesys.c
//  Description    : This is the implementation of the Lion Cloud device 
//                   filesystem interfaces.
//
//   Author        :NAGA SAI SRINIVAS VUTUKURI
//   Last Modified : 3/6/2020
//

// Include files
#include <stdlib.h>
#include <string.h>
#include <cmpsc311_log.h>

// Project include files
#include <lcloud_filesys.h>
#include <lcloud_controller.h>
#include <lcloud_cache.h>
uint64_t b0,b1,c0,c1,c2,d0,d1;

////////////////////////////////////////////////////////////////////////////////
//
// Function     : create_lcloud_registers
// Description  :Create the registers
//
// Inputs       : unsigned 64 bit integers
// Outputs      : it will create the registers if it's succesful

LCloudRegisterFrame create_lcloud_registers(uint64_t b0,uint64_t b1,uint64_t c0,uint64_t c1,uint64_t c2,uint64_t d0,uint64_t d1){
  b0=b0<<60;
  b1=b1<<56;
  c0=c0<<48;
  c1=c1<<40;
  c2=c2<<32;
  d0=d0<<16;


  uint64_t busregister = b0 | b1 | c0 | c1 | c2 | d0 | d1 ;

  return busregister;

}
////////////////////////////////////////////////////////////////////////////////
//
// Function     : extract_lcloud_registers
// Description  : It helps to extract the registers
//
// Inputs       : it takes a data type of LCloudRegisterFrame 
// Outputs      : it will extract the registers if it's succesful

void extract_lcloud_registers(LCloudRegisterFrame rbus) {
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

//It helps to create the unique file handler.

int file_counter;

//this is a file structure it deals with the file properties.



//this is a device structure it deals with device properties
struct device_body{
    int deviceID;
    int device_sector;
    int device_block;
};

struct{
	char filename[256];
	int length;
    int pos;
	int fh;
	int open;
    int LAST_SECTOR;
    int LAST_BLOCK;
    int device_storage_tracker;/////******VERY IMPORTANT//////////
    struct device_body used_info[100000];
}file_body[1025];


struct{
    int original_deviceid;
    int max_blocks;
    int max_sectors;
}device_info[16];

int seek_deviceid,seek_sector,seek_block;

int seek_sector_block(int given_handle,int given_number){
    seek_deviceid=file_body[given_handle].used_info[given_number].deviceID;
    seek_sector=file_body[given_handle].used_info[given_number].device_sector;
    seek_block=file_body[given_handle].used_info[given_number].device_block;

}

int MAIN_DEVICE_TRACKER;
int MAIN_SECTOR_TRACKER;
int MAIN_BLOCK_TRACKER;
int DEVICE_COUNTER;
int update_block;
int update_sector;

int MAIN_TRACKER(){
 
    if (((update_block<device_info[DEVICE_COUNTER].max_blocks-1))&&(update_sector<=device_info[DEVICE_COUNTER].max_sectors-1)){
        update_block++;
    }
    else{
        if (update_sector==device_info[DEVICE_COUNTER].max_sectors-1){
        update_block=0;
        update_sector=0;
        DEVICE_COUNTER=DEVICE_COUNTER+1;
         }
         else{
        update_sector++;
        update_block=0;
         }
    }

}
//INCREMENTING BLOCK AND SECTOR

int given_block,given_sector; //TRACKS THE BLOCKS AND SECTORS WHEN THE POINTER IS NOT SAME AS FILE LENGTH
int SECTORS,BLOCKS;          //TRACKS THE SECTORS AND BLOCKS WHEN THE POINTER AND LENGTH IS AT SAME POSITION
int power_on=0;              //***INITIALIZING MY POWER ON TO ZERO
int DEVICE_ID=0;             //******SETTING MY DEVICE ID
int DEVICE_NUMBER_INIT;
// File system interface implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function     : lcopen
// Description  : Open the file for for reading and writing
//
// Inputs       : path - the path/filename of the file to be read
// Outputs      : file handle if successful test, -1 if failure
//////////////////////////////////////////////////////////////////////////////////

LcFHandle lcopen( const char *path ) {
    
    
    if(power_on==0){
        
        lcloud_initcache(LC_CACHE_MAXBLOCKS);

        
	    LCloudRegisterFrame power_registers=create_lcloud_registers(0,0,LC_POWER_ON,0,0,0,0);
        LCloudRegisterFrame powerbus= client_lcloud_bus_request(power_registers,NULL);
        extract_lcloud_registers(powerbus);
        power_on=1;
    
        LCloudRegisterFrame device_registers=create_lcloud_registers(0,0,LC_DEVPROBE,0,0,0,0);
        LCloudRegisterFrame devicebus= client_lcloud_bus_request(device_registers,NULL);
        extract_lcloud_registers(devicebus);


        //HELPS TO GET THE DEVICE ID
        uint64_t DEVICE_ID=0;
        int arr[16];
        int i=0;

        while (d0!=0){
            if (d0%2==0){
                //;
            }
            else{
                arr[i]=DEVICE_ID;

                i++;
            }
            DEVICE_ID=DEVICE_ID+1;

            d0=d0>>1;
                
        };
        
        for (int u=0;u<i;u++){
            LCloudRegisterFrame deviceid_r=create_lcloud_registers(0,0,LC_DEVINIT,arr[u],0,0,0);
            LCloudRegisterFrame device_id_bus=client_lcloud_bus_request(deviceid_r,NULL);
            extract_lcloud_registers(device_id_bus);
            device_info[u].original_deviceid=arr[u];
            device_info[u].max_sectors=d0;
            device_info[u].max_blocks=d1;
        }

        }
    else{
        power_on=1;
    }

  
    ///IF THE FILE IS ALERADY OPENED RETURN -1
    
    if (2==1){
        return -1;

    }

    //IF IT'S NOT OPENED

    else{

        for (int i=0;i<1025;i++){
            if (file_body[i].fh==0){
            

            if (strcmp(file_body[i].filename,path)==0){  

         
                    file_body[i].open=1;
                    file_body[i].pos=0;
                    return file_body[i].fh;
                
            }
            else{
                if(file_body[i].open==1){
                    return -1;
                }


                strcpy(file_body[i].filename,path);
                file_body[i].open=1;
                file_body[i].fh=i+1;
                file_body[i].pos=0;
                file_body[i].length=0;


                return file_body[i].fh;


            }
            }
            else{
                //return -1;
            }

       }


    }

    return( 0 ); // Likely wrong
} 

////////////////////////////////////////////////////////////////////////////////
//
// Function     : lcread
// Description  : Read data from the file 
//
// Inputs       : fh - file handle for the file to read from
//                buf - place to put the data
//                len - the length of the read
// Outputs      : number of bytes read, -1 if failure


int lcread( LcFHandle fh, char *buf, size_t len ) {
       


    int i=fh-1;
    seek_sector_block(i,file_body[i].pos/256);
    char temp_buff[256],read_buff[256];
    if (file_body[i].open==1){
        if (file_body[i].fh==fh){
               if (file_body[i].pos+len>file_body[i].length){
                   len=file_body[i].length;
               }
               else{
;
               }
                if ((file_body[i].pos%256)+len<=256){

                    char *cache_pointer;
                    cache_pointer=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                    if (cache_pointer==NULL){
                        logMessage(LOG_INFO_LEVEL,"LionCloud Cache ** MISS ** ");
                        LCloudRegisterFrame reading=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_READ,seek_sector,seek_block);
                        client_lcloud_bus_request(reading,read_buff);
                    }
                    else{
                        logMessage(LOG_INFO_LEVEL,"LionCloud Cache **** HIT *****");
                        memcpy(read_buff,cache_pointer,256);
                    }
                    int p=0;
                    logMessage(LOG_INFO_LEVEL,"Started to read the elements in [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                    int temporary=(file_body[i].pos)%256;


                    for (temporary;temporary<file_body[i].pos%256+len;temporary++){

                        buf[p]=read_buff[temporary];
                        p++;
                    }
                    

                    if ((file_body[i].pos%256)+len==256){  

                        
                            file_body[i].length=file_body[i].length;
                            file_body[i].pos=file_body[i].pos+len;
                        
                        return len;
                 
                    }
                    else{
                       
                            file_body[i].length=file_body[i].length;
                            file_body[i].pos=file_body[i].pos+len;
                        
                        return len;

                        }

                    }


                
                else{

                    int change_length;
                    char res_buff[256];
                    char *cache_pointer;
                    cache_pointer=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                    if (cache_pointer==NULL){
                        logMessage(LOG_INFO_LEVEL,"LionCloud Cache **** MISS *****");
                        logMessage(LOG_INFO_LEVEL,"Started to read the elements in [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                        LCloudRegisterFrame reading=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_READ,seek_sector,seek_block);
                        client_lcloud_bus_request(reading,res_buff);
                    }
                    else{
                        logMessage(LOG_INFO_LEVEL,"LionCloud Cache **** HIT *****");
                        logMessage(LOG_INFO_LEVEL,"Started to read the elements in [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                        memcpy(res_buff,cache_pointer,256);
                    }

                    int p=0;
                    int temporary=file_body[i].pos%256;
                    for (temporary;temporary<=255;temporary++){
                        buf[p]=res_buff[temporary];
                        p++;

                    }

                    
                   
                    
                    int buff_leftwith=len-p;
                    int start_from=p;

                    change_length=file_body[i].pos+(256-(file_body[i].pos%256));
                    seek_sector_block(i,(change_length/256));
 
                    if (buff_leftwith==256){
                        char read1buff[256];

                        char *read_pointer;
                        read_pointer=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                        if (read_pointer==NULL){
                            logMessage(LOG_INFO_LEVEL,"LionCloud Cache **** MISS *****");
                        logMessage(LOG_INFO_LEVEL,"Started to read the elements in [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);

                            LCloudRegisterFrame reading1=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_READ,seek_sector,seek_block);
                            client_lcloud_bus_request(reading1,read1buff);
                        }
                        else{
                            logMessage(LOG_INFO_LEVEL,"LionCloud Cache **** HIT *****");
                       logMessage(LOG_INFO_LEVEL,"Started to read the elements in [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                            memcpy(read1buff,read_pointer,256);
                        }

                        for(int a=0;a<256;a++){
                            buf[start_from+a]=read1buff[a];

                        }
                        
                       
                            file_body[i].length=file_body[i].length;
                            file_body[i].pos=file_body[i].pos+len;
                        
                        return len;

                    }

                    if (buff_leftwith<256){

                        char read2buff[256];
                        char *read2_pointer;
                        read2_pointer=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                        if (read2_pointer==NULL){
                            logMessage(LOG_INFO_LEVEL,"LionCloud Cache **** MISS *****");
                           logMessage(LOG_INFO_LEVEL,"Started to read the elements in [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                            LCloudRegisterFrame reading2=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_READ,seek_sector,seek_block);
                            client_lcloud_bus_request(reading2,read2buff);
                        }
                        else{
                            logMessage(LOG_INFO_LEVEL,"LionCloud Cache **** HIT *****");
                        logMessage(LOG_INFO_LEVEL,"Started to read the elements in [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                            memcpy(read2buff,read2_pointer,256);
                        }
                        for (int a=0;a<buff_leftwith;a++){
                            buf[start_from+a]=read2buff[a];

                        }
                        
                            file_body[i].length=file_body[i].length;
                            file_body[i].pos=file_body[i].pos+len;
                        
                      
                       return len;

                    }
                    else{

                        int pos_tracker=0;
                        for(int a=0;a<buff_leftwith/256;a++){
                            char newbuff[256];
                            int j=0;
                            char *pointer_read;
                            pointer_read=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                            if (pointer_read==NULL){
                                logMessage(LOG_INFO_LEVEL,"LionCloud Cache **** MISS *****");
                               logMessage(LOG_INFO_LEVEL,"Started to read the elements in [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                LCloudRegisterFrame readregister=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_READ,seek_sector,seek_block);
                                client_lcloud_bus_request(readregister,newbuff);
                            }
                            else{
                               logMessage(LOG_INFO_LEVEL,"LionCloud Cache **** HIT *****");
                                logMessage(LOG_INFO_LEVEL,"Started to read the elements in [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                memcpy(newbuff,pointer_read,256);
                            }
                            for(int u=256*a;u<=(256*(a+1)-1);u++){
                                buf[start_from+u]=newbuff[j];

                                j++;


                            }
                               
                            change_length=change_length+256;
                            seek_sector_block(i,(change_length/256));


                        }
                        if((len-((buff_leftwith/256)*256+start_from)<256)&&(buff_leftwith%256!=0)){

                            int j=0;
                            char newbuff[256];
                            seek_sector_block(i,(change_length/256));
                            char *new_pointer;
                            new_pointer=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                            if (new_pointer==NULL){
                               logMessage(LOG_INFO_LEVEL,"LionCloud Cache **** MISS *****");
                              logMessage(LOG_INFO_LEVEL,"Started to read the elements in [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                LCloudRegisterFrame readregister=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_READ,seek_sector,seek_block);
                                client_lcloud_bus_request(readregister,newbuff);
                            }
                            else{
                                logMessage(LOG_INFO_LEVEL,"LionCloud Cache **** HIT *****");
                                logMessage(LOG_INFO_LEVEL,"Started to read the elements in [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                memcpy(newbuff,new_pointer,256);
                            }

                            for(int a=0;a<(len-((buff_leftwith/256)*256+start_from));a++){
                                buf[((buff_leftwith/256)*256+start_from)+a]=newbuff[a];

                            }
                            
                            file_body[i].length=file_body[i].length;
                            file_body[i].pos=file_body[i].pos+len;


                          
                            return len;

                        }
                        else{
                            file_body[i].length=file_body[i].length;
                            file_body[i].pos=file_body[i].pos+len;
                            

                            return len;

                            }
                            
                        }


                    }


                    }
                    else{
                        
                        return -1;
                    }


                
            }
            else{
                
                return -1;
            }

            
        
        
      return( 0 );

    }
    



////////////////////////////////////////////////////////////////////////////////
//
// Function     : lcwrite
// Description  : write data to the file
//
// Inputs       : fh - file handle for the file to write to
//                buf - pointer to data to write
//                len - the length of the write
// Outputs      : number of bytes written if successful test, -1 if failure

int lcwrite( LcFHandle fh, char *buf, size_t len ) {
    char temp_buff[256];
    char read_buff[256];
    int i=fh-1;
    if (fh==file_body[i].fh){



        if (file_body[i].open==0){
            return -1;
        }

        else{           
              
                if (file_body[i].pos==file_body[i].length){
                    if (file_body[i].pos==0){
                        if (len<256){

                        ///MY CACHE WRIITNG CONDITION STARTS
                        char *cachewrite1;
                        cachewrite1=lcloud_getcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                        if (cachewrite1==NULL){
                            lcloud_putcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block,buf);
                            logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                            LCloudRegisterFrame less_buffer=create_lcloud_registers(0,0,LC_BLOCK_XFER,device_info[DEVICE_COUNTER].original_deviceid,LC_XFER_WRITE,update_sector,update_block);
                            client_lcloud_bus_request(less_buffer,buf);

                        }
                        else{
                            lcloud_putcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block,buf);
                           logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                            LCloudRegisterFrame less_buffer=create_lcloud_registers(0,0,LC_BLOCK_XFER,device_info[DEVICE_COUNTER].original_deviceid,LC_XFER_WRITE,update_sector,update_block);
                            client_lcloud_bus_request(less_buffer,buf);


                        }
                       
                        
              
                        
                        file_body[i].used_info[file_body[i].device_storage_tracker].deviceID=device_info[DEVICE_COUNTER].original_deviceid;
                        file_body[i].used_info[file_body[i].device_storage_tracker].device_sector=update_sector;
                        file_body[i].used_info[file_body[i].device_storage_tracker].device_block=update_block;
                        MAIN_TRACKER();
                        file_body[i].length=file_body[i].length+len;
                        file_body[i].pos=file_body[i].length;
                        return len;

                        }
                    
                        if (len==256){  

                            char *equal_pointer;
                            equal_pointer=lcloud_getcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                            if (equal_pointer==NULL){
                                lcloud_putcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block,buf);
                               logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                                LCloudRegisterFrame frm=create_lcloud_registers(0,0,LC_BLOCK_XFER,device_info[DEVICE_COUNTER].original_deviceid,LC_XFER_WRITE,update_sector,update_block);
                                client_lcloud_bus_request(frm,buf);

                            }
                            else{
                                lcloud_putcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block,buf);
                              logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                                LCloudRegisterFrame frm=create_lcloud_registers(0,0,LC_BLOCK_XFER,device_info[DEVICE_COUNTER].original_deviceid,LC_XFER_WRITE,update_sector,update_block);
                                client_lcloud_bus_request(frm,buf);

                            }
                            
                            file_body[i].used_info[file_body[i].device_storage_tracker].deviceID=device_info[DEVICE_COUNTER].original_deviceid;
                            file_body[i].used_info[file_body[i].device_storage_tracker].device_sector=update_sector;
                            file_body[i].used_info[file_body[i].device_storage_tracker].device_block=update_block;
                        
                        file_body[i].device_storage_tracker++;
                        MAIN_TRACKER();
                        file_body[i].used_info[file_body[i].device_storage_tracker].deviceID=device_info[DEVICE_COUNTER].original_deviceid;
                        file_body[i].used_info[file_body[i].device_storage_tracker].device_sector=update_sector;
                        file_body[i].used_info[file_body[i].device_storage_tracker].device_block=update_block;
                        MAIN_TRACKER();
                        file_body[i].length=file_body[i].length+len;
                        file_body[i].pos=file_body[i].length;

                        return len;
                        }
                        if (len>256){
                            

                            for(int a=0;a<len/256;a++){
                                        char UPDATE_BUS_ARRAY[256];
                                        int j=0;
                                        for(int i=256*a;i<=(256*(a+1)-1);i++){ 
                                            UPDATE_BUS_ARRAY[j]=buf[i];
                                            j++;
                                        }
                                        char temp_new[256];
                                        memcpy(temp_new,UPDATE_BUS_ARRAY,256);
                                        char *greater_pointer;
                                        greater_pointer=lcloud_getcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                                        if(greater_pointer==NULL){
                                            lcloud_putcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block,temp_new);
                                           logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                                            LCloudRegisterFrame frm=create_lcloud_registers(0,0,LC_BLOCK_XFER,device_info[DEVICE_COUNTER].original_deviceid,LC_XFER_WRITE,update_sector,update_block);
                                            client_lcloud_bus_request(frm,temp_new);

                                        }
                                        else{
                                            lcloud_putcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block,temp_new);
                                            logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                                            LCloudRegisterFrame frm=create_lcloud_registers(0,0,LC_BLOCK_XFER,device_info[DEVICE_COUNTER].original_deviceid,LC_XFER_WRITE,update_sector,update_block);
                                            client_lcloud_bus_request(frm,temp_new);

                                        }
                                    
                                        file_body[i].used_info[file_body[i].device_storage_tracker].deviceID=device_info[DEVICE_COUNTER].original_deviceid;
                                        file_body[i].used_info[file_body[i].device_storage_tracker].device_sector=update_sector;
                                        file_body[i].used_info[file_body[i].device_storage_tracker].device_block=update_block;
                                        file_body[i].device_storage_tracker++;

                                        MAIN_TRACKER();
            
                                        }
                                
                            
                            if((len%256<256)&&(len%256!=0)){
                            char less_buff[256];
                            int left_amount=len%256;
                            for (int b=0;b<left_amount;b++){
                                less_buff[b]=buf[(len-left_amount)+b];
                            }
                            char temp_rbuffer[256];
                            memcpy(temp_rbuffer,less_buff,256);

                            char *pointer1;
                            pointer1=lcloud_getcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                            if (pointer1==NULL){
                                lcloud_putcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block,temp_rbuffer);
                                logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                                LCloudRegisterFrame frm=create_lcloud_registers(0,0,LC_BLOCK_XFER,device_info[DEVICE_COUNTER].original_deviceid,LC_XFER_WRITE,update_sector,update_block);
                                client_lcloud_bus_request(frm,temp_rbuffer);

                            }
                            else{
                                lcloud_putcache(device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block,temp_rbuffer);
                                logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",device_info[DEVICE_COUNTER].original_deviceid,update_sector,update_block);
                                LCloudRegisterFrame frm=create_lcloud_registers(0,0,LC_BLOCK_XFER,device_info[DEVICE_COUNTER].original_deviceid,LC_XFER_WRITE,update_sector,update_block);
                                client_lcloud_bus_request(frm,temp_rbuffer);

                            }

                          
                            file_body[i].used_info[file_body[i].device_storage_tracker].deviceID=device_info[DEVICE_COUNTER].original_deviceid;
                            file_body[i].used_info[file_body[i].device_storage_tracker].device_sector=update_sector;
                            file_body[i].used_info[file_body[i].device_storage_tracker].device_block=update_block;
                            MAIN_TRACKER();

                   
                            file_body[i].length=file_body[i].length+len;
                            file_body[i].pos=file_body[i].length;
                            return len;

                            }
                            else{
                                file_body[i].used_info[file_body[i].device_storage_tracker].deviceID=device_info[DEVICE_COUNTER].original_deviceid;
                                file_body[i].used_info[file_body[i].device_storage_tracker].device_sector=update_sector;
                                file_body[i].used_info[file_body[i].device_storage_tracker].device_block=update_block;
                                file_body[i].length=file_body[i].length+len;
                                file_body[i].pos=file_body[i].length;
                                MAIN_TRACKER();
                                return len;
                            }
        
                        }
                    
                    }
                   if (file_body[i].pos!=0){
                            if ((file_body[i].pos%256)+len<256){
                            int temporary_pointer=file_body[i].pos-(256*(file_body[i].pos/256));
                            char *cachepointer;
                            cachepointer=lcloud_getcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                            if(cachepointer==NULL){
                                LCloudRegisterFrame read_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_READ, file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                client_lcloud_bus_request(read_registers,read_buff);
                                memcpy(&read_buff[file_body[i].pos%256],buf,(len+(file_body[i].pos%256))-(file_body[i].pos%256));
                                lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,read_buff);
                                LCloudRegisterFrame write1_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                client_lcloud_bus_request(write1_registers,read_buff);
                            }
                            else{
                                memcpy(read_buff,cachepointer,256);
                                memcpy(&read_buff[file_body[i].pos%256],buf,(len+(file_body[i].pos%256))-(file_body[i].pos%256));
                               logMessage(LOG_INFO_LEVEL,"Removing found cache item");
                               logMessage(LOG_INFO_LEVEL,"Updating the found cache item");
                                lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,read_buff);
                                LCloudRegisterFrame write1_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                client_lcloud_bus_request(write1_registers,read_buff);
                            }
                            int k=0;
                           
                    
                            
                            file_body[i].length=file_body[i].length+len;
                            file_body[i].pos=file_body[i].length; 
                            return len;
                            }
                          
                            if (((file_body[i].pos%256)+len==256)){
                       
                                int temporary_pointer=file_body[i].pos-(256*(file_body[i].pos/256));
                            char *cachepointer;
                            cachepointer=lcloud_getcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                            if(cachepointer==NULL){
                                LCloudRegisterFrame read_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_READ, file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                client_lcloud_bus_request(read_registers,read_buff);
                                memcpy(&read_buff[file_body[i].pos%256],buf,(len+(file_body[i].pos%256))-(file_body[i].pos%256));
                                lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,read_buff);
                                LCloudRegisterFrame write1_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                client_lcloud_bus_request(write1_registers,read_buff);
                            }
                            else{
                                memcpy(read_buff,cachepointer,256);
                                
                                memcpy(&read_buff[file_body[i].pos%256],buf,(len+(file_body[i].pos%256))-(file_body[i].pos%256));
                             logMessage(LOG_INFO_LEVEL,"Removing found cache item");
                              logMessage(LOG_INFO_LEVEL,"Updating the found cache item");
                                lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,read_buff);
                                LCloudRegisterFrame write1_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                client_lcloud_bus_request(write1_registers,read_buff);
                            }

                                MAIN_TRACKER();
                                file_body[i].device_storage_tracker++;
                                file_body[i].used_info[file_body[i].device_storage_tracker].deviceID=device_info[DEVICE_COUNTER].original_deviceid;
                                file_body[i].used_info[file_body[i].device_storage_tracker].device_sector=update_sector;
                                file_body[i].used_info[file_body[i].device_storage_tracker].device_block=update_block;
                                 MAIN_TRACKER();
                                file_body[i].length=file_body[i].length+len;
                                file_body[i].pos=file_body[i].length; 
                                return len;
                                }
                        else{

                            int temporary_pointer=file_body[i].pos-(256*(file_body[i].pos/256));
                            char *cache_marker;
                            cache_marker=lcloud_getcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                            if (cache_marker==NULL){
                                LCloudRegisterFrame read_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_READ,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                LCloudRegisterFrame rfrm_read=client_lcloud_bus_request(read_registers,read_buff);
                                memcpy(&read_buff[temporary_pointer],buf,256-temporary_pointer);
                                lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,read_buff);
                                logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                LCloudRegisterFrame write_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                client_lcloud_bus_request(write_registers,read_buff);

                            }
                            else{
                                logMessage(LOG_INFO_LEVEL,"Removing found cache item");
                               logMessage(LOG_INFO_LEVEL,"Updating the found cache item");
                                memcpy(read_buff,cache_marker,256);
                                memcpy(&read_buff[temporary_pointer],buf,256-temporary_pointer);
                                lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,read_buff);
                                logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                LCloudRegisterFrame write_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                client_lcloud_bus_request(write_registers,read_buff);
                            }
                            int k=256-temporary_pointer;
                            int buff_leftwith=len-k;
                            int start_from=k;
                                
                                file_body[i].device_storage_tracker++;
                                file_body[i].used_info[file_body[i].device_storage_tracker].deviceID=device_info[DEVICE_COUNTER].original_deviceid;//DOUBT
                                file_body[i].used_info[file_body[i].device_storage_tracker].device_sector=update_sector;///doubt
                                file_body[i].used_info[file_body[i].device_storage_tracker].device_block=update_block;

                                if (buff_leftwith==256){
                                    char new_buff[256];
                                    char *temp_tracker2;
                                    temp_tracker2=lcloud_getcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                    if(temp_tracker2==NULL){
                                        memcpy(new_buff,&buf[start_from],256);
                                        LCloudRegisterFrame write1_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector,file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                        client_lcloud_bus_request(write1_registers,new_buff);
                                        lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,new_buff);
                                        logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);

                                    }
                                    else{
                                        logMessage(LOG_INFO_LEVEL,"Updating the found cache item");
                                        memcpy(new_buff,&buf[start_from],256);
                                        LCloudRegisterFrame write1_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector,file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                        client_lcloud_bus_request(write1_registers,new_buff);
                                        lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,new_buff);
                                        logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);

                                    }
                                    
                                  file_body[i].device_storage_tracker++;
                                    MAIN_TRACKER();
                                    file_body[i].used_info[file_body[i].device_storage_tracker].deviceID=device_info[DEVICE_COUNTER].original_deviceid;//DOUBT
                                    file_body[i].used_info[file_body[i].device_storage_tracker].device_sector=update_sector;///doubt
                                    file_body[i].used_info[file_body[i].device_storage_tracker].device_block=update_block;
                                   
                                    file_body[i].length=file_body[i].length+len;
                                    file_body[i].pos=file_body[i].length;
                                    MAIN_TRACKER();
                                    return len;

                                }
                                if(buff_leftwith<256){
                                    char new_buff[256];
                                    memcpy(new_buff,&buf[start_from],buff_leftwith);
                                    char *new_point;
                                    new_point=lcloud_getcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                    if(new_point==NULL){
                                        lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,new_buff);
                                        logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                        LCloudRegisterFrame write2_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);                                    
                                        client_lcloud_bus_request(write2_registers,new_buff);

                                    }
                                    else{
                                        lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,new_buff);
                                        logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                        LCloudRegisterFrame write2_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);                                    
                                        client_lcloud_bus_request(write2_registers,new_buff);

                                    }
                                    file_body[i].length=file_body[i].length+len;
                                    file_body[i].pos=file_body[i].length;
                                    MAIN_TRACKER();
                                    return len;
                                }
                                else{
                                    for(int a=0;a<buff_leftwith/256;a++){
                                        char UPDATE_BUS_ARRAY[256];
                                        int j=0;
                                        for(int i=256*a;i<=(256*(a+1)-1);i++){
                                            UPDATE_BUS_ARRAY[j]=buf[start_from+i];

                                            j++;
                                        }
                                        char temporay_more_buffer[256];
                                        memcpy(temporay_more_buffer,UPDATE_BUS_ARRAY,256);
                                        char *temp_pointer;
                                        temp_pointer=lcloud_getcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                        if (temp_pointer==NULL){
                                            lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,temporay_more_buffer);
                                            logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                            LCloudRegisterFrame more_buffer=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                            client_lcloud_bus_request(more_buffer,temporay_more_buffer);

                                        }
                                        else{
                                            lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,temporay_more_buffer);
                                            logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                            LCloudRegisterFrame more_buffer=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                            client_lcloud_bus_request(more_buffer,temporay_more_buffer);

                                        }
                                        MAIN_TRACKER();
                                        file_body[i].device_storage_tracker++;
                                        file_body[i].used_info[file_body[i].device_storage_tracker].deviceID=device_info[DEVICE_COUNTER].original_deviceid;//DOUBT
                                        file_body[i].used_info[file_body[i].device_storage_tracker].device_sector=update_sector;///doubt
                                        file_body[i].used_info[file_body[i].device_storage_tracker].device_block=update_block;
                                                   
                                        }
                                    if((buff_leftwith%256<256)&&(buff_leftwith%256!=0)){
                                        int left1=buff_leftwith%256;
                                        int changer= k + 256*(buff_leftwith/256);
                                        char Less_array[256];
                                        for (int j=0;j<(left1);j++){
                                            Less_array[j]=buf[changer+j];
                                        }
                                        char temporay_less_buffer[256];
                                        memcpy(temporay_less_buffer,Less_array,256);
                                        char *last_pointer;
                                        last_pointer=lcloud_getcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                        if (last_pointer==NULL){
                                            lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,temporay_less_buffer);
                                           logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                            LCloudRegisterFrame less_buffer=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                            client_lcloud_bus_request(less_buffer,temporay_less_buffer);

                                        }
                                        else{
                                            lcloud_putcache(file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block,temporay_less_buffer);
                                            logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                            LCloudRegisterFrame less_buffer=create_lcloud_registers(0,0,LC_BLOCK_XFER,file_body[i].used_info[file_body[i].device_storage_tracker].deviceID,LC_XFER_WRITE,file_body[i].used_info[file_body[i].device_storage_tracker].device_sector, file_body[i].used_info[file_body[i].device_storage_tracker].device_block);
                                            client_lcloud_bus_request(less_buffer,temporay_less_buffer);

                                        }
                                  
                                        MAIN_TRACKER();
                                        file_body[i].length=file_body[i].length+len;
                                        file_body[i].pos=file_body[i].length;
                                        return len;
                                    }
                                    else{

                                        file_body[i].length=file_body[i].length+len;
                                        file_body[i].pos=file_body[i].length;
                                        MAIN_TRACKER();

                                        return len;

                                    }    
                                }                     
                        }           
                    }
                }
                
                //OVERWRITE CONDITION STARTS HERE
               else{
                   seek_sector_block(i,file_body[i].pos/256);
                            if((file_body[i].pos%256)+len<=256){
                            char *cache_over;
                            cache_over=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                            if (cache_over==NULL){
                                LCloudRegisterFrame reading_overwrite=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_READ,seek_sector,seek_block);
                                client_lcloud_bus_request(reading_overwrite,read_buff);
                                memcpy(&read_buff[file_body[i].pos%256],buf,len);
                                lcloud_putcache(seek_deviceid,seek_sector,seek_block,read_buff);
                                logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                LCloudRegisterFrame write_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                client_lcloud_bus_request(write_registers,read_buff);

                            }
                            else{
                                logMessage(LOG_INFO_LEVEL,"Removing found cache item");
                                logMessage(LOG_INFO_LEVEL,"Updating the found cache item");
                                memcpy(read_buff,cache_over,256);
                                memcpy(&read_buff[file_body[i].pos%256],buf,len);
                                lcloud_putcache(seek_deviceid,seek_sector,seek_block,read_buff);
                                logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                LCloudRegisterFrame write_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                client_lcloud_bus_request(write_registers,read_buff);

                            }
                            if (((file_body[i].pos%256)+len==256)){
                                if (file_body[i].pos+len>file_body[i].length){
                                        file_body[i].pos=file_body[i].pos+len;
                                        file_body[i].length=file_body[i].pos;
                                }
                                else{
                                    file_body[i].length=file_body[i].length;
                                    file_body[i].pos=file_body[i].pos+len;
                                }
                                return len;
                            }
                            if (((file_body[i].pos%256)+len)<256){
                                if (file_body[i].pos+len>file_body[i].length){
                                        file_body[i].pos=file_body[i].pos+len;
                                        file_body[i].length=file_body[i].pos;
                                }
                                else{
                                    file_body[i].length=file_body[i].length;
                                    file_body[i].pos=file_body[i].pos+len;
                                }                               
                                return len;
                            }
                        

                            }
                            else{
                                int change1_length=0;
                                int temporary_pointer=file_body[i].pos-(256*(file_body[i].pos/256));
                                char *cache_over1;
                                cache_over1=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                                if (cache_over1==NULL){
                                    LCloudRegisterFrame read_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_READ,seek_sector,seek_block);
                                    client_lcloud_bus_request(read_registers,read_buff);
                                     memcpy(&read_buff[temporary_pointer],buf,256-temporary_pointer);
                                     lcloud_putcache(seek_deviceid,seek_sector,seek_block,read_buff);
                                     logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                    LCloudRegisterFrame write_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                    client_lcloud_bus_request(write_registers,read_buff);

                                }
                                else{
                                    logMessage(LOG_INFO_LEVEL,"Removing found cache item");
                                    logMessage(LOG_INFO_LEVEL,"Updating the found cache item");
                                    memcpy(read_buff,cache_over1,256);
                                   memcpy(&read_buff[temporary_pointer],buf,256-temporary_pointer);
                                   lcloud_putcache(seek_deviceid,seek_sector,seek_block,read_buff);
                                    logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                    LCloudRegisterFrame write_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                    client_lcloud_bus_request(write_registers,read_buff);

                                }
                                int k=256-temporary_pointer;
                                int buff_leftwith=len-k;
                                int start_from=k;
                                change1_length=file_body[i].pos+(256-(file_body[i].pos%256));
                                seek_sector_block(i,(change1_length/256));
                                if (buff_leftwith==256){
                                    char new_buff[256];
                                    char *new_pointer;
                                    new_pointer=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                                    if(new_pointer==NULL){
                                        memcpy(new_buff,&buf[start_from],256);
                                        LCloudRegisterFrame write1_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                        lcloud_putcache(seek_deviceid,seek_sector,seek_block,new_buff);
                                       logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                        client_lcloud_bus_request(write1_registers,new_buff);
                                    }
                                    else{
                                        memcpy(new_buff,&buf[start_from],256);
                                        LCloudRegisterFrame write1_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                        lcloud_putcache(seek_deviceid,seek_sector,seek_block,new_buff);
                                        logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                        client_lcloud_bus_request(write1_registers,new_buff);
                                    }
                                    if (file_body[i].pos+len>file_body[i].length){
                                        file_body[i].pos=file_body[i].pos+len;
                                        file_body[i].length=file_body[i].pos;
                                    }
                                    else{
                                        file_body[i].length=file_body[i].length;
                                        file_body[i].pos=file_body[i].pos+len;
                                    } 
                                    return len;

                                }
                                if(buff_leftwith<256){
                                    char read1_buff[256];
                                    char *cache_over3;
                                    cache_over3=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                                    if (cache_over3==NULL){

                                        LCloudRegisterFrame read2_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_READ,seek_sector,seek_block);
                                        client_lcloud_bus_request(read2_registers,read1_buff); 
                                        memcpy(read1_buff,&buf[start_from],buff_leftwith); 
                                        lcloud_putcache(seek_deviceid,seek_sector,seek_block,read1_buff);
                                        logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                        LCloudRegisterFrame write2_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                        client_lcloud_bus_request(write2_registers,read1_buff);  

                                    }
                                    else{
                                        logMessage(LOG_INFO_LEVEL,"Removing found cache item");
                                        logMessage(LOG_INFO_LEVEL,"Updating the found cache item");
                                        memcpy(read1_buff,cache_over3,256);
                                        memcpy(read1_buff,&buf[start_from],buff_leftwith);   
                                        lcloud_putcache(seek_deviceid,seek_sector,seek_block,read1_buff);
                                        logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                        LCloudRegisterFrame write2_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                        client_lcloud_bus_request(write2_registers,read1_buff);
                                    }
                                    if (file_body[i].pos+len>file_body[i].length){
                                        file_body[i].pos=file_body[i].pos+len;
                                        file_body[i].length=file_body[i].pos;
                                    }
                                    else{
                                        file_body[i].length=file_body[i].length;
                                        file_body[i].pos=file_body[i].pos+len;

                                    }
                                    return len;
                                }
                                else{
                                    for(int a=0;a<buff_leftwith/256;a++){

                                        char UPDATE_BUS_ARRAY[256];
                                        int j=0;
                                        for(int i=256*a;i<=(256*(a+1)-1);i++){
                                            UPDATE_BUS_ARRAY[j]=buf[start_from+i];
                                            j++;
                                        }
                                        char temporay_more_buffer[256];
                                        char *more_pointer;
                                        more_pointer=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                                        if(more_pointer==NULL){
                                            memcpy(temporay_more_buffer,UPDATE_BUS_ARRAY,256);
                                            lcloud_putcache(seek_deviceid,seek_sector,seek_block,temporay_more_buffer);
                                           logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                            LCloudRegisterFrame more_buffer=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                            client_lcloud_bus_request(more_buffer,temporay_more_buffer);

                                        }
                                        else{
                                            memcpy(temporay_more_buffer,UPDATE_BUS_ARRAY,256);
                                            lcloud_putcache(seek_deviceid,seek_sector,seek_block,temporay_more_buffer);
                                            logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                            LCloudRegisterFrame more_buffer=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                            client_lcloud_bus_request(more_buffer,temporay_more_buffer);

                                        }
                                        change1_length=change1_length+256;
                                        seek_sector_block(i,(change1_length/256));
            
                                    }
                                    if((len-(buff_leftwith/256)<256)&&(buff_leftwith%256!=0)){
                                        char Less_array[256];
                                        char read3_array[256];
                                        seek_sector_block(i,(change1_length/256));
                                        
                                        char *cache_over4;
                                        cache_over4=lcloud_getcache(seek_deviceid,seek_sector,seek_block);
                                        if(cache_over4==NULL){
                                            LCloudRegisterFrame read3_registers=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_READ,seek_sector,seek_block);
                                            client_lcloud_bus_request(read3_registers,read3_array);
                                            for (int j=0;j<(len-((buff_leftwith/256)*256+start_from));j++){
                                                    read3_array[j]=buf[(len-((buff_leftwith/256)*256+start_from))+j];
                                            }
                                            lcloud_putcache(seek_deviceid,seek_sector,seek_block,read3_array);
                                             logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);

                                            LCloudRegisterFrame less_buffer=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                            client_lcloud_bus_request(less_buffer,read3_array);
                                        }
                                        else{
                                            memcpy(read3_array,cache_over4,256);
                                            for (int j=0;j<(len-((buff_leftwith/256)*256+start_from));j++){
                                                read3_array[j]=buf[(len-((buff_leftwith/256)*256+start_from))+j];
                                             }
                                             lcloud_putcache(seek_deviceid,seek_sector,seek_block,read3_array);
                                            logMessage(LOG_INFO_LEVEL,"LC success writing blkc [%d/%d/%d]",seek_deviceid,seek_sector,seek_block);
                                            LCloudRegisterFrame less_buffer=create_lcloud_registers(0,0,LC_BLOCK_XFER,seek_deviceid,LC_XFER_WRITE,seek_sector,seek_block);
                                            client_lcloud_bus_request(less_buffer,read3_array);
                                        }
                                        if (file_body[i].pos+len>file_body[i].length){
                                            file_body[i].pos=file_body[i].pos+len;
                                            file_body[i].length=file_body[i].pos;
                                        }
                                        else{
                                            file_body[i].pos=file_body[i].pos+len;
                                            file_body[i].length=file_body[i].length;
                                        }
                                        return len;

                                    }
                                    else{
                                        if (file_body[i].pos+len>file_body[i].length){
                                            file_body[i].pos=file_body[i].pos+len;
                                            file_body[i].length=file_body[i].pos;
                                        }
                                        else{
                                            file_body[i].pos=file_body[i].pos+len;
                                            file_body[i].length=file_body[i].length;
                                        }
                                        return len;

                                    }    


                                }


                            }

                    

                    

            }
                    
                                

                

            }
            

        }
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : lcseek
// Description  : Seek to a specific place in the file
//
// Inputs       : fh - the file handle of the file to seek in
//                off - offset within the file to seek to
// Outputs      : 0 if successful test, -1 if failure

int lcseek( LcFHandle fh, size_t off ) {



    int i=fh-1;

    if (file_body[i].fh==fh){

        if (file_body[i].open==1){

                if (off<=file_body[i].length){

                    file_body[i].pos=off;
                    return file_body[i].pos;

                }
                else{
                    return -1;
                }
        }
        else{
            return -1;
        }

    }
    
    else{                       
         
        return -1;
    }

    return( 0 );
}
////////////////////////////////////////////////////////////////////////////////
//
// Function     : lcclose
// Description  : Close the file
//
// Inputs       : fh - the file handle of the file to close
// Outputs      : 0 if successful test, -1 if failure

int lcclose( LcFHandle fh ) {
    int i=fh-1;
    if (file_body[i].fh==fh){
        if(file_body[i].open==1){
            file_body[i].open=0;
            return 0;
        }
        else{
            return -1;
        }
    }
    else{
        return -1;
    }
    
    return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : lcshutdown
// Description  : Shut down the filesystem
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

int lcshutdown( void ) {
    if (power_on==1){
        lcloud_closecache();
        LCloudRegisterFrame shut_registers=create_lcloud_registers(0,0,LC_POWER_OFF,0,0,0,0);
        LCloudRegisterFrame shutbus= client_lcloud_bus_request(shut_registers,NULL);


    }
    else{
        return -1;
    }

    return( 0 );
}


