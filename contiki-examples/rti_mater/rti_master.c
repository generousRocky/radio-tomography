
#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//configuration header file
#include "node_config.h"

PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);

static signed short recentRssi[TOTAL_NODE_NUM] = {0};
static unsigned short iterationCount=0;
static int sum_of_rssi = 0;
static char wSpcae[]=" ";
static tempNum=100;
char printBuffer[PACKET_BUFFER_SIZE] = {0};

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  signed short RSSI =0;
  char *receivedPacket;

  receivedPacket = (char *)packetbuf_dataptr();
  RSSI = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);
  char *t = NULL;
  char pBptr;
  int transmitterNodeNum;

  pBptr = strcpy(printBuffer, receivedPacket);

  t = strtok(receivedPacket, wSpcae);
  t = strtok(NULL, wSpcae);
  transmitterNodeNum=atoi(t);

  if(tempNum != transmitterNodeNum){
    printf("%s\r\n", printBuffer);
  }

  tempNum = transmitterNodeNum;

/*

  if(!strncmp( receivedPacket , "T",1)) {
    if(tempNum != transmitterNodeNum)
      printf("%s\r\n", receivedPacket);
  }
  else if(!strncmp( receivedPacket , "N",1)){
    if(tempNum != transmitterNodeNum)
      printf("%s\r\n", receivedPacket);
  }

  tempNum = transmitterNodeNum;

*/

}//end broadcast_recv


static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
static int iteration;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  char rssi[3];
  char buffer[PACKET_BUFFER_SIZE];

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();
  
  broadcast_open(&broadcast, 129, &broadcast_call);

  static struct etimer et;

  etimer_set(&et, CLOCK_SECOND*SCHEDULING_TIME);
  while(1) {

    for(iteration=0 ; iteration < TOTAL_NODE_NUM; iteration++ ){
      //clock_wait(CLOCK_SECOND*SCHEDULING_TIME);
      PROCESS_WAIT_EVENT_UNTIL( etimer_expired(&et));
      

      sprintf(buffer,"M %d\0",iteration);
      //printf("%s\r\n",buffer );
      packetbuf_copyfrom( buffer, sizeof(buffer));
      broadcast_send(&broadcast);

      etimer_restart(&et);   
    }// end iteration
  }//end while

  PROCESS_END();
}