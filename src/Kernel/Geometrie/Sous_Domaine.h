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

#ifndef Sous_Domaine_included
#define Sous_Domaine_included

#include <TRUST_List.h>
#include <TRUSTVect.h>
#include <TRUST_Ref.h>

class Domaine;

/*! @brief Classe Sous_Domaine Represente une sous domaine volumique i.
 *
 * e un sous ensemble
 *     de polyedres d'un objet de type Domaine.
 *     Un objet Sous_Domaine porte une reference vers le domaine qu'il
 *     subdivise.
 *
 * @sa Domaine Sous_Domaines
 */
class Sous_Domaine : public Objet_U
{
  Declare_instanciable(Sous_Domaine);

public :

  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  inline const Nom& le_nom() const override;
  inline int operator()(int) const;
  inline int operator[](int) const;
  inline int nb_elem_tot() const; // Returns the subdomaine number of elements (real+virtual elements in parallel)
  void associer_domaine(const Domaine&);
  int associer_(Objet_U&) override;
  void nommer(const Nom&) override;
  inline Domaine& domaine()
  {
    return le_dom_.valeur();
  }
  inline const Domaine& domaine() const
  {
    return le_dom_.valeur();
  }
  int add_poly(const int poly);
  int remove_poly(const int poly);
  inline const IntVect& les_polys()
  {
    return les_polys_;
  }
protected :

  IntVect les_polys_;
  REF(Domaine) le_dom_;
  Nom nom_;
};

/*! @brief Renvoie le nom de la sous-domaine.
 *
 * @return (Nom&) le nom de la sous-domaine
 */
inline const Nom& Sous_Domaine::le_nom() const
{
  return nom_;
}


/*! @brief Renvoie le numero du i-ieme polyedre de la sous-domaine.
 *
 * @return (int) le numero du i-ieme polyedre de la sous-domaine
 */
inline int Sous_Domaine::operator()(int i) const
{
  return les_polys_[i];
}


/*! @brief Renvoie le numero du i-ieme polyedre de la sous-domaine.
 *
 * @return (int) le numero du i-ieme polyedre de la sous-domaine
 */
inline int Sous_Domaine::operator[](int i) const
{
  return les_polys_[i];
}


/*! @brief Renvoie le nombre de polyedre de la sous-domaine.
 *
 * @return (int) le nombre de polyedre de la sous-domaine
 */
inline int Sous_Domaine::nb_elem_tot() const
{
  return les_polys_.size();
}

#endif

