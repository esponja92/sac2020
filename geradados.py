import os
import glob
from sac import *

ews1 = geraDadosExperimento(400)
path = '/home/hugo/contiki/examples/ipv6/rpl-udp'

files = glob.glob(path + '/*.z1')
for f in files:
    os.remove(f)

f = open(path + "/consts.h","w")
f.write("struct _mensagem {\n")
f.write("  char label[20];\n")
f.write("  int valor;\n")
f.write("};\n")
f.write("\ntypedef struct _mensagem mensagem;\n")
f.write("\nstatic int EWS[400] = {")
for e in ews1['ews']:
    f.write(str(e) + ",")
f.write("};\n")
f.close()

f = open(path + "/constshr.h","w")
f.write("struct _mensagem {\n")
f.write("  char label[20];\n")
f.write("  int valor;\n")
f.write("};\n")
f.write("\ntypedef struct _mensagem mensagem;\n")
f.write("\nstatic int hr[400] = {")
for e in ews1['hr']:
    f.write(str(e) + ",")
f.write("};\n")
f.close()

f = open(path + "/constsresp.h","w")
f.write("struct _mensagem {\n")
f.write("  char label[20];\n")
f.write("  int valor;\n")
f.write("};\n")
f.write("\ntypedef struct _mensagem mensagem;\n")
f.write("\nstatic int resp[400] = {")
for e in ews1['resp']:
    f.write(str(e) + ",")
f.write("};\n")
f.close()

f = open(path + "/constsspo2.h","w")
f.write("struct _mensagem {\n")
f.write("  char label[20];\n")
f.write("  int valor;\n")
f.write("};\n")
f.write("\ntypedef struct _mensagem mensagem;\n")
f.write("\nstatic int spo2[400] = {")
for e in ews1['spo2']:
    f.write(str(e) + ",")
f.write("};\n")
f.close()

f = open(path + "/results.txt","a")
f.write("\n#-\n")
f.write("EWS = \n")
for e in ews1['ews']:
    f.write(str(e) + ",")
f.close()
