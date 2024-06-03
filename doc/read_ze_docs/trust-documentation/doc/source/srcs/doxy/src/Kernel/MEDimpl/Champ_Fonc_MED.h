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

#ifndef Champ_Fonc_MED_included
#define Champ_Fonc_MED_included

#include <Domaine_VF_inst.h>
#include <TRUSTArray.h>
#include <Champ_Fonc.h>
#include <Param.h>
#include <med++.h>
#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDCouplingFieldDouble.hxx>
#endif


/*! @brief classe Champ_Fonc_MED Load a field from a MED file for a given time.
 *
 * @sa Champ_Fonc_P0
 */
class Champ_Fonc_MED: public Champ_Fonc_base
{
  Declare_instanciable(Champ_Fonc_MED);
public :
  inline void associer_domaine_dis_base(const Domaine_dis_base&) override;
  const Domaine_dis_base& domaine_dis_base() const override;
  const Domaine_VF& domaine_vf() const override { throw; }

  void mettre_a_jour(double ) override;
  int creer(const Nom&,const Domaine& dom,const Motcle& localisation,ArrOfDouble& temps_sauv);

#ifdef MEDCOUPLING_
  MCAuto<MEDCoupling::MEDCouplingField> lire_champ(const std::string& fileName, const std::string& meshName, const std::string& fieldName, const int iteration, const int order);
  ArrOfDouble lire_temps_champ(const std::string& fileName, const std::string& fieldName);
  virtual void lire_donnees_champ(const std::string& fileName, const std::string& meshName, const std::string& fieldName,
                                  ArrOfDouble& temps_sauv, int& size, int& nbcomp, Nom& type_champ);
#endif

  const Domaine& domaine() const { return mon_dom; }
  virtual void lire(double tps,int given_iteration=-1);
  int nb_pas_temps() { return nb_dt; }
  using Champ_Fonc_base::valeurs;
  inline DoubleTab& valeurs() override;
  inline  const DoubleTab& valeurs() const override;

  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const override;
  inline DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& valeurs, int ncomp) const override;
  inline DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const override;
  inline DoubleVect& valeur_aux_sommets_compo(const Domaine&, DoubleVect&, int) const override;
  inline DoubleVect& valeur_a(const DoubleVect& position, DoubleVect& valeurs) const override;

  inline DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& valeurs, int le_poly) const override ;
  inline DoubleTab& remplir_coord_noeuds(DoubleTab& ) const override;
  inline int remplir_coord_noeuds_et_polys(DoubleTab&, IntVect&) const override;
  inline virtual const Champ_Fonc_base& le_champ() const;
  inline virtual Champ_Fonc_base& le_champ();
  const ArrOfDouble& get_saved_times(void) const;

protected:
  // Parameters read in the dataset:
  Nom nom_fichier_med_;
  Nom nom_champ_, nom_decoup_, nom_dom_, nom_maillage_;
  Motcle loc_;
  int use_existing_domain_=0;
  double temps_=0.0;
  int last_time_only_=0;

  // Other:
  REF(Domaine) mon_dom;
  Domaine dom_med_;
  Domaine_VF_inst domainebidon_inst;
  int numero_ch = -10;
  int nb_dt = -10;
#ifdef MED_
  med_entity_type type_ent;
  med_geometry_type type_geo;
#ifdef MEDCOUPLING_
  INTERP_KERNEL::NormalizedCellType cell_type;
  MEDCoupling::TypeOfField field_type = MEDCoupling::ON_CELLS;
  std::vector< std::pair<int,int> > time_steps_;
#endif
#endif
  Champ_Fonc vrai_champ_;
  Nom nom_champ_dans_fichier_med_;
  std::vector<int> filter;
  ArrOfDouble temps_sauv_;

  virtual void set_param(Param& param);
  void readOn_old_syntax(Entree& is, Nom& chaine_lue, bool& nom_decoup_lu);
};

inline void Champ_Fonc_MED::associer_domaine_dis_base(const Domaine_dis_base& le_dom_dis_base)
{
  Cerr<<"Champ_Fonc_MED::associer_domaine_dis_base does nothing"<<finl;
}
inline const Champ_Fonc_base& Champ_Fonc_MED::le_champ() const
{
  return vrai_champ_.valeur();
}

inline Champ_Fonc_base& Champ_Fonc_MED::le_champ()
{
  return vrai_champ_.valeur();
}
inline DoubleTab& Champ_Fonc_MED::valeurs()
{
  return le_champ().valeurs();
}
inline const DoubleTab& Champ_Fonc_MED::valeurs() const
{
  return le_champ().valeurs();
}

inline DoubleTab& Champ_Fonc_MED::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& tab_valeurs) const
{
  return le_champ().valeur_aux_elems(positions, les_polys, tab_valeurs);
}
inline DoubleVect& Champ_Fonc_MED::valeur_a(const DoubleVect& position, DoubleVect& tab_valeurs) const
{
  const Domaine& domaine=domainebidon_inst.domaine();
  IntVect le_poly(1);
  domaine.chercher_elements(position, le_poly);
  return le_champ().valeur_a_elem(position,tab_valeurs,le_poly(0));
}

inline DoubleVect& Champ_Fonc_MED::valeur_a_elem(const DoubleVect& position, DoubleVect& tab_valeurs, int le_poly) const
{
  // Le codage de cette methode passe par valeur_a() et ignore l'entier le_poly
  // car si on utilise le_champ().valeur_a_elem(position,tab_valeurs,le_poly), on
  // a des ecarts sequentiel-parallele pour les champs P0 (et probablement aussi pour
  // les autres champs).
  // Je m'explique cela par le fait qu'en parallele, le_poly designe un numero d'element
  // dans la partition associee a un processeur, alors que le champ designe par le pointeur vrai_champ_
  // est defini sur la totalite du domaine.
  // Pour cette raison, j'utilise valeur_a(), qui localise la position dans la totalite du domaine,
  // et qui retourne la valeur de vrai_champ_ dans cet element.
  return le_champ().valeur_a(position,tab_valeurs);
}

inline DoubleVect& Champ_Fonc_MED::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& tab_valeurs, int ncomp) const
{
  return le_champ().valeur_aux_elems_compo(positions, les_polys, tab_valeurs, ncomp);
}

inline DoubleTab& Champ_Fonc_MED::valeur_aux_sommets(const Domaine& un_dom, DoubleTab& sommets) const
{
  return le_champ().valeur_aux_sommets(un_dom, sommets);
}

inline DoubleVect& Champ_Fonc_MED::valeur_aux_sommets_compo(const Domaine& un_dom, DoubleVect& sommets, int compo) const
{
  return le_champ().valeur_aux_sommets_compo(un_dom, sommets, compo);
}

inline DoubleTab& Champ_Fonc_MED::remplir_coord_noeuds(DoubleTab& coord) const
{
  return le_champ().remplir_coord_noeuds(coord);
}

inline int Champ_Fonc_MED::remplir_coord_noeuds_et_polys(DoubleTab& coord, IntVect& elems) const
{
  return le_champ().remplir_coord_noeuds_et_polys(coord, elems);
}


#endif
