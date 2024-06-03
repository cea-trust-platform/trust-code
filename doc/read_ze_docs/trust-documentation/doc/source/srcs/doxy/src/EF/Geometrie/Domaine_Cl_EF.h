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

#ifndef Domaine_Cl_EF_included
#define Domaine_Cl_EF_included



/*! @brief class Domaine_Cl_EF Cette classe porte les tableaux qui servent a mettre en oeuvre
 *
 *   les condition aux limites dans la formulation EF
 *
 *
 * @sa Domaine_Cl_dis_base
 */
#include <Domaine_Cl_dis_base.h>
#include <Champ_Don.h>


class Champ_Inc;
class Domaine_EF;
class Matrice_Morse;

class Domaine_Cl_EF : public Domaine_Cl_dis_base
{

  Declare_instanciable(Domaine_Cl_EF);

public :

  void associer(const Domaine_EF& );
  void completer(const Domaine_dis& ) override;
//  void mettre_a_jour(double );
  int initialiser(double temps) override;
  void imposer_cond_lim(Champ_Inc&, double) override;

  void imposer_symetrie(DoubleTab&,int tous_les_sommets_sym=0) const;
  void imposer_symetrie_partiellement(DoubleTab&,const Noms&) const;
  void imposer_symetrie_matrice_secmem(Matrice_Morse& la_matrice, DoubleTab& secmem) const;

  void modifie_gradient(ArrOfDouble& grad_mod,const ArrOfDouble& grad,int num_som) const;


  int nb_faces_sortie_libre() const;
  Domaine_EF& domaine_EF();
  const Domaine_EF& domaine_EF() const;

  int nb_bord_periodicite() const;
  inline  const ArrOfInt& get_type_sommet() const
  {
    return type_sommet_ ;
  };
protected:

  // Attributs:

  int modif_perio_fait_=0;

  // Fonctions de creation des membres prives du domaine:

  void remplir_type_elem_Cl(const Domaine_EF& );
  ArrOfInt type_sommet_;  // -1 interne 0 Neumann 1 Symetrie >2 Dirichlet
  // Un sommet est d'abord diri , puis Symetrie , puis Neumann , puis interne
  Champ_Don normales_symetrie_,normales_symetrie_bis_,normales_symetrie_ter_;
};

//
// Fonctions inline de la classe Domaine_Cl_EF
//





#endif
