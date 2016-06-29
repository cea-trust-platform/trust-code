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
// File:        Champ_Generique_Statistiques_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Generique_Statistiques_base_included
#define Champ_Generique_Statistiques_base_included

#include <Champ_Gen_de_Champs_Gen.h>
#include <Integrale_tps_Champ.h>

class Probleme_base;
class Zone_dis_base;
class Domaine;
class Nom;
class Operateur_Statistique_tps_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//   Classe Champ_Generique_Statistiques_base
//   Classe de base des classes de champs generiques dedies aux statistiques
//   Une classe heritant de Champ_Generique_Statistiques_base porte un operateur statistique
//   Les operateurs consideres sont moyenne, ecart_Type et correlation
//////////////////////////////////////////////////////////////////////////////
class Champ_Generique_Statistiques_base : public Champ_Gen_de_Champs_Gen
{

  Declare_base_sans_constructeur(Champ_Generique_Statistiques_base);

public:

  void set_param(Param& param);
  Champ_Generique_Statistiques_base();

  inline virtual void associer_op_stat(const Champ_Generique_Statistiques_base&) {};

  virtual double temps() const =0;
  virtual const Integrale_tps_Champ& integrale() const =0;

  virtual int completer_post_statistiques(const Domaine& dom,const int is_axi,Format_Post_base& format);
  virtual const Operateur_Statistique_tps_base& Operateur_Statistique() const = 0;
  virtual Operateur_Statistique_tps_base& Operateur_Statistique() = 0;

  int sauvegarder(Sortie& os) const;
  int reprendre(Entree& is);
  virtual void   mettre_a_jour(double temps);

  void fixer_tdeb_tfin(const double& t_deb,const double& t_fin);
  virtual void fixer_serie(const double& t1,const double& t2);
  virtual void fixer_tstat_deb(const double& t1,const double& t2);
  virtual void lire_bidon(Entree& is) const;
  inline double tstat_deb() const
  {
    return tstat_deb_;
  }
  inline double tstat_fin() const
  {
    return tstat_fin_;
  }

protected:

  double tstat_deb_, tstat_fin_; //temps de debut et de fin des statistiques pour ce champ
};

#endif
