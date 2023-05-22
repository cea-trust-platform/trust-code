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
#include <set>

class Domaine;

/*! @brief Classe Sous_Domaine Represente un sous domaine volumique i.
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
  inline const Nom& le_nom() const override { return nom_; }
  inline int operator()(int) const;
  inline int operator[](int) const;
  inline int nb_elem_tot() const; // Returns the subdomaine number of elements (real+virtual elements in parallel)
  void associer_domaine(const Domaine&);
  int associer_(Objet_U&) override;
  void nommer(const Nom& nom) override { nom_ = nom; }
  inline Domaine& domaine() { return le_dom_.valeur(); }
  inline const Domaine& domaine() const { return le_dom_.valeur(); }
  int add_poly(const int poly);
  int remove_poly(const int poly);
protected :

  std::set<int> les_polys_;
  REF(Domaine) le_dom_;
  Nom nom_;

private:
  void check_dimension(int d, const std::string m, const std::string mm);
  void read_polynomials(const std::set<int>& les_polys_possibles_, Entree& is);
  void read_box(const std::set<int>& les_polys_possibles_, Entree& is);
  void read_rectangle(const std::set<int>& les_polys_possibles_, Entree& is);
  void read_interval(Entree& is);
  void read_in_file(Entree& is);
  void read_plaque(const std::set<int>& les_polys_possibles_, Entree& is);
  void read_segment(const std::set<int>& les_polys_possibles_, Entree& is);
  void read_couronne(const std::set<int>& les_polys_possibles_, Entree& is);
  void read_tube(const std::set<int>& les_polys_possibles_, Entree& is);
  void read_hexagonal_tube(const std::set<int>& les_polys_possibles_, Entree& is);
  void read_function(const std::set<int>& les_polys_possibles_, Entree& is);
  void read_union(Entree& is);
  template <typename F> void get_polys_generique(const std::set<int>& les_polys_possibles_, const F& func);
};

/*! @brief Renvoie le numero du i-ieme polyedre du sous-domaine.
 *
 * @return (int) le numero du i-ieme polyedre du sous-domaine
 */
inline int Sous_Domaine::operator()(int i) const
{
  return *next(les_polys_.begin(), i);
}


/*! @brief Renvoie le numero du i-ieme polyedre du sous-domaine.
 *
 * @return (int) le numero du i-ieme polyedre du sous-domaine
 */
inline int Sous_Domaine::operator[](int i) const
{
  return *next(les_polys_.begin(), i);
}


/*! @brief Renvoie le nombre de polyedre du sous-domaine.
 *
 * @return (int) le nombre de polyedre du sous-domaine
 */
inline int Sous_Domaine::nb_elem_tot() const
{
  return (int)les_polys_.size();
}

#endif

