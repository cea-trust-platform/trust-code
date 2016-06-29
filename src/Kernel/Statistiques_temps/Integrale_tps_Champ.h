/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Integrale_tps_Champ.h
// Directory:   $TRUST_ROOT/src/Kernel/Statistiques_temps
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Integrale_tps_Champ_included
#define Integrale_tps_Champ_included


#include <Champ_Fonc.h>
#include <Ref_Champ_base.h>
#include <Ref_Champ_Generique_base.h>
#include <Champ_Generique_base.h>

class DoubleTab;
class Zone_dis_base;

//.DESCRIPTION class Integrale_tps_Champ
//

class Integrale_tps_Champ : public Champ_Fonc
{
  Declare_instanciable(Integrale_tps_Champ);

public:

  inline const REF(Champ_Generique_base)& le_champ()         const
  {
    return mon_champ;
  };
  inline double temps_integrale()                const
  {
    return tps_integrale;
  }; // le temps courant de l'integrale
  inline double t_debut()                         const
  {
    return t_debut_;
  };        // le temps de debut d'integration
  inline double t_fin()                         const
  {
    return t_fin_;
  };           //  le temps de fin d'integration
  inline double dt_integration()                 const
  {
    return dt_integr_calcul;
  };// la duree d'integration deja effectuee
  inline void fixer_t_debut(double t)
  {
    t_debut_ = t;
  };                // Fixe le temps de debut d'integration
  inline void fixer_t_fin(double t)
  {
    t_fin_ = t;
  };                // Fixe le temps de fin d'integration
  inline void fixer_dt_integr(double t)
  {
    dt_integr_calcul= t;
  };// Fixe la valeur de tps_integrale
  inline void fixer_tps_integrale(double t)
  {
    tps_integrale = t;
  };        // Fixe le temps courant de l'integrale (derniere date a laquelle on a mis l'integrale a jour)
  inline void associer(const Champ_base&, int, double, double);
  inline void associer(const Champ_Generique_base&, int, double, double);
  virtual inline void mettre_a_jour(double );
  virtual void mettre_a_jour_integrale();

protected :

  REF(Champ_Generique_base) mon_champ;
  int puissance;
  double t_debut_;
  double t_fin_;
  double tps_integrale;
  double dt_integr_calcul;
};

///////////////////////////////////////////////////////////////////
//
//     Fonctions inline de la classe Integrale_tps_Champ
//
///////////////////////////////////////////////////////////////////

inline void Integrale_tps_Champ::associer(const Champ_base& le_ch, int n, double t0, double t1)
{

}

inline void Integrale_tps_Champ::associer(const Champ_Generique_base& le_ch, int n, double t0, double t1)
{
  mon_champ= le_ch;
  puissance = n;
  t_debut_ = t0;
  t_fin_ = t1;
  tps_integrale =t_debut_;
  dt_integr_calcul=0;
}

inline void Integrale_tps_Champ::mettre_a_jour(double )
{
  mettre_a_jour_integrale();
  Champ espace_stockage_source;
  double le_temps = mon_champ->get_time();
  changer_temps(le_temps);
}
#endif
