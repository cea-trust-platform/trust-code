import sys
from math import exp, log, sqrt
# Lois physiques du sodium issues de fits sur DEN/DANS/DM2S/STMF/LMES/RT/12-018/A

# Conversion Celcius -> Kelvin


def Tk(T):
  return T + 273.15

# inverse de la densite du liquide (hors pression)


def IRhoL(T):
  return (9.829728e-4 + Tk(T) * (2.641186e-7 + Tk(T) * (-3.340743e-11 + Tk(T) * 6.680973e-14)))

# sa derivee


def DTIRhoL(T):
  return (2.641186e-7 + Tk(T) * (-3.340743e-11 * 2 + Tk(T) * 6.680973e-14 * 3))

# densite du liquide


def RhoL(T, P):
  return exp(1e-9 * (P - 1e5)) / IRhoL(T)

# derivees


def DTRhoL(T, P):
  return - exp(1e-9 * (P - 1e5)) * DTIRhoL(T) / (IRhoL(T) * IRhoL(T))


def DPRhoL(T, P):
  return 1e-9 * RhoL(T, P)

# enthalpie du liquide


def HL(T, P):
  return (2992600 / Tk(T) - 365770 + Tk(T) * (1658.2 + Tk(T) * (-.42395 + Tk(T) * 1.4847e-4))
          + (IRhoL(T) - Tk(T) * DTIRhoL(T)) * (1 - exp(1e-9 * (1e5 - P))) / 1e-9)

# derivees


def DTHL(T, P):
  return (-2992600 / (Tk(T) * Tk(T)) + 1658.2 + Tk(T) * (-.42395 * 2 + Tk(T) * 1.4847e-4 * 3)
          - Tk(T) * (-3.340743e-11 * 2 + Tk(T) * 6.680973e-14 * 6) * (1 - exp(1e-9 * (1e5 - P))) / 1e-9)


def DPHL(T, P):
  return (IRhoL(T) - Tk(T) * DTIRhoL(T)) * exp(1e-9 * (1e5 - P))

# energie volumique du liquide


def EL(T, P):
  return HL(T, P) * RhoL(T, P)

# derivees


def DTEL(T, P):
  return DTHL(T, P) * RhoL(T, P) + HL(T, P) * DTRhoL(T, P)


def DPEL(T, P):
  return DPHL(T, P) * RhoL(T, P) + HL(T, P) * DPRhoL(T, P)

# inverses par methode de Newton


def TLh(h, T0, P):
  T = T0
  while (abs(HL(T, P) - h) > 1e-3):
    sec = HL(T, P) - h
    T -= sec / DTHL(T, P)
  return T


def TLe(e, T0, P):
  T = T0
  while (abs(EL(T, P) - e) > 1e-3):
    sec = EL(T, P) - e
    T -= sec / DTEL(T, P)
  return T

# viscosite du liquide*/


def MuL(T):
  return exp(-6.4406 - 0.3958 * log(Tk(T)) + 556.835 / Tk(T))

# conductivite du liquide


def LambdaL(T):
  return max(124.67 + Tk(T) * (-.11381 + Tk(T) * (5.5226e-5 - Tk(T) * 1.1848e-8)), 0.045)

# tension superficielle


def SigmaL(T, P):
  return 0.2405 * (1 - Tk(T) / 2503.7)**1.126

# Nusselt du liquide -> Skupinski


def NuL(T, P, w, Dh):
  Pe = RhoL(T, P) * dabs(w) * Dh * DTHL(T, P) / LambdaL(T)
  return 4.82 + 0.0185 * Pe**0.827

# temperature de saturation


def Tsat(P):
  A = 7.8130
  B = 11209
  C = 5.249e5
  return 2 * C / (-B + sqrt(B * B + 4 * A * C - 4 * C * log(P / 1e6))) - 273.15

# sa derivee


def DTsat(P):
  A = 7.8130
  B = 11209
  C = 5.249e5
  Tsk = Tsat(P) + 273.15
  return Tsk * Tsk / (P * sqrt(B * B + 4 * A * C - 4 * C * log(P / 1e6)))

# pression de vapeur saturante


def Psat(T):
  A = 7.8130
  B = 11209
  C = 5.249e5
  return 1e6 * exp(A - B / Tk(T) - C / (Tk(T) * Tk(T)))

# sa derivee


def DPsat(T):
  B = 11209
  C = 5.249e5
  return (B / (Tk(T) * Tk(T)) + 2 * C / (Tk(T) * Tk(T) * Tk(T))) * Psat(T)

# enthalpie massique de saturation


def Hsat(P):
  return HL(Tsat(P), P)

# sa derivee


def DHsat(P):
  return DTsat(P) * DTHL(Tsat(P), P) + DPHL(Tsat(P), P)

# chaleur latente, a prendre a Tsat


def Lvap(P):
  Tc = 2503.7
  Tsk = Tsat(P) + 273.15
  return 3.9337e5 * (1 - Tsk / Tc) + 4.3986e6 * (1 - Tsk / Tc)**0.29302

# sa derivee


def DLvap(P):
  Tc = 2503.7
  Tsk = Tsat(P) + 273.15
  return DTsat(P) * (-3.9337e5 / Tc - 4.3986e6 * .29302 * (1 - Tsk / Tc)**(0.29302 - 1) / Tc)

# densite de la vapeur : (gaz parfait) * f1(Tsat) * f2(DTsat)*/


def f1(Ts):
  return (2.49121 + Ts * (-5.53796e-3 + Ts * (7.5465e-6 + Ts * (-4.20217e-9 + Ts * 8.59212e-13))))


def Df1(Ts):
  return (-5.53796e-3 + Ts * (2 * 7.5465e-6 + Ts * (-3 * 4.20217e-9 + Ts * 4 * 8.59212e-13)))


def f2(dT):
  return (1 + dT * (-5e-4 + dT * (6.25e-7 - dT * 4.1359e-25)))


def Df2(dT):
  return (-5e-4 + dT * (2 * 6.25e-7 - dT * 3 * 4.1359e-25))


def RhoV(T, P):
  Tsk = Tsat(P) + 273.15
  dT = Tk(T) - Tsk
  return P * 2.7650313e-3 * f1(Tsk) * f2(dT) / Tk(T)

# ses derivees


def DTRhoV(T, P):
  Tsk = Tsat(P) + 273.15
  dT = Tk(T) - Tsk
  return P * 2.7650313e-3 * f1(Tsk) * (Df2(dT) - f2(dT) / Tk(T)) / Tk(T)


def DPRhoV(T, P):
  Tsk = Tsat(P) + 273.15
  dT = Tk(T) - Tsk
  return 2.7650313e-3 * (f1(Tsk) * f2(dT) + P * DTsat(P) * (Df1(Tsk) * f2(dT) - f1(Tsk) * Df2(dT))) / Tk(T)

# inverse de la densite de la vapeur


def IRhoV(T, P):
  return 1 / RhoV(T, P)

# ses derivees


def DTIRhoV(T, P):
  return -DTRhoV(T, P) / (RhoV(T, P) * RhoV(T, P))


def DPIRhoV(T, P):
  return -DPRhoV(T, P) / (RhoV(T, P) * RhoV(T, P))

# enthalpie de la vapeur


def CpVs(Ts):
  return (-1223.89 + Ts * (14.1191 + Ts * (-1.62025e-2 + Ts * 5.76923e-6)))


def DCpVs(Ts):
  return (14.1191 + Ts * (- 2 * 1.62025e-2 + Ts * 3 * 5.76923e-6))


def HV(T, P):
  Ts = Tsat(P)
  dT = T - Ts
  Cp0 = 910
  k = -4.6e-3
  return HL(Ts, P) + Lvap(P) + Cp0 * dT + (Cp0 - CpVs(Ts)) * (1 - exp(k * dT)) / k

# ses derivees


def DTHV(T, P):
  Ts = Tsat(P)
  dT = T - Ts
  Cp0 = 910
  k = -4.6e-3
  return Cp0 - (Cp0 - CpVs(Ts)) * exp(k * dT)


def DPHV(T, P):
  Ts = Tsat(P)
  dT = T - Ts
  Cp0 = 910
  k = -4.6e-3
  return DPHL(Ts, P) + DLvap(P) + DTsat(P) * (DTHL(Ts, P) - Cp0 - DCpVs(Ts) * (1 - exp(k * dT)) / k + (Cp0 - CpVs(Ts)) * exp(k * dT))

# energie volumique de la vapeur


def EV(T, P):
  return RhoV(T, P) * HV(T, P)

# ses derivees


def DTEV(T, P):
  return DTRhoV(T, P) * HV(T, P) + RhoV(T, P) * DTHV(T, P)


def DPEV(T, P):
  return DPRhoV(T, P) * HV(T, P) + RhoV(T, P) * DPHV(T, P)

# conductivite de la vapeur


def LambdaV(T):
  return 0.045

# viscosite de la vapeur


def MuV(T):
  return 2.5e-6 + 1.5e-8 * Tk(T)

# Nusselt de la vapeur -> Dittus/ Boetler


def NuV(T, P, w, Dh):
  Re = RhoV(T, P) * dabs(w) * Dh / MuV(T)
  Pr = MuV(T) * DTHV(T, P) / LambdaV(T)
  return 0.023 * Re**0.8 * Pr**0.4


def Rho(T, x, P):
  return 1 / ((1 - x) / RhoL(T, P) + x / RhoV(T, P))


def gazConstant():
  R = 8.31446261815324
  masse_molaire = 22.989769e-3
  return R / masse_molaire


def TmeltK():
  return 370.9


if __name__ == "__main__":
  datafile = sys.argv[1]
  ue = 1
  Te = 800
  P = 1e5
  phi = 1e8
  Ts = Tsat(P)
  rin = RhoL(Te, P)
  rv = RhoV(Tsat(P), P)
  hls = Hsat(P)
  hvs = HV(Tsat(P), P)
  cp = DTHL(Te, P)
  Hin = hls + cp * (Te - Ts)

  zl = (hls - Hin) * rin * ue / phi
  zv = (hvs - Hin) * rin * ue / phi
  print("e_ebu = {}m, z_vapeur_seche = {}m".format(zl, zv))
  print("Hls = {}, cp = {}, Te = {}, Ts = {}, Hin = {}".format(hls, cp, Te, Ts, Hin))
  d = {"__v__" : ue,
       "__l__" : 1,
       "__n__" : 51,
       "__power__" : phi,
       "__Tin__" : Te,
       "__p__" : P,
       "__Tsat__" : Ts,
       "__hin__" : Hin,
       "__rho__" : rin,
       "__yl__" : zl,
       "__rl__" : rin,
       "__rv__" : rv,
       "__hls__" : hls,
       "__hvs__" : hvs,
       }

  with open(datafile, "r") as file:
    filedata = file.read()

  # Replace the target string
  for k, v in d.items():
    filedata = filedata.replace(k, str(v))

  # Write the file out again
  with open(datafile, "w") as file:
    file.write(filedata)
