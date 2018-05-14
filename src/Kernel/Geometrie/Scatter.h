/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Scatter.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Scatter_included
#define Scatter_included

#include <Interprete.h>
#include <Ref_Domaine.h>
#include <Joint.h>

class Joints;
class DoubleTab;
class Zone_VF;
class VECT(ArrOfInt);

class Scatter : public Interprete
{
  Declare_instanciable_sans_constructeur(Scatter);
public:
  Scatter();
  Entree&         interpreter(Entree&);
  virtual void    lire_domaine(Nom&, Noms& liste_bords_periodiques);
  Domaine&        domaine();

  static int Chercher_Correspondance(const DoubleTab& sommets1,
                                     const DoubleTab& sommets2,
                                     ArrOfInt& correspondance,
                                     const double epsilon);
  static void construire_correspondance_sommets_par_coordonnees(Domaine& dom);
  static void construire_correspondance_aretes_par_coordonnees(Zone_VF& zvf);
  static void construire_correspondance_items_par_coordonnees(Joints& joints, const Joint::Type_Item type_item, const DoubleTab& coord_items);

  static void construire_structures_paralleles(Domaine& dom, const Noms& liste_bords_perio);

  //static void rechercher_elems_joints(Domaine & domaine);

  static void calculer_espace_distant(Zone&                  zone,
                                      const int           nb_items_reels,
                                      const VECT(ArrOfInt) & items_to_send,
                                      const Joint::Type_Item type_item);

  static void calculer_nb_items_virtuels(Joints& joints,
                                         const Joint::Type_Item type_item);

  static void calculer_renum_items_communs(Joints& joints, const Joint::Type_Item type_item);
  static void calculer_espace_distant_faces(Zone& zone, const int nb_faces_reelles, const IntTab& elem_faces);
  static void calculer_espace_distant_aretes(Zone& zone, const int nb_aretes_reelles, const IntTab& elem_aretes);

  static void calculer_espace_distant_elements(Domaine& dom);
  static void corriger_espace_distant_elements_perio(Domaine& dom,
                                                     const Noms& liste_bords_periodiques);

  static void calculer_espace_distant_sommets(Domaine& dom,
                                              const Noms& liste_bords_periodiques);
  static void construire_espace_virtuel_traduction(const MD_Vector& md_indice,
                                                   const MD_Vector& md_valeur,
                                                   IntTab& tableau,
                                                   const int error_is_fatal = 1);

  static void reordonner_faces_de_joint(Domaine& dom);
  static void ajouter_joints(Zone& zone, ArrOfInt& pe_voisins);

  static void trier_les_joints(Joints& joints);
  static void construire_md_vector(const Domaine&, int nb_items_reels, const Joint::Type_Item, MD_Vector&);
  static void init_sequential_domain(Domaine&);
  static void uninit_sequential_domain(Domaine&);

protected:
  REF(Domaine) le_domaine;
};

class ScatterFormatte : public Scatter
{
  Declare_instanciable_sans_constructeur(ScatterFormatte);
public :
  ScatterFormatte();
};

#endif
