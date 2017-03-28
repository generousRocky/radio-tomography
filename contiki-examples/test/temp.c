
#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**************************************************************************************/
//Configuration Parameters
#include "node_conf.h"


//must check this 4 definition
#define THIS_NODE_NUM 1
#define THIS_NODE_CODE _1_NODE_CODE //send this msg in packet



#define THIS_NODE_TOKEN _1_NODE_TOKEN //send this msg in packet to notice "your turn"
#define WAITING_THIS_NODE _3_NODE_CODE
#define WAITING_THIS_TOKEN _3_NODE_TOKEN //wait until this msg

//the number of total nodes in the same environment(same channel)
#define TOTAL_NODE_NUM 3

//one node send siganl "ITERATION_PER_NODE" times
#define ITERATION_PER_NODE 3

#define SIGNAL_INTERVAL 1/64

//check main processs is stucked in Deadlock or not
//the interval time is powered with CLOCK_SECOND
//so it is possible kind of 1/16 or 1/32 as well as integer number
#define DEADLOCK_CHECK_INTERVAL ITERATION_PER_NODE*SIGNAL_INTERVAL*TOTAL_NODE_NUM*2
/**************************************************************************************/


PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);

signed short recentRssi[TOTAL_NODE_NUM] = {0};



static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  signed short RSSI =0;
  RSSI = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);

  //(char *)packetbuf_dataptr()[0] == '3'
  //Important!
  //after exam do it this code
  if(!strcmp( (char *)packetbuf_dataptr() , WAITING_THIS_TOKEN)){

    printf("from node 3: %d\n\r", RSSI );
    process_post_synch(&example_broadcast_process, PROCESS_EVENT_CONTINUE, NULL);
  }
  else if(!strcmp( (char *)packetbuf_dataptr() , WAITING_THIS_NODE)){

    printf("from node 3: %d\n\r", RSSI );
  }
  
  else{
    //printf("who's siganl?\n\r");// print TOTAL_NODE_NUM+1 white space and RSSI
    printf("from node 2: %d\n\r", RSSI );

  }

 
}//end broadcast_recv


static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  int iteration;
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();
  


  broadcast_open(&broadcast, 129, &broadcast_call);
  clock_wait(CLOCK_SECOND);

  //process_post_synch(&recovery_process, PROCESS_EVENT_INIT, NULL);

  etimer_set(&et, CLOCK_SECOND*DEADLOCK_CHECK_INTERVAL);
  while(1) {

    for(iteration=0 ; iteration < ITERATION_PER_NODE-1; iteration++ ){

      //change here
      packetbuf_copyfrom( THIS_NODE_CODE, sizeof(THIS_NODE_CODE));
      



      broadcast_send(&broadcast);
      clock_wait(CLOCK_SECOND*SIGNAL_INTERVAL);
    }

    //change here
    packetbuf_copyfrom( THIS_NODE_TOKEN, sizeof(THIS_NODE_TOKEN));
    


    broadcast_send(&broadcast);

    PROCESS_WAIT_EVENT_UNTIL( (ev == PROCESS_EVENT_CONTINUE) || etimer_expired(&et) );
    clock_wait(CLOCK_SECOND*SIGNAL_INTERVAL);
    
    if(etimer_expired(&et) ==1){
      leds_toggle(LEDS_ALL);
    }
    etimer_restart(&et);



  }//end while

  PROCESS_END();
}
