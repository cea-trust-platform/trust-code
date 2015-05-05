/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Marqueur_Lagrange_base.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Marqueur_Lagrange_base_included
#define Marqueur_Lagrange_base_included

#include <Objet_U.h>
#include <Motcle.h>
#include <Champ_Fonc.h>
#include <Champs_compris.h>
#include <Champs_compris_interface.h>

class Ensemble_Lagrange_base;
class Probleme_base;
class Discretisation_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Marqueur_Lagrange_base
//     La classe Marqueur_Lagange_base est la classe de base des classes de marqueurs
//     Actuellement une seule classe instanciable derivant : Marqueur_FT
//     Un marqueur est destine a suivre le mouvement du fluide en integrant la trajectoire
//     d un ensemble de particules a partir de la vitesse (interpolee) du fluide.
//     -l ensemble des points suivis est attribut de Marqueur_FT car de type Maillage_FT_Disc
//     -le postraitement est realise sur le nombre de particules par maille (densite_particules_)
//      ou sur le nuage de points
//     -l integration est demarree a partir d un temps t_debut_integr_ fixe par l utilisateur
//        ou egal a t_init par defaut
// .SECTION voir aussi
//
//     Classe abstraite.
//     Methodes abstraites:
//     Ensemble_Lagrange_base&  ensemble_points()
//     void calculer_valeurs_champs()
//////////////////////////////////////////////////////////////////////////////
class Marqueur_Lagrange_base: public Champs_compris_interface, public Objet_U
{
  Declare_base_sans_constructeur(Marqueur_Lagrange_base);

public :

  Marqueur_Lagrange_base();
  virtual Ensemble_Lagrange_base& ensemble_points() = 0;
  virtual const Ensemble_Lagrange_base& ensemble_points() const = 0;
  virtual void calculer_valeurs_champs() = 0;
  const inline double& temps_debut_integration() const;
  virtual void discretiser(const Probleme_base& pb, const  Discretisation_base& dis);
  virtual void mettre_a_jour(double temps);

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

protected :

  Champ_Fonc densite_particules_; //Exprime le nombre de particules par maille
  double t_debut_integr_;            //Temps de debut d integration des trajectoires

private :

  Champs_compris champs_compris_;

};

const double& Marqueur_Lagrange_base::temps_debut_integration() const
{
  return t_debut_integr_;
}

#endif
