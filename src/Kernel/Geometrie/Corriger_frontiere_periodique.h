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
#ifndef Corriger_frontiere_periodique_included
#define Corriger_frontiere_periodique_included

#include <Interprete_geometrique_base.h>
#include <Connectivite_som_elem.h>
#include <Domaine_forward.h>
#include <Domaine_bord.h>
#include <Octree_Double.h>
#include <ArrOfBit.h>

/*! @brief Cet interprete doit etre utilise en sequentiel (avant decoupage) si les sommets opposes d'un bord periodique ne sont pas parfaitement alignes.
 *
 *   (cas de certains maillages tetra si le mailleur est trop contraint par la CAO).
 *   Il tente de deplacer les sommets pour les aligner.
 *
 *  Cet interprete permet de corriger les frontieres periodiques pour etre conformes aux besoins de TRUST:
 *    - reordonner les faces du bord periodique pour que la face i+n/2 soit en face de la face i,
 *      et toutes les faces [0 .. n/2-1] du meme cote et [n/2 .. n-1] de l'autre cote
 *    - deplacer les sommets des faces periodiques si besoin (si la CAO est fausse)
 *  Syntaxe:
 *   Corriger_frontiere_periodique {
 *      domaine NOMDOMAINE
 *      bord    NOMBORDPERIO
 *      [ direction DIMENSION dx dy [ dz ] ]
 *      [ fichier_post BASENAME ]
 *   }
 */
template <typename _SIZE_>
class Corriger_frontiere_periodique_32_64 : public Interprete_geometrique_base_32_64<_SIZE_>
{
  Declare_instanciable_32_64(Corriger_frontiere_periodique_32_64);
public:
  using int_t = _SIZE_;
  using IntTab_t = IntTab_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;
  using ArrOfDouble_t = ArrOfDouble_T<_SIZE_>;

  using ArrOfBit_t = ArrOfBit_32_64<_SIZE_>;
  using Octree_Double_t = Octree_Double_32_64<_SIZE_>;
  using Bord_t = Bord_32_64<_SIZE_>;
  using Domaine_bord_t = Domaine_bord_32_64<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;

  Entree& interpreter_(Entree& is) override;
  static void corriger_coordonnees_sommets_perio(Domaine_t& dom, const Nom& nom_bord, const ArrOfDouble& vecteur_perio, const Nom& nom_fichier_post);
};

using Corriger_frontiere_periodique = Corriger_frontiere_periodique_32_64<int>;
using Corriger_frontiere_periodique_64 = Corriger_frontiere_periodique_32_64<trustIdType>;

#endif
