#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H
#include <linux/ioctl.h>

#define DEVICE_RANGE_NAME "message slot"
#define BUF_SIZE 128
#define SUCCESS 0
#define MAJOR_NUM 244
#define MSG_SLOT_CHANNEL _IOW(MAJOR_NUM, 0, unsigned long)

#endif