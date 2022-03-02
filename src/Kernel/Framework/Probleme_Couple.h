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
// File:        Probleme_Couple.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Probleme_Couple_included
#define Probleme_Couple_included


#include <Couplage_U.h>
#include <Ref_Champ_base.h>
#include <Vect_Schema_Temps.h>
#include <TRUSTArray.h>

class Schema_Temps_base;
class Discretisation_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Probleme_Couple
//     C'est la classe historique de couplage de TRUST.
//     Il s'agit d'un Couplage_U particulier.
//     Le Probleme_Couple ne couple que des Probleme_base, tous
//     associes au meme schema en temps.
//     Il possede son propre mecanisme d'echange de champs par "raccords".
//     Les problemes sont groupes. Chaque groupe fait un iteration, puis les
//     nouveaux champs sont disponibles pour les groupes suivants. A l'interieur
//     d'un groupe, ce sont les anciens champs qui sont echanges.
//
// .SECTION voir aussi
//     Probleme_base Probleme
//////////////////////////////////////////////////////////////////////////////

//  WEC :
//  Le schema en temps est clone autant de fois qu'il y a de problemes
//  Ce clonage n'est pas tres propre :
//  * ils ont tous le meme nom
//  Pour eviter ca :
//  changer la syntaxe des .data et avoir vraiment un schema par
//  probleme

class Probleme_Couple : public Couplage_U
{
  Declare_instanciable(Probleme_Couple);
public :

  ///////////////////////////////////////////////
  //                                           //
  // Implementation de l'interface de Problem  //
  //                                           //
  ///////////////////////////////////////////////

  // interface UnsteadyProblem
  bool initTimeStep(double dt) override;
  double computeTimeStep(bool& stop) const override;
  bool solveTimeStep() override;

  // interface IterativeUnsteadyProblem

  bool iterateTimeStep(bool& converged) override;

  ////////////////////////////////////////////////////////
  //                                                    //
  // Fin de l'implementation de l'interface de Problem  //
  //                                                    //
  ////////////////////////////////////////////////////////

  bool updateGivenFields() override;

  void ajouter(Probleme_base&);
  int associer_(Objet_U&) override;
  virtual void associer_sch_tps_base(Schema_Temps_base&);
  virtual const Schema_Temps_base& schema_temps() const;
  virtual Schema_Temps_base& schema_temps();

  virtual void discretiser(const Discretisation_base&);
  inline virtual void mettre_a_jour_modele_rayo(double temps);
  void initialize() override;

protected:

  // Definitions des groupes de problemes.
  // Vecteur d'entiers specifiant chacun la taille d'un groupe.
  // Vecteur vide = un seul groupe.
  ArrOfInt groupes;

  // Liste des schemas clones
  VECT(Schema_Temps) sch_clones;
};

inline void Probleme_Couple::mettre_a_jour_modele_rayo(double temps)
{

  Cerr<<"The method Probleme_Couple::mettre_a_jour_modele_rayo does nothing"<<finl;
  Cerr<<"We should not pass through here"<<finl;
  exit();
}


#endif
