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
// File:        Op_Diff_RotRot.h
// Directory:   $TRUST_ROOT/src/P1NCP1B/Operateurs
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Op_Diff_RotRot_included
#define Op_Diff_RotRot_included

#include <Operateur_Diff_base.h>
#include <Op_Curl_VEFP1B.h>
#include <Op_Rot_VEFP1B.h>

#include <Champ_base.h>
#include <Ref_Champ_Uniforme.h>
#include <Equation_base.h>
#include <Matrice_Morse.h>
#include <SolveurSys.h>
//#include <Solv_GCP.h>

class Zone_VEF_PreP1b;

class Op_Diff_RotRot : public Operateur_Diff_base
{

  Declare_instanciable(Op_Diff_RotRot);

public:

  ///////////////////////////////////////////////////
  // 5 methodes a surcharger car heritees de Op_base.
  ///////////////////////////////////////////////////
  void associer( const Zone_dis&, const Zone_Cl_dis&, const Champ_Inc&) override;
  void associer_diffusivite(const Champ_base&) override;
  const Champ_base& diffusivite() const override;
  DoubleTab& calculer( const DoubleTab&, DoubleTab&) const override;
  DoubleTab& ajouter( const DoubleTab&, DoubleTab&) const override;

  int tester() const;

  /* Fonction permettant de calculer la vorticite */
  /* a partir de la vitesse. */
  int calculer_vorticite(DoubleTab&,const DoubleTab& ) const;

  /* Assemble la matrice de vorticite */
  int assembler_matrice(Matrice&);

  /* Pour un element donne "numero_elem" retourne */
  /* la liste des sommets appartenant a cet element */
  IntList sommets_pour_element(int numero_elem) const;

  /* Pour un sommet donne "numero_som" retourne */
  /* la liste des elements contenant ce sommet */
  IntList elements_pour_sommet(int numero_som) const;

  /* Pour un sommet donne "numero_som" retourne */
  /* la liste des sommets voisins de "numero_som */
  /* Parametre: la liste des elements contenant "numero_som" */
  /* Il suffit de chercher dans ces elements pour avoir le resultat */
  /* REM: la liste resultat contient le sommet "numero_som" */
  IntList sommets_voisins(int numero_som, const IntList& liste) const;

  /* Pour l'element "numero_elem" retourne le coefficient */
  /* a placer dans la sous matrice de taille nb_elem * nb_elem */
  /* a la ligne "numero_elem" */
  /* Matrice EF */
  double remplir_elem_elem_EF(const int numero_elem) const;

  /* Pour l'element "numero_elem" retourne le coefficient */
  /* a placer dans la sous matrice de taille nb_elem * nb_som */
  /* a la ligne "numero_elem" */
  /* Matrice EF */
  double remplir_elem_som_EF(const int numero_elem,
                             const int numero_som) const;

  /* Pour le sommet "numero_som" retourne le coefficient */
  /* a placer dans la sous matrice de taille nb_som * nb_elem */
  /* a la ligne "numero_som" */
  /* Matrice EF */
  double remplir_som_elem_EF(const int numero_elem,
                             const int numero_som) const;

  /* Pour l'element "numero_elem" retourne le coefficient */
  /* a placer dans la sous matrice de taille nb_elem * nb_som */
  /* a la ligne "numero_som" */
  /* "IntList" est le tableau des elements qui contiennent "numero_som" */
  /* Matrice EF */
  double remplir_som_som_EF(const int numero_som,
                            const int sommet_voisin,
                            const IntList&) const;

  /* Fonction de tri d'une IntList */
  /* Le tri s'effectue par ordre croissant */
  void Tri(IntList& liste_a_trier) const;

  //Methode pour rendre le vecteur normal a la "face" de l'element "elem"
  DoubleTab vecteur_normal(const int face, const int elem) const;

  const Zone_VEF_PreP1b& zone_Vef() const;

protected:

  mutable Champ_Inc vorticite_;
  Matrice matrice_vorticite_;

  Op_Rot_VEFP1B rot_;
  Op_Curl_VEFP1B curl_;

  SolveurSys solveur_;
  //mutable Solv_GCP solveur_;

  REF(Zone_VEF) la_zone_vef;
  REF(Zone_Cl_VEF) la_zcl_vef;
  REF(Champ_Uniforme) diffusivite_;

};


#endif
