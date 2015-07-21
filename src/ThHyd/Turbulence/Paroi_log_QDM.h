/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Paroi_log_QDM.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Paroi_log_QDM_included
#define Paroi_log_QDM_included

#include <Table.h>
#include <Mod_turb_hyd_base.h>

#define K_DEF 0.415   // Valeur par defaut de Kappa
#define E_DEF 9.11    // Valeur par defaut de Erugu
#define A_DEF 26      // Valeur par defaut de A_plus

class Param;

//.DESCRIPTION
// Classe rassemblant les specificites d une loi logarithmique
//

class Paroi_log_QDM
{
public:

  Paroi_log_QDM();
  void set_param(Param& param);
  void init_lois_paroi_hydraulique_();
  double calcul_lm_plus(double d_plus);

  inline void set_kappa(double& val)
  {
    Kappa=val;
  };
  inline void set_A_plus(double& val)
  {
    A_plus=val;
  };
  inline void set_Erugu(double& val)
  {
    Erugu=val;
  };

protected:

  double A_plus;
  double Cmu;
  double Kappa; // Constante de Karman
  double Erugu; // La valeur du parametre Erugu depend de la rugosite de la paroi

  Table table_hyd;  // table qui contient les valeurs de U+d+ en fonction de d+.
};

double Fdypar_direct(double lm_plus);
double Fdypar(double d_plus,double kappa,double a_plus);

// Calcul approche de l'integrale entre les bornes a et b
// d'une fonction definie et bornee par la methode de Gauss Legendre
double integrale(double a,double b,double param1,double param2, double (*pf) (double,double,double) ) ;


#endif
