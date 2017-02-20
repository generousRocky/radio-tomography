
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

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  signed short RSSI =0;
  signed short transmitterNodeNum =0;
  
  char *t = NULL;
  char *receivedPacket;

  receivedPacket = (char *)packetbuf_dataptr();
  RSSI = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);

  if(!strncmp( receivedPacket , "T",1)) {
    //printf("%s\n\r",receivedPacket );
    
    t = strtok(receivedPacket, wSpcae);
    t = strtok(NULL, wSpcae);
    transmitterNodeNum=atoi(t);

    sum_of_rssi = sum_of_rssi + RSSI;
    recentRssi[transmitterNodeNum] = sum_of_rssi/(iterationCount+1);
    sum_of_rssi = 0;
    iterationCount =0;

    if( ((transmitterNodeNum+1)%TOTAL_NODE_NUM) == THIS_NODE_NUM ){
      //printf("my token\r\n");

      clock_wait(CLOCK_SECOND*SIGNAL_INTERVAL);
      process_post_synch(&example_broadcast_process, PROCESS_EVENT_CONTINUE, NULL);
    }
    //else{
    
    //}

  }
  else if(!strncmp( receivedPacket , "N",1)){// it is not a token packet
    //printf("%s ",receivedPacket );

    t = strtok(receivedPacket, wSpcae);
    t = strtok(NULL, wSpcae);
    transmitterNodeNum=atoi(t);

    if(iterationCount<ITERATION_PER_NODE){

      sum_of_rssi = sum_of_rssi + RSSI;
      recentRssi[transmitterNodeNum] = sum_of_rssi/(iterationCount+1);
      iterationCount++;
    }
    else{

      //printf("Packet loss of previous Token Packet: %s\n\r", receivedPacket);

      iterationCount=0;
      sum_of_rssi = sum_of_rssi + RSSI;
      recentRssi[transmitterNodeNum] = sum_of_rssi/(iterationCount+1);
      iterationCount++;
    }

  }
  else{
    printf("Unknown Packet error: %s\n\r", receivedPacket);
  }

}//end broadcast_recv


static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  int iteration;
  int i;
  char rssi[3];
  char buffer[PACKET_BUFFER_SIZE];

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();
  
  broadcast_open(&broadcast, 129, &broadcast_call);

  static struct etimer et;

  etimer_set(&et, CLOCK_SECOND*DEADLOCK_CHECK_INTERVAL);
  while(1) {

    for(iteration=0 ; iteration < ITERATION_PER_NODE-1; iteration++ ){

      sprintf(buffer,"N %d\0",THIS_NODE_NUM);
      // strcat(buffer, "\0");
      packetbuf_copyfrom( buffer, sizeof(buffer));
      broadcast_send(&broadcast);
      clock_wait(CLOCK_SECOND*SIGNAL_INTERVAL);
      //memset( buffer, NULL, sizeof(buffer)-1 );//flush      
    }

    //printf("Send packet\r\n");


    sprintf(buffer, "T %d",THIS_NODE_NUM);
    for(i=0; i< (TOTAL_NODE_NUM); i++ ){

        itoa(recentRssi[i] ,rssi, 10);
        strcat(buffer, " ");
        strcat(buffer, rssi);
        memset( rssi , "", sizeof(rssi)-1 );//flush
    }
    strcat(buffer, "\0");
    packetbuf_copyfrom( buffer, sizeof(buffer));
    broadcast_send(&broadcast);

    //printf("Send Token\r\n");

    //memset( recentRssi, 0, sizeof(recentRssi) );//flush
    memset( recentRssi, 0, sizeof(recentRssi) );//flush



    PROCESS_WAIT_EVENT_UNTIL( ev == PROCESS_EVENT_CONTINUE || etimer_expired(&et) );   
    if(etimer_expired(&et) ==1){
      //Deadlock
      printf("Deadlock and Recover\r\n");

      leds_toggle(LEDS_ALL);
    }
    // clock_wait(CLOCK_SECOND*SIGNAL_INTERVAL);
    etimer_restart(&et);


  }//end while

  PROCESS_END();
}
