/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Traitement_particulier_NS_THI.h>
#include <MD_Vector_tools.h>
#include <ArrOfBit.h>
#include <Scatter.h>
#include <Domaine.h>

Implemente_base(Traitement_particulier_NS_THI,"Traitement_particulier_NS_THI",Traitement_particulier_NS_base);
// XD thi traitement_particulier_base thi -1 Keyword for a THI (Homogeneous Isotropic Turbulence) calculation.
// XD   attr init_Ec entier init_Ec 0 Keyword to renormalize initial velocity so that kinetic energy equals to the value given by keyword val_Ec.
// XD   attr val_Ec floattant val_Ec 1 Keyword to impose a value for kinetic energy by velocity renormalizated if init_Ec value is 1.
// XD   attr facon_init entier(into=["0","1"]) facon_init 1 Keyword to specify how kinetic energy is computed (0 or 1).
// XD   attr calc_spectre entier(into=["0","1"]) calc_spectre 1 Calculate or not the spectrum of kinetic energy. NL2 Files called Sorties_THI are written with inside four columns : NL2 time:t global_kinetic_energy:Ec enstrophy:D skewness:S NL2 If calc_spectre is set to 1, a file Sorties_THI2_2 is written with three columns : NL2 time:t kinetic_energy_at_kc=32 enstrophy_at_kc=32 NL2 If calc_spectre is set to 1, a file spectre_xxxxx is written with two columns at each time xxxxx : NL2 frequency:k energy:E(k).
// XD   attr periode_calc_spectre floattant periode_calc_spectre 1 Period for calculating spectrum of kinetic energy
// XD   attr spectre_3D entier(into=["0","1"]) Spectre_3D 1 Calculate or not the 3D spectrum
// XD   attr spectre_1D entier(into=["0","1"]) Spectre_1D 1 Calculate or not the 1D spectrum
// XD   attr conservation_Ec rien conservation_Ec 1 If set to 1, velocity field will be changed as to have a constant kinetic energy (default 0)
// XD   attr longueur_boite floattant longueur_boite 1 Length of the calculation domain

Sortie& Traitement_particulier_NS_THI::printOn(Sortie& is) const
{
  return is;
}

Entree& Traitement_particulier_NS_THI::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_NS_THI::lire(Entree& is)
{
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle valec="val_Ec";
  Motcle facon="facon_init";
  Motcle motbidon, motlu;
  is >> motbidon ;
  if (motbidon == accouverte)
    {
      Motcles les_mots(2);
      {
        les_mots[0] = "init_Ec";
        les_mots[1] = "calc_spectre";
      }
      {
        is >> motlu;
        while(motlu != accfermee)
          {
            int rang=les_mots.search(motlu);
            switch(rang)
              {
              case 0 :
                {
                  is >> init;  // init_Ec = 1
                  if (init!=0)
                    {
                      is >> motlu;
                      if (motlu==valec)
                        {
                          is >> Ec_init; // on lit la valeur de Ec_init
                          Cerr << "Avec Initialisation de l Energie Cinetique Ec_init= " << Ec_init << finl;
                          is >> motlu;
                          if (motlu==facon)
                            {
                              is >> fac_init; // Sur quelle valeur se base l initialisation??
                              Cerr << "Avec Initialisation de l Energie Cinetique sur Ecspat (init_fac==0) ou Ecspec (init_fac==1) : fac_init=" << fac_init << finl;
                            }
                        }
                      else
                        {
                          Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI_VDF" << finl;
                          Cerr << "Le seul mot cle possible ici est : val_Ec" << finl;
                          Cerr << "Vous avez lu :" << motlu << finl;
                          exit();
                        }
                    }
                  break;
                }
              case 1 :
                {
                  is >> oui_calc_spectre;
                  if (oui_calc_spectre!=0) Cerr << "Calcul des spectres." << finl;
                  break;
                }

              default :
                {
                  Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI";
                  Cerr << "Les mots cles possibles sont : init_Ec, calc_spectre, { et }" << finl;
                  Cerr << "Vous avez lu :" << motlu << finl;
                  exit();
                  break;
                }
              }
            is >> motlu;
          }
        is >> motlu;
        if (motlu != accfermee)
          {
            Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI";
            Cerr << "On attendait une }" << finl;
            exit();
          }
      }
    }
  else
    {
      Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI";
      Cerr << "On attendait une {" << finl;
      exit();
    }
  return is;
}


void Traitement_particulier_NS_THI::preparer_calcul_particulier()
{
  if ((oui_calc_spectre != 0)||(fac_init!=0))
    init_calc_spectre();
  if (init==1) renorm_Ec();
  if (oui_calc_spectre != 0)
    calcul_spectre();
  sorties_globales();
}

void Traitement_particulier_NS_THI::post_traitement_particulier()
{
  if (oui_calc_spectre != 0)
    calcul_spectre();

  sorties_globales();
}

void Traitement_particulier_NS_THI::en_cours_de_resolution(int nb_op, DoubleTab& u, DoubleTab& u_av, double dt)
{
  // il n'est en fait jamais appele cf. ci-dessous
  if (oui_transf == 1)
    {
      calcul_spectre_operateur( nb_op, u, u_av,dt);
    }
  return;
}

int& Traitement_particulier_NS_THI::calcul_nb_som_dir(const Domaine& domaine)
{
  const char* methode_actuelle="Traitement_particulier_NS_THI::calcul_nb_som_dir";

  // Sert a calculer le nombre de sommet commun en parallele
  ArrOfBit unused;
  trustIdType nb_som = domaine.les_sommets().get_md_vector()->get_sequential_items_flags(unused);
  // Somme sur tous les processeurs
  nb_som = mp_sum(nb_som);

  // [ABN] OMG, the below is so ugly ... how could this ever work?!
  double nb=pow(static_cast<double>(nb_som),1./3.);
  nb_som_dir = static_cast<int>(nb);
  if (nb_som_dir*nb_som_dir*nb_som_dir != nb_som)
    {
      nb_som_dir=nb_som_dir+1;
      if (nb_som_dir*nb_som_dir*nb_som_dir != nb_som)
        msg_erreur_maillage(methode_actuelle);
    }
  nb_som_dir=nb_som_dir-1;
  return nb_som_dir;
}

int& Traitement_particulier_NS_THI::calcul_nb_elem_dir(const Domaine& domaine)
{
  const char* methode_actuelle="Traitement_particulier_NS_THI::calcul_nb_elem_dir";
  trustIdType nb_elem = domaine.nb_elem();

  // Somme sur tous les processeurs
  nb_elem=mp_sum(nb_elem);

  // [ABN] (same as above) -> OMG, the below is so ugly ... how could this ever work?!
  double nb=pow(static_cast<double>(nb_elem),1./3.);
  nb_elem_dir = static_cast<int>(nb);
  if (nb_elem_dir*nb_elem_dir*nb_elem_dir != nb_elem)
    {
      nb_elem_dir=nb_elem_dir+1;
      if (nb_elem_dir*nb_elem_dir*nb_elem_dir != nb_elem)
        msg_erreur_maillage(methode_actuelle);
    }
  return nb_elem_dir;
}

void Traitement_particulier_NS_THI::msg_erreur_maillage(const char* methode_actuelle)
{
  if (je_suis_maitre())
    {
      Cerr << finl;
      Cerr << "Probleme dans " << methode_actuelle << " :" << finl;
      Cerr << "Votre maillage ne semble pas comporter le meme nombre de noeuds" << finl;
      Cerr << "suivant toutes les directions. Verifiez votre jeu de donnee... " << finl << finl;
      // Abort pour les cas paralleles !
      abort();
    }
}

