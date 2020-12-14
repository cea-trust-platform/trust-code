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
// File:        Champ_Fonc_MED.h
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//
// .SECTION voir aussi
//
//
///////////////////////////////////////////////////////////////////////////

#ifndef Champ_Fonc_MED_included
#define Champ_Fonc_MED_included

#include <Champ_Fonc.h>
#include <Zone_VF_inst.h>
#include <ArrOfDouble.h>
#include <med++.h>
#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDCouplingFieldDouble.hxx>
#endif


//.DESCRIPTION classe Champ_Fonc_MED
//

//.SECTION voir aussi
// Champ_Fonc_P0

class Champ_Fonc_MED: public Champ_Fonc_base
{

  Declare_instanciable(Champ_Fonc_MED);

public :

  inline void associer_zone_dis_base(const Zone_dis_base&);
  const Zone_dis_base& zone_dis_base() const;
  virtual void mettre_a_jour(double );
  /*  double valeur_au_bord(int face) const;
      DoubleVect moyenne() const;
      double moyenne(int ) const;
      int imprime(Sortie& , int ) const ;
  */
  int creer(const Nom&,const Domaine& dom,const Motcle& localisation,ArrOfDouble& temps_sauv);

#ifdef MEDCOUPLING_
  MCAuto<MEDCoupling::MEDCouplingField> lire_champ(const std::string& fileName, const std::string& meshName, const std::string& fieldName, const int iteration, const int order);
  ArrOfDouble lire_temps_champ(const std::string& fileName, const std::string& fieldName);
  virtual void lire_donnees_champ(const std::string& fileName, const std::string& meshName, const std::string& fieldName,
                                  ArrOfDouble& temps_sauv, int& size, int& nbcomp, Nom& type_champ);
#endif

  const Domaine& domaine() const
  {
    return mon_dom;
  }
  virtual void lire(double tps,int given_iteration=-1);
  int nb_pas_temps()
  {
    return nb_dt;
  }
  inline DoubleTab& valeurs();
  inline  virtual const DoubleTab& valeurs() const;

  inline virtual DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                                             const IntVect& les_polys,
                                             DoubleTab& valeurs) const;
  inline virtual DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                                    const IntVect& les_polys,
                                                    DoubleVect& valeurs,
                                                    int ncomp) const ;
  inline virtual DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const;
  inline virtual DoubleVect& valeur_aux_sommets_compo(const Domaine&,
                                                      DoubleVect&, int) const;
  inline virtual DoubleVect&  valeur_a(const DoubleVect& position,
                                       DoubleVect& valeurs) const ;

  inline virtual DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& valeurs, int le_poly) const ;
  inline virtual DoubleTab& remplir_coord_noeuds(DoubleTab& ) const;
  inline virtual int remplir_coord_noeuds_et_polys(DoubleTab&, IntVect&) const;
  inline virtual const Champ_Fonc_base& le_champ() const;
  inline virtual Champ_Fonc_base& le_champ();
  const ArrOfDouble& get_saved_times(void) const;


protected:
  REF(Domaine) mon_dom;
  Domaine dom_med_;
  Zone_VF_inst zonebidon_inst;
  int numero_ch;
  int nb_dt;
  Nom nom_fichier_med_;
#ifdef MED_
  med_entity_type type_ent;
  med_geometry_type type_geo;
#ifdef MEDCOUPLING_
  INTERP_KERNEL::NormalizedCellType cell_type;
  MEDCoupling::TypeOfField field_type;
  std::vector< std::pair<int,int> > time_steps_;
#endif
#endif
  Champ_Fonc vrai_champ_;
  Nom nom_champ_dans_fichier_med_;
  ArrOfInt filter;
  ArrOfDouble temps_sauv_;
  int last_time_only_;
  bool use_medcoupling_;
};

inline void Champ_Fonc_MED::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  Cerr<<"Champ_Fonc_MED::associer_zone_dis_base does nothing"<<finl;
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

inline DoubleTab& Champ_Fonc_MED::valeur_aux_elems(const DoubleTab& positions,
                                                   const IntVect& les_polys,
                                                   DoubleTab& tab_valeurs) const
{
  return le_champ().valeur_aux_elems(positions, les_polys, tab_valeurs);
}
inline DoubleVect&  Champ_Fonc_MED::valeur_a(const DoubleVect& position,
                                             DoubleVect& tab_valeurs) const
{
  const Zone& zone=zonebidon_inst.zone();
  IntVect le_poly(1);
  zone.chercher_elements(position, le_poly);
  return le_champ().valeur_a_elem(position,tab_valeurs,le_poly(0));
}


// J'ajoute la methode valeur_a_elem()
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

inline DoubleVect& Champ_Fonc_MED::valeur_aux_elems_compo(const DoubleTab& positions,
                                                          const IntVect& les_polys,
                                                          DoubleVect& tab_valeurs,
                                                          int ncomp) const
{
  return le_champ().valeur_aux_elems_compo(positions, les_polys, tab_valeurs, ncomp);
}

inline DoubleTab& Champ_Fonc_MED::valeur_aux_sommets(const Domaine& un_dom, DoubleTab& sommets) const
{
  return le_champ().valeur_aux_sommets(un_dom, sommets);
}

inline DoubleVect& Champ_Fonc_MED::valeur_aux_sommets_compo(const Domaine& un_dom,
                                                            DoubleVect& sommets, int compo) const
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

/**
 * Read field with MEDFile API (soon deprecated)
 */
class Champ_Fonc_MEDfile: public Champ_Fonc_MED
{
  Declare_instanciable(Champ_Fonc_MEDfile);
};
#endif
