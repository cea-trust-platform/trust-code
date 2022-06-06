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

#include <Champ_Don_base.h>

Implemente_base(Champ_Don_base,"Champ_Don_base",Champ_base);

Sortie& Champ_Don_base::printOn(Sortie& os) const { return Champ_base::printOn(os); }
Entree& Champ_Don_base::readOn(Entree& is) { return Champ_base::readOn(is); }

// Description: Fixe le nombre de degres de liberte par composante
// Parametre: int nb_noeuds
//    Signification: le nombre de degre de liberte par composante
// Postcondition: le tableaux des valeurs peut etre redimensionne
int Champ_Don_base::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  valeurs_.resize(nb_noeuds, nb_compo_);
  return nb_noeuds;
}

// Description:
//    Provoque une erreur !  A surcharger par les classes derivees !
//    non virtuelle pure par commoditees de developpement !
// Postcondition:
Champ_base& Champ_Don_base::affecter_(const Champ_base&)
{
  Cerr << "Champ_Don_base::affecter_ : " << que_suis_je() << "::affecter_ must be overloaded in derived classes" << finl;
  throw;
}

// Description:
//    Provoque une erreur ! A surcharger par les classes derivees !
//    non virtuelle pure par commoditees de developpement !
Champ_base& Champ_Don_base::affecter_compo(const Champ_base&, int)
{
  Cerr << "Champ_Don_base::affecter_compo must be overloaded in derived classes" << finl;
  throw;
}

// Description: Mise a jour en temps.
// Parametre: double
//    Signification: le temps de mise a jour
void Champ_Don_base::mettre_a_jour(double t)
{
  changer_temps(t);
  valeurs_.echange_espace_virtuel();
}

// Description:
//   NE FAIT RIEN. A surcharger dans les classes derivees. Provoque l'initialisation du champ si necessaire
int Champ_Don_base::initialiser(const double un_temps)
{
  mettre_a_jour(un_temps);
  return 1;
}

// Description: NE FAIT RIEN. A surcharger dans les classes derivees
int Champ_Don_base::reprendre(Entree& )
{
  return 1;
}

// Description: NE FAIT RIEN. A surcharger dans les classes derivees
int Champ_Don_base::sauvegarder(Sortie& ) const
{
  return 1;
}

// Description: NE FAIT RIEN. EXIT ! A surcharger dans les classes derivees
int Champ_Don_base::imprime(Sortie& os, int ncomp) const
{
  Cerr << que_suis_je() << "::imprime not coded." << finl;
  Process::exit();
  return 1;
}

// Description: Surcharge Champ_base::valeurs() Renvoie le tableau des valeurs
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champs
DoubleTab& Champ_Don_base::valeurs()
{
  return valeurs_;
}

const DoubleTab& Champ_Don_base::valeurs() const
{
  return valeurs_;
}

// Description: Fixe le nombre de composantes et le nombre de valeurs nodales.
// Parametre: int
//    Signification: le nombre de noeud par composante du champ (le nombre de dl par composante)
// Parametre: int
//    Signification: le nombre de composante du champ
void Champ_Don_base::dimensionner(int nb_noeuds, int nb_compo)
{
  fixer_nb_comp(nb_compo);
  fixer_nb_valeurs_nodales(nb_noeuds);
}
