/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Testing the broadcast layer in Rime
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

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
#define THIS_NODE_BATON _1_NODE_BATON //send this msg in packet to notice "your turn"
#define WAITING_THIS_BATON _3_NODE_BATON //wait until this msg




//the number of total nodes in the same environment(same channel)
#define TOTAL_NODE_NUM 3

//one node send siganl "ITERATION_PER_NODE" times
#define ITERATION_PER_NODE 5

#define SIGNAL_INTERVAL 1/64

//check main processs is stucked in Deadlock or not
//the interval time is powered with CLOCK_SECOND
//so it is possible kind of 1/16 or 1/32 as well as integer number
#define DEADLOCK_CHECK_INTERVAL ITERATION_PER_NODE*SIGNAL_INTERVAL*TOTAL_NODE_NUM*2
/**************************************************************************************/


PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);



static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  signed short RSSI =0;
  RSSI = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);

  if(!strcmp( (char *)packetbuf_dataptr() , WAITING_THIS_BATON)){

    switch(THIS_NODE_NUM){

      case 1: printf("%11d\n\r", RSSI ); break;
      case 2: printf("%3d\n\r", RSSI ); break;
      case 3: printf("%7d\n\r", RSSI ); break;
    }
    process_post_synch(&example_broadcast_process, PROCESS_EVENT_CONTINUE, NULL);
  }
  else if( !strcmp( (char *)packetbuf_dataptr() , _1_NODE_CODE) || !strcmp( (char *)packetbuf_dataptr() , _1_NODE_BATON) ){
    printf("%3d\n\r", RSSI);
  }
  else if( !strcmp( (char *)packetbuf_dataptr() , _2_NODE_CODE) || !strcmp( (char *)packetbuf_dataptr() , _2_NODE_BATON) ){
    printf("%7d\n\r", RSSI);
  }
  else if( !strcmp( (char *)packetbuf_dataptr() , _3_NODE_CODE) || !strcmp( (char *)packetbuf_dataptr() , _3_NODE_BATON) ){
    printf("%11d\n\r", RSSI);
  }
  else{
    printf("who's siganl?\n\r");// print TOTAL_NODE_NUM+1 white space and RSSI
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
  leds_toggle(LEDS_ALL);


  broadcast_open(&broadcast, 129, &broadcast_call);
  clock_wait(CLOCK_SECOND);

  //process_post_synch(&recovery_process, PROCESS_EVENT_INIT, NULL);

  etimer_set(&et, CLOCK_SECOND*DEADLOCK_CHECK_INTERVAL);
  while(1) {

    for(iteration=0 ; iteration < ITERATION_PER_NODE-1; iteration++ ){

      packetbuf_copyfrom( THIS_NODE_CODE, sizeof(THIS_NODE_CODE));
      broadcast_send(&broadcast);
      clock_wait(CLOCK_SECOND*SIGNAL_INTERVAL);
    }
    packetbuf_copyfrom( THIS_NODE_BATON, sizeof(THIS_NODE_BATON));
    broadcast_send(&broadcast);

    PROCESS_WAIT_EVENT_UNTIL( (ev == PROCESS_EVENT_CONTINUE) || etimer_expired(&et) );
    //PROCESS_WAIT_EVENT_UNTIL( (ev == PROCESS_EVENT_CONTINUE) );
    clock_wait(CLOCK_SECOND*SIGNAL_INTERVAL);
    
    if(etimer_expired(&et) ==1){
      leds_toggle(LEDS_ALL);
    }
    
    
    etimer_restart(&et);

  }//end while

  PROCESS_END();
}
