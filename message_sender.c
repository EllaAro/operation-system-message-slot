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
int main(int argc, char** argv){
   assert(argc==4);
   int file_desc;
   int ret_val;
   char* DEVICE_FILE_NAME;
   int channel_id;
   char* msg_to_send;
   int msg_to_send_len;
  //int len_msg_to_send;
   DEVICE_FILE_NAME = argv[1];
   channel_id = atoi(argv[2]);
   msg_to_send = argv[3];
   file_desc = open( DEVICE_FILE_NAME, O_RDWR );
   if( file_desc < 0 ) 
   {
    printf ("Can't open device file: %s\n", 
            DEVICE_FILE_NAME);
    exit(-1);
   }
   channel_id = atoi(argv[2]);
   if( channel_id < 0 || channel_id>3 ) 
   {
    printf ("Wrong channel ID: %d\n",channel_id);
    exit(-1);
   }
  ret_val = ioctl( file_desc, MSG_SLOT_CHANNEL, channel_id);
  if(ret_val==-1 || errno==EINVAL){
  //printf("return_val:%d \n errno:%d\n",ret_val,errno);
	printf("icotl failed \n");
    exit(-1);
  }
  //len_msg_to_send=strlen(msg_to_send); 
  msg_to_send_len=strlen(msg_to_send);
  ret_val = write( file_desc, msg_to_send,msg_to_send_len );

  //printf("fd: %d \n",file_desc);
  //printf("return_val:%d \n ",ret_val);
  if(ret_val==-1){
	printf("write failed \n");
    exit(-1);
  }
  if(ret_val==EINVAL){
  printf("write failed, Message too long \n");
    exit(-1);
  }
  close(file_desc); 
  printf("%d bytes written to %s \n",ret_val,DEVICE_FILE_NAME);
  return SUCCESS;
 }
