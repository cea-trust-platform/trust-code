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

#include <Loi_Etat.h>
#include <Motcle.h>

Implemente_deriv(Loi_Etat_base);
Implemente_instanciable(Loi_Etat,"Loi_Etat",DERIV(Loi_Etat_base));

Sortie& Loi_Etat::printOn(Sortie& os) const
{
  return DERIV(Loi_Etat_base)::printOn(os);
}

Entree& Loi_Etat::readOn(Entree& is)
{
  Cerr<<"Lecture et typage de la loi d'etat :"<<finl;
  Motcle motlu;
  is>>motlu;
  Nom type = "Loi_Etat_";
  Motcles les_mots_loi(8);
  {
    les_mots_loi[0] = "gaz_parfait_QC";
    les_mots_loi[1] = "gaz_parfait_WC";
    les_mots_loi[2] = "binaire_gaz_parfait_QC";
    les_mots_loi[3] = "binaire_gaz_parfait_WC";
    les_mots_loi[4] = "multi_gaz_parfait_QC";
    les_mots_loi[5] = "multi_gaz_parfait_WC";
    les_mots_loi[6] = "rhoT_gaz_parfait_QC";
    les_mots_loi[7] = "rhoT_gaz_reel_QC";
  }
  int rang_loi = les_mots_loi.search(motlu);
  switch(rang_loi)
    {
    case 0 :
      {
        type += "Gaz_Parfait_QC";
        break;
      }
    case 1 :
      {
        type += "Gaz_Parfait_WC";
        break;
      }
    case 2 :
      {
        type += "Binaire_Gaz_Parfait_QC";
        break;
      }
    case 3 :
      {
        type += "Binaire_Gaz_Parfait_WC";
        break;
      }
    case 4 :
      {
        type += "Multi_Gaz_Parfait_QC";
        break;
      }
    case 5 :
      {
        type += "Multi_Gaz_Parfait_WC";
        break;
      }
    case 6 :
      {
        type += "rhoT_Gaz_Parfait_QC";
        break;
      }
    case 7 :
      {
        type += "rhoT_Gaz_Reel_QC";
        break;
      }
    default :
      {
        Cerr<<"ERREUR : Les lois d'etat actuellement implementees sont :"<<finl;
        Cerr<<les_mots_loi<<finl;
        abort();
      }
    }
  typer(type);

  is >> valeur();
  return is;
}

// Description:
//    Type la loi d'etat
// Precondition:
// Parametre: Nom& typ
//    Signification: le nom de type a donner a la loi d'etat
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat::typer(const Nom& type)
{
  DERIV(Loi_Etat_base)::typer(type);
  Cerr<<" "<<valeur().que_suis_je()<<finl;
}
