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
// File:        Champ_P1NC_implementation.h
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/26
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_P1NC_implementation_included
#define Champ_P1NC_implementation_included

#include <Champ_implementation.h>
#include <Matrice_Morse_Sym.h>
#include <SolveurSys.h>
#include <Champ_implementation_P1.h>
#include <Equation_base.h>
#include <Frontiere_dis_base.h>

class Zone_VEF;
#include <TRUSTTab.h>
class Champ_P1NC;
class Champ_Fonc_P1NC;
//
class Champ_P1NC_implementation: public Champ_implementation
{

public:

  Champ_P1NC_implementation();
  ~Champ_P1NC_implementation() override {};

  int fixer_nb_valeurs_nodales(int);

  inline double fonction_forme_2D(double x, double y, int le_poly,
                                  int face, const IntTab& sommet_poly,
                                  const DoubleTab& coord) const
  {
    return 1 - 2 * coord_barycentrique_P1_triangle(sommet_poly, coord, x, y, le_poly, face);
  }

  inline double fonction_forme_3D(double x, double y, double z,
                                  int le_poly, int face,
                                  const IntTab& sommet_poly,
                                  const DoubleTab& coord) const
  {
    return 1 - 3 * coord_barycentrique_P1_tetraedre(sommet_poly, coord, x, y, z, le_poly, face);
  }

  inline double coord_barycentrique(const IntTab& sommet_poly, const DoubleTab& coord,
                                    double x, double y,
                                    int le_poly, int face) const
  {
    return coord_barycentrique_P1_triangle(sommet_poly, coord, x, y, le_poly, face);
  }

  inline double coord_barycentrique(const IntTab& sommet_poly, const DoubleTab& coord,
                                    double x, double y, double z,
                                    int le_poly, int face) const
  {
    return coord_barycentrique_P1_tetraedre(sommet_poly, coord, x, y, z, le_poly, face);
  }


  // Retourne dans val la valeur du champ interpole aux coordonnees position de l'element le_poly
  DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& val, int le_poly) const override;
  // Retourne la valeur de la composante ncomp du champ interpole aux coordonnees position de l'element le_poly
  double valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const override;
  // Retourne dans valeurs les valeurs du champ interpolees aux coordonnees positions des elements les_polys
  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const override;
  // Retourne les valeurs de la composante ncomp du champ interpolees aux coordonnees positions des elements les_polys
  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& valeurs, int ncomp) const override ;
  // Retourne dans ch_som les valeurs du champ interpole aux sommets
  DoubleTab& valeur_aux_sommets(const Domaine& dom, DoubleTab& ch_som) const override;
  // Retourne dans ch_som les valeurs de la composante ncomp du champ interpole aux sommets
  DoubleVect& valeur_aux_sommets_compo(const Domaine& dom, DoubleVect& ch_som, int ncomp) const override;
  // Retourne la valeur de la composante ncomp du champ au sommet num_som sur l'element le_poly
  double valeur_a_sommet_compo(int num_som, int le_poly, int ncomp) const;

  DoubleTab& valeur_aux_elems_smooth(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs);
  DoubleVect& valeur_aux_elems_compo_smooth(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& valeurs, int ncomp);

  DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override;
  int remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                    IntVect& polys) const override;
  int imprime_P1NC(Sortie&, int) const;
  void filtrer_L2(DoubleTab& ) const;
  void filtrer_H1(DoubleTab& ) const;
  void filtrer_resu(DoubleTab& ) const;

  DoubleTab& ch_som()
  {
    return ch_som_;
  };
  DoubleVect& ch_som_vect()
  {
    return ch_som_vect_;
  };

  const Matrice_Morse_Sym& get_MatP1NC2P1_L2() const;
  const SolveurSys& get_solveur_L2() const;

  //////////////////////////////////////////////////
  // Fonctions rajoutees.
  /////////////////////////////////////////////////
  void filtrer_L2_H1(DoubleTab&) const;

  const Matrice_Morse_Sym& get_MatP1NC2P1_L2_H1() const;
  const Matrice_Morse_Sym& get_MatP1NC2P1_H1() const;

  const SolveurSys& get_solveur_L2_H1() const;
  const SolveurSys& get_solveur_H1() const;

  //  int construire_MatP1NC2P1_L2(Champ_P1NC&,const Domaine&);
  //  int construire_MatP1NC2P1_H1(Champ_P1NC&,const Domaine&,const DoubleTab&,double&);

  //  int construire_secmem_L2(Champ_P1NC&,const Domaine&,DoubleTab&) const;
  //  int construire_secmem_H1(Champ_P1NC&,const Domaine&,DoubleTab&,
  //                           const DoubleTab&,const double&) const;

  /////////////////////////////////////////////////
  // Fin des fonctions rajoutees.
  /////////////////////////////////////////////////


  int nb_colonnes_tot();
  int nb_colonnes();
  void dimensionner_Mat_Bloc_Morse_Sym(Matrice& matrice_tmp);
  void Mat_Morse_to_Mat_Bloc(Matrice& matrice_tmp);

protected :

  Matrice_Morse_Sym MatP1NC2P1_L2;
  // Matrice pour le parallele
  Matrice MatP1NC2P1_L2_Parallele;

  SolveurSys  solveur_L2;
  Matrice_Morse_Sym MatP1NC2P1_H1;
  SolveurSys  solveur_H1;
  virtual const Zone_VEF& zone_vef() const=0;
  friend DoubleTab& valeur_P1_L2(Champ_P1NC& ,
                                 const Domaine&);
  friend DoubleTab& valeur_P1_L2(Champ_Fonc_P1NC& ,
                                 const Domaine&);
  friend DoubleTab& valeur_P1_H1(const Champ_P1NC& ,
                                 const Domaine& ,
                                 DoubleTab& );
  DoubleTab ch_som_;
  DoubleVect ch_som_vect_;
  int filtrer_L2_deja_appele_;

  //////////////////////////////////////////////////////////
  // Membres rajoutes
  //////////////////////////////////////////////////////////
  friend int construire_MatP1NC2P1_L2(Champ_P1NC&,const Domaine&);
  friend int construire_MatP1NC2P1_H1(Champ_P1NC&,const Domaine&,double&);

  friend int construire_secmem_L2(Champ_P1NC&,const Domaine&,DoubleTab&);
  friend int construire_secmem_H1(Champ_P1NC&,const Domaine&,DoubleTab&,
                                  const double&);

  Matrice_Morse_Sym MatP1NC2P1_L2_H1;
  SolveurSys solveur_L2_H1;

  friend DoubleTab& valeur_P1_L2_H1(Champ_P1NC&,const Domaine&,DoubleTab& );
  friend int test(Champ_P1NC&,const Domaine&);

  inline DoubleTab& trace(const Frontiere_dis_base& fr, const DoubleTab& y, DoubleTab& x,int distant) const;
  //////////////////////////////////////////////////////////////
  // FIn des membres rajoutes
  //////////////////////////////////////////////////////////////
};


inline const Matrice_Morse_Sym& Champ_P1NC_implementation::get_MatP1NC2P1_L2() const
{
  return MatP1NC2P1_L2;
}

inline const Matrice_Morse_Sym& Champ_P1NC_implementation::get_MatP1NC2P1_H1() const
{
  return MatP1NC2P1_H1;
}

inline const Matrice_Morse_Sym& Champ_P1NC_implementation::get_MatP1NC2P1_L2_H1() const
{
  return MatP1NC2P1_L2_H1;
}

inline const SolveurSys& Champ_P1NC_implementation::get_solveur_L2() const
{
  return solveur_L2;
}

inline const SolveurSys& Champ_P1NC_implementation::get_solveur_H1() const
{
  return solveur_H1;
}

inline const SolveurSys& Champ_P1NC_implementation::get_solveur_L2_H1() const
{
  return solveur_L2_H1;
}

inline DoubleTab& Champ_P1NC_implementation::trace(const Frontiere_dis_base& fr, const DoubleTab& y, DoubleTab& x,int distant) const
{
  if (distant)
    fr.frontiere().trace_face_distant(y, x);
  else
    {
      fr.frontiere().trace_face_local(y, x);
      x.echange_espace_virtuel();
    }
  return x;
}
#endif
