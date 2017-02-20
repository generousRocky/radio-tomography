
#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
//PROCESS(sender_broadcast_process, "Broadcast sender");
//PROCESS(receiver_broadcast_process, "Broadcast receiver");


AUTOSTART_PROCESSES(&example_broadcast_process);
//AUTOSTART_PROCESSES(&sender_broadcast_process);
//AUTOSTART_PROCESSES(&receiver_broadcast_process);


/*---------------------------------------------------------------------------*/





/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{

  signed short RSSI =0;
  RSSI = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);

  printf("%d\n\r", RSSI );

  //process_post_synch(&example_broadcast_process, PROCESS_EVENT_CONTINUE, NULL);

/*
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());

*/

}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;
  static short i=0;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  clock_wait(CLOCK_SECOND*4);

  leds_set(LEDS_RED);
  leds_on(LEDS_RED);
/*
  packetbuf_copyfrom("R", 2);
  broadcast_send(&broadcast);

  */

  while(1) {

    //PROCESS_WAIT_EVENT();
    
    packetbuf_copyfrom("R", 2);
    broadcast_send(&broadcast);


    etimer_set(&et, CLOCK_SECOND/128); // 0.5 sec?
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    printf("broadcast message sent - %d\n\r" , i++);


  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
