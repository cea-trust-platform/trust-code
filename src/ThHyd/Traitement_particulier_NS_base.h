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
// File:        Traitement_particulier_NS_base.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_NS_base_included
#define Traitement_particulier_NS_base_included

#include <Ref_Navier_Stokes_std.h>
#include <DoubleTab.h>
#include <Ref_Champ_base.h>
#include <Motcle.h>
#include <Support_Champ_Masse_Volumique.h>
#include <Champs_compris.h>
#include <Champs_compris_interface.h>

class Equation_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Traitement_particulier_NS_base
//     Derive de Support_Champ_Masse_Volumique: utilisation de rho
//     pour le calcul de l'energie cinetique par exemple (front-tracking)
//     voir Traitement_particulier_NS_EC
// .SECTION voir aussi
//      Navier_Stokes_Standard
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_base : public Objet_U,
  public Support_Champ_Masse_Volumique,
  public Champs_compris_interface
{
  Declare_base(Traitement_particulier_NS_base);

public :

  //ALEX
  virtual void associer_eqn(const Equation_base& );
  //ALEX
  virtual void preparer_calcul_particulier(void) =0;
  virtual void post_traitement_particulier(void) =0;
  virtual Entree& lire(Entree& is) = 0;
  virtual void sauver_stat(void)  const = 0;
  virtual void reprendre_stat(void)  = 0;
  virtual void en_cours_de_resolution(int , DoubleTab& , DoubleTab& ,double)  = 0;

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle&);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

protected :

  REF(Navier_Stokes_std) mon_equation;

  Champs_compris champs_compris_;

};
#endif
