
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
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-udp-packet.h"

#ifdef WITH_COMPOWER
#include "powertrace.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define UDP_EXAMPLE_ID 190

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#include "simple-udp.h"
#include "servreg-hack.h"

#include "net/rpl/rpl.h"

#include "constsspo2.h"

#ifndef PERIOD
#define PERIOD 60
#endif

#define START_INTERVAL		(15 * CLOCK_SECOND)
#define SEND_INTERVAL		(CLOCK_SECOND)
#define SEND_TIME		(random_rand() % (SEND_INTERVAL))
#define MAX_PAYLOAD_LEN		30

#define tamanho_janela 10

#define UDP_PORT 1234
#define SERVICE_ID 190

static struct simple_udp_connection broadcast_connection;

static struct uip_udp_conn *client_conn;
static uip_ipaddr_t server_ipaddr;

static int leituras[tamanho_janela];
static int ultima_leitura = 0;
static int emergencia = 0;

static int leitura_spo2 = -1;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client process");
//PROCESS(unicast_receiver_process, "Unicast receiver example process");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static int EWS(void){
    static int dado;

    dado = leituras[ultima_leitura];

    if(dado <= 91){
      return 3;
    }

    if((92 <= dado) && (dado <= 93)){
      return 2;
    }

    if((94 <= dado) && (dado <= 95)){
      return 1;
    }

    //dado >= 96
    return 0;
}
/*---------------------------------------------------------------------------*/
static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

  //char buf[MAX_PAYLOAD_LEN];
  char *str;
  str = data;
  str[uip_datalen()] = '\0';

  if(!strcmp(str,"EMERGENCIA")){
    static int leitura_mais_recente;

    leitura_mais_recente = leituras[ultima_leitura];

    int ews = EWS();

    // PRINTF("EMERGÊNCIA! ENVIANDO EWS %d da LEITURA MAIS RECENTE: %d\n", ews, leitura_spo2);
    mensagem *m = (mensagem *)uip_appdata;
    strcpy(m->label,"ews");
    m->valor = ews;
    uip_udp_packet_sendto(client_conn, m, sizeof(mensagem),
          &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));
  }

  else{
    static int ultima_leitura_fusor;
    ultima_leitura_fusor = atoi(str);
    // PRINTF(" ======= %s\n",str);
    if(ultima_leitura_fusor > leitura_spo2){
      leitura_spo2 = ultima_leitura_fusor;
    }
  }

}
/*---------------------------------------------------------------------------*/
// static void
// tcpip_handler(void)
// {
  // char *str;
  //
  // if(uip_newdata()) {
  //   str = uip_appdata;
  //   str[uip_datalen()] = '\0';
  //   printf("DATA recv '%s'\n", str);
  // }
// }
/*---------------------------------------------------------------------------*/
static void insere(int e){
    leituras[ultima_leitura] = e;
    ultima_leitura = ultima_leitura + 1;

    if(ultima_leitura == tamanho_janela){
        static int j;
        for(j = 1; j < tamanho_janela; j++){
            leituras[j - 1] = leituras[j];
        }
        ultima_leitura = tamanho_janela - 1;
    }
}
/*---------------------------------------------------------------------------*/
static int sqrt(int number){
    //PROBLEMAS DE PRECISAO!!!!
    if(number < 0){
      // PRINTF("Problemas de precisão\n");
      return 0;
    }

    int temp = 0;
    int s = number / 2;
    int tentativas = 0;

    while(s != temp){
        // PRINTF("%d, %d\n",s,temp);
        //PROBLEMAS DE CONVERGENCIA!!!!
        if(tentativas == 10){
          // PRINTF("Problemas de convergência\n");
          // s = 0;
          break;
        }
        temp = s;
        s = (number/temp + temp) / 2;
        tentativas++;
    }

    return s;
}
/*---------------------------------------------------------------------------*/
static float powerto(float number){
    return number*number;
}
/*---------------------------------------------------------------------------*/
// static int sorteia(int num){
//     int numero = rand() % num;
//     if(numero < 0){
//         numero = (-1) * numero;
//     }
//     return numero;
// }
/*---------------------------------------------------------------------------*/
// static int coleta(){
//     //heart rate
//     //return 132 - sorteia(93);
//
//
//
//     return spo2[leitura_spo2];
// }
/*---------------------------------------------------------------------------*/
static void
send_packet(void *ptr)
{
  //char buf[MAX_PAYLOAD_LEN];

  static int i;

  static int media = 0;
  static unsigned int somatorio = 0;
  static int sd = 0;
  static int novo_elemento;

  media = 0;
  for(i = 0; i < tamanho_janela; i++){
      media = media + leituras[i];
  }

  media = media / i;

  sd = 0;
  somatorio = 0;
  for(i = 0; i < tamanho_janela; i++){
    // PRINTF("%d -> %d\n",(leituras[i] - media), (leituras[i] - media)*(leituras[i] - media));
      somatorio = somatorio + (leituras[i] - media)*(leituras[i] - media);
  }
  // PRINTF ("%d -> %d\n",(somatorio / i), sqrt(somatorio / i));
  sd = sqrt(somatorio / i);

  PRINTF("Media: %d e Desvio Padrao: %d\n", media, sd);
  // if(ultima_leitura == tamanho_janela){
  //   PRINTF("Janela cheia!");
  // }
  leitura_spo2++;
  novo_elemento = spo2[leitura_spo2];

  // PRINTF("Novo elemento: %d\n", novo_elemento);

    if(tamanho_janela - 1 == ultima_leitura){
        // PRINTF("Buffer cheio. Testando anomalia...\n");
        if(( novo_elemento > media +  3*sd ) || (novo_elemento < media -  3*sd)){

          // sprintf(buf, "%d", novo_elemento);
          mensagem *m = (mensagem *)uip_appdata;
          strcpy(m->label,"suspeita");
          m->valor = leitura_spo2;
          emergencia = 1;

          // PRINTF("SUSPEITA DE EMERGÊNCIA! COLETA ENVIADA PARA %d : '%s'\n", server_ipaddr.u8[sizeof(server_ipaddr.u8) - 1], m->label);


          uip_udp_packet_sendto(client_conn, m, sizeof(mensagem),
                                &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));
        }
        else if(emergencia == 1){
          // sprintf(buf, "%d", novo_elemento);
          mensagem *m = (mensagem *)uip_appdata;
          strcpy(m->label,"suspeita");
          m->valor = leitura_spo2;
          emergencia = 0;
          // PRINTF("acusei emergencia por causa do momento anterior");

          // PRINTF("SUSPEITA DE EMERGÊNCIA! COLETA ENVIADA PARA %d : '%s'\n", server_ipaddr.u8[sizeof(server_ipaddr.u8) - 1], m->label);


          uip_udp_packet_sendto(client_conn, m, sizeof(mensagem),
                                &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));
        }
    }


  insere(novo_elemento);

}
/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  //PRINTF("Client IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      //PRINTF("\n");
      /* hack to make address "final" */
      if (state == ADDR_TENTATIVE) {
	uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;

  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

/* The choice of server address determines its 6LoPAN header compression.
 * (Our address will be compressed Mode 3 since it is derived from our link-local address)
 * Obviously the choice made here must also be selected in udp-server.c.
 *
 * For correct Wireshark decoding using a sniffer, add the /64 prefix to the 6LowPAN protocol preferences,
 * e.g. set Context 0 to aaaa::.  At present Wireshark copies Context/128 and then overwrites it.
 * (Setting Context 0 to aaaa::1111:2222:3333:4444 will report a 16 bit compressed address of aaaa::1111:22ff:fe33:xxxx)
 *
 * Note the IPCMV6 checksum verification depends on the correct uncompressed addresses.
 */

#if 0
/* Mode 1 - 64 bits inline */
   uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
#elif 1
/* Mode 2 - 16 bits inline */
  uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0, 0x00ff, 0xfe00, 1);
#else
/* Mode 3 - derived from server link-local (MAC) address */
  uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0x0250, 0xc2ff, 0xfea8, 0xcd1a); //redbee-econotag
#endif
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic;
  static struct ctimer backoff_timer;
#if WITH_COMPOWER
  static int print = 0;
#endif

  PROCESS_BEGIN();

  PROCESS_PAUSE();

  //uip_ipaddr_t *ipaddr;

  //servreg_hack_init();

  set_global_address();

  //servreg_hack_register(SERVICE_ID, ipaddr);

  //PRINTF("UDP client process started\n");

  print_local_addresses();

  /* new connection with remote host */
  client_conn = udp_new(NULL, UIP_HTONS(UDP_SERVER_PORT), NULL);
  if(client_conn == NULL) {
    //PRINTF("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(client_conn, UIP_HTONS(UDP_CLIENT_PORT));

  //PRINTF("Created a connection with the server ");
  //PRINT6ADDR(&client_conn->ripaddr);
  //PRINTF(" local/remote port %u/%u\n", UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));

  simple_udp_register(&broadcast_connection, UDP_PORT,
                      NULL, UDP_PORT, receiver);

#if WITH_COMPOWER
  powertrace_sniff(POWERTRACE_ON);
#endif

  etimer_set(&periodic, SEND_INTERVAL);
  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      // tcpip_handler();
    }

    if(etimer_expired(&periodic)) {

      etimer_reset(&periodic);
      ctimer_set(&backoff_timer, SEND_TIME, send_packet, NULL);

#if WITH_COMPOWER
      if (print == 0) {
	powertrace_print("#P");
      }
      if (++print == 3) {
	print = 0;
      }
#endif

    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
