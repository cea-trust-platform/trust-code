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

#include <Operateur_Conv_base.h>
#include <Champ_base.h>

Implemente_base_sans_constructeur(Operateur_Conv_base,"Operateur_Conv_base",Operateur_base);


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Operateur_Conv_base::printOn(Sortie& os) const
{
  return os;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: Entree& is
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
Entree& Operateur_Conv_base::readOn(Entree& is)
{
  return is;
}
//Description:
// initialise dt_stab_conv a -123
Operateur_Conv_base::Operateur_Conv_base():dt_stab_conv_(-123.) {}

void Operateur_Conv_base::associer_norme_vitesse(const Champ_base& norme_vitesse)
{
  if (je_suis_maitre())
    {
      Cerr << "Operateur_Conv_base::associer_norme_vitesse\n";
      Cerr << " field name : " << norme_vitesse.le_nom();
      Cerr << " unit : "       << norme_vitesse.unite() << finl;
    }
  la_norme_vitesse = norme_vitesse;
}
void Operateur_Conv_base::associer_vitesse_pour_pas_de_temps(const Champ_base& vitesse)
{
  vitesse_pour_pas_de_temps_=vitesse;
}

void Operateur_Conv_base::set_incompressible(const int flag)
{
  if (flag == 0)
    {
      Cerr << "Compressible form of operator \"" << que_suis_je() << "\" not implemented!" << finl;
      Process::exit();
    }
  incompressible_ = flag;
}
