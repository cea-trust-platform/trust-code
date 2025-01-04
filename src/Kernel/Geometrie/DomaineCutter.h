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

#ifndef DomaineCutter_included
#define DomaineCutter_included

#include <DomaineCutter_Correspondance.h>
#include <Static_Int_Lists.h>
#include <TRUST_Ref.h>
#include <Decouper.h>
#include <Noms.h>

#include <Domaine_forward.h>

/*! @brief Classe outil permettant de generer des sous-domaines pour un calcul parallele a partir
 *  d'un domaine de depart (domaine_global) et d'un tableau de decoupage des elements de ce domaine (elem_part).
 *
 *   @sa Partitionneur_base.
 */
template <typename _SIZE_>
class DomaineCutter_32_64: public Objet_U
{
  Declare_instanciable_32_64(DomaineCutter_32_64);
public:

  using int_t = _SIZE_;
  using ArrOfInt_t = TRUSTArray<_SIZE_, _SIZE_>;
  using BigArrOfInt_t = BigArrOfInt_T<_SIZE_>;  // always storing 'int' (=proc num) but might have a lot of entries
  using BigIntVect_t = BigIntVect_T<_SIZE_>;    // same as above, but parallel
  using SmallArrOfTID_t = SmallArrOfTID_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;
  using Static_Int_Lists_t = Static_Int_Lists_32_64<_SIZE_>;

  using Domaine_t = Domaine_32_64<_SIZE_>;
  using Domaine32 = Domaine;  // To insist on the fact that we are using *both* variant of Domains. This type is typically for (small) partial domains.
  using Sous_Domaine_t = Sous_Domaine_32_64<_SIZE_>;
  using DomaineCutter_Correspondance_t = DomaineCutter_Correspondance_32_64<_SIZE_>;
  using Frontiere_t = Frontiere_32_64<_SIZE_>;
  using Decouper_t = Decouper_32_64<_SIZE_>;

  void initialiser(const Domaine_t& domaine_global, const BigIntVect_t& elem_part, const int nb_parts, const int epaisseur_joint,
                   const Noms& bords_periodiques, const bool permissif=false);
  void reset();

  // NOTE: in the API below we use both
  //   - Domain_t -> which might be 64b. THis is typically the initial (big) unsplitted domain.
  //   - Domaine32 -> which is always a 32b type. This is typically used for sub-domains, which will always fit in the 32b limit
  void construire_sous_domaine(const int part, DomaineCutter_Correspondance_t& correspondance, Domaine32& sous_domaine, const Static_Int_Lists_t *som_raccord=nullptr) const;
  void construire_sous_domaine(const int part, Domaine32& sous_domaine) const
  {
    DomaineCutter_Correspondance_t correspondance;
    construire_sous_domaine(part, correspondance, sous_domaine);
  }

  void ecrire_domaines(const Nom& basename, const DomainesFileOutputType format, const int reorder, const Static_Int_Lists_t *som_raccord=nullptr);
  inline const Noms& bords_internes() const { return bords_a_pb_; }

private:
  void construire_faces_bords_ssdom(const BigArrOfInt_t& liste_inverse_sommets, const int partie, Domaine32& domaine_partie) const;
  void construire_faces_raccords_ssdom(const BigArrOfInt_t& liste_inverse_sommets, const int partie, Domaine32& domaine_partie) const;
  void construire_frontieres_internes_ssdom(const BigArrOfInt_t& liste_inverse_sommets, const int partie, Domaine32& domaine_partie) const;
  void construire_groupe_faces_ssdom(const BigArrOfInt_t& liste_inverse_sommets, const int partie, Domaine32& domaine_partie) const;
  void construire_sommets_joints_ssdom(const SmallArrOfTID_t& liste_sommets, const BigArrOfInt_t& liste_inverse_sommets, const int partie,
                                       const Static_Int_Lists_t *som_raccord, Domaine32& domaine_partie) const;

  void construire_faces_joints_ssdom(const int partie, const DomaineCutter_Correspondance_t& correspondance, Domaine32& domaine_partie) const;

  void construire_elements_distants_ssdom(const int partie, const SmallArrOfTID_t& liste_sommets, const BigArrOfInt_t& liste_inverse_elements, Domaine32& domaine_partie) const;

  void writeData(const Domaine32& sous_domaine, Sortie& os) const;

  void ajouter_joints(Domaine32& domaine, const ArrOfInt& voisins) const;
  void parcourir_epaisseurs_elements(SmallArrOfTID_t liste_sommets_depart, const int partie_a_ignorer, SmallArrOfTID_t& liste_elements_trouves) const;


  // Ne pas utiliser ces deux methodes:
  const DomaineCutter_32_64& operator=(const DomaineCutter_32_64& dc) = delete;
  DomaineCutter_32_64(const DomaineCutter_32_64& dc) : Objet_U() { abort(); Process::exit(-1); } // Can't use delete because of TRUST macros ...

  // Reference au domaine global
  OBS_PTR(Domaine_t) ref_domaine_;
  // Reference au tableau de decoupage
  //  (pour chaque element du domaine global, numero de sous-domaine)
  OBS_PTR(BigIntVect_t) ref_elem_part_;
  // Liste des noms des bords periodiques
  Noms liste_bords_periodiques_;
  // Nombre total de parties (>= a max_array(elem_part) + 1)
  int nb_parties_ = -1;
  // Epaisseur du joint
  int epaisseur_joint_ = -1;
  // Connectivite sommets_elements du domaine global:
  Static_Int_Lists_t som_elem_;
  // Pour chaque partie, liste des elements du domaine source de cette partie
  Static_Int_Lists_t liste_elems_sous_domaines_;
  // Pour chaque frontiere du domaine source, liste des elements adjacent a
  // chaque face de la frontiere
  Static_Int_Lists_t voisins_bords_;
  Noms bords_a_pb_;
};

using DomaineCutter = DomaineCutter_32_64<int>;
using DomaineCutter_64 = DomaineCutter_32_64<trustIdType>;

#endif
