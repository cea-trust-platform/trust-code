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

#include <Option_VDF.h>
#include <Motcle.h>
#include <Param.h>

Implemente_instanciable(Option_VDF,"Option_VDF",Interprete);
// XD option_vdf interprete option_vdf 1 Class of VDF options.

double Option_VDF::coeff_P_neumann=1.;
int Option_VDF::traitement_coins=0;


/*! @brief Simple appel a: Interprete::printOn(Sortie&)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Option_VDF::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


/*! @brief Simple appel a: Interprete::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Option_VDF::readOn(Entree& is)
{
  return Interprete::readOn(is);
}


int Option_VDF::test_mot(const Motcle& motlu) const
{
  Motcles les_mots(2);
  {
    les_mots[1] = "oui";
    les_mots[0] = "non";
  }

  int rang=les_mots.search(motlu);
  if (rang==-1)
    {
      Cerr<<"Method "<<que_suis_je()<<"::test_mot : Tested keyword is "<<motlu<<finl;
      Cerr<<"One expected a keyword among yhe following :"<<finl<<les_mots<<finl;
      abort();
      return -1;
    }
  else
    return rang;
}
/*! @brief Fonction principale de l'interprete Option_VDF Lit la dimension d'espace du probleme.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Option_VDF::interpreter(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter_non_std("traitement_coins",(this)); // XD_ADD_P chaine(into=["oui","non"]) Treatment of corners (yes or no). This option modifies slightly the calculations at the outlet of the plane channel. It supposes that the boundary continues after channel outlet (i.e. velocity vector remains parallel to the boundary).
  param.ajouter_non_std("P_imposee_aux_faces",(this)); // XD_ADD_P chaine(into=["oui","non"]) Pressure imposed at the faces (yes or no).
  param.lire_avec_accolades_depuis(is);
  return is;
}

int Option_VDF::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  if (mot=="traitement_coins")
    {
      is >>motlu;
      if (test_mot(motlu))
        {
          Cerr<< motlu <<" will affect the action of the method Zone_Cl_VDF::completer"<<finl;
          Option_VDF::traitement_coins=1;
        }
      else
        Option_VDF::traitement_coins=0;
      return 1;
    }
  else if (mot=="P_imposee_aux_faces")
    {
      is >>motlu;
      if (test_mot(motlu))
        {
          Cerr<<motlu<<" will affect the action of the operator gradient for Neumann_sortie_libre boundary condition" <<finl;
          Cerr<<motlu<<" and therefore the assembleur action will also be affected."<<finl;
          Option_VDF::coeff_P_neumann=2.;
        }
      else
        Option_VDF::coeff_P_neumann=1.;
      return 1;
    }
  else
    {
      Cerr << mot << "is not a keyword understood by " << que_suis_je() << "in methode lire_motcle_non_standard"<< finl;
      exit();
    }
  return -1;
}
