/*
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

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ip/uip.h"
#include "net/rpl/rpl.h"

#include "net/netstack.h"
#include "dev/button-sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "simple-udp.h"
#include "servreg-hack.h"

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#include "consts.h"

// #include "powertrace.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define UDP_EXAMPLE_ID  190

#define SERVICE_ID 190
#define UDP_PORT 1234

static struct uip_udp_conn *server_conn;
static int total = 0;
#define num_sensores 3

static struct simple_udp_connection connection;

static uip_ipaddr_t *addr;

static int scores[num_sensores];

static int suspeita_em_andamento = 0;

static int leitura = -1;

static int internal_clock = 0;

static int aconteceu_emergencia = 0;
static int aconteceu_emergencia_antes = 0;

static int inicio = 0;

static int periodo = 15;

static int fim = 15;

static int num_suspeitas_emergencia = 0;

static int TP = 0;
static int TN = 0;
static int FP = 0;
static int FN = 0;

PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&udp_server_process);
/*----*/
static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  //printf("Data received on port %d from port %d with length %d\n",         receiver_port, sender_port, datalen);
}
/*---------------------------------------------------------------------------*/
// static int procura_emergencia(){
//
//
//   return resposta;
// }
/*---------------------------------------------------------------------------*/
static void alerta_emergencia(){

    //int indice = (int) UIP_IP_BUF->srcipaddr.u8[sizeof(UIP_IP_BUF->srcipaddr.u8) - 1];


    uip_ipaddr_t addr_bc;

    // PRINTF("Suspeita de emergência detectada! Enviando broadcast\n");
    suspeita_em_andamento = 1;
    num_suspeitas_emergencia++;
    uip_create_linklocal_allnodes_mcast(&addr_bc);
    simple_udp_sendto(&connection, "EMERGENCIA", 10, &addr_bc);

}
/*---------------------------------------------------------------------------*/
static void esvazia_buffer(){
    int i;
    for(i = 0; i < num_sensores; i++){
        scores[i] = -1;
    }
}
/*---------------------------------------------------------------------------*/
static int buffer_cheio(){
    int i;
    for(i = 0; i < num_sensores; i++){
        if(scores[i] == -1){
            return 0;
        }
    }

    return 1;
}
/*---------------------------------------------------------------------------*/
static void calcula_score(mensagem *m){
    int i = 0;
    while(scores[i] != -1){
        //PRINTF(" i = %d\n",i);
        i++;
    }

    if(i < num_sensores){
        scores[i] = m->valor;
        int indice = (int) UIP_IP_BUF->srcipaddr.u8[sizeof(UIP_IP_BUF->srcipaddr.u8) - 1];
        //PRINTF("ARMAZENADO O SCORE ENVIADO PELO SENSOR %d\n",indice);
    }

    //buffer cheio, soma e decide
    if(buffer_cheio()){
        total = 0;
        for(i = 0; i < num_sensores; i++){
            // PRINTF(" scores[%d] = %d\n",i,scores[i]);
            total = total + scores[i];
        }

        PRINTF("Score total acionado no instante t = %d: %d\n", internal_clock, total);
        if(total >= 7){
          aconteceu_emergencia = 1;
        }
        suspeita_em_andamento = 0;
        esvazia_buffer();

        // //faz broadcast com o valor de t
        // uip_ipaddr_t addr_bc;
        // uip_create_linklocal_allnodes_mcast(&addr_bc);
        //
        // char str[3];
        // sprintf(str, "%d", internal_clock);
        // simple_udp_sendto(&connection, str, 3, &addr_bc);
    }
}
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{

// ROTINA PARA ENVIAR PARA O SINK!!!!!
//    addr = servreg_hack_lookup(SERVICE_ID);
/*
    if(0) {
      if(total >= 7){
          static unsigned int message_number;
          char buf[20];

          //printf("Sending unicast to ");
          //uip_debug_ipaddr_print(addr);
          //printf("\n");
          //sprintf(buf, "%d", total);
          //message_number++;

          //simple_udp_sendto(&connection, buf, strlen(buf) + 1, addr);

      }
    } else {
      printf("Service %d not found\n", SERVICE_ID);
    }


#if SERVER_REPLY
    PRINTF("DATA sending reply\n");
    uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
    uip_udp_packet_send(server_conn, "Reply", sizeof("Reply"));
    uip_create_unspecified(&server_conn->ripaddr);
#endif
  }
*/
}
/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  //PRINTF("Server IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(state == ADDR_TENTATIVE || state == ADDR_PREFERRED) {
      //PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      //PRINTF("\n");
      /* hack to make address "final" */
      if (state == ADDR_TENTATIVE) {
	uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  uip_ipaddr_t ipaddr;
  struct uip_ds6_addr *root_if;
  static struct etimer periodic;
  static struct etimer myclock;

  PROCESS_BEGIN();

  PROCESS_PAUSE();

  SENSORS_ACTIVATE(button_sensor);

  //PRINTF("UDP server started\n");
  // printf("Ticks per second: %u\n", RTIMER_SECOND);
  // powertrace_start(CLOCK_SECOND * 60);

  int i;
  for(i = 0; i < num_sensores; i++){
    scores[i] = -1;
  }

#if UIP_CONF_ROUTER
/* The choice of server address determines its 6LoPAN header compression.
 * Obviously the choice made here must also be selected in udp-client.c.
 *
 * For correct Wireshark decoding using a sniffer, add the /64 prefix to the 6LowPAN protocol preferences,
 * e.g. set Context 0 to aaaa::.  At present Wireshark copies Context/128 and then overwrites it.
 * (Setting Context 0 to aaaa::1111:2222:3333:4444 will report a 16 bit compressed address of aaaa::1111:22ff:fe33:xxxx)
 * Note Wireshark's IPCMV6 checksum verification depends on the correct uncompressed addresses.
 */

#if 0
/* Mode 1 - 64 bits inline */
   uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
#elif 1
/* Mode 2 - 16 bits inline */
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0x00ff, 0xfe00, 1);
#else
/* Mode 3 - derived from link local (MAC) address */
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
#endif

  uip_ds6_addr_add(&ipaddr, 0, ADDR_MANUAL);
  root_if = uip_ds6_addr_lookup(&ipaddr);
  if(root_if != NULL) {
    rpl_dag_t *dag;
    dag = rpl_set_root(RPL_DEFAULT_INSTANCE,(uip_ip6addr_t *)&ipaddr);
    uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    rpl_set_prefix(dag, &ipaddr, 64);
    //PRINTF("created a new RPL dag\n");
  } else {
    //PRINTF("failed to create a new RPL DAG\n");
  }
#endif /* UIP_CONF_ROUTER */

  print_local_addresses();

  /* The data sink runs with a 100% duty cycle in order to ensure high
     packet reception rates. */
  NETSTACK_MAC.off(1);

  server_conn = udp_new(NULL, UIP_HTONS(UDP_CLIENT_PORT), NULL);
  if(server_conn == NULL) {
    //PRINTF("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(server_conn, UIP_HTONS(UDP_SERVER_PORT));

  //PRINTF("Created a server connection with remote address ");
  //PRINT6ADDR(&server_conn->ripaddr);
  //PRINTF(" local/remote port %u/%u\n", UIP_HTONS(server_conn->lport),          UIP_HTONS(server_conn->rport));

  simple_udp_register(&connection, UDP_PORT,
                      NULL, UDP_PORT, receiver);

  etimer_set(&periodic, periodo*CLOCK_SECOND);
  etimer_set(&myclock, CLOCK_SECOND);

  static int resposta = 0;
  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      //tcpip_handler();
      if(uip_newdata()) {
        mensagem *m = (struct _mensagem *)uip_appdata;

        if((strcmp(m->label,"suspeita") == 0) && (!suspeita_em_andamento)){
          //PRINTF("REDIRECIONANDO PARA O ALERTA DE EMERGENCIA!\n");
          // if(internal_clock < m->valor){
          //   leitura = m->valor;
          //   internal_clock = leitura;
          //   inicio = internal_clock;
          //   fim = inicio + periodo;
          // }
          // else{
          //
          if(internal_clock != m->valor){
            uip_ipaddr_t addr_bc;
            char str[3];
            sprintf(str, "%d", internal_clock);
            uip_create_linklocal_allnodes_mcast(&addr_bc);
            simple_udp_sendto(&connection, str, 3, &addr_bc);
          }

          alerta_emergencia();
        }

        if(strcmp(m->label,"ews") == 0){
          //PRINTF("REDIRECIONANDO PARA O CALCULO DE SCORE!\n");
          calcula_score(m);
        }
      }

    } else if (ev == sensors_event && data == &button_sensor) {
      //PRINTF("Initiaing global repair\n");
      //rpl_repair_root(RPL_DEFAULT_INSTANCE);
      alerta_emergencia();
    }
    else if(etimer_expired(&myclock)) {
      etimer_reset(&myclock);

      internal_clock = internal_clock + 1;

    }
    else if(etimer_expired(&periodic)) {
      etimer_reset(&periodic);

      PRINTF("PROCURANDO NA JANELA ENTRE %d E %d\n",inicio,fim);
      int k;
      for(k = inicio; k <= fim; k++){
        if(EWS[k] >= 7){
          resposta = 1;
          PRINTF("ACHADO SCORE >= 7 NA LEITURA %d\n",k);
          break;
        }
      }

      inicio = inicio + periodo;
      fim = fim + periodo;

      if(resposta == 1){
        if(aconteceu_emergencia){
          PRINTF("TP\n");
          TP++;
          // aconteceu_emergencia_antes = 1;
        }
        else{
          // if(!aconteceu_emergencia_antes){
            PRINTF("FN\n");
            FN++;
          // }
          // else{
          //   PRINTF("DEU FN MAS FOI DESCONTADO\n");
          // }
        }
      }
      else{
        if(aconteceu_emergencia){
          PRINTF("FP\n");
          FP++;
        }
        else{
            PRINTF("TN\n");
            TN++;
            // aconteceu_emergencia_antes = 0;
        }
      }
      aconteceu_emergencia = 0;
      resposta = 0;
      PRINTF("TP: %d/ TN: %d/ FP: %d/ FN %d\n/ Suspeitas %d\n/",TP,TN,FP,FN,num_suspeitas_emergencia);

      internal_clock = inicio;
      // if(TP + TN + FP + FN == 30){
      if(fim >= 451){
        PRINTF("========================================================================================================================\n");
        PRINTF("========================================================================================================================\n");
        PRINTF("========================================================================================================================\n");
        PRINTF("========================================================================================================================\n");
        PRINTF("========================================================================================================================\n");
        PRINTF("========================================================================================================================\n");
        PRINTF("========================================================================================================================\n");
        PRINTF("========================================================================================================================\n");
        PRINTF("========================================================================================================================\n");
        PRINTF("========================================================================================================================\n");
      }
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
