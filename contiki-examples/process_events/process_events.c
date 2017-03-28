/**
 * \file
 *        An example showing events work in Contiki.
 * \author
 *         mds
 */
#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"
#include "net/rime/rime.h"
#include "leds.h"
#include "net/rime/broadcast.h"


/* This is the structure of broadcast messages. */
struct broadcast_message {
  uint8_t seqno;
};

/* This is the structure of unicast ping messages. */
struct unicast_message {
  uint8_t type;
};

/* These are the types of unicast messages that we can send. */
enum {
  UNICAST_TYPE_PING,
  UNICAST_TYPE_PONG
};

/* This structure holds information about neighbors. */
struct neighbor {
  /* The ->next pointer is needed since we are placing these on a
     Contiki list. */
  struct neighbor *next;

  /* The ->addr field holds the Rime address of the neighbor. */
  linkaddr_t addr;

  /* The ->last_rssi and ->last_lqi fields hold the Received Signal
     Strength Indicator (RSSI) and CC2420 Link Quality Indicator (LQI)
     values that are received for the incoming broadcast packets. */
  uint16_t last_rssi, last_lqi;

  /* Each broadcast packet contains a sequence number (seqno). The
     ->last_seqno field holds the last sequenuce number we saw from
     this neighbor. */
  uint8_t last_seqno;

  /* The ->avg_gap contains the average seqno gap that we have seen
     from this neighbor. */
  uint32_t avg_seqno_gap;

};

/* This #define defines the maximum amount of neighbors we can remember. */
#define MAX_NEIGHBORS 16

/* This MEMB() definition defines a memory pool from which we allocate
   neighbor entries. */
MEMB(neighbors_memb, struct neighbor, MAX_NEIGHBORS);

/* The neighbors_list is a Contiki list that holds the neighbors we
   have seen thus far. */
LIST(neighbors_list);

/* These hold the broadcast and unicast structures, respectively. */
static struct broadcast_conn broadcast;
static struct unicast_conn unicast;

/* These two defines are used for computing the moving average for the
   broadcast sequence number gaps. */
#define SEQNO_EWMA_UNITY 0x100
#define SEQNO_EWMA_ALPHA 0x040








static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  struct neighbor *n;
  struct broadcast_message *m;
  uint8_t seqno_gap;


  /* The packetbuf_dataptr() returns a pointer to the first data byte
     in the received packet. */
  m = packetbuf_dataptr();

  /* Check if we already know this neighbor. */
  for(n = list_head(neighbors_list); n != NULL; n = list_item_next(n)) {

    /* We break out of the loop if the address of the neighbor matches
       the address of the neighbor from which we received this
       broadcast message. */
    if(linkaddr_cmp(&n->addr, from)) {
      break;
    }
  }

  /* If n is NULL, this neighbor was not found in our list, and we
     allocate a new struct neighbor from the neighbors_memb memory
     pool. */
  if(n == NULL) {
    n = memb_alloc(&neighbors_memb);

    /* If we could not allocate a new neighbor entry, we give up. We
       could have reused an old neighbor entry, but we do not do this
       for now. */
    if(n == NULL) {
      return;
    }

    /* Initialize the fields. */
    linkaddr_copy(&n->addr, from);
    n->last_seqno = m->seqno - 1;
    n->avg_seqno_gap = SEQNO_EWMA_UNITY;

    /* Place the neighbor on the neighbor list. */
    list_add(neighbors_list, n);
  }

  /* We can now fill in the fields in our neighbor entry. */
  n->last_rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
  n->last_lqi = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);

  /* Compute the average sequence number gap we have seen from this neighbor. */
  seqno_gap = m->seqno - n->last_seqno;
  n->avg_seqno_gap = (((uint32_t)seqno_gap * SEQNO_EWMA_UNITY) *
                      SEQNO_EWMA_ALPHA) / SEQNO_EWMA_UNITY +
                      ((uint32_t)n->avg_seqno_gap * (SEQNO_EWMA_UNITY -
                                                     SEQNO_EWMA_ALPHA)) /
    SEQNO_EWMA_UNITY;

  /* Remember last seqno we heard. */
  n->last_seqno = m->seqno;

  /* Print out a message. */
  printf("broadcast message received from %d.%d with seqno %d, RSSI %u, LQI %u, avg seqno gap %d.%02d\n",
         from->u8[0], from->u8[1],
         m->seqno,
         packetbuf_attr(PACKETBUF_ATTR_RSSI),
         packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY),
         (int)(n->avg_seqno_gap / SEQNO_EWMA_UNITY),
         (int)(((100UL * n->avg_seqno_gap) / SEQNO_EWMA_UNITY) % 100));

  if( m->seqno%2 == 1){

    leds_off(LEDS_RED);
    leds_set(LEDS_BLUE);
    leds_on(LEDS_BLUE);

  }
  else{

    leds_off(LEDS_BLUE);
    leds_set(LEDS_RED);
    leds_on(LEDS_RED);


  }
}





/* This is where we define what function to be called when a broadcast
   is received. We pass a pointer to this structure in the
   broadcast_open() call below. */
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;







/*---------------------------------------------------------------------------*/
/* This function is called for every incoming unicast packet. */
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  struct unicast_message *msg;

  /* Grab the pointer to the incoming data. */
  msg = packetbuf_dataptr();

  /* We have two message types, UNICAST_TYPE_PING and
     UNICAST_TYPE_PONG. If we receive a UNICAST_TYPE_PING message, we
     print out a message and return a UNICAST_TYPE_PONG. */
  if(msg->type == UNICAST_TYPE_PING) {
    printf("unicast ping received from %d.%d\n",
           from->u8[0], from->u8[1]);
    msg->type = UNICAST_TYPE_PONG;
    packetbuf_copyfrom(msg, sizeof(struct unicast_message));
    /* Send it back to where it came from. */
    unicast_send(c, from);
  }
}













/*---------------------------------------------------------------------------*/
PROCESS(give_process, "give process");
PROCESS(take_process, "take process");
AUTOSTART_PROCESSES(&take_process, &give_process);
/*---------------------------------------------------------------------------*/

//Take process - waits for message.
PROCESS_THREAD(take_process, ev, data) {

	static struct etimer et;
 	static uint8_t seqno;
  	struct broadcast_message msg;

  	PROCESS_EXITHANDLER(broadcast_close(&broadcast));

   	PROCESS_BEGIN();

	printf("take process\n\r");
 
   	while(1) {

		//Wait for any event to occur
		PROCESS_WAIT_EVENT();
    	printf("Take event number:%d msg %s\n\r", ev, (char *) data);
   
	}
 
   	PROCESS_END();
}

static unsigned char msg_cnt = 0;		//message count

//Give process - sends message to take process (a to z).
PROCESS_THREAD(give_process, ev, data) {

	
	PROCESS_BEGIN();

	char msg;		//message to be sent

	printf("give process\n\r");
	
   	while(1) {

		msg = (char)('a' + msg_cnt);	//create message to be sent

		printf("Give event: %c\n\r", msg);

		//give event and message		
		process_post_synch(&take_process, PROCESS_EVENT_CONTINUE, &msg);
		
		clock_wait(CLOCK_SECOND);			//Delay for 1 second
		msg_cnt = (msg_cnt + 1)%26;			//Create next message - only send a to z
	}

	PROCESS_END();
}
