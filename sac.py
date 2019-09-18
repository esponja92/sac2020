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
from utils import *

NUM_LEITURAS = 400
from random import randint

def resp(r):
    r = float(r)
    if((r <= 8)or(r >= 25)):
        return 3
    elif((r >= 21)and(r <= 24)):
        return 2
    elif((r >= 9)and(r <= 11)):
        return 1
    elif((r>=12)and(r <= 20)):
        return 1

def spo2(s):
    s = float(s)
    if(s <= 91):
        return 3
    if((s >= 92)and(s <= 93)):
        return 2
    if((s >= 94)and(s <= 95)):
        return 1
    if(s >= 96):
        return 0

def hr(hr):
    hr = float(hr)
    if(hr <= 40):
        return 3
    if((hr >= 41)and(hr <= 50)):
        return 1
    if((hr >= 51)and(hr <= 90)):
        return 0
    if((hr >= 91)and(hr <= 110)):
        return 1
    if((hr >= 111)and(hr <= 130)):
        return 2
    if(hr >= 131):
        return 3

def ews(paciente = getPaciente()):
    EWS = []
    n = randint(0,len(paciente) - NUM_LEITURAS - 1)
    leituras_hr = [int(i) for i in getParametroPaciente(paciente,"'HR'")[n:-1]]
    leituras_resp = [int(i) for i in getParametroPaciente(paciente,"'RESP'")[n:-1]]
    leituras_spo2 = [int(i) for i in getParametroPaciente(paciente,"'SpO2'")[n:-1]]

    for i in range(min(len(leituras_hr),len(leituras_resp),len(leituras_spo2))):
        e = hr(leituras_hr[i])
        e = e + resp(leituras_resp[i])
        e = e + spo2(leituras_spo2[i])
        EWS.append(e)

    # if(7 in EWS):
    #     inicio = max(0, EWS.index(7)-200)
    #     fim = min(len(EWS), inicio+NUM_LEITURAS)
    #     return {'ews':EWS[inicio:fim],'hr':leituras_hr[inicio:fim],'resp':leituras_resp[inicio:fim],'spo2':leituras_spo2[inicio:fim]}
    # if(8 in EWS):
    #     inicio = max(0, EWS.index(8)-200)
    #     fim = min(len(EWS), inicio+NUM_LEITURAS)
    #     return {'ews':EWS[inicio:fim],'hr':leituras_hr[inicio:fim],'resp':leituras_resp[inicio:fim],'spo2':leituras_spo2[inicio:fim]}
    # if(9 in EWS):
    #     inicio = max(0, EWS.index(9)-200)
    #     fim = min(len(EWS), inicio+NUM_LEITURAS)
    #     return {'ews':EWS[inicio:fim],'hr':leituras_hr[inicio:fim],'resp':leituras_resp[inicio:fim],'spo2':leituras_spo2[inicio:fim]}
    # else:
    inicio = n
    fim = min(len(EWS), inicio+NUM_LEITURAS)
    return {'ews':EWS[inicio:fim],'hr':leituras_hr[inicio:fim],'resp':leituras_resp[inicio:fim],'spo2':leituras_spo2[inicio:fim]}

def consumption(ev, c, v, rtimer, runtime):
    return (ev * c * v) / (rtimer * runtime)

def total(tripa):
    resp = 0
    for j in range(4,5):
        resp += int(tripa[j])

    return resp

if __name__ == '__main__':
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
