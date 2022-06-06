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

#include <Champ_front_uniforme.h>

Implemente_instanciable(Champ_front_uniforme,"Champ_front_uniforme",Champ_front_base);



// Description:
//    Imprime le champ sur flot de sortie.
//    Imprime la taille du champ et la valeur (constante) sur
//    la frontiere.
// Precondition:
// Parametre: Sortie& os
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
Sortie& Champ_front_uniforme::printOn(Sortie& os) const
{
  const DoubleTab& tab=valeurs();
  os << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    os << tab(0,i);
  return os;
}


// Description:
//    Lit le champ a partir d'un flot d'entree.
//    Format:
//      Champ_front_uniforme nb_compo vrel_1 ... [vrel_i]
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& is
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le champ uniforme a la valeur lue
Entree& Champ_front_uniforme::readOn(Entree& is)
{
  int dim;
  DoubleTab& tab=les_valeurs->valeurs();
  dim=lire_dimension(is,que_suis_je());
  tab.resize(1,dim);
  fixer_nb_comp(dim);
  for(int i=0; i<dim; i++)
    is >> tab(0,i);
  return is;
}



// Description:
//    Renvoie l'objet upcaste en Champ_front_base&
// Precondition:
// Parametre: Champ_front_base& ch
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour: Champ_front_base&
//    Signification: (*this) upcaste en Champ_front_base&
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_front_base& Champ_front_uniforme::affecter_(const Champ_front_base& ch)
{
  return *this;
}

// Description:
//    Renvoie le vecteur des valeurs du champ pour la face donnee.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification: le tableau des valeurs du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_front_uniforme::valeurs_face(int face,DoubleVect& var) const
{
  var.resize(nb_compo_);
  int i;
  for (i=0 ; i<nb_compo_ ; i++)
    var(i) = valeurs()(0,i);
}

// Description:
//    Renvoie les valeurs sans s'occuper du temps puisque le champ
//    est stationnaire.
DoubleTab& Champ_front_uniforme::valeurs_au_temps(double temps)
{
  return les_valeurs->valeurs();
}

// Description:
//    Renvoie les valeurs sans s'occuper du temps puisque le champ
//    est stationnaire.
const DoubleTab& Champ_front_uniforme::valeurs_au_temps(double temps) const
{
  return les_valeurs->valeurs();
}

// Description:
//    Avance en temps : rien a faire pour un champ stationnaire !
int Champ_front_uniforme::avancer(double temps)
{
  return 1;
}

// Description:
//    Recule en temps : rien a faire pour un champ stationnaire !
int Champ_front_uniforme::reculer(double temps)
{
  return 1;
}

// Description :
// rien a faire pour un champ stationnaire !
void Champ_front_uniforme::changer_temps_futur(double temps,int i)
{
}
