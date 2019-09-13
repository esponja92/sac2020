'''
2408 P 193.250 17 11309 578542 0 2574 0 2574 689 32078 0 144 0 144 
(radio 0.43% / 0.43% tx 0.00% / 0.00% listen 0.43% / 0.43%) 

Parameter and value Explanation

clock_time() = 2408     [0]  
rimeaddr = 193, 250     [2]
seqno = 17              [3]  
all_cpu = 11309         [4]  
all_lpm = 578542        [5]  
all_transmit = 0        [6]  
all_listen = 2574       [7]  
all_idle_transmit = 0   [8] 
all_idle_listen = 2574  [9]  
cpu = 689               [10]  
lpm = 32078             [11]  
transmit = 0            [12]  
listen = 144            [13]  
idle_transmit = 0       [14]  
idle_listen = 144       [15]  
'''

import matplotlib.pyplot as plt

def consumption(ev, c, v, rtimer, runtime):
    return (ev * c * v) / (rtimer * runtime)

def total(tripa):
    resp = 0
    for j in range(4,5):
        resp += int(tripa[j])

    return resp

F = open("60_minutes_no_fusor_sac_2020.txt", "r")
dados = F.readlines()

tempos = []
leituras = []
runtime = 60
rtimer = 32768
v = 3
c = 18.8

for i in range(len(dados) - 1):
    tempos.append(i)

    ev = (total(dados[i+1].split(" ")[1::]) - total(dados[i].split(" ")[1::]))

    consumo = consumption(ev, c, v, rtimer, runtime)

    leituras.append(consumo)

plt.plot(tempos, leituras, 'ro-')
plt.show()
