/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Op_Moyenne_included
#define Op_Moyenne_included

#include <Operateur_Statistique_tps_base.h>
#include <TRUSTTabs_forward.h>


class Op_Moyenne: public Operateur_Statistique_tps_base
{
  Declare_instanciable(Op_Moyenne);
public:
  Entree& lire_ch_moyenne(Entree& is);
  inline const Nom& le_nom() const override { return integrale_champ_.le_champ_calcule().le_nom(); }
  inline void set_pdi_name(const Nom& nom) { integrale_champ_.le_champ_calcule().set_pdi_name(nom); }
  inline double temps() const override { return integrale_champ_.le_champ_calcule().temps(); }
  inline Champ_Don_base& moyenne_convergee() { return ch_moyenne_convergee_; }
  inline const Integrale_tps_Champ& integrale() const override { return integrale_champ_; }
  inline const DoubleTab& valeurs() const { return integrale_champ_.le_champ_calcule().valeurs(); }
  inline DoubleTab& valeurs() { return integrale_champ_.le_champ_calcule().valeurs(); }
  inline double dt_integration() const { return integrale_champ_.dt_integration(); }

  inline void mettre_a_jour(double temps) override;
  inline void initialiser(double val) override;
  inline void associer(const Domaine_dis_base& une_zdis, const Champ_Generique_base& le_champ, double t1, double t2) override;
  inline void fixer_tstat_deb(double, double) override;
  inline void fixer_tstat_fin(double tps) override;
  inline std::vector<YAML_data> data_a_sauvegarder() const override;
  inline int sauvegarder(Sortie& os) const override;
  inline int reprendre(Entree& is) override;
  void completer(const Probleme_base&, const Nom&) override;
  DoubleTab calculer_valeurs() const override;

protected:
  Integrale_tps_Champ integrale_champ_;
  OWN_PTR(Champ_Don_base) ch_moyenne_convergee_;
};

inline void Op_Moyenne::mettre_a_jour(double tps)
{
  integrale_champ_.mettre_a_jour(tps);
  if (ch_moyenne_convergee_.non_nul())
    {
      //const Champ_Fonc_reprise& ch_moy = ref_cast(Champ_Fonc_reprise,ch_moyenne_convergee_.valeur());
      DoubleTab& valeurs_integrale = valeurs();
      valeurs_integrale = ch_moyenne_convergee_->valeurs();
      valeurs_integrale *= dt_integration();
    }
}

inline void Op_Moyenne::initialiser(double val_init)
{
  integrale_champ_.le_champ_calcule().valeurs() = val_init;
}

inline void Op_Moyenne::associer(const Domaine_dis_base& une_zdis, const Champ_Generique_base& le_champ, double t1, double t2)
{
  OWN_PTR(Champ_base) espace_stockage_source;
  const Champ_base& source = le_champ.get_champ(espace_stockage_source);
  Nom type_le_champ = source.que_suis_je();

  int renomme;
  renomme = 0;
  if (type_le_champ.debute_par("Champ"))
    renomme = 1;

  type_le_champ.suffix("Champ_");
  type_le_champ.suffix("Fonc_");
  Nom type("Champ_Fonc_");
  if (renomme == 1)
    type += type_le_champ;
  else
    type = type_le_champ;

  integrale_champ_.typer_champ(type);
  integrale_champ_.le_champ_calcule().associer_domaine_dis_base(une_zdis);
  integrale_champ_.associer(le_champ, 1, t1, t2);
}

/*! @brief for PDI IO: retrieve name, type and dimensions of the field to save/restore
 *
 */
inline std::vector<YAML_data> Op_Moyenne::data_a_sauvegarder() const
{
  const Nom& name = integrale_champ_.le_champ_calcule().get_pdi_name();
  int nb_dim = integrale_champ_.le_champ_calcule().valeurs().nb_dim();
  YAML_data d(name.getString(), "double", nb_dim);

  std::vector<YAML_data> data;
  data.push_back(d);
  return data;
}

inline int Op_Moyenne::sauvegarder(Sortie& os) const
{
  return integrale_champ_.le_champ_calcule().sauvegarder(os);
}

inline int Op_Moyenne::reprendre(Entree& is)
{
  return integrale_champ_.le_champ_calcule().reprendre(is);
}

inline void Op_Moyenne::fixer_tstat_deb(double tdeb, double tps)
{
  integrale_champ_.fixer_t_debut(tdeb);
  integrale_champ_.fixer_tps_integrale(tps);
  integrale_champ_.fixer_dt_integr(tps - tdeb);
}

inline void Op_Moyenne::fixer_tstat_fin(double tps)
{
  integrale_champ_.fixer_t_fin(tps);
}

#endif /* Op_Moyenne_included */
