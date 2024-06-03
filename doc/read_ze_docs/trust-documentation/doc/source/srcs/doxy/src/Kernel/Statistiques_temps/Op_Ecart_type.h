/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Op_Ecart_type_included
#define Op_Ecart_type_included

#include <TRUSTTabs_forward.h>
#include <Op_Moyenne.h>
#include <TRUST_Ref.h>

class Integrale_tps_Champ;

class Op_Ecart_type : public Operateur_Statistique_tps_base
{
  Declare_instanciable(Op_Ecart_type);
public:
  inline const Nom& le_nom() const override { return integrale_carre_champ.le_nom(); }
  inline double temps() const override { return integrale_carre_champ.temps(); }
  inline const Integrale_tps_Champ& integrale() const override { return integrale_carre_champ; }
  inline const Op_Moyenne& moyenne() const { return la_moyenne.valeur(); }
  inline DoubleTab& valeurs() { return integrale_champ->valeurs(); }
  inline const DoubleTab& valeurs() const { return integrale_champ->valeurs(); }

  inline DoubleTab& valeurs_carre() { return integrale_carre_champ.valeurs(); }
  inline const DoubleTab& valeurs_carre() const { return integrale_carre_champ.valeurs(); }
  inline double dt_integration() const { return integrale_champ->dt_integration(); }
  inline double dt_integration_carre() const { return integrale_carre_champ.dt_integration(); }
  inline void mettre_a_jour(double tps) override;
  inline void initialiser(double val) override;
  inline void associer(const Domaine_dis_base& , const Champ_Generique_base& ,double t1, double t2 ) override;
  inline void fixer_tstat_deb(double, double ) override;
  inline void fixer_tstat_fin(double ) override;
  inline void associer_op_stat(const Operateur_Statistique_tps_base& ) override;
  void completer(const Probleme_base& ) override;
  DoubleTab calculer_valeurs() const override;
  inline int sauvegarder(Sortie& os) const override;
  inline int reprendre(Entree& is) override;

protected:
  REF(Op_Moyenne) la_moyenne;
  REF(Integrale_tps_Champ) integrale_champ;
  Integrale_tps_Champ integrale_carre_champ;
};

inline void Op_Ecart_type::associer_op_stat(const Operateur_Statistique_tps_base& un_op_stat)
{
  la_moyenne = ref_cast(Op_Moyenne,un_op_stat);
}

inline void Op_Ecart_type::mettre_a_jour(double tps)
{
  integrale_carre_champ.mettre_a_jour(tps);
}

inline void Op_Ecart_type::initialiser(double val_init)
{
  integrale_carre_champ.valeurs()= val_init;
}

inline void Op_Ecart_type::associer(const Domaine_dis_base& une_zdis,const Champ_Generique_base& le_champ,double t1,double t2)
{
  Champ espace_stockage_source;
  const Champ_base& source = le_champ.get_champ(espace_stockage_source);
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

  integrale_carre_champ.typer(type);
  integrale_carre_champ.associer_domaine_dis_base(une_zdis);
  integrale_carre_champ.associer(le_champ,2,t1,t2);
}

inline int Op_Ecart_type::sauvegarder(Sortie& os) const
{
  return integrale_carre_champ.sauvegarder(os);
}

inline int Op_Ecart_type::reprendre(Entree& is)
{
  return integrale_carre_champ.reprendre(is);
}

inline void Op_Ecart_type::fixer_tstat_deb(double tdeb,double tps)
{
  integrale_carre_champ.fixer_t_debut(tdeb);
  integrale_carre_champ.fixer_tps_integrale(tps);
  integrale_carre_champ.fixer_dt_integr(tps-tdeb);
}

inline void Op_Ecart_type::fixer_tstat_fin(double tps)
{
  integrale_carre_champ.fixer_t_fin(tps);
}

#endif
