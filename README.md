# sac2020
Códigos do Contiki para a simulação do SAC 2020

#### TO-DO LIST (última atualização: 05/09/2019 00:20):

- [x] Montar simulação com nós sensores, 1 nó fusor e 1 nó sink
- [ ] Definir um arquivo de constantes com as constantes pertinentes às mensagens
- [ ] Quando o nó sensor iniciar, deve enviar uma mensagem para o nó fusor para que o nó fusor saiba quantos nós sensores estão na rede
- [x] Se o nó sensor coletar uma medida maior do que 3 desvios padrões das últimas 30 medidas, enviar um ping para o nó fusor
  - [ ] Definir mensagem para isso
- [x] O nó fusor deve enviar broadcast para todos os nós sensores
- [ ] O nó sensor deve responder com o seu respectivo EWS
  - [ ] Incluir dados reais da base do MIMIC usando uma constante em um arquivo .h
- [ ] O nó fusor deve aguardar todos os envios antes de somar os scores e realizar a decisão
  - [ ] E se a quantidade de nós sensores for menor do que o nó fusor tinha calculado? Será necessário uma atualização constante dessa informação no nó sensor?
