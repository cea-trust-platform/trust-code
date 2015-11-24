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
// File:        Champ_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/30
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_Face_included
#define Champ_Face_included

#include <Champ_Inc_base.h>
#include <Ref_Zone_VDF.h>
#include <Champ_Face_implementation.h>


class Zone_Cl_VDF;

//.DESCRIPTION class Champ_Face
//  Cette classe sert a representer un champ vectoriel dont on ne calcule
//  que les composantes normales aux faces.Il n'y a donc qu'un degre de
//  liberte par face et l'attribut nb_comp_ d'un objet de type Champ_Face
//  vaut 1. On peut neammoins imposer toutes les composantes du champ sur
//  le bord. Si n est le nombre de faces total de la zone et nb_faces_bord
//  le nombre de faces de bord le tableau de valeurs associe au champ
//  est construit comme suit:
//      - n valeurs pour representer les composantes normales aux faces
//      - nb_faces_bord*dimension pour stocker les valeurs imposees
//        sur les faces de bord
// Rq : cette classe est specifique au module VDF

//.SECTION voir aussi
// Champ_Inc_base

class Champ_Face : public Champ_Inc_base, public Champ_Face_implementation
{

  Declare_instanciable(Champ_Face);

public:

  //
  // Methodes reimplementees
  //
  void   associer_zone_dis_base(const Zone_dis_base&);
  //Pour creation d une reference a Zone_Cl_VDF
  //const Zone_Cl_dis_base& associer_zone_Cl_dis_base(const Zone_Cl_dis_base&);
  int fixer_nb_valeurs_nodales(int );

  const Zone_dis_base& zone_dis_base() const;
  inline const Zone_VDF& zone_vdf() const;
  Champ_base& affecter_(const Champ_base& );
  virtual const Champ_Proto& affecter(const double x1,const double x2);
  virtual const Champ_Proto& affecter(const double x1,const double x2,const double x3);
  virtual const Champ_Proto& affecter(const DoubleTab&);
  void verifie_valeurs_cl();
  int compo_normale_sortante(int ) const;

  inline const DoubleTab& tau_diag() const;
  inline const DoubleTab& tau_croises() const;
  inline DoubleTab& tau_diag();
  inline DoubleTab& tau_croises();
  void dimensionner_tenseur_Grad();
  void calculer_dercov_axi(const Zone_Cl_VDF& );
  void calculer_rotationnel_ordre2_centre_element(DoubleTab& ) const;
  int imprime(Sortie& , int ) const;
  DoubleTab& trace(const Frontiere_dis_base& , DoubleTab&, double ) const;
  virtual void mettre_a_jour(double temps);

  inline DoubleVect& valeur_a_elem(const DoubleVect& position,
                                   DoubleVect& val,
                                   int le_poly) const;
  inline double valeur_a_elem_compo(const DoubleVect& position,
                                    int le_poly, int ncomp) const;
  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                                     const IntVect& les_polys,
                                     DoubleTab& tab_valeurs) const;
  inline DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                            const IntVect& les_polys,
                                            DoubleVect& tab_valeurs,
                                            int ncomp) const ;
  inline DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const;
  inline DoubleVect& valeur_aux_sommets_compo(const Domaine&,
                                              DoubleVect&, int) const;
  inline DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const;
  inline int remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                           IntVect& polys) const;
  void calculer_dscald_centre_element(DoubleTab& ) const;


  void calcul_critere_Q(DoubleTab&, const Zone_Cl_VDF&  );
  void calcul_y_plus(DoubleTab&, const Zone_Cl_VDF&  );

  DoubleTab& calcul_duidxj(const DoubleTab&, DoubleTab&) const;
  DoubleTab& calcul_duidxj(const DoubleTab&, DoubleTab&, const Zone_Cl_VDF&) const;
  DoubleVect& calcul_S_barre(const DoubleTab&, DoubleVect&, const Zone_Cl_VDF&) const;
  DoubleVect& calcul_S_barre_sans_contrib_paroi(const DoubleTab&, DoubleVect&, const Zone_Cl_VDF&) const;

private:
  double val_imp_face_bord_private(int face,int comp) const;
  double val_imp_face_bord_private(int face,int comp1,int comp2) const;

  REF(Zone_VDF) la_zone_VDF;

  DoubleTab tau_diag_;       // termes diagonaux du tenseur Grad
  DoubleTab tau_croises_;    // termes extradiagonaux du tenseur Grad
  inline virtual const Champ_base& le_champ() const;
  inline virtual Champ_base& le_champ();

};

double Champ_Face_get_val_imp_face_bord( const double& temp,int face,int comp, const Zone_Cl_VDF& zclo) ;
double Champ_Face_get_val_imp_face_bord( const double& temp,int face,int comp, int comp2, const Zone_Cl_VDF& zclo) ;

double Champ_Face_get_val_imp_face_bord_sym(const DoubleTab& tab_valeurs, const double& temp,int face,int comp, const Zone_Cl_VDF& zclo);
//////////////////////////////////////////////////
//
//   Fonctions inline de la classe Champ_Face
//
//////////////////////////////////////////////////

inline const Champ_base& Champ_Face::le_champ() const
{
  return *this;
}

inline Champ_base& Champ_Face::le_champ()
{
  return *this;
}

inline DoubleVect& Champ_Face::valeur_a_elem(const DoubleVect& position,
                                             DoubleVect& val,
                                             int le_poly) const
{
  return Champ_Face_implementation::valeur_a_elem(position, val, le_poly);
}
inline double  Champ_Face::valeur_a_elem_compo(const DoubleVect& position,
                                               int le_poly, int ncomp) const
{
  return Champ_Face_implementation::valeur_a_elem_compo(position, le_poly, ncomp);
}
inline DoubleTab&  Champ_Face::valeur_aux_elems(const DoubleTab& positions,
                                                const IntVect& les_polys,
                                                DoubleTab& tab_valeurs) const
{
  return Champ_Face_implementation::valeur_aux_elems(positions, les_polys, tab_valeurs);
}
inline DoubleVect&  Champ_Face::valeur_aux_elems_compo(const DoubleTab& positions,
                                                       const IntVect& les_polys,
                                                       DoubleVect& tab_valeurs,
                                                       int ncomp) const
{
  return Champ_Face_implementation::valeur_aux_elems_compo(positions, les_polys,
                                                           tab_valeurs, ncomp);
}
inline DoubleTab&  Champ_Face::valeur_aux_sommets(const Domaine& dom, DoubleTab& val) const
{
  return Champ_Face_implementation::valeur_aux_sommets(dom, val);
}
inline DoubleVect&  Champ_Face::valeur_aux_sommets_compo(const Domaine& dom,
                                                         DoubleVect& val, int comp) const
{
  return Champ_Face_implementation::valeur_aux_sommets_compo(dom, val, comp);
}
inline DoubleTab&  Champ_Face::remplir_coord_noeuds(DoubleTab& positions) const
{
  return Champ_Face_implementation::remplir_coord_noeuds(positions);
}
inline int  Champ_Face::remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                                      IntVect& polys) const
{
  return Champ_Face_implementation::remplir_coord_noeuds_et_polys(positions,
                                                                  polys);
}

inline const DoubleTab& Champ_Face::tau_diag() const
{
  return tau_diag_;
}

inline const DoubleTab& Champ_Face::tau_croises() const
{
  return tau_croises_;
}

inline DoubleTab& Champ_Face::tau_diag()
{
  return tau_diag_;
}

inline DoubleTab& Champ_Face::tau_croises()
{
  return tau_croises_;
}

inline const Zone_VDF& Champ_Face::zone_vdf() const
{
  return la_zone_VDF.valeur();
}

int Champ_Face_test();

#endif
