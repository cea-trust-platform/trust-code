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
// File:        Periodique.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Periodique_included
#define Periodique_included

#include <Cond_lim_base.h>
#include <TRUSTTab.h>

class Faces;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Periodique
//     Cette classe represente une condition aux limites periodique.
//     On peut specifier une periodicite en X, Y ou Z.
//     Toutes les faces de la frontiere associee a cette condition doivent
//     avoir la meme orientation
// .SECTION voir aussi
//     Cond_lim_base
//     Toutes les faces de la frontiere associee a cette condition doivent
//     avoir la meme orientation
//////////////////////////////////////////////////////////////////////////////
class Periodique  : public Cond_lim_base
{

  Declare_instanciable(Periodique);

public:

  void mettre_a_jour(double temps) override;
  int face_associee(int i) const
  {
    return face_front_associee_[i];
  }
  double distance() const;
  inline const ArrOfDouble& direction_perio() const
  {
    return direction_perio_;
  };
  int direction_periodicite() const;
  inline int est_periodique_selon_un_axe() const
  {
    return  direction_xyz_ >= 0;
  };

protected:
  // Tableau de taille nb_faces() + nb_faces_virt()
  // face_front_associee_[i] est l'indice de la face opposee sur cette frontiere
  // (vaut -1 si la face opposee n'existe pas pour une face virtuelle)
  ArrOfInt face_front_associee_;
  ArrOfDouble direction_perio_;
  double distance_;
  // -1, 0, 1, ou 2
  int direction_xyz_;
  int compatible_avec_eqn(const Equation_base&) const override;
  void   completer() override;
};

#endif
