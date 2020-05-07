#include "message_slot.h"    
#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE
#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/module.h>   /* Specifically, a module */
#include <linux/fs.h>       /* for register_chrdev */
#include <linux/uaccess.h>  /* for get_user and put_user */
#include <linux/string.h>   /* for memset. NOTE - not string.h!*/
#include <linux/list.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

typedef struct _SLOTNODE{
        
  char channels[4][BUF_SIZE+1];
	int minor;
	struct _SLOTNODE* next;
}slotnode;

//the head of the linked list
static slotnode* p_head = NULL;

static int minor_is_not_found(int minor_node){
	slotnode* temp=NULL;
  temp= p_head;
	while(temp!=NULL){
		if (temp->minor==minor_node){
			return -1;
		}
		temp=temp->next;
	}
	return 1;
}

static int create_msg_slot(int minor_node){
	slotnode* slot_node=NULL;
  slot_node= kmalloc(sizeof(slotnode),GFP_KERNEL);
	if(slot_node==NULL){
	return -ENOMEM;
	}
	slot_node->minor=minor_node;
  memset(&(slot_node->channels[0][0]), 0, sizeof(slot_node->channels[0])); //nulify the array.
  memset(&(slot_node->channels[1][0]), 0, sizeof(slot_node->channels[1])); //nulify the array.
  memset(&(slot_node->channels[2][0]), 0, sizeof(slot_node->channels[2])); //nulify the array.
  memset(&(slot_node->channels[3][0]), 0, sizeof(slot_node->channels[3])); //nulify the array.
	if(p_head==NULL){
		p_head=slot_node;
	}
	else{
		slot_node->next=p_head;
		p_head=slot_node;	
	}
return 0;
}

static  slotnode* find_slot_node(int minor_node){
	slotnode* temp=NULL;
  temp= p_head;
	while(temp!=NULL){
		if (temp->minor==minor_node){
			return temp;
		}
		temp=temp->next;
	}
	return NULL;
}

static int device_open(struct inode *inode, struct file *file) {

    int minus;
    int minor_node;
    minor_node = iminor(inode);
    minus=-1;
    file->private_data = (void*)minus;
    if(minor_is_not_found(minor_node)==1){
	  create_msg_slot(minor_node);
    }	
    return SUCCESS;
}

static int device_release( struct inode* inode,
                           struct file*  file)
{
  
  return SUCCESS;
}

static long device_ioctl( struct   file* file,
                          unsigned int   ioctl_command_id,
                          unsigned long  ioctl_param )
{
  int channel;
  channel=(int)ioctl_param;
  //("We're inside device_ioctl: testing the channel %d\n ",channel);
  file->private_data = (void*)ioctl_param;
  if( MSG_SLOT_CHANNEL != ioctl_command_id ){
  return -EINVAL;  
  }
  else if(channel< 0 || channel>3 ){     
  return -EINVAL;
  }
  return SUCCESS;
}


// a process which has already opened
// the device file attempts to read from it
static ssize_t device_read( struct file* file,
                            char __user* buffer,
                            size_t       length,
                            loff_t*      offset )
{
  int minor;
  int channel;
  int i,j;
  slotnode* slot_node;
  minor=iminor(file_inode(file));
  channel = (int)(file->private_data); 
  if(channel == -1){
	  return -EINVAL;
    }
  if(minor_is_not_found(minor)==1){  //let's make sure that there is a node.
     return -EINVAL;
  }
  slot_node=find_slot_node(minor); //let's get the node.
  
 //calculate number of chars in the message channel
  for(j=0; j<BUF_SIZE; j++){
		if(slot_node->channels[channel][j] == 0) //or null?
			break;
		}

  if(j==0){  //lets see if there's a message in the channel.
  return -EWOULDBLOCK;
  }
  if(length<j){ //or maybe the length is too long
    return -EWOULDBLOCK;
  }
  for( i = 0; i < length; ++i )
  {
    put_user(slot_node->channels[channel][i], buffer+i);
  }
  return j;    // return the number of input characters used
}


//---------------------------------------------------------------
// a processs which has already opened
// the device file attempts to write to it
static ssize_t device_write( struct file*       file,
                             const char __user* buffer,
                             size_t             length,
                             loff_t*            offset)
{ 
  int num_written;
  int minor;
  int channel;
  int i;
  slotnode* slot_node=NULL;
  minor = iminor(file_inode(file)); 
  channel = (int)(file->private_data);
  
  if(channel == -1){
    return -EINVAL;
    }
  
  if(length > BUF_SIZE){
    return -EINVAL;
    }
 
  if(minor_is_not_found(minor)==1){  //let's make sure that there is a node.
     return -EINVAL;
  }
  slot_node=find_slot_node(minor); //let's get the node.
  if(slot_node!=NULL){
  memset(&(slot_node->channels[channel][0]), 0, sizeof(slot_node->channels[channel])); //nulify the array.
  for( i = 0; i < length; ++i )
  { 
    get_user(slot_node->channels[channel][i], &buffer[i]);
  } 
}
  num_written=strlen(slot_node->channels[channel]);
  return num_written;    // return the number of input characters used
}

//==================== DEVICE SETUP =============================

// This structure will hold the functions to be called
// when a process does something to the device we created
struct file_operations Fops =
{
  .read           = device_read,
  .write          = device_write,
  .open           = device_open,
  .unlocked_ioctl = device_ioctl,
  .release        = device_release,
};

//---------------------------------------------------------------



// Initialize the module - Register the character device
static int __init simple_init(void)
{
  int rc = -1;
  // Register driver capabilities. Obtain major num
  rc = register_chrdev( MAJOR_NUM, DEVICE_RANGE_NAME, &Fops );

  // Negative values signify an error
  if( rc < 0 )
  {
    printk( KERN_ALERT "%s registraion failed for  %d\n",
                       DEVICE_RANGE_NAME, MAJOR_NUM );
    return rc;
  }    
 return SUCCESS;
}

//---------------------------------------------------------------
static void __exit simple_cleanup(void)
{
  //release sources
   slotnode* tmp;

   while (p_head != NULL)
    {
       tmp = p_head;
       p_head = p_head->next;
       kfree(tmp);
    }
  // Unregister the device
  unregister_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME);
}

//---------------------------------------------------------------
module_init(simple_init);
module_exit(simple_cleanup);
