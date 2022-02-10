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
// File:        Parametre_diffusion_implicite.h
// Directory:   $TRUST_ROOT/src/Kernel/Solveurs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Parametre_diffusion_implicite_included
#define Parametre_diffusion_implicite_included



#include <Parametre_equation_base.h>
#include <SolveurSys.h>
#include <Matrice_Morse.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Parametre_diffusion_implicite
//     Un objet Parametre_diffusion_implicite est un objet regroupant les differentes
//     options specifiques pour diffusion_implicite
//
//////////////////////////////////////////////////////////////////////////////
class Parametre_diffusion_implicite : public Parametre_equation_base
{
  Declare_instanciable(Parametre_diffusion_implicite);
public:
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  inline const double& seuil_diffusion_implicite() const
  {
    return seuil_diffusion_implicite_ ;
  };
  inline const int& nb_it_max() const
  {
    return nb_it_max_ ;
  };
  inline const int& crank() const
  {
    return crank_ ;
  };
  inline const int& precoditionnement_diag() const
  {
    return  preconditionnement_diag_ ;
  };
  inline SolveurSys& solveur() { return solveur_; }
  inline Matrice_Morse& matrice() { return matrice_; }
protected:
  int preconditionnement_diag_ ;
  int crank_;
  int nb_it_max_;
  double seuil_diffusion_implicite_;
  SolveurSys solveur_;  // Solveur utilise pour la resolution diffusion implicite
  Matrice_Morse matrice_; // Matrice pour  la resolution diffusion implicite
};
#endif
