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
// File:        Fluide_Incompressible.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Fluide_Incompressible_included
#define Fluide_Incompressible_included

#include <Milieu_base.h>
#include <Ref_Champ_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Fluide_Incompressible
//    Cette classe represente un d'un fluide incompressible ainsi que
//    ses proprietes:
//        - viscosite cinematique, (mu)
//        - viscosite dynamique,   (nu)
//        - masse volumique,       (rho)
//        - diffusivite,           (alpha)
//        - conductivite,          (lambda)
//        - capacite calorifique,  (Cp)
//        - dilatabilite thermique du constituant (beta_co)
// .SECTION voir aussi
//     Milieu_base
//////////////////////////////////////////////////////////////////////////////
class Fluide_Incompressible : public Milieu_base
{
  Declare_instanciable_sans_constructeur(Fluide_Incompressible);

public :

  Fluide_Incompressible();
  inline const Champ_Don& viscosite_cinematique() const;
  inline const Champ_Don& viscosite_dynamique() const;
  inline const Champ_Don& beta_c() const;
  inline Champ_Don& viscosite_cinematique();
  inline Champ_Don& viscosite_dynamique();
  inline Champ_Don& beta_c();

  // Modif CHD 07/05/03 Ajout des parametres pour un fluide semi
  // transparent on les ramene ici pour ne plus avoir a utiliser
  // de Fluide incompressible semi transparent.
  int is_rayo_semi_transp() const;
  int is_rayo_transp() const;
  void fixer_type_rayo();
  void reset_type_rayo();
  int longueur_rayo_is_discretised();
  inline Champ_Don& kappa();
  inline const Champ_Don& kappa() const;
  inline Champ_Don& indice();
  inline const Champ_Don& indice() const;
  inline Champ_Don& longueur_rayo();
  inline const Champ_Don& longeur_rayo() const;

  void set_param(Param& param);
  void verifier_coherence_champs(int& err,Nom& message);
  void mettre_a_jour(double );
  int initialiser(const double& temps);
  void creer_champs_non_lus();
  virtual void discretiser(const Probleme_base& pb, const  Discretisation_base& dis);



protected :

  Champ_Don mu;
  Champ_Don nu;
  Champ_Don beta_co;

  // Parametres du fluide rayonnant semi transparent
  Champ_Don coeff_absorption_;
  Champ_Don indice_refraction_;
  // Longueur caractaristique de la longueur de penetration
  // du rayonnement dans le milieu semi transparent definie
  // comme l = 1/(3*kappa)
  Champ_Don longueur_rayo_;

  void creer_nu();
  void calculer_nu();

protected:

};


// Description:
//    Renvoie la viscosite dynamique, mu.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: la viscosite dynamique, mu
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Don& Fluide_Incompressible::viscosite_dynamique() const
{
  return mu;
}


// Description:
//    Renvoie la viscosite dynamique, mu.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: la viscosite dynamique, mu
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Don& Fluide_Incompressible::viscosite_dynamique()
{
  return mu;
}


// Description:
//    Renvoie la viscosite cinematique, nu.
//    (vesrion const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: la viscosite cinematique, nu
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Don& Fluide_Incompressible::viscosite_cinematique() const
{
  return nu;
}


// Description:
//    Renvoie la viscosite cinematique, nu.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: la viscosite cinematique, nu
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Don& Fluide_Incompressible::viscosite_cinematique()
{
  return nu;
}


// Description:
//    Renvoie la dilatabilite du constituant, beta_co.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: la dilatabilite du constituant, beta_co
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Don& Fluide_Incompressible::beta_c() const
{
  return beta_co;
}


// Description:
//    Renvoie la dilatabilite du constituant, beta_co.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: la dilatabilite du constituant, beta_co
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Don& Fluide_Incompressible::beta_c()
{
  return beta_co;
}


// Description:
//    Renvoie le coefficient d'absorbtion du fluide
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: le coefficient d'absorbtion du fluide
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Don& Fluide_Incompressible::kappa()
{
  return coeff_absorption_;
}

// Description:
//    Renvoie le coefficient d'absorbtion du fluide
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: le coefficient d'absorbtion du fluide
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition:
inline const Champ_Don& Fluide_Incompressible::kappa() const
{
  return coeff_absorption_;
}

// Description:
//    Renvoie l'indice de refraction du fluide
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: l'indice de refraction du fluide
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Don& Fluide_Incompressible::indice()
{
  return indice_refraction_;
}

// Description:
//    Renvoie l'indice de refraction du fluide
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: l'indice de refraction du fluide
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition:
inline const Champ_Don& Fluide_Incompressible::indice() const
{
  return indice_refraction_;
}

// Description:
//    Renvoie la longueur de penetration du rayonnement dans le fluide
//    definie comme l = 1/(3*kappa)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: la longueur de penetration du rayonnement dans le fluide
//    definie comme l = 1/(3*kappa)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Don& Fluide_Incompressible::longueur_rayo()
{
  return longueur_rayo_;
}


// Description:
//    Renvoie la longueur de penetration du rayonnement dans le fluide
//    definie comme l = 1/(3*kappa)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: la longueur de penetration du rayonnement dans le fluide
//    definie comme l = 1/(3*kappa)
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition:
inline const Champ_Don& Fluide_Incompressible::longeur_rayo() const
{
  return longueur_rayo_;
}


#endif
