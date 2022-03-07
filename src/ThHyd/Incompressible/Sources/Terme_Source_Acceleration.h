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
// File:        Terme_Source_Acceleration.h
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Sources
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Terme_Source_Acceleration_included
#define Terme_Source_Acceleration_included

#include <TRUSTVect.h>
#include <Ref_Navier_Stokes_std.h>
#include <Champ_Don.h>
#include <Champ_Fonc.h>
#include <Source_base.h>
#include <Motcle.h>
#include <Ref_Zone_VF.h>

class Probleme_base;

class Terme_Source_Acceleration : public Source_base
{
  // C'est une classe de base: voir Terme_Source_Acceleration_VDF_Faces
  Declare_base_sans_constructeur(Terme_Source_Acceleration);
public:

  Terme_Source_Acceleration();
  void         associer_pb(const Probleme_base& ) override;
  DoubleTab&   calculer(DoubleTab& ) const override;
  int       a_pour_Champ_Fonc(const Motcle& mot,
                              REF(Champ_base) & ch_ref) const override;
  inline const Champ_Don& champ_vitesse() const
  {
    return champ_vitesse_;
  };
  inline const Champ_Don& omega() const
  {
    return omega_;
  };

protected:
  virtual void                      lire_data(Entree& s);
  virtual const Champ_Fonc_base&    get_terme_source_post() const;
  virtual Champ_Fonc&               get_set_terme_source_post() const;
  virtual const DoubleTab&          calculer_vitesse_faces(DoubleTab& stockage) const = 0;
  const DoubleTab&                  calculer_la_source(DoubleTab& src_faces) const;
  virtual const Navier_Stokes_std& get_eq_hydraulique() const;

  void                 mettre_a_jour(double temps) override;


private:
// Le terme source, homogene a d/dt(rho*v) et discretise comme la vitesse,
// stocke pour pouvoir etre postraite.
// Il est calcule par ajouter() (voir commentaires dans a_pour_champ_fonc)
  mutable Champ_Fonc terme_source_post_;

  // **********************************************************************
  // Description du champ de vitesse impose au repere mobile (lu optionnellement
  // dans le jeu de donnees, non utilise par le terme source mais potentiellement
  // pour le code pour calculer la vitesse dans le repere fixe)
  Champ_Don champ_vitesse_;

  // Description du champ d'acceleration impose (lu dans le jeu de donnees)
  // Champ_Don lu dans le jeu de donnees, homogene a d/dt(v) en m/(s^2).
  // Ce doit etre un champ vectoriel uniforme a "dimension" composantes.
  // Si champ_acceleration.non_nul()==0, c'est qu'on n'en a pas mis
  // dans le jeu de donnees.
  Champ_Don champ_acceleration_;

  // Description d'un champ de rotation instationnaire:
  // Les trois champs suivants peuvent etre nuls (omega_.non_nul()==0).

  // Vitesse de rotation: champ uniforme a trois composantes
  // (attention: en 2D, vecteur oriente selon Z)
  Champ_Don omega_;
  // Derivee de la vitesse de rotation par rapport au temps:
  //  champ uniforme a trois composantes
  Champ_Don domegadt_;
  // Position du centre de rotation:
  //  champ uniforme a "dimension" composantes
  Champ_Don centre_rotation_;


  enum Option_TSA { TERME_COMPLET, CORIOLIS_SEUL, ENTRAINEMENT_SEUL };
  Option_TSA option_;
  // *****************************************************************
};

#endif
