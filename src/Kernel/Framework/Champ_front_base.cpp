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
// File:        Champ_front_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_front_base.h>
#include <Frontiere_dis_base.h>
#include <Frontiere.h>

Implemente_base_sans_constructeur(Champ_front_base,"Champ_front_base",Field_base);

Champ_front_base::Champ_front_base()
{
  temps_defaut = -1.;
}
// Description:
//    Imprime le nom du champ sur un flot de sortie
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Champ_front_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Champ_front_base::readOn(Entree& s )
{
  return s ;
}


// Description:
//    Initialisation en debut de calcul.
//    Imperativement cette methode ne doit pas utiliser de donnees
//    externes a l'equation (couplage). Si mettre_a_jour le fait,
//    alors initialiser ne doit pas appeler mettre_a_jour.
// Precondition: appel a completer
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: 0 en cas d'erreur, 1 sinon.
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Champ_front_base::initialiser(double temps, const Champ_Inc_base& inco)
{
  return 1;
}

// Description:
//    Associe une frontiere discretisee au champ.
// Precondition:
// Parametre: Frontiere_dis_base& fr
//    Signification: la frontiere discretisee a associer au champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le champ a une frontiere associee
void Champ_front_base::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  la_frontiere_dis=fr;
}


// Description:
//   NE FAIT RIEN, a surcharger.
//   Cette methode est appelee au debut de chaque pas de temps ou
//   sous-pas-de-temps, elle peut eventuellement utiliser des donnees
//   exterieures a l'equation. A charge a l'algorithme de s'assurer
//   que ces donnees sont pertinentes...
//   Calcule la valeur de la condition aux limites au temps demande.
// Precondition:
// Parametre: double
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_front_base::mettre_a_jour(double temps)
{
}

// Description:
//     Appele par Conds_lim::completer
//     Par defaut ne fait rien.
//     A surcharger dans les champs_front instationnaires.
void Champ_front_base::fixer_nb_valeurs_temporelles(int nb_cases)
{
}

// Description:
//   NE FAIT RIEN, a surcharger
//   Cette methode peut calculer et stocker des donnees utiles a la
//   CL, et dependant uniquement de l'inconnue sur laquelle porte
//   cette CL (pas de l'exterieur). cf Champ_front_contact_VEF par exemple.
//   Elle est appelee lorsque l'inconnue est modifiee.
// Precondition:
// Parametre: double
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_front_base::calculer_coeffs_echange(double temps)
{
}

// Description:
//    Renvoie le vecteur des valeurs du champ pour la face donnee.
//    Par defaut pour les champs fonc, on suppose que le tableau des
//    valeurs porte nb_faces * nb_compo_ valeurs.
//    Exemple de cas particulier: champ_front_uniforme::valeurs_face
// Parametre:     num_face
// Signification: l'indice d'une face sur la frontiere
//                0 <= num_face < frontiere_dis().frontiere().nb_faces()
// Parametre:     val
// Signification: On resize ce tableau et on le remplit.
void Champ_front_base::valeurs_face(int num_face, DoubleVect& val) const
{
#ifndef NDEBUG
  const int nb_faces = frontiere_dis().frontiere().nb_faces();
  // Si plantage ici, c'est que le tableau des valeurs ne contient pas une
  // valeur pour chaque face, il faut reimplementer la methode dans la
  // classe derivee...
  assert(num_face >= 0 && num_face < nb_faces);
  assert(valeurs().dimension(0) == nb_faces);
#endif
  const int n = nb_compo_;
  val.resize(n);
  const DoubleTab& valeurs_a_copier=valeurs();
  for (int i = 0; i < n; i++)
    val[i] = valeurs_a_copier(num_face, i);
}

const Zone_dis_base& Champ_front_base::zone_dis() const
{
  return frontiere_dis().zone_dis();
}


// Description:
//     A implementer dans les classes derivees.
//     Avance en temps : le nouveau temps present sera le temps passe
//     en parametre.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si OK, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Champ_front_base::avancer(double temps)
{
  Cerr << "Champ_front_base::avancer(double temps) should be overloaded"
       << endl;
  exit();
  return 0;
}

// Description:
//     A implementer dans les classes derivees.
//     Recule en temps : le nouveau temps present sera le temps passe
//     en parametre.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si OK, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Champ_front_base::reculer(double temps)
{
  Cerr << "Champ_front_base::reculer(double temps) should be overloaded "
       << "by " << que_suis_je() << endl;
  exit();
  return 0;
}

// Description :
// Change la valeur du temps pour la ieme valeur temporelle apres le present
void Champ_front_base::changer_temps_futur(double temps,int i)
{
  les_valeurs->futur(i).changer_temps(temps);
}
