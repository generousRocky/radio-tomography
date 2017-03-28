/*Configuration Parameters*/

//the id of node
#define THIS_NODE_NUM 19

//the number of total nodes in the same network(same radio channel)
#define TOTAL_NODE_NUM 20

//one node send siganl "ITERATION_PER_NODE" times
#define ITERATION_PER_NODE 3

#define SIGNAL_INTERVAL 1/128 // * second

#define PACKET_BUFFER_SIZE 4*(TOTAL_NODE_NUM)+4