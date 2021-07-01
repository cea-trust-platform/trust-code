/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Traitement_particulier_NS_chmoy_faceperio.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Traitement_particulier
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_chmoy_faceperio.h>
#include <LecFicDistribue.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Schema_Temps.h>

Implemente_base_sans_constructeur_ni_destructeur(Traitement_particulier_NS_chmoy_faceperio,"Traitement_particulier_NS_chmoy_faceperio",Traitement_particulier_NS_base);


// Description:
//
// Precondition:
// Parametre: Sortie& is
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
Sortie& Traitement_particulier_NS_chmoy_faceperio::printOn(Sortie& is) const
{
  return is;
}


// Description:
//
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Traitement_particulier_NS_chmoy_faceperio::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_NS_chmoy_faceperio::lire(Entree& is)
{
  // Initialisation
  oui_stat = 0;

  // FIN Initialisation
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle motbidon, motlu;
  is >> motbidon ;
  if (motbidon == accouverte)
    {
      Motcles les_mots(1);
      les_mots[0] = "stats";

      is >> motlu;
      while(motlu != accfermee)
        {
          int rang=les_mots.search(motlu);
          switch(rang)
            {
            case 0 :
              {
                // Pour stats
                is >> temps_deb;     // temps de debut de calcul des stats temp.
                is >> temps_fin;     // temps de fin de calcul des stats temp.
                // initialisation pour le calcul des stats temp.
                oui_stat=1;          // =1 : on calcul des stats temp.
                Cerr << "Reading time statitics parameters..." << finl;
                Cerr << "Initial time : " << temps_deb << " End time : " << temps_fin << finl;
                // Verif ensuite pour voir si on a rentrer les valeurs specifiques aux calculs des stats spat.
                break;
              }
            default :
              {
                Cerr << "Default case..." << finl;
                Cerr << "Les mots cles possibles sont "<< les_mots <<" { et }" << finl;
                Cerr << "Vous avez lu :" << motlu << finl;
                break;
              }
            }
          is >> motlu;
        }
      is >> motlu;
      if (motlu != accfermee)
        {
          Cerr << "Error while reading in Traitement_particulier_NS_canal" << finl;;
          Cerr << "We were expecting a }" << finl;
          exit();
        }
    }
  else
    {
      Cerr << "Error while reading in Traitement_particulier_NS_canal" << finl;
      Cerr << "We were expecting a {" << finl;
      exit();
    }

  return is;
}


void Traitement_particulier_NS_chmoy_faceperio::preparer_calcul_particulier()
{

  if(Objet_U::dimension!=3)
    {
      Cerr << " Traitement_particulier_NS_chmoy_faceperio : non prevu pour des calculs autres que 3D " << finl;
      exit();
    }

  if (oui_stat != 0)
    init_calcul_stats();

  double temps = mon_equation->inconnue().temps();
  int Nbfaces,num_face;

  if (temps>temps_deb)
    {
      Nom fichier = "chmoy_face_perio";
      ifstream fic(fichier);
      if (!fic)
        {
          Cerr << " pas de fichier : chmoy_face_perio  - reprise des calculs impossible" << finl;
          exit();
        }
      else
        {
          LecFicDistribue fic2(fichier);
          fic2 >> Nbfaces;

          for (int i=0; i<Nbfaces; i++)
            fic2 >> num_face >> chmoy_faceperio(num_face,0) >> chmoy_faceperio(num_face,1) >> chmoy_faceperio(num_face,2);
        }
    }
}

void Traitement_particulier_NS_chmoy_faceperio::post_traitement_particulier()
{
  double temps = mon_equation->inconnue().temps();
  double dt = mon_equation->schema_temps().pas_de_temps();
  if (temps>temps_deb && temps<temps_fin)
    calcul_chmoy_faceperio(temps_deb,temps,dt);
}


