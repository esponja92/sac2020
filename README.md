# sac2020
Códigos do Contiki para a simulação do SAC 2020

#### TO-DO LIST:

- [x] Montar simulação com nós sensores, 1 nó fusor e 1 nó sink
- [x] Se o nó sensor coletar uma medida maior do que 3 desvios padrões das últimas 30 medidas, enviar um ping para o nó fusor
  - [ ] Definir mensagem para isso
- [x] O nó fusor deve enviar broadcast para todos os nós sensores
- [ ] O nó sensor deve responder com o seu respectivo EWS
  - [ ] Incluir dados reais da base do MIMIC usando uma constante em um arquivo .h
- [ ] O nó fusor deve aguardar todos os envios antes de somar os scores e realizar a decisão (enviar para o nó sink ou não)
