/*Configuration Parameters*/

//the id of node
#define THIS_NODE_NUM 19

//the number of total nodes in the same environment(same channel)
#define TOTAL_NODE_NUM 20

//one node send siganl "ITERATION_PER_NODE" times
#define ITERATION_PER_NODE 3 //including Token packet

#define SIGNAL_INTERVAL 1/128

//#define PACKET_BUFFER_SIZE 4*(TOTAL_NODE_NUM-1)*ITERATION_PER_NODE+2*ITERATION_PER_NODE+4
#define PACKET_BUFFER_SIZE 4*(TOTAL_NODE_NUM)+4

//if there's no incomming packet within this time, it can be considered deadlock.
#define IGNORE_TIME ITERATION_PER_NODE*SIGNAL_INTERVAL*TOTAL_NODE_NUM/4