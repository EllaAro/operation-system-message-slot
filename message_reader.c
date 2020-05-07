#include "message_slot.h"    

#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <assert.h>
#define EINVAL 22
#define EWOULDBLOCK     EAGAIN
int main(int argc, char** argv){
   assert(argc==3);
   char ret_read[BUF_SIZE];
   int file_desc;
   int ret_val;
   char* DEVICE_FILE_NAME;
   int channel_id;
  // int len_read;
   DEVICE_FILE_NAME = argv[1];
   channel_id = atoi(argv[2]);
   memset(&ret_read, 0, sizeof(ret_read));
   file_desc = open( DEVICE_FILE_NAME, O_RDWR );
   if( file_desc < 0 ) 
   {
    printf ("Can't open device file: %s\n",DEVICE_FILE_NAME);
    exit(-1);
   }
   if( channel_id < 0 || channel_id>3 ) 
   {
    printf ("Wrong channel ID: %d\n",channel_id);
    exit(-1);
   }
  ret_val = ioctl( file_desc, MSG_SLOT_CHANNEL, channel_id);
  if(ret_val==-1 || errno==EINVAL){
	printf("icotl failed \n");
    exit(-1);
  } 
  ret_val = read( file_desc, ret_read, BUF_SIZE);
  if(ret_val== -EWOULDBLOCK || ret_val==-1){
	printf("read failed \n");
    exit(-1);
  } 
  close(file_desc); 
 // len_read=strlen(ret_read);
  printf("%d bytes read from %s  and what we read is: %s\n",ret_val,DEVICE_FILE_NAME,ret_read);
  return SUCCESS; }


