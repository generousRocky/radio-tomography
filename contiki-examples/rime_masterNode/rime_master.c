#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node_config.h"

PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);


static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  signed short RSSI =0;
  RSSI = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);

  char *receivedPacket = malloc(TOTAL_NODE_NUM*ITERATION_PER_NODE*4 + 4);///////////////////////
  receivedPacket = (char *)packetbuf_dataptr();

  
  if(!strncmp( receivedPacket , "T",1)) {
    printf("%s\r\n", receivedPacket);
  }
  //else{
    //printf("%s ", receivedPacket);
  //}

  
 
}//end broadcast_recv


static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();

  static struct etimer et;

  leds_toggle(LEDS_ALL);


  broadcast_open(&broadcast, 129, &broadcast_call);
  clock_wait(CLOCK_SECOND);

  //process_post_synch(&recovery_process, PROCESS_EVENT_INIT, NULL);

  while(1) {

    /* Delay 2-4 seconds */
    etimer_set(&et, 1000);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }//end while

  PROCESS_END();
}

  