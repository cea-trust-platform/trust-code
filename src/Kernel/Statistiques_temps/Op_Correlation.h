/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Op_Correlation.h
// Directory:   $TRUST_ROOT/src/Kernel/Statistiques_temps
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Correlation_included
#define Op_Correlation_included

#include <Integrale_tps_produit_champs.h>
#include <Ref_Integrale_tps_Champ.h>
#include <TRUSTTabs_forward.h>
#include <Ref_Op_Moyenne.h>
#include <Op_Moyenne.h>

class Op_Correlation : public Operateur_Statistique_tps_base
{
  Declare_instanciable(Op_Correlation);
public:
  inline const Nom& le_nom() const override
  {
    return integrale_tps_ab_.le_nom();
  };
  inline double temps() const override
  {
    return integrale_tps_ab_.temps();
  };
  inline const Integrale_tps_produit_champs& integrale() const override
  {
    return integrale_tps_ab_;
  };
  inline const Op_Moyenne& la_moyenne_a() const
  {
    return la_moyenne_a_.valeur();
  };
  inline const Op_Moyenne& la_moyenne_b() const
  {
    return la_moyenne_b_.valeur();
  };
  inline const DoubleTab& valeurs_a() const
  {
    return integrale_tps_a_->valeurs();
  };
  inline const DoubleTab& valeurs_b() const
  {
    return integrale_tps_b_->valeurs();
  };
  inline DoubleTab& valeurs_ab()
  {
    return integrale_tps_ab_.valeurs();
  };
  inline double dt_integration_a() const
  {
    return integrale_tps_a_->dt_integration();
  };
  inline double dt_integration_b() const
  {
    return integrale_tps_b_->dt_integration();
  };
  inline double dt_integration_ab() const
  {
    return integrale_tps_ab_.dt_integration();
  };
  inline void mettre_a_jour(double tps) override;
  inline void initialiser(double val) override;
  inline void associer(const Zone_dis_base& , const Champ_base& , double t1, double t2 );
  inline void associer(const Zone_dis_base& , const Champ_Generique_base& , double t1, double t2 ) override;
  inline void associer(const Zone_dis_base& ,const Champ_Generique_base& ,const Champ_Generique_base& , double t1, double t2 );
  inline void fixer_tstat_deb(double, double ) override;
  inline void fixer_tstat_fin(double ) override;
  int completer_post_statistiques(const Domaine& dom,const int is_axi,Format_Post_base& format) override;
  inline int sauvegarder(Sortie& os) const override;
  inline int reprendre(Entree& is) override;
  inline void associer_op_stat(const Operateur_Statistique_tps_base&) override;
  void completer(const Probleme_base& ) override;
  DoubleTab calculer_valeurs() const override;

protected:
  REF(Op_Moyenne) la_moyenne_a_;
  REF(Op_Moyenne) la_moyenne_b_;
  REF(Integrale_tps_Champ) integrale_tps_a_;
  REF(Integrale_tps_Champ) integrale_tps_b_;
  Integrale_tps_produit_champs integrale_tps_ab_;
};

inline void Op_Correlation::associer_op_stat(const Operateur_Statistique_tps_base& un_op_stat)
{
  if (!la_moyenne_a_.non_nul())
    la_moyenne_a_ = ref_cast(Op_Moyenne,un_op_stat);
  else if (!la_moyenne_b_.non_nul())
    la_moyenne_b_ = ref_cast(Op_Moyenne,un_op_stat);
  else
    {
      Cerr << "Op_Correlation::associer_op_stat : Two operators of type Moyenne have already been associated." << finl;
      exit();
    }
}

inline void Op_Correlation::mettre_a_jour(double tps)
{
  integrale_tps_ab_.mettre_a_jour(tps);
}

inline void Op_Correlation::initialiser(double val_init)
{
  integrale_tps_ab_.valeurs()= val_init;
}

inline void Op_Correlation::associer(const Zone_dis_base& une_zdis, const Champ_base& le_champ, double t1,double t2)
{
  Cerr << "Exactly two fields must be associated to Correlation operator." << finl;
  exit();
}

inline void Op_Correlation::associer(const Zone_dis_base& une_zdis, const Champ_Generique_base& le_champ, double t1,double t2)
{
  Cerr << "Exactly two fields must be associated to Correlation operator." << finl;
  exit();
}

inline void Op_Correlation::associer(const Zone_dis_base& une_zdis, const Champ_Generique_base& le_champ_a, const
                                     Champ_Generique_base& le_champ_b, double t1,double t2)
{
  Champ espace_stockage_source;
  const Champ_base& source = le_champ_a.get_champ(espace_stockage_source);
  Nom type_le_champ = source.que_suis_je();

  int renomme;
  renomme=0;
  if (type_le_champ.debute_par("Champ"))
    renomme=1;

  type_le_champ.suffix("Champ_");
  type_le_champ.suffix("Fonc_");
  Nom type("Champ_Fonc_");
  if (renomme==1)
    type+=type_le_champ;
  else
    type = type_le_champ;

  integrale_tps_ab_.typer(type);
  integrale_tps_ab_.associer_zone_dis_base(une_zdis);
  integrale_tps_ab_.associer(le_champ_a,le_champ_b,1,1,t1,t2);
}

inline int Op_Correlation::sauvegarder(Sortie& os) const
{
  return integrale_tps_ab_.sauvegarder(os);
}

inline int Op_Correlation::reprendre(Entree& is)
{
  return integrale_tps_ab_.reprendre(is);
}

inline void Op_Correlation::fixer_tstat_deb(double tdeb,double tps)
{
  integrale_tps_ab_.fixer_t_debut(tdeb);
  integrale_tps_ab_.fixer_tps_integrale(tps);
  integrale_tps_ab_.fixer_dt_integr(tps-tdeb);
}

inline void Op_Correlation::fixer_tstat_fin(double tps)
{
  integrale_tps_ab_.fixer_t_fin(tps);
}

#endif
