/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        LoiParoiHybride.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <LoiParoiHybride.h>
#include <Motcle.h>
#include <Zone_dis.h>
#include <Zone_Cl_dis.h>
#include <EChaine.h>
#include <Les_Cl.h>
#include <Front_VF.h>




Entree& LoiParoiHybride::lire(Entree& is, const Noms& noms_bord, const Mod_turb_hyd_base& mod)
{
  const int nbord = noms_bord.size();
  int lp_lue=0;

  Motcle accouverte = "{" ;//, accfermee = "}" ;
  int N;
  Motcle motlu;

  //Lecture nb de loi de parois (LP par defaut comprise)
  is >> N;
  vect_lp.dimensionner(N);
  lp_bord=0;

  //Lecture de la loi de paroi par defaut :
  is >> motlu;

  if (motlu!="defaut")
    {
      Cerr << "Avec LoiParoiHybride, vous devez rentrer obligatoirement une loi de paroi par defaut  !"<< finl;
      Process::exit();
    }
  vect_lp[0].associer_modele(mod);
  is >> vect_lp[0];

  is >> motlu;
  if (motlu==accouverte)
    is >>  vect_lp[0].valeur();
  else
    lp_lue=1;

  //Lecture des autres lois de paroi
  for (int i=1; i<N; i++)
    {
      int m;

      vect_lp[i].associer_modele(mod);
      if (lp_lue==0)
        is >> vect_lp[i];
      else // C est un peu tordu cette histoire de lecture des lois de paroi seulement si un "{" est lu
        {
          EChaine ech(motlu);
          ech >> vect_lp[i];
          lp_lue=0;
        }



      is >> m;

      for (int j=0; j<m; j++)
        {
          Nom bord;
          is>>bord;
          for (int ibord=0; ibord<nbord; ibord++)
            {
              if (bord==noms_bord[ibord])
                {
                  lp_bord(ibord)=i;
                  break;
                }
            }
        }

      is >> motlu;
      if (motlu==accouverte)
        is >>  vect_lp[i].valeur();
      else
        lp_lue=1;

    }
  //for (int ibord=0;ibord<nbord;ibord++)
  //  Cout << "lp_bord("<<ibord<<") " << lp_bord(ibord) << finl;

  if (lp_lue==0)
    is >> motlu;
  return is;
}

void LoiParoiHybride::associer(const Zone_dis& zd, const Zone_Cl_dis& zcl)
{
  int size = vect_lp.size();
  for (int ilp=0; ilp<size; ilp++)
    {
      vect_lp[ilp]->associer(zd, zcl);
    }
  lp_bord.resize(zd.valeur().nb_front_Cl());
}


int LoiParoiHybride::init_lois_paroi()
{
  //dimensionnement du cisaillement ?? selon discretisation ???....

  int size = vect_lp.size();
  for (int ilp=0; ilp<size; ilp++)
    {
      vect_lp[ilp]->init_lois_paroi();
    }

  return 1;
}

int LoiParoiHybride::calculer_hyd(DoubleTab& tab1)
{
  int size = vect_lp.size();
  for (int ilp=0; ilp<size; ilp++)
    {
      vect_lp[ilp]->calculer_hyd(tab1);
    }

  //Remplissage du tableau Cisaillement en fonction du choix de la LP par bord


  return 1;
}

int LoiParoiHybride::calculer_hyd(DoubleTab& tab1, const Zone_dis_base& zd, const Zone_Cl_dis_base& zcl, DoubleTab& tab_cis)
{
  int size = vect_lp.size();
  for (int ilp=0; ilp<size; ilp++)
    {
      vect_lp[ilp]->calculer_hyd(tab1);
    }

  //Remplissage du tableau Cisaillement en fonction du choix de la LP par bord
  for (int n_bord=0; n_bord<zd.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zcl.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante,la_cl.valeur() ))
        {
          const DoubleTab& tau = vect_lp[lp_bord(n_bord)]->Cisaillement_paroi();

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              for (int idim=0; idim<Objet_U::dimension; idim++)
                tab_cis(num_face,idim) = tau(num_face,idim);
            }
        }
    }

  tab_cis.echange_espace_virtuel();


  return 1;
}


int LoiParoiHybride::calculer_hyd(DoubleTab& tab1, DoubleTab& tab2)
{
  int size = vect_lp.size();
  for (int ilp=0; ilp<size; ilp++)
    {
      vect_lp[ilp]->calculer_hyd(tab1, tab2);
    }

  //Remplissage du tableau Cisaillement en fonction du choix de la LP par bord


  return 1;
}

int LoiParoiHybride::calculer_hyd(DoubleTab& tab1, DoubleTab& tab2, const Zone_dis_base& zd, const Zone_Cl_dis_base& zcl, DoubleTab& tab_cis)
{
  int size = vect_lp.size();
  for (int ilp=0; ilp<size; ilp++)
    {
      vect_lp[ilp]->calculer_hyd(tab1, tab2);
    }

  //Remplissage du tableau Cisaillement en fonction du choix de la LP par bord
  for (int n_bord=0; n_bord<zd.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zcl.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante,la_cl.valeur() ))
        {
          const DoubleTab& tau = vect_lp[lp_bord(n_bord)]->Cisaillement_paroi();

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              for (int idim=0; idim<Objet_U::dimension; idim++)
                tab_cis(num_face,idim) = tau(num_face,idim);
            }
        }
    }

  tab_cis.echange_espace_virtuel();



  return 1;
}

