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
// File:        Champ_P1NC.h
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/34
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_P1NC_included
#define Champ_P1NC_included

#include <Champ_P1NC_implementation.h>
#include <Ref_Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Champ_Inc.h>
#include <Ok_Perio.h>
//////////////////////////////////////////////////////////////
//.DESCRIPTION class Champ_P1NC
//
// Rq : cette classe est specifique au module VEF

//.SECTION voir aussi
// Champ_Inc_base
//////////////////////////////////////////////////////////////

extern void calculer_gradientP1NC(const DoubleTab& ,
                                  const Zone_VEF&,
                                  const Zone_Cl_VEF& ,
                                  DoubleTab& );

class Champ_P1NC : public Champ_Inc_base, public Champ_P1NC_implementation
{

  Declare_instanciable(Champ_P1NC);

public :
  int fixer_nb_valeurs_nodales(int nb_noeuds);
  void   mettre_a_jour(double temps);
  virtual void abortTimeStep();
  const Zone_dis_base& zone_dis_base() const;
  void associer_zone_dis_base(const Zone_dis_base&);
  inline const Zone_VEF& zone_vef() const;
  int compo_normale_sortante(int ) const;
  DoubleTab& trace(const Frontiere_dis_base& , DoubleTab& , double ,int distant) const;
  void cal_rot_ordre1(DoubleTab&) const;
  /*extern void calculer_gradientP1NC(const DoubleTab& ,
    const Zone_VEF&,
    const Zone_Cl_VEF& ,
    DoubleTab& );*/
  void gradient(DoubleTab&) const;
  int imprime(Sortie& , int ) const ;
  void calcul_critere_Q(DoubleVect&) const;
  void calcul_y_plus(const Zone_Cl_VEF& , DoubleVect&) const;
  void calcul_grad_T(const Zone_Cl_VEF& , DoubleTab&) const;
  ////void calcul_h_conv(const Zone_Cl_VEF& , DoubleTab&);
  void calcul_h_conv(const Zone_Cl_VEF& , DoubleTab&, int temp_ref) const;

  inline DoubleVect& valeur_a_elem(const DoubleVect& position,
                                   DoubleVect& val,
                                   int le_poly) const;
  inline double valeur_a_elem_compo(const DoubleVect& position,
                                    int le_poly, int ncomp) const;
  inline double valeur_a_sommet_compo(int num_som, int le_poly, int ncomp) const;
  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                                     const IntVect& les_polys,
                                     DoubleTab& tab_valeurs) const;
  inline DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                            const IntVect& les_polys,
                                            DoubleVect& tab_valeurs,
                                            int ncomp) const ;
  inline DoubleTab& valeur_aux_elems_smooth(const DoubleTab& positions,
                                            const IntVect& les_polys,
                                            DoubleTab& tab_valeurs);
  inline DoubleVect& valeur_aux_elems_compo_smooth(const DoubleTab& positions,
                                                   const IntVect& les_polys,
                                                   DoubleVect& tab_valeurs,
                                                   int ncomp);
  inline DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const;
  //inline DoubleTab& valeur_aux_sommets_d_elem(const Domaine& dom, int poly, DoubleTab& ch_som) const;
  inline DoubleVect& valeur_aux_sommets_compo(const Domaine&,
                                              DoubleVect&, int) const;
  inline DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const;
  inline int remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                           IntVect& polys) const;
  inline void filtrer_L2(DoubleTab&) const;
  inline void filtrer_H1(DoubleTab&) const;
  inline void filtrer_resu(DoubleTab&) const;
  virtual double norme_L2(const Domaine& ) const;
  virtual Champ_base& affecter_(const Champ_base& ) ;
  //  virtual double norme_L2(const Domaine& ) const;
  //  virtual Champ_base& affecter_(const Champ_base& ) ;
  void verifie_valeurs_cl();

  static DoubleVect& calcul_S_barre(const DoubleTab&, DoubleVect&, const Zone_Cl_VEF&);
  static DoubleTab& calcul_gradient(const DoubleTab&, DoubleTab&, const Zone_Cl_VEF&);
  static DoubleTab& calcul_duidxj_paroi(DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab& , const int, const int, const Zone_Cl_VEF&);

  /////////////////////////////////////////////////
  // Fonctions rajoutees.
  //////////////////////////////////////////////////
  virtual double norme_H1(const Domaine& ) const;
  virtual double norme_L2_H1(const Domaine& dom) const;
  static double calculer_integrale_volumique(const Zone_VEF&, const DoubleVect&, Ok_Perio ok);
  //////////////////////////////////////////////////
  // Fin des fonctions rajoutees
  //////////////////////////////////////////////////
private:

  REF(Zone_VEF) la_zone_VEF;
  inline virtual const Champ_base& le_champ() const;
  inline virtual Champ_base& le_champ();

};


////////////////////////////////////////////

inline const Champ_base& Champ_P1NC::le_champ() const
{
  return *this;
}

inline Champ_base& Champ_P1NC::le_champ()
{
  return *this;
}

inline void Champ_P1NC::filtrer_L2(DoubleTab& x) const
{
  Champ_P1NC_implementation::filtrer_L2(x);
}

inline void Champ_P1NC::filtrer_H1(DoubleTab& x) const
{
  Champ_P1NC_implementation::filtrer_H1(x);
}
inline void Champ_P1NC::filtrer_resu(DoubleTab& x) const
{
  Champ_P1NC_implementation::filtrer_resu(x);
}

inline DoubleVect& Champ_P1NC::valeur_a_elem(const DoubleVect& position,
                                             DoubleVect& val,
                                             int le_poly) const
{
  return Champ_P1NC_implementation::valeur_a_elem(position, val, le_poly);
}
inline double  Champ_P1NC::valeur_a_elem_compo(const DoubleVect& position,
                                               int le_poly, int ncomp) const
{
  return Champ_P1NC_implementation::valeur_a_elem_compo(position, le_poly, ncomp);
}
inline double  Champ_P1NC::valeur_a_sommet_compo(int num_som,
                                                 int le_poly, int ncomp) const
{
  return Champ_P1NC_implementation::valeur_a_sommet_compo(num_som, le_poly, ncomp);
}
inline DoubleTab&  Champ_P1NC::valeur_aux_elems(const DoubleTab& positions,
                                                const IntVect& les_polys,
                                                DoubleTab& tab_valeurs) const
{
  return Champ_P1NC_implementation::valeur_aux_elems(positions, les_polys, tab_valeurs);
}
inline DoubleVect&  Champ_P1NC::valeur_aux_elems_compo(const DoubleTab& positions,
                                                       const IntVect& les_polys,
                                                       DoubleVect& tab_valeurs,
                                                       int ncomp) const
{
  return Champ_P1NC_implementation::valeur_aux_elems_compo(positions, les_polys,
                                                           tab_valeurs, ncomp);
}
inline DoubleTab&  Champ_P1NC::valeur_aux_elems_smooth(const DoubleTab& positions,
                                                       const IntVect& les_polys,
                                                       DoubleTab& tab_valeurs)
{
  return Champ_P1NC_implementation::valeur_aux_elems_smooth(positions, les_polys, tab_valeurs);
}
inline DoubleVect&  Champ_P1NC::valeur_aux_elems_compo_smooth(const DoubleTab& positions,
                                                              const IntVect& les_polys,
                                                              DoubleVect& tab_valeurs,
                                                              int ncomp)
{
  return Champ_P1NC_implementation::valeur_aux_elems_compo_smooth(positions, les_polys,
                                                                  tab_valeurs, ncomp);
}
inline DoubleTab&  Champ_P1NC::valeur_aux_sommets(const Domaine& dom, DoubleTab& val) const
{
  return Champ_P1NC_implementation::valeur_aux_sommets(dom, val);
}

/* inline DoubleTab& Champ_P1NC::valeur_aux_sommets_d_elem(const Domaine& dom, int poly, DoubleTab& ch_som) const */
/* { */
/*   return Champ_P1NC_implementation::valeur_aux_sommets_d_elem(dom,poly,ch_som); */
/* } */

inline DoubleVect&  Champ_P1NC::valeur_aux_sommets_compo(const Domaine& dom,
                                                         DoubleVect& val, int comp) const
{
  return Champ_P1NC_implementation::valeur_aux_sommets_compo(dom, val, comp);
}
inline DoubleTab&  Champ_P1NC::remplir_coord_noeuds(DoubleTab& positions) const
{
  return Champ_P1NC_implementation::remplir_coord_noeuds(positions);
}
inline int  Champ_P1NC::remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                                      IntVect& polys) const
{
  return Champ_P1NC_implementation::remplir_coord_noeuds_et_polys(positions,
                                                                  polys);
}

inline const Zone_VEF& Champ_P1NC::zone_vef() const
{
  return la_zone_VEF.valeur();
}

#endif
