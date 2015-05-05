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
// File:        ScatterMED.cpp
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////


#include <ScatterMED.h>
#include <LireMED.h>
#include <Domaine.h>
#include <SFichier.h>

Implemente_instanciable(ScatterMED,"ScatterMED",Scatter);


// Description:
//    Simple appel a: Interprete::printOn(Sortie&)
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
Sortie& ScatterMED::printOn(Sortie& os) const
{
  return Scatter::printOn(os);
}

Entree& ScatterMED::readOn(Entree& is)
{
  return Scatter::readOn(is);
}


// Description:
//  Lit le domaine dans le fichier de nom "nomentree",
//  de type LecFicDistribueBin ou LecFicDistribue
//  (selon fichier_zone_format_binaire_)
//  Le format historique n'est plus supporte.
//  Format attendu : Domaine::ReadOn
//  La zone est renommee comme le domaine (pour lance_test_seq_par)
void ScatterMED::lire_domaine(Nom& nomentree, Noms& liste_bords_periodiques)
{


  Nom fichiermed(nomentree);
  fichiermed.prefix(".med");
  Nom n(me()+1);
  fichiermed+=n;
  fichiermed+=".med";
  Domaine& dom = domaine();
  LireMED lire_med;
  Nom nom_lu(dom.le_nom());
  nom_lu+="_";
  nom_lu+=n;
  lire_med.lire_geom(fichiermed,dom,nom_lu,dom.le_nom());

  // Renseigne dans quel fichier le domaine a ete lu
  dom.set_fichier_lu(nomentree);

  // On renomme la zone (important pour le format lml:
  // nom de la topologie utilise par lance_test_seq_par)
  const Nom& nom_dom = dom.le_nom();
  dom.zone(0).nommer(nom_dom);

  //fichier >> liste_bords_periodiques;

  //fichier.close();
  if (0)
    {
      Nom org=("debug.Zones");
      org=org.nom_me(me());
      SFichier  file_out(org);
      file_out << dom << finl<<"0 "<<finl;
    }
  barrier();
}

