/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Turbulence_hyd_sous_maille_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Turbulence_hyd_sous_maille_VDF_included
#define Turbulence_hyd_sous_maille_VDF_included



#include <Mod_turb_hyd_ss_maille_VDF.h>

// .DESCRIPTION classe Turbulence_hyd_sous_maille_VDF
// Cette classe correspond a la mise en oeuvre du modele sous
// maille fonction de structure en VDF
//

// .SECTION  voir aussi
// Mod_turb_hyd_ss_maille

class Turbulence_hyd_sous_maille_VDF : public Mod_turb_hyd_ss_maille_VDF
{

  Declare_instanciable_sans_constructeur(Turbulence_hyd_sous_maille_VDF);

public:

  inline Turbulence_hyd_sous_maille_VDF();
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);

protected :

  virtual Champ_Fonc& calculer_viscosite_turbulente();
  virtual void calculer_energie_cinetique_turb();
  virtual void calculer_fonction_structure();

  int nb_points;    // nb_points=4 ou 6 selon que nous utilisons la formulation de la FST en 4 ou 6 points!!
  int dir1,dir2; // direction du plan dans lequel on veut calculer la FST en 4 points
  int dir3; // 3eme direction!!

  DoubleVect F2;

  double Csm1_; // constante du modele (differente d'une classe fille a l'autre)
  double Csm2_; // constante pour calcul de l'energie ( idem )
};


inline Turbulence_hyd_sous_maille_VDF::Turbulence_hyd_sous_maille_VDF()
{
  nb_points=6;
  Csm1_ = CSM1;
  Csm2_ = CSM2;
}

#endif
