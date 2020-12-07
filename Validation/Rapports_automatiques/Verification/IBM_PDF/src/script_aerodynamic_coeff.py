print("DISCLAIMER")
print("\tUsage :")
print("\tscript_aerodynamic_coeff.py <input_file_name_as_str> <output_file_name_1_as_str> <output_file_name_2_as_str>")
print('IMPORTING SYS AND CHECKING ARGUMENTS...')
import sys
if len(sys.argv) != 4:
  print("\t%i" % len(sys.argv)) 
  print("\tERROR : too few or too many arguments. The script needs exactly 3.")
  sys.exit(-1)
inputFileName = sys.argv[1]
outputFileName = sys.argv[2]
outputFileName2 = sys.argv[3]

import sys
from math import pi
from math import sqrt


def mean(x):
  return sum(x) / len(x)

def stddev(x,mu):
  n = len(x)
  over_n = 1.0 / n
  sum1 = 0.0
  for i in range(0,n):
    sum1 += (x[i]-mu)*(x[i]-mu)
  return sqrt(over_n*sum1)

def stdvar(x,mu):
  n = len(x)
  over_n = 1.0 / n
  sum1 = 0.0
  for i in range(0,n):
    sum1 += (x[i]-mu)*(x[i]-mu)
  return over_n*sum1

def fluctuation(x,mu):
  xmin = min(x)
  xmax = max(x)
  return max(abs(xmax-mu),abs(xmin-mu))

def fluctuation2(x,mu):
  flu = -1e6
  for i in range(0,len(x)):
    e = abs(x[i] - mu)
    if (e > flu):
      flu = e
  return flu

#PARAMS
rho = 1.0
v = 1.0
d = 1.0
S = d
#S = 0.25*pi*d*d
csvSepOut = "," 

rawFile = open(inputFileName,'r')
cdclFile = open(outputFileName,'w')
lines = rawFile.readlines()

timeTreshold = 0.
ts = []
cds = []
cls = []
for line in lines:
  if (line[0] != '#'):
    listLine = line.split(" ")
    try:
      t = float(listLine[0])
      fx = float(listLine[1])
      fy = float(listLine[2])
      cd = fx / (0.5*rho*v*v*S)
      cl = fy / (0.5*rho*v*v*S)
      cdclFile.write(str(t)+csvSepOut+str(-cd)+csvSepOut+str(-cl)+'\n')
      #print(t,-fx,-fy,-cd,-cl)
      if (t > timeTreshold):
        ts.append(t)
        cds.append(-cd)
        cls.append(-cl)
    except:
      print('ERROR : Badly formatted file.')

mu_cd = mean(cds)
mu_cl = mean(cls)
sig_cd = stddev(cds,mu_cd)
sig_cl = stddev(cls,mu_cl)
var_cd = stdvar(cds,mu_cd)
var_cl = stdvar(cls,mu_cl)
fl_cd = fluctuation(cds,mu_cd)
fl_cl = fluctuation(cls,mu_cl)
fl2_cd = fluctuation2(cds,mu_cd)
fl2_cl = fluctuation2(cls,mu_cl)

file2 = open(outputFileName2,"w")

#print(mu_cd,mu_cl)
file2.write(str(mu_cd)+","+str(mu_cl)+"\n")
#print(sig_cd,sig_cl)
#print(var_cd,var_cl)
#print(fl_cd,fl_cl)
#print(fl2_cd,fl2_cl)
file2.write(str(fl2_cd)+","+str(fl2_cl)+"\n")

cpt = 0.0
for i in range(0,len(cls)-1):
  if ((cls[i] > mu_cl) and (cls[i+1] < mu_cl)):
    cpt += 1.0  
if (cpt > 0):
  T = (ts[-1]-ts[0]) / cpt
  f = 1.0/T

  #print(f)

  file2.write(str(f))

file2.close()

rawFile.close()
cdclFile.close()
