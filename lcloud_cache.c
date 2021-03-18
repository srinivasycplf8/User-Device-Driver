////////////////////////////////////////////////////////////////////////////////
//
//  File           : lcloud_cache.c
//  Description    : This is the cache implementation for the LionCloud
//                   assignment for CMPSC311.
//
//   Author        : Naga Sai Srinivas
//   Last Modified : Thu 19 Mar 2020 09:27:55 AM EDT
//

// Includes 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmpsc311_log.h>
#include <lcloud_cache.h>

int time_tracker;
int hit_tracker;
int miss_tracker;
int maximum_blocks;
int cache_bytes;
int total_number_of_accesses;
//
// Functions
struct cache_info{
    int cacheline;
    int device_id;
    int sector;
    int block;
    int accesstime;
    int ch;
    char data[256];

}*cache_pointer;

int cacheline_index;


////////////////////////////////////////////////////////////////////////////////
//
// Function     : lcloud_getcache
// Description  : Search the cache for a block 
//
// Inputs       : did - device number of block to find
//                sec - sector number of block to find
//                blk - block number of block to find
// Outputs      : cache block if found (pointer), NULL if not or failure
char * lcloud_getcache( LcDeviceId did, uint16_t sec, uint16_t blk ) {
    /* Return not found */
    total_number_of_accesses++;
    for (int i=0;i<maximum_blocks;i++){
        if (((cache_pointer+i)->device_id==did)&&((cache_pointer+i)->sector==sec)&&((cache_pointer+i)->block==blk)){
            hit_tracker++;
            logMessage(LOG_INFO_LEVEL,"Getting found cache item !!");
            return (cache_pointer+i)->data;
        }

    }
    logMessage(LOG_INFO_LEVEL,"Getting cache item (not found !!)");

    return( NULL );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : lcloud_putcache
// Description  : Put a value in the cache 
//
// Inputs       : did - device number of block to insert
//                sec - sector number of block to insert
//                blk - block number of block to insert
// Outputs      : 0 if succesfully inserted, -1 if failure


int lcloud_putcache( LcDeviceId did, uint16_t sec, uint16_t blk, char *block ) {
    /* Return successfully */
    int counter_tracker;//SETS THE UPDATE
    counter_tracker=0;
    for (int i=0;i<maximum_blocks;i++){
        if (((cache_pointer+i)->device_id==did)&&((cache_pointer+i)->sector==sec)&&((cache_pointer+i)->block==blk)){
            
            memcpy((cache_pointer+i)->data,block,256);
            (cache_pointer+i)->accesstime=time_tracker;
            time_tracker++;
           logMessage(LOG_INFO_LEVEL,"LionCloud Cache success in inserting and updating the cache item (%d/%d/%d)",did,sec,blk);
            break;

        }
        else{
            counter_tracker++;
        }
    }
    if(counter_tracker==maximum_blocks){

        if ((cache_pointer+(maximum_blocks-1))->ch==0){
            for (int i=0;i<maximum_blocks;i++){
                if ((cache_pointer+i)->ch==0){
                    (cache_pointer+i)->cacheline=i;
                    (cache_pointer+i)->device_id=did;
                    (cache_pointer+i)->sector=sec;
                    (cache_pointer+i)->block=blk;
                    (cache_pointer+i)->accesstime=time_tracker+1;
                    time_tracker++;
                    (cache_pointer+i)->ch=i+1;
                    memcpy((cache_pointer+i)->data,block,256);
                    cache_bytes=cache_bytes+256;
                    logMessage(LOG_INFO_LEVEL,"Cache state [%d items, 256 bytes used]",(cache_pointer+i)->ch);
                    logMessage(LOG_INFO_LEVEL,"LionCloud Cache success inserting cache item (%d/%d/%d)",did,sec,blk); 
                    break;

                }
                
            }


        }//STARTS WHEN ITS FULL
        else{
            //LRU POLICY
            int minimum_element;
            int minimum_tracker=0;
            minimum_element=(cache_pointer+0)->accesstime;
            for (int j=0;j<maximum_blocks;j++){
                if (minimum_element>(cache_pointer+j)->accesstime){
                    minimum_element=(cache_pointer+j)->accesstime;
                    minimum_tracker=j;
                }
            }
            (cache_pointer+minimum_tracker)->accesstime=time_tracker+1;
            time_tracker++;
            (cache_pointer+minimum_tracker)->device_id=did;
            (cache_pointer+minimum_tracker)->sector=sec;
            (cache_pointer+minimum_tracker)->block=blk;
            memcpy(((cache_pointer+minimum_tracker)->data),block,256);
            logMessage(LOG_INFO_LEVEL,"LionCloud Cache success inserting cache item (%d/%d/%d) ,used LRU Policy ",did,sec,blk); 
        }



    }
    else{

        //;
    }
    
    
    return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : lcloud_initcache
// Description  : Initialze the cache by setting up metadata a cache elements.
//
// Inputs       : maxblocks - the max number number of blocks 
// Outputs      : 0 if successful, -1 if failure

int lcloud_initcache( int maxblocks ) {
    /* Return successfully */
    maximum_blocks=maxblocks;
    hit_tracker=0;
    miss_tracker=0;
    cache_bytes=0;
    total_number_of_accesses=0;
    cache_pointer=(struct cache_info*)malloc(sizeof(struct cache_info)*maxblocks);


    return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : lcloud_closecache
// Description  : Clean up the cache when program is closing
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

int lcloud_closecache( void ) {
    /* Return successfully */
    float hit_ratio=(hit_tracker/(float)total_number_of_accesses)*100.00;
    free(cache_pointer);
    logMessage(LOG_INFO_LEVEL," Closed cmpsc311 cache, deleting %d items",maximum_blocks);
    logMessage(LOG_INFO_LEVEL,"Cache Hits        [%d]",hit_tracker);
    logMessage(LOG_INFO_LEVEL,"Cache Misses      [%d]",total_number_of_accesses-hit_tracker);
    logMessage(LOG_INFO_LEVEL,"Cache efficiency  [%d%%] ",(int)((hit_tracker/(float)total_number_of_accesses)*100));
    return( 0 );
}
