import csv
import matplotlib.pyplot as plt
from scipy.stats import kendalltau as kt
from random import randint

PACIENTE_1 = './DB-MIMIC-II/a40466/samples.csv'
PACIENTE_2 = './DB-MIMIC-II/a44466n/samples.csv'
PACIENTE_3 = './DB-MIMIC-II/mimic221n/samples.csv'
PACIENTE_3_ANNOTATION = './DB-MIMIC-II/mimic221n/annotation.txt'
PACIENTE_3_ANOMALO = './anomalo.csv'

PACIENTE_4 = './DB-MIMIC-II/mimic054n/samples.csv'
PACIENTE_4_ANNOTATION = './DB-MIMIC-II/mimic054n/annotation.txt'

PACIENTE_5 = './DB-MIMIC-II/mimic466n/samples.csv'
PACIENTE_5_ANNOTATION = './DB-MIMIC-II/mimic466n/annotation.txt'

#para resp
PACIENTE_5_ANOMALIAS = [[0,60],[1300,1400],[1500,1560]]

#para HR:
#PACIENTE_5_ANOMALIAS = [[0, 60], [92, 152], [1854, 1914]]

#FATIA_PARAMETRO_INICIAL = randint(0,18999)
FATIA_PARAMETRO_INICIAL = randint(0,1900)

def plotsimples(x,y,titulo='About as simple as it gets, folks',labelx='time (s)',labely='voltage (mV)'):
    fig, ax = plt.subplots()
    ax.plot(x, y)
    ax.legend(['Kendal-tau='+str(kt(x,y)[0])])

    ax.set(xlabel=labelx, ylabel=labely, title=titulo)
    ax.grid()

    plt.show()

def csvParaLista(arquivo_csv):
    with open(arquivo_csv, 'rb') as f:
        reader = csv.reader(f)
        your_list = list(reader)

    return your_list

def colunaParaLista(matriz, coluna_j):
    lista = []

    for linha_i in range(len(matriz)):
        try:
            elemento = float(matriz[linha_i][coluna_j])
            lista.append(elemento)
        except:
            pass
    
    return lista

def getPaciente():
    return csvParaLista(PACIENTE_5)   

def getParametroPaciente(paciente, parametro):
    parametros = paciente[0]

    coluna_parametro = parametros.index(parametro)
    return colunaParaLista(paciente, coluna_parametro)

def respEWS(resps):
    lista = []
    for resp in resps:
        resp = float(resp)
        if((resp <= 8)or(resp >= 25)):
            lista.append("3")
        elif((resp >= 21)and(resp <= 24)):
            lista.append("2")
        elif((resp >= 9)and(resp <= 11)):
            lista.append("1")
        elif((resp>=12)and(resp <= 20)):
            lista.append("0")
    return lista


def batimentosCardiacosEWS(batimentos):
    lista = []
    for batimento in batimentos:
        batimento = float(batimento)
        if((batimento <= 40)or(batimento >= 131)):
            lista.append("3")
        elif((batimento >= 111)and(batimento <= 130)):
            lista.append("2")
        elif((batimento >= 41)and(batimento <= 50)):
            lista.append("1")
        elif((batimento >= 91)and(batimento <= 110)):
            lista.append("1")
        elif((batimento>=51)and(batimento <= 90)):
            lista.append("0")
    return lista

def spo2EWS(spo2):
    spo2 = float(spo2)
    if(spo2 <= 91):
        return "3"
    elif((spo2 >= 92)and(spo2 <= 93)):
        return "2"
    elif((spo2 >= 94)and(spo2 <= 95)):
        return "1"
    elif(spo2 >= 96):
        return "0"

def tempEWS(temp):
    temp = float(temp)
    if(temp <= 35):
        return "3"
    if(temp >= 39.1):
        return "2"
    elif((temp >= 35.1)and(temp <= 36)):
        return "1"
    elif((temp >= 38.1)and(temp <= 39)):
        return "1"
    elif((temp >= 36.1)and(temp <= 38)):
        return "0"

def getAnomaliasAnotadas():
    lista = []

    with open(PACIENTE_5_ANNOTATION, 'rb') as f:
        reader = f.readlines()
        lista = list(reader)
    
    anomalias = []

    for i in range(1,len(lista)-1): #para excluir o cabecalho e o CR final
        anomalias.append(int(lista[i].split()[2]))

    return anomalias

def getTimestamps():
    paciente = getPaciente()
    return colunaParaLista(paciente, 0)

def acertouAnomalia(inicio,fim):
    for anomalia in PACIENTE_5_ANOMALIAS:
        if((inicio >= anomalia[0]) and (fim <= anomalia[1])):
            return 1
    return 0

def acertouNormalidade(inicio,fim):
    for anomalia in PACIENTE_5_ANOMALIAS:
        if((inicio >= anomalia[0]) and (fim <= anomalia[1])):
            return 0
    return 1

