/****************************************************************************
* Copyright (c) 2020, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        Lois_sodium_liquide.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Lois_sodium_liquide.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Lois_sodium_liquide
//      utilitaire pour les operateurs de frottement interfacial prenant la forme
//      F_{kl} = - F_{lk} = - C_{kl} (u_k - u_l)
//      cette classe definit une fonction C_{kl} dependant de :
//        alpha, p, T -> inconnues (une valeur par phase chacune)
//        rho, mu, sigma -> proprietes physiques (idem)
//        dv_abs(i, k, l) -> i-eme ecart ||v_k - v_l||, a remplir pour k < l
//    sortie :
//        coeff(i, k, l) -> i_eme coefficient C_{kl}, rempli pour k < l
//////////////////////////////////////////////////////////////////////////////

#define Tk ((T) + 273.15)
const double Ksil = 1e-9;

inline double IRhoL(double T)
{
  return (9.829728e-4 + Tk * (2.641186e-7 + Tk * (-3.340743e-11 + Tk * 6.680973e-14)));
}

inline double DTIRhoL(double T)
{
  return (2.641186e-7 + Tk * (-3.340743e-11 * 2 + Tk * 6.680973e-14 * 3));
}

inline double RhoL(double T, double P)
{
  return exp(Ksil * (P - 1e5)) / IRhoL(T);
}

inline double DTRhoL(double T, double P)
{
  return - exp(Ksil * (P - 1e5)) * DTIRhoL(T) / (IRhoL(T) * IRhoL(T));
}

inline double DPRhoL(double T, double P)
{
  return Ksil * RhoL(T, P);
}

inline double Tsat( double P )
{
  double A = 7.8130, B = 11209, C = 5.249e5;
  return 2 * C / ( -B + sqrt(B*B + 4 * A * C - 4 * C * log(P / 1e6))) - 273.15;
}

inline double DTsat( double P )
{
  double A = 7.8130, B = 11209, C = 5.249e5, Tsk = Tsat(P) + 273.15;
  return Tsk * Tsk / ( P * sqrt(B*B + 4 * A * C - 4 * C * log(P / 1e6)));
}

#define  f1(Ts) (2.49121 + Ts * (-5.53796e-3 + Ts * (7.5465e-6 + Ts * (-4.20217e-9 + Ts * 8.59212e-13))))
#define Df1(Ts) (-5.53796e-3 + Ts * (2 * 7.5465e-6 + Ts * (-3 * 4.20217e-9 + Ts * 4 * 8.59212e-13)))
#define  f2(dT) (1 + dT * (-5e-4 + dT * (6.25e-7 - dT * 4.1359e-25)))
#define Df2(dT) (-5e-4 + dT * (2 * 6.25e-7 - dT * 3 * 4.1359e-25))
inline double RhoV( double T, double P )
{
  double Tsk = Tsat(P) + 273.15, dT = Tk - Tsk;
  return P * 2.7650313e-3 * f1(Tsk) * f2(dT) / Tk;
}
/* ses derivees */
inline double DTRhoV( double T, double P )
{
  double Tsk = Tsat(P) + 273.15, dT = Tk - Tsk;
  return P * 2.7650313e-3 * f1(Tsk) * (Df2(dT) - f2(dT) / Tk) / Tk;
}
inline double DPRhoV( double T, double P )
{
  double Tsk = Tsat(P) + 273.15, dT = Tk - Tsk;
  return 2.7650313e-3 * (f1(Tsk) * f2(dT) + P * DTsat(P) * (Df1(Tsk) * f2(dT) - f1(Tsk) * Df2(dT))) / Tk;
}
#undef f1
#undef Df1
#undef f2
#undef Df2

inline double HL(const double T, const double P)
{
  return (2992600 / Tk - 365487.2 + Tk * (1658.2 + Tk * (-.42395 + Tk * 1.4847e-4))) + (IRhoL(T) - Tk * DTIRhoL(T)) * (1 - exp(Ksil*(1e5 - P))) / Ksil;
}

inline double DTHL(double T, double P)
{
  return (-2992600 / (Tk*Tk) + 1658.2 + Tk * (-.42395 * 2 + Tk * 1.4847e-4 * 3)) - Tk * (-3.340743e-11 * 2 + Tk * 6.680973e-14 * 6) * (1 - exp(Ksil*(1e5 - P))) / Ksil;
}

inline double DPHL(double T, double P)
{
  return (IRhoL(T) - Tk * DTIRhoL(T)) * exp(Ksil * (1e5 - P));
}


inline double Lvap( double P )
{
  double Tc = 2503.7, Tsk = Tsat(P) + 273.15;
  return 3.9337e5 * ( 1 - Tsk / Tc) + 4.3986e6 * pow( 1 - Tsk / Tc, .29302);
}

inline double DLvap( double P )
{
  double Tc = 2503.7, Tsk = Tsat(P) + 273.15;
  return DTsat(P) * (-3.9337e5 / Tc - 4.3986e6  * .29302 * pow( 1 - Tsk / Tc, .29302 - 1) / Tc);
}

#define  CpVs(Ts) (-1223.89 + Ts * (14.1191 + Ts * (-1.62025e-2 + Ts * 5.76923e-6)))
#define DCpVs(Ts) (14.1191 + Ts * (- 2 * 1.62025e-2 + Ts * 3 * 5.76923e-6))
inline double HV( double T, double P)
{
  double Ts = Tsat(P), dT = T - Ts, Cp0 = 910, k = -4.6e-3;
  return HL(Ts, P) + Lvap(P) + Cp0 * dT + (Cp0 - CpVs(Ts)) * (1 - exp(k * dT)) / k;
}
/* ses derivees */
inline double DTHV( double T, double P)
{
  double Ts = Tsat(P), dT = T - Ts, Cp0 = 910, k = -4.6e-3;
  return Cp0 - (Cp0 - CpVs(Ts)) * exp(k * dT);
}
inline double DPHV( double T, double P)
{
  double Ts = Tsat(P), dT = T - Ts, Cp0 = 910, k = -4.6e-3;
  return DPHL(Ts, P) + DLvap(P) + DTsat(P) * (DTHL(Ts, P) - Cp0 - DCpVs(Ts) * (1 - exp(k * dT)) / k + (Cp0 - CpVs(Ts)) * exp(k * dT));
}
#undef CpVs
#undef DCpVs


/*  _       _           _        _ _ _       _             __
   | |     (_)         | |      | ( |_)     | |           / _|
   | | ___  _ ___    __| | ___  | |/ _ _ __ | |_ ___ _ __| |_ __ _  ___ ___
   | |/ _ \| / __|  / _` |/ _ \ | | | | '_ \| __/ _ \ '__|  _/ _` |/ __/ _ \
   | | (_) | \__ \ | (_| |  __/ | | | | | | | ||  __/ |  | || (_| | (_|  __/
   |_|\___/|_|___/  \__,_|\___| |_| |_|_| |_|\__\___|_|  |_| \__,_|\___\___|
*/

double Lois_sodium_liquide::rho_(const double T, const double P) const
{
  return RhoL(T, P);
}

double Lois_sodium_liquide::dT_rho_(const double T, const double P) const
{
  return DTRhoL(T, P);
}

double Lois_sodium_liquide::dP_rho_(const double T, const double P) const
{
  return DPRhoL(T, P);
}

double Lois_sodium_liquide::cp_(const double T, const double P) const
{
  const double eint = HL(T, P) - P / rho_(T, P);
  return eint / T;
}

double Lois_sodium_liquide::dT_cp_(const double T, const double P) const
{
  Process::exit("pas code");
  return 0;
}

double Lois_sodium_liquide::dP_cp_(const double T, const double P) const
{
  Process::exit("pas code");
  return 0;
}

double Lois_sodium_liquide::mu_(const double T) const
{
  return exp(-6.4406 - 0.3958 * log(Tk) + 556.835 / (273.15 + Tk));
}

double Lois_sodium_liquide::lambda_(const double T) const
{
  return std::max(124.67 + Tk * (-.11381 + Tk * (5.5226e-5 - Tk * 1.1848e-8)), 0.045);
}

double Lois_sodium_liquide::beta_(const double T) const
{
  return DTIRhoL(T) / IRhoL(T);
}

// double Lois_sodium_liquide::rho_v(const double T, const double P) const
// {
//   if (debile) return 1;
//   else RhoV(T, P);
// }

// double Lois_sodium_liquide::dT_rho_v(const double T, const double P) const
// {
//   if (debile) return 0;
//   else return DTRhoV(T, P);
// }

// double Lois_sodium_liquide::dP_rho_v(const double T, const double P) const
// {
//   if (debile) return 0;
//   else return DPRhoV(T, P);
// }
// double Lois_sodium_liquide::ei_v(const double T, const double P) const
// {
//   const double hv = debile ? 1.0 * T + P / 1.0 : HV(T, P);
//   return hv - P / rho_v(T, P);
// }

// double Lois_sodium_liquide::dT_ei_v(const double T, const double P) const
// {
//   if (debile)
//     {
//       const double cp = 1;
//       return cp;
//     }
//   else
//     {
//       Process::exit("pas code");
//       return 0;
//     }
// }

// double Lois_sodium_liquide::dP_ei_v(const double T, const double P) const
// {
//   if (debile)
//     {
//       return 0;
//     }
//   else
//     {
//       Process::exit("pas code");
//       return 0;
//     }
// }

// Nom Lois_sodium_liquide::get_lambdav_formula() const
// {
//   return Nom("0.045");
// }
// Nom Lois_sodium_liquide::get_muv_formula() const
// {
//   return Nom("2.5e-6+1.5e-8*(273.15+val)");
// }

#undef Tk
