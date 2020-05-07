# Message Slot
## Description
This is one of the projects that were given to us in the 'Operation System' course in Tel Aviv University.
In this project, a new mechanism for inner-proccess-communication was implemented - Message Slot.
A message slot is a character device file through which processes communicate using multiple message channels. 
Once a message slot device file is opened, a process uses ioctl() to specify the id of the
message channel it wants to use.  
4 channels are supported per one slot. A message slot subsequently uses read()/write() to receive/send messages on the channel. 
## Implementaion
This project was written in `C` language and preformed over the `Linux` operation system.
