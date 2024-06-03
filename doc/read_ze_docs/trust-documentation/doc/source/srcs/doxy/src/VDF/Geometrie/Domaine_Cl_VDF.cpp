/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Dirichlet_entree_fluide_leaves.h>
#include <Champ_front_var_instationnaire.h>
#include <Sortie_libre_pression_imposee.h>
#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_paroi_fixe.h>
#include <Champ_Face_VDF.h>
#include <Domaine_Cl_VDF.h>
#include <Champ_P0_VDF.h>
#include <Domaine_VDF.h>
#include <Periodique.h>
#include <Option_VDF.h>
#include <Champ_Inc.h>
#include <Navier.h>
#include <Debog.h>

Implemente_instanciable(Domaine_Cl_VDF,"Domaine_Cl_VDF",Domaine_Cl_dis_base);

Sortie& Domaine_Cl_VDF::printOn(Sortie& os) const
{
  Domaine_Cl_dis_base::printOn(os);
  return os << "type_arete_bord_ : " << type_arete_bord_ << finl << "num_Cl_face_ : " << num_Cl_face_ << finl;
}

Entree& Domaine_Cl_VDF::readOn(Entree& is) { return Domaine_Cl_dis_base::readOn(is); }

void Domaine_Cl_VDF::associer(const Domaine_VDF& domaine_vdf)
{
  type_arete_bord_.resize(domaine_vdf.nb_aretes_bord());
  type_arete_coin_.resize(domaine_vdf.nb_aretes_coin());
  num_Cl_face_.resize(domaine_vdf.nb_faces_bord());
}

void Domaine_Cl_VDF::completer(const Domaine_dis& un_domaine_dis)
{
  Cerr << "Domaine_Cl_VDF::completer ..." << finl;
  if (sub_type(Domaine_VDF,un_domaine_dis.valeur()))
    {
      const Domaine_VDF& le_dom_VDF = ref_cast(Domaine_VDF,un_domaine_dis.valeur());

      //  Remplissage du tableau d'entiers type_arete_bord_ + tableau intermediaire : les_faces_Cl
      //  On remplit le tableau d'entiers local les_faces_Cl qui donne la condition aux limites pour chaque face de bord  avec les conventions suivantes :
      //    0 pour une condition de paroi
      //    1 pour une condition d'entree ou de sortie de fluide (face "fluide")
      //    2 pour une condition de navier (symmetrie/paroi frottante)
      //    3 pour une condition de periodicite
      //    0 pour toute autre Cl.
      //  On ne considere que les conditions aux limites qui interviennent pour la quantite de mouvement

      int nb_aretes_bord = le_dom_VDF.nb_aretes_bord();
      IntVect les_faces_Cl;
      le_dom_VDF.creer_tableau_faces_bord(les_faces_Cl);

      // Boucle sur les conditions aux limites pour remplir les_faces_Cl:
      for (int n_bord = 0; n_bord < le_dom_VDF.nb_front_Cl(); n_bord++)
        {
          // pour chaque Condition Limite on regarde son type
          const Cond_lim_base& la_cl = les_conditions_limites_[n_bord].valeur();

          int numero_cl = 0;

          if ((sub_type(Dirichlet_paroi_fixe, la_cl)) || (sub_type(Dirichlet_paroi_defilante, la_cl)))
            numero_cl = 0;
          else if ((sub_type(Dirichlet_entree_fluide, la_cl)) || (sub_type(Neumann_sortie_libre, la_cl)))
            numero_cl = 1;
          else if (sub_type(Navier, la_cl)) // (symmetrie/paroi frottante)
            numero_cl = 2;
          else if (sub_type(Periodique, la_cl))
            numero_cl = 3;

          const Frontiere& fr = la_cl.frontiere_dis().frontiere();
          const int ndeb = fr.num_premiere_face(), nfin = ndeb + fr.nb_faces();
          for (int i = ndeb; i < nfin; i++)
            {
              les_faces_Cl[i] = numero_cl;
              num_Cl_face_[i] = n_bord;
            }
        }

      les_faces_Cl.echange_espace_virtuel();

      // Boucle sur les aretes bord pour remplir type_arete_bord_

      type_arete_bord_ = TypeAreteBordVDF::VIDE; // on initialise

      int face1, face2, rang1, rang2;
      int ndeb = le_dom_VDF.premiere_arete_bord(), nfin = ndeb + nb_aretes_bord;
      int num_arete_;

      int decal_virt = le_dom_VDF.nb_faces();
      int sz_faces_Cl = les_faces_Cl.size_reelle();
      const ArrOfInt& ind_faces_virt_bord = le_dom_VDF.domaine().ind_faces_virt_bord();

      for (int num_arete = ndeb; num_arete < nfin; num_arete++)
        {
          num_arete_ = num_arete - ndeb;
          face1 = le_dom_VDF.Qdm(num_arete, 0);
          face2 = le_dom_VDF.Qdm(num_arete, 1);
          rang1 = face1;
          rang2 = face2;

          if (rang1 >= sz_faces_Cl)
            rang1 = ind_faces_virt_bord[rang1 - decal_virt];

          if (rang2 >= sz_faces_Cl)
            rang2 = ind_faces_virt_bord[rang2 - decal_virt];

          if (les_faces_Cl[rang1] == 0) // paroi
            {
              if (les_faces_Cl[rang2] == 0) // paroi
                type_arete_bord_[num_arete_] = TypeAreteBordVDF::PAROI_PAROI;
              else if (les_faces_Cl[rang2] == 1) // entree/sortie fluide
                type_arete_bord_[num_arete_] = TypeAreteBordVDF::PAROI_FLUIDE;
              else if (les_faces_Cl[rang2] == 2) // navier
                type_arete_bord_[num_arete_] = TypeAreteBordVDF::PAROI_NAVIER;
              else
                {
                  Cerr << "On traite une arete qui separe deux faces de meme orientation : " << finl;
                  Cerr << "l'une de ces faces porte une condition limite de type paroi " << finl;
                  Cerr << "et l'autre porte une condition de type Periodicite" << finl;
                  Cerr << "TRUST ne sait pas traiter cette situation" << finl;
                  Process::Journal() << "ERREUR Faces_CL" << finl;
                  Process::Journal() << "face1:" << face1 << " , face2:" << face2 << finl;
                  Process::Journal() << "rang1:" << rang1 << " , rang2:" << rang2 << finl;
                  Process::Journal() << "les_faces_Cl[rang1]:" << les_faces_Cl[rang1] << " , les_faces_Cl[rang2]:" << les_faces_Cl[rang2] << finl;
                  Process::exit();
                }
            }
          else if (les_faces_Cl[rang1] == 1) // entree/sortie fluide
            {
              if (les_faces_Cl[rang2] == 0) // paroi
                type_arete_bord_[num_arete_] = TypeAreteBordVDF::PAROI_FLUIDE;
              else if (les_faces_Cl[rang2] == 1) // fluide
                type_arete_bord_[num_arete_] = TypeAreteBordVDF::FLUIDE_FLUIDE;
              else if (les_faces_Cl[rang2] == 2) // navier
                type_arete_bord_[num_arete_] = TypeAreteBordVDF::FLUIDE_NAVIER;
              else
                {
                  Cerr << "On traite une arete qui separe deux faces de meme orientation : " << finl;
                  Cerr << "l'une de ces faces porte une condition limite de type frontiere fluide " << finl;
                  Cerr << "et l'autre porte une condition de type  Periodicite" << finl;
                  Cerr << "TRUST ne sait pas traiter cette situation" << finl;
                  Process::exit();
                }
            }
          else if (les_faces_Cl[rang1] == 2) // navier
            {
              if (les_faces_Cl[rang2] == 0) // paroi
                type_arete_bord_[num_arete_] = TypeAreteBordVDF::PAROI_NAVIER;
              else if (les_faces_Cl[rang2] == 1) // entree
                type_arete_bord_[num_arete_] = TypeAreteBordVDF::FLUIDE_NAVIER;
              else if (les_faces_Cl[rang2] == 2) // navier
                type_arete_bord_[num_arete_] = TypeAreteBordVDF::NAVIER_NAVIER;
              else
                {
                  Cerr << "On traite une arete qui separe deux faces de meme orientation : " << finl;
                  Cerr << "l'une de ces faces porte une condition limite de type Symetrie " << finl;
                  Cerr << "et l'autre porte une condition de type Periodicite" << finl;
                  Cerr << "TRUST ne sait pas traiter cette situation" << finl;
                  Process::exit();
                }
            }
          else if (les_faces_Cl[rang1] == 3)
            {
              if (les_faces_Cl[rang2] == 3)
                type_arete_bord_[num_arete_] = TypeAreteBordVDF::PERIO_PERIO;
              else
                {
                  Cerr << "On traite une arete qui separe deux faces de meme orientation : " << finl;
                  Cerr << "l'une de ces faces porte une condition limite de type Periodicite " << finl;
                  Cerr << "et l'autre porte une condition d'un autre type" << finl;
                  Cerr << "TRUST ne sait pas traiter cette situation" << finl;
                  Process::exit();
                }
            }
        }

      // MODIFS CA : 21/09/99
      // Boucle sur les aretes coin pour remplir type_arete_coin_

      type_arete_coin_= TypeAreteCoinVDF::VIDE; // on initialise

      ndeb = le_dom_VDF.premiere_arete_coin();
      nfin = ndeb + le_dom_VDF.nb_aretes_coin();
      int fac1, fac2, fac3, fac4;

      for (int num_arete = ndeb; num_arete < nfin; num_arete++)
        {
          num_arete_ = num_arete - ndeb;

          // On cherche les 2 valeurs differentes de -1
          fac1 = le_dom_VDF.Qdm(num_arete_, 0);
          fac2 = le_dom_VDF.Qdm(num_arete_, 1);
          fac3 = le_dom_VDF.Qdm(num_arete_, 2);
          fac4 = le_dom_VDF.Qdm(num_arete_, 3);

          IntVect f(2);
          f = -2;
          int i = 0;
          if (fac1 != -1)
            {
              f(i) = fac1;
              i++;
            }
          if (fac2 != -1)
            {
              f(i) = fac2;
              i++;
            }
          if (fac3 != -1)
            {
              f(i) = fac3;
              i++;
            }
          if (fac4 != -1)
            {
              f(i) = fac4;
              i++;
            }

          rang1 = f(0);
          rang2 = f(1);

          if (rang1 >= sz_faces_Cl)
            rang1 -= decal_virt;

          if (rang2 >= sz_faces_Cl)
            rang2 -= decal_virt;

          if (les_faces_Cl[rang1] == 3) // face de periodicite
            {
              if (les_faces_Cl[rang2] == 3)  // face de periodicite
                type_arete_coin_[num_arete_] = TypeAreteCoinVDF::PERIO_PERIO;
              else if (les_faces_Cl[rang2] == 0) // face de paroi
                type_arete_coin_[num_arete_] = TypeAreteCoinVDF::PERIO_PAROI;
              else if (les_faces_Cl[rang2] == 1) // Sortie libre ou entree de fluide
                type_arete_coin_[num_arete_] = TypeAreteCoinVDF::PERIO_FLUIDE;
              else
                {
                  Cerr << "On traite une arete qui separe deux faces dans un coin : " << finl;
                  Cerr << "l'une de ces faces porte une condition limite de type periodicite " << finl;
                  Cerr << "et l'autre porte une condition autre que periodicite ou paroi" << finl;
                  Cerr << "On n a pas encore fait la modif!!!" << finl;
                  // exit();
                }
            }
          else if (les_faces_Cl[rang1] == 0)  // face de paroi
            {
              if (les_faces_Cl[rang2] == 3)   // face de periodicite
                type_arete_coin_[num_arete_] = TypeAreteCoinVDF::PERIO_PAROI;
              else if (les_faces_Cl[rang2] == 0) // face de paroi
                type_arete_coin_[num_arete_] = TypeAreteCoinVDF::PAROI_PAROI;
              else if (les_faces_Cl[rang2] == 1)   // face de fluide
                {
                  if (Option_VDF::traitement_coins)
                    type_arete_coin_[num_arete_] = TypeAreteCoinVDF::PAROI_FLUIDE;
                  else
                    /* {
                     Cerr << "On traite une arete qui separe deux faces dans un coin : " << finl;
                     Cerr << "l'une de ces faces porte une condition limite de type paroi " << finl;
                     Cerr << "et l'autre porte une condition autre que periodicite" << finl;
                     Cerr << "On n a pas encore fait la modif!!!" << finl;
                     } */
                    // On ne change rien. De toute facon, ce type d'arete ne necessite pas de traitement particulier
                    type_arete_coin_[num_arete_] = TypeAreteCoinVDF::VIDE;
                }
            }
          else if (les_faces_Cl[rang1] == 1)  // face sortie libre ou entree de fluide
            {
              if (les_faces_Cl[rang2] == 2)  // face symetrie
                type_arete_coin_[num_arete_] = TypeAreteCoinVDF::FLUIDE_NAVIER;
              else if (les_faces_Cl[rang2] == 0) // face paroi
                {
                  if (Option_VDF::traitement_coins)
                    type_arete_coin_[num_arete_] = TypeAreteCoinVDF::FLUIDE_PAROI;
                  else
                    // On ne change rien. De toute facon, ce type d'arete ne necessite pas de traitement particulier
                    type_arete_coin_[num_arete_] = TypeAreteCoinVDF::VIDE;
                }
              else if (les_faces_Cl[rang2] == 1) // face sortie libre ou entree de fluide
                {
                  if (Option_VDF::traitement_coins)
                    type_arete_coin_[num_arete_] = TypeAreteCoinVDF::FLUIDE_FLUIDE;
                  else
                    // On ne change rien. De toute facon, ce type d'arete ne necessite pas de traitement particulier
                    type_arete_coin_[num_arete_] = TypeAreteCoinVDF::VIDE;
                }
              // Modif AC : 27/02/03
              else if (les_faces_Cl[rang2] == 3) // face periodique
                {
                  type_arete_coin_[num_arete_] = TypeAreteCoinVDF::PERIO_FLUIDE;
                }
              // Fin Modif AC : 27/02/03
              else
                {
                  Cerr << "On ne traite pas ce cas pour les_faces_Cl[rang1] = 1 " << finl;
                  Cerr << "les_faces_Cl[rang2] = " << les_faces_Cl[rang2] << finl;
                  exit();
                }
            }
          else if (les_faces_Cl[rang1] == 2)   // face symetrie
            {
              if (les_faces_Cl[rang2] == 0)  // face paroi
                type_arete_coin_[num_arete_] = TypeAreteCoinVDF::PAROI_NAVIER;
              else if (les_faces_Cl[rang2] == 1)  // face sortie libre ou entree de fluide
                type_arete_coin_[num_arete_] = TypeAreteCoinVDF::FLUIDE_NAVIER;
              else if (les_faces_Cl[rang2] == 2) // symetrie
                type_arete_coin_[num_arete_] = TypeAreteCoinVDF::NAVIER_NAVIER;
              else if (les_faces_Cl[rang2] == 3) // periodicite
                {
                  // On ne change rien. De toute facon, ce type d'arete ne necessite pas de traitement particulier
                  type_arete_coin_[num_arete_] = TypeAreteCoinVDF::VIDE;
                }
              else
                {
                  Cerr << "On ne traite pas ce cas pour les_faces_Cl[rang1] = 2 " << finl;
                  Cerr << "les_faces_Cl[rang2] = " << les_faces_Cl[rang2] << finl;
                  Process::exit();
                }
            }
        }
    }

  else
    {
      Cerr << "Domaine_Cl_VDF::completer() attend en argument un Domaine_VDF\n";
      Process::exit();
    }
  Cerr << "Domaine_Cl_VDF::completer OK" << finl;
}

/*! @brief Impose les conditions aux limites a la valeur temporelle "temps" du Champ_Inc
 *
 */
void Domaine_Cl_VDF::imposer_cond_lim(Champ_Inc& ch, double temps)
{
  static int init=0;
  Champ_Inc_base& ch_base=ch.valeur();
  DoubleTab& ch_tab = ch_base.valeurs(temps);
  const int N = ch_tab.line_size();
  if (sub_type(Champ_P0_VDF,ch_base)) { /* Do nothing */}
  else if(ch_base.nature_du_champ()==scalaire) { /* Do nothing */}
  else if (sub_type(Champ_Face_VDF,ch_base))
    {
      Champ_Face_VDF& ch_face = ref_cast(Champ_Face_VDF, ch_base);
      const Domaine_VDF& mon_dom_VDF = ch_face.domaine_vdf();
      int ndeb,nfin, num_face;

      for(int i=0; i<nb_cond_lim(); i++)
        {
          const Cond_lim_base& la_cl = les_conditions_limites(i).valeur();
          if (sub_type(Periodique,la_cl))
            {
              if (init == 0)
                {
                  // On fait en sorte que le champ ait la meme valeur
                  // sur deux faces de periodicite qui sont en face l'une de l'autre
                  const Periodique& la_cl_perio = ref_cast(Periodique,la_cl);
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  int voisine;
                  double moy;
                  for (num_face=ndeb; num_face<nfin; num_face++)
                    {
                      voisine = la_cl_perio.face_associee(num_face-ndeb) + ndeb;
                      if ( ch_tab[num_face] != ch_tab[voisine] )
                        {
                          //                           Cerr << "dans Domaine_Cl_VDF::imposer_cond_lim : on reajuste les vitesses!! pour la face num=" << num_face << finl;
                          //                           Cerr << "difference = ch_tab[num_face]-ch_tab[voisine]=" << ch_tab[num_face]-ch_tab[voisine] << finl;
                          moy = 0.5*(ch_tab[num_face] + ch_tab[voisine]);
                          ch_tab[num_face] = moy;
                          ch_tab[voisine] = moy;
                        }
                    }
                  // Il ne faut pas le faire a la premiere cl mais une fois toutes les cl faites une fois, cas multi perio avec ci non perio
                  // init = 1;
                }
            }
          else if( sub_type(Navier,la_cl) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                for (int n = 0; n < N; n++)
                  ch_tab(num_face, n) = 0;
            }
          else if ( sub_type(Dirichlet_entree_fluide,la_cl) )
            {
              const Dirichlet_entree_fluide& la_cl_diri = ref_cast(Dirichlet_entree_fluide,la_cl);
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (num_face = ndeb; num_face < nfin; num_face++)
                for (int n = 0; n < N; n++)
                  {
                    // WEC : optimisable (pour chaque face recherche le bon temps !)
                    ch_tab(num_face, n) = la_cl_diri.val_imp_au_temps(temps, num_face - ndeb, N * mon_dom_VDF.orientation(num_face) + n);
                  }
            }
          else if ( sub_type(Dirichlet_paroi_fixe,la_cl) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                for (int n = 0; n < N; n++)
                  ch_tab(num_face, n) = 0;
            }
          else if ( sub_type(Dirichlet_paroi_defilante,la_cl) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                for (int n = 0; n < N; n++)
                  ch_tab(num_face, n) = 0;
            }
        }
      init = 1;
    }
  else
    {
      Cerr << "Le type de Champ_Inc " <<  ch->que_suis_je() << " n'est pas prevu en VDF\n";
      exit();
    }
  ch_tab.echange_espace_virtuel();
  Debog::verifier("Domaine_Cl_VDF::imposer_cond_lim ch_tab",ch_tab);
}


int Domaine_Cl_VDF::nb_faces_sortie_libre() const
{
  int compteur = 0;
  for (const auto &itr : les_conditions_limites_)
    {
      if (sub_type(Sortie_libre_pression_imposee, itr.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF, itr->frontiere_dis());
          compteur += le_bord.nb_faces();
        }
    }
  return compteur;
}

Domaine_VDF& Domaine_Cl_VDF::domaine_VDF()
{
  return ref_cast(Domaine_VDF, domaine_dis().valeur());
}

const Domaine_VDF& Domaine_Cl_VDF::domaine_VDF() const
{
  return ref_cast(Domaine_VDF, domaine_dis().valeur());
}

int Domaine_Cl_VDF::nb_faces_bord() const
{
  return domaine_VDF().nb_faces_bord();
}
