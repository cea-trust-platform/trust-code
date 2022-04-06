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
// File:        Traitement_particulier_Solide_canal.cpp
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_Solide_canal.h>
#include <Periodique.h>
#include <Champ_Uniforme.h>
#include <EcrFicCollecteBin.h>
#include <TRUSTTrav.h>
#include <Conduction.h>
#include <Probleme_base.h>

Implemente_base(Traitement_particulier_Solide_canal,"Traitement_particulier_Solide_canal",Traitement_particulier_Solide_base);


Sortie& Traitement_particulier_Solide_canal::printOn(Sortie& is) const
{
  return is;
}


Entree& Traitement_particulier_Solide_canal::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_Solide_canal::lire(Entree& is)
{

  // FIN Initialisation
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle motbidon, motlu;
  is >> motbidon ;
  if (motbidon == accouverte)
    {
      Motcles les_mots(3);
      les_mots[0] = "u_inst";
      les_mots[1] = "stats";
      les_mots[2] = "nb_points";

      is >> motlu;
      while(motlu != accfermee)
        {
          int rang=les_mots.search(motlu);
          switch(rang)
            {
            case 0 :
              {
                // Pour spat, ex u_inst
                is >> dt_post_inst;      // intervalle de temps de sorties des moyennes spatiales
                // initialisation pour le calcul des stats spat.
                Cerr << "Spatial average parameters read..." << finl;
                Cerr << "dt_post_inst : " << dt_post_inst << finl;
                break;
              }
            case 1 :
              {
                // Pour stats
                is >> temps_deb;     // temps de debut de calcul des stats temp.
                is >> temps_fin;     // temps de fin de calcul des stats temp.
                is >> dt_post_stat;  // intervalle de temps de sorties des stats temp.
                // initialisation pour le calcul des stats temp.
                Cerr << "Reading time statitics parameters..." << finl;
                Cerr << "Initial time : " << temps_deb << " End time : " << temps_fin << finl;
                Cerr << "Time interval for output : " << dt_post_stat << finl;
                // Verif ensuite pour voir si on a rentrer les valeurs specifiques aux calculs des stats spat.
                break;
              }
            case 2 :
              {
                is  >> N ;
                Cerr << "Number of points estimated in the solid for profiles : " << N << finl;
                break;
              }
            }
          is >> motlu;
        }
      is >> motlu;
      if (motlu != accfermee)
        {
          Cerr << "Error while reading in Traitement_particulier_Solide_canal" << finl;;
          Cerr << "We were expecting a }" << finl;
          exit();
        }
    }
  else
    {
      Cerr << "Error while reading in Traitement_particulier_Solide_canal" << finl;
      Cerr << "We were expecting a {" << finl;
      exit();
    }

  return is;
}



void Traitement_particulier_Solide_canal::preparer_calcul_particulier()
{
  init_calcul_moyenne();
  init_calcul_stats();
}

void Traitement_particulier_Solide_canal::init_calcul_stats(void)
{
  /*      Equation_base& eqn=mon_equation.valeur();
          const Discretisation_base& discr=eqn.discretisation();
          Nom nom_discr=discr.que_suis_je();
          if(nom_discr=="VEFPreP1B" || nom_discr=="VEF")
          {
          N=20;
          val_moy_temp.resize(N,N);
          val_moy_temp=0.;
          }
          else
          {*/

  Tmoy_temp.resize(N);
  Tmoy_temp=0;
  T2_moy_temp.resize(N);
  T2_moy_temp=0;

  //      }
}

