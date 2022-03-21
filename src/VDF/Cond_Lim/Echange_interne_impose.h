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
// File:        Echange_interne_impose.h
// Directory:   $TRUST_ROOT/src/VDF/Cond_Lim
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Echange_interne_impose_included
#define Echange_interne_impose_included

#include <Echange_externe_impose.h>
#include <Ref_Champ_Don.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Echange_interne_impose:
//    Cette classe represente le cas particulier de la classe
//    Echange_externe_impose ou la paroi d'echange est interne au domaine.
//    La temperature exterieures definie dans Echange_externe_impose devient alors la
//    temperature situee de l'autre cote de la paroi interne.
//    Lorsque la température est au centre de l'élément (VDF par ex), on modifie aussi le h_gap
//    fourni pour prendre en compte la demi-maille située de l'autre côté de la paroi.
// .SECTION voir aussi
//    Echange_impose_base Echange_global_impose
//////////////////////////////////////////////////////////////////////////////
class Echange_interne_impose  : public Echange_externe_impose
{

  Declare_instanciable(Echange_interne_impose);

public:
  int initialiser(double temps) override;
  void completer() override;
  void mettre_a_jour(double tps) override;

  virtual void init();

  void verifie_ch_init_nb_comp() const override;

  // Overriden in Echange_interne_parfait to have h_gap=+infinity:
  virtual double calcul_h_imp(const double h_gap, const double invLambda) const;

  const DoubleTab& inv_lambda() const
  {
    return inv_lambda_;
  }

  // re-implemented to account for the second Champ_front in the class (h_gap_):
  //
  void  set_temps_defaut(double temps) override;
  void  fixer_nb_valeurs_temporelles(int nb_cases) override;
  void  changer_temps_futur(double temps,int i) override;
  int   avancer(double temps) override;
  int   reculer(double temps) override;
  void  associer_fr_dis_base(const Frontiere_dis_base& fr) override;
  //

protected:
  virtual void update_inv_lambda();

  Champ_front h_gap_;
  REF(Champ_Don) lambda_ref_;   // reference to the field of thermic conductivity
  DoubleTab inv_lambda_;        // = e/lambda on the internal boundary

};


#endif
