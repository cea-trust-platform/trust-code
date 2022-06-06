/****************************************************************************
* Copyright (c) 2017, CEA
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

#ifndef Operateur_included
#define Operateur_included

#include <Discretisation.h>
#include <Ref_Champ_Inc_base.h>
#include <Ref_Champ_Inc.h>
#include <Champ_Inc.h>
#include <Motcle.h>

class Operateur_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Operateur
//     Classe generique de la hierarchie des operateurs.
//     Cette classe generique est particuliere car elle n'HERITE PAS
//     de DERIV(Operateur_base) mais de MorEqn.
//     Une reference sur un Operateur_base est renvoye par la methode
//     Operateur::l_op_base() qui est virtuelle pure et doit etre surchargee
//     dans ses derivees.
// .SECTION voir aussi
//     MorEqn Operateur_base
//     Classe abstraite
//     Methodes abstraites:
//       Operateur_base& l_op_base()
//       const Operateur_base& l_op_base() const
//       DoubleTab& calculer(const DoubleTab&,DoubleTab& ) const
//       DoubleTab& calculer(const DoubleTab&,DoubleTab& ) const
//////////////////////////////////////////////////////////////////////////////
class Operateur : public MorEqn
{
public :

  virtual Operateur_base& l_op_base()=0;
  virtual const Operateur_base& l_op_base() const=0;
  virtual DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const=0;
  virtual DoubleTab& calculer(const DoubleTab&,DoubleTab& ) const=0;
  DoubleTab& ajouter(const Champ_Inc&, DoubleTab& ) const;
  DoubleTab& calculer(const Champ_Inc&,DoubleTab& ) const;
  DoubleTab& ajouter(DoubleTab& ) const;
  DoubleTab& calculer(DoubleTab& ) const;
  const Nom& type() const;
  double calculer_pas_de_temps() const;
  void calculer_pas_de_temps_locaux(DoubleTab&) const; //Local time step calculation
  int impr(Sortie& os) const;

  virtual void typer()=0;
  virtual void completer();
  virtual void mettre_a_jour(double temps);
  const Champ_Inc& mon_inconnue() const;
  const Discretisation_base& discretisation() const;
  int limpr() const;
  void imprimer(Sortie& os) const;
  Sortie& ecrire(Sortie& ) const;
  Entree& lire(Entree& );
  void ajouter_contribution_explicite_au_second_membre(const Champ_Inc_base& inconnue, DoubleTab& derivee) const;
  void associer_champ(const Champ_Inc&);

  void set_fichier(const Nom& nom);
  void set_description(const Nom& nom);
  //Methode non_nul declaree car Operateur n est pas un DERIV de Operateur_base
  //et n accede pas a la methode non_nul
  virtual int op_non_nul() const =0;

protected :
  REF(Champ_Inc) le_champ_inco;
  Motcle typ;
};
#endif
