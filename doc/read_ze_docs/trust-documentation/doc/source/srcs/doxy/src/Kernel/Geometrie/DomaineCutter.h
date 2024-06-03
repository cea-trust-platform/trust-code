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

#ifndef DomaineCutter_included
#define DomaineCutter_included

#include <DomaineCutter_Correspondance.h>
#include <Static_Int_Lists.h>
#include <TRUST_Ref.h>
#include <Decouper.h>
#include <Noms.h>

class Domaine;

/*! @brief Classe outil permettant de generer des sous-domaines pour un calcul parallele a partir d'un domaine de depart (domaine_global) et d'un tableau de decoupage
 *
 *   des elements de ce domaine (elem_part).
 *   Voir aussi Partitionneur_base.
 *
 */
class DomaineCutter: public Objet_U
{
  Declare_instanciable(DomaineCutter);
public:
  void initialiser(const Domaine& domaine_global, const IntVect& elem_part, const int nb_parts, const int epaisseur_joint, const Noms& bords_periodiques, const int permissif = 0);

  void reset();

  void construire_sous_domaine(const int part, DomaineCutter_Correspondance& correspondance_, Domaine& sous_domaine_, const Static_Int_Lists *som_raccord = NULL) const;
  void construire_sous_domaine(const int part, Domaine& sous_domaine_) const
  {
    DomaineCutter_Correspondance correspondance;
    construire_sous_domaine(part, correspondance, sous_domaine_);
  }

  void ecrire_domaines(const Nom& basename, const Decouper::DomainesFileOutputType format, IntVect& elem_part, const int reorder, const Static_Int_Lists *som_raccord = NULL);
  inline const Noms& bords_internes() const { return bords_a_pb_; }

private:
  void construire_faces_bords_ssdom(const ArrOfInt& liste_inverse_sommets, const int partie, Domaine& domaine_partie) const;
  void construire_faces_raccords_ssdom(const ArrOfInt& liste_inverse_sommets, const int partie, Domaine& domaine_partie) const;
  void construire_frontieres_internes_ssdom(const ArrOfInt& liste_inverse_sommets, const int partie, Domaine& domaine_partie) const;
  void construire_groupe_faces_ssdom(const ArrOfInt& liste_inverse_sommets, const int partie, Domaine& domaine_partie) const;
  void construire_sommets_joints_ssdom(const ArrOfInt& liste_sommets, const ArrOfInt& liste_inverse_sommets, const int partie, const Static_Int_Lists *som_raccord, Domaine& domaine_partie) const;

  void construire_faces_joints_ssdom(const int partie, const DomaineCutter_Correspondance& correspondance, Domaine& domaine_partie) const;

  void construire_elements_distants_ssdom(const int partie, const ArrOfInt& liste_sommets, const ArrOfInt& liste_inverse_elements, Domaine& domaine_partie) const;

  void writeData(const Domaine& sous_domaine, Sortie& os) const;

  // Ne pas utiliser ces deux methodes:
  const DomaineCutter& operator=(const DomaineCutter& dc);
  DomaineCutter(const DomaineCutter& dc);

  // Reference au domaine global
  REF(Domaine) ref_domaine_;
  // Reference au tableau de decoupage
  //  (pour chaque element du domaine global, numero de sous-domaine)
  REF(IntVect) ref_elem_part_;
  // Liste des noms des bords periodiques
  Noms liste_bords_periodiques_;
  // Nombre total de parties (>= a max_array(elem_part) + 1)
  int nb_parties_ = -1;
  // Epaisseur du joint
  int epaisseur_joint_ = -1;
  // Connectivite sommets_elements du domaine global:
  Static_Int_Lists som_elem_;
  // Pour chaque partie, liste des elements du domaine source de cette partie
  Static_Int_Lists liste_elems_sous_domaines_;
  // Pour chaque frontiere du domaine source, liste des elements adjacent a
  // chaque face de la frontiere
  Static_Int_Lists voisins_bords_;
  Noms bords_a_pb_;
};
#endif
