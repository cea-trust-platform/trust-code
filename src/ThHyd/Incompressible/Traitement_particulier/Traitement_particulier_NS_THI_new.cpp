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
// File:        Traitement_particulier_NS_THI_new.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Traitement_particulier
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_THI_new.h>
#include <Domaine.h>
#include <MD_Vector_base.h>
Implemente_base(Traitement_particulier_NS_THI_new,"Traitement_particulier_NS_THI_new",Traitement_particulier_NS_base);


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
Sortie& Traitement_particulier_NS_THI_new::printOn(Sortie& is) const
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
Entree& Traitement_particulier_NS_THI_new::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_NS_THI_new::lire(Entree& is)
{
  // valeurs par defaut
  init = 0;
  fac_init = 0;
  oui_transf = 0;
  oui_calc_spectre = 0;
  // FIN valeurs par defaut
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
                          Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI_new_VDF" << finl;
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
                  Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI_new";
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
            Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI_new";
            Cerr << "On attendait une }" << finl;
            exit();
          }
      }
    }
  else
    {
      Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI_new";
      Cerr << "On attendait une {" << finl;
      exit();
    }
  return is;
}


void Traitement_particulier_NS_THI_new::preparer_calcul_particulier()
{
  if ((oui_calc_spectre != 0)||(fac_init!=0))
    init_calc_spectre();
  if (init==1) renorm_Ec();
  if (oui_calc_spectre != 0)
    calcul_spectre();
}

void Traitement_particulier_NS_THI_new::post_traitement_particulier()
{
  if (oui_calc_spectre != 0)
    //    renorm_Ec();
    calcul_spectre();
}

void Traitement_particulier_NS_THI_new::en_cours_de_resolution(int nb_op, DoubleTab& u, DoubleTab& u_av, double dt)
{
  // il n'est en fait jamais appele cf. ci-dessous (Caroline)
  if (oui_transf == 1)
    {
      //      calcul_spectre_operateur( nb_op, u, u_av,dt);

      // Patrick

      Cerr << " finalement Traitement_particulier_NS_THI_new::en_cours_de_resolution  est bien appele (?) " << finl;
      exit();
    }
  return;
}

int& Traitement_particulier_NS_THI_new::calcul_nb_som_dir(const Zone& zone)
{
  const char* methode_actuelle="Traitement_particulier_NS_THI_new::calcul_nb_som_dir";

  // Sert a calculer le nombre de sommet commun en parallele
  double nb_som = zone.domaine().md_vector_sommets().valeur().nb_items_seq_tot();
  // Somme sur tous les processeurs

  double nb=pow(nb_som*1.,1./3.);
  nb_som_dir = (int)(nb);
  if (nb_som_dir*nb_som_dir*nb_som_dir != nb_som)
    {
      nb_som_dir=nb_som_dir+1;
      if (nb_som_dir*nb_som_dir*nb_som_dir != nb_som)
        msg_erreur_maillage(methode_actuelle);
    }
  nb_som_dir=nb_som_dir-1;
  return nb_som_dir;
}

void Traitement_particulier_NS_THI_new::msg_erreur_maillage(const char* methode_actuelle)
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

/******************************************************************************/
/****** A propos du calcul des tranferts d'energie par chaque operateur *******/
/****** Il n'a pas ete mis dans la nouvelle version                     *******/
/****** Copie de la methode permettant son calcul dans Navier_Stokes_std.cpp          *******/
/******************************************************************************/

// DoubleTab& Navier_Stokes_std::derivee_en_temps_inco(DoubleTab& vpoint)
// {
//   // Cerr << " Navier_Stokes_std::derivee_en_temps_inco " << finl;
//   DoubleTab& pression=la_pression.valeurs();
//   DoubleTab& vitesse=la_vitesse.valeurs();
//   DoubleTrav secmem(pression);
//   DoubleTrav gradP(vitesse);
//   double dt=le_schema_en_temps->pas_de_temps();

//    //  B M-1 F(Un)

// //////////////////////////////////
// //  Equation_base::derivee_en_temps_inco(vpoint);
// // je recopie ici pour faire les modifs ici!!!! 17/08/99 CA

//   DoubleTab u_diff, u_press, u_av,u_tot;
//  vpoint=0;
//   int nb_dim = vpoint.nb_dim();
//   int n1 = vpoint.dimension(0);
//   int n2;
//   if (nb_dim == 2)
//     n2 = vpoint.dimension(1);

//   if (nb_dim == 1)
//     {
//       u_diff.resize(n1);
//       u_av.resize(n1);
//       u_press.resize(n1);
//       u_tot.resize(n1);
//     }
//   else
//     {
//       u_diff.resize(n1,n2);
//       u_av.resize(n1,n2);
//       u_press.resize(n1,n2);
//       u_tot.resize(n1,n2);
//     }

//   u_av = inconnue().valeurs();

//   for(int i=0; i<nombre_d_operateurs(); i++)
//     {
//       u_diff = vpoint;
//       operateur(i).ajouter(vpoint);

//       u_diff -= vpoint;
//       u_diff *= -1.0;
//       if (le_traitement_particulier.non_nul())
//         le_traitement_particulier.en_cours_de_resolution(i,u_diff, u_av,dt);

//       vpoint.echange_espace_virtuel();
//     }

//   les_sources.ajouter(vpoint);
//   vpoint.echange_espace_virtuel();
//   solveur_masse.appliquer(vpoint);
//   ////////////////////////////////////////////////////


//   vpoint.echange_espace_virtuel();

//   divergence.ajouter(vpoint, secmem);
//   secmem *= -1;
//   secmem.echange_espace_virtuel();

//   // Correction du second membre d'apres les conditions aux limites :
//   solveur_pression.modifier_secmem_pression(secmem, pression);

//   // Resolution en pression :

//   solveur_pression.resoudre(secmem, pression);

//   // M-1 Bt P
//   gradient.calculer(pression, gradP);
//   gradP.echange_espace_virtuel();
//   solveur_masse.appliquer(gradP);
//   // Cerr << "M-1 Bt P " <<  gradP << finl;
//   // Correction en pression

//   vpoint -= gradP;
//   vpoint.echange_espace_virtuel();

//   u_press = -gradP;
//   if (le_traitement_particulier.non_nul())
//     le_traitement_particulier.en_cours_de_resolution(2,u_press, u_av,dt);

//   u_av = inconnue().valeurs();
//   u_tot = vpoint;
//     if (le_traitement_particulier.non_nul())
//       le_traitement_particulier.en_cours_de_resolution(3,u_tot, u_av,dt);

//   return vpoint;

// }
