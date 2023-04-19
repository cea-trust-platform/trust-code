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
// File:        Domaine_Cl_PolyMAC_old.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC_old/Domaines
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Domaine_Cl_PolyMAC_old.h>
#include <Domaine_PolyMAC_old.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Periodique.h>
#include <Dirichlet_entree_fluide_leaves.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Equation_base.h>
#include <Champ_Inc_P0_base.h>
#include <Champ_front_softanalytique.h>
#include <Probleme_base.h>
#include <Discretisation_base.h>
#include <Matrice_Morse.h>
#include <Champ_Face_PolyMAC_old.h>
#include <Debog.h>


Implemente_instanciable(Domaine_Cl_PolyMAC_old,"Domaine_Cl_PolyMAC_old",Domaine_Cl_dis_base);
//// printOn
//

Sortie& Domaine_Cl_PolyMAC_old::printOn(Sortie& os ) const
{
  return os;
}

//// readOn
//

Entree& Domaine_Cl_PolyMAC_old::readOn(Entree& is )
{
  return Domaine_Cl_dis_base::readOn(is) ;
}


/////////////////////////////////////////////////////////////////////
//
// Implementation des fonctions de la classe Domaine_Cl_PolyMAC_old
//
/////////////////////////////////////////////////////////////////////

// Description:
// etape de discretisation : dimensionnement des tableaux
void Domaine_Cl_PolyMAC_old::associer(const Domaine_PolyMAC_old& la_domaine_PolyMAC_old)
{
  //  int nb_elem_Cl  = la_domaine_PolyMAC_old.nb_elem_Cl();
}

// Description:
// remplissage des tableaux
void Domaine_Cl_PolyMAC_old::completer(const Domaine_dis& une_domaine_dis)
{
  modif_perio_fait_ =0;
  if (sub_type(Domaine_PolyMAC_old,une_domaine_dis.valeur()))
    {
      const Domaine_PolyMAC_old& la_domaine_PolyMAC_old = ref_cast(Domaine_PolyMAC_old, une_domaine_dis.valeur());
      remplir_type_elem_Cl(la_domaine_PolyMAC_old);
    }
  else
    {
      cerr << "Domaine_Cl_PolyMAC_old::completer() prend comme argument une Domaine_PolyMAC_old\n";
      exit();
    }
}
// Description:
// appele par remplir_volumes_entrelaces_Cl() : remplissage de type_elem_Cl_
void Domaine_Cl_PolyMAC_old::remplir_type_elem_Cl(const Domaine_PolyMAC_old& la_domaine_PolyMAC_old)
{
}

// Description:
// Impose les conditions aux limites a la valeur temporelle "temps" du
// Champ_Inc
void Domaine_Cl_PolyMAC_old::imposer_cond_lim(Champ_Inc& ch, double temps)
{

  Champ_Inc_base& ch_base=ch.valeur();
  DoubleTab& ch_tab = ch_base.valeurs(temps);
  if (sub_type(Champ_Inc_P0_base,ch_base))
    ;
  else if(ch_base.nature_du_champ()==scalaire)
    ;
  else if (sub_type(Champ_Face_PolyMAC_old,ch_base))
    {
      Champ_Face_PolyMAC_old& ch_face = ref_cast(Champ_Face_PolyMAC_old, ch_base);
      const Domaine_VF& ma_domaine_VF = ch_face.domaine_vf();
      int ndeb,nfin, num_face;

      for(int i=0; i<nb_cond_lim(); i++)
        {
          const Cond_lim_base& la_cl = les_conditions_limites(i).valeur();
          if (sub_type(Periodique,la_cl))
            {
              if (modif_perio_fait_  == 0)
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
                          moy = 0.5*(ch_tab[num_face] + ch_tab[voisine]);
                          ch_tab[num_face] = moy;
                          ch_tab[voisine] = moy;
                        }
                    }
                  // Il ne faut pas le faire a la premiere cl mais une fois toutes les cl faites une fois, cas multi perio avec ci non perio
                  // init = 1;
                }
            }
          else if( sub_type(Symetrie,la_cl) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                ch_tab[num_face] = 0;
            }
          else if ( sub_type(Dirichlet_entree_fluide,la_cl) )
            {
              const Dirichlet_entree_fluide& la_cl_diri = ref_cast(Dirichlet_entree_fluide,la_cl);
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (num_face=ndeb; num_face<nfin; num_face++)
                {
                  // WEC : optimisable (pour chaque face recherche le bon temps !)
                  // vn
                  double vn=0;
                  for (int d=0; d<dimension; d++)
                    vn+=ma_domaine_VF.face_normales(num_face,d)*la_cl_diri.val_imp_au_temps(temps,num_face-ndeb,d);
                  vn/=ma_domaine_VF.face_surfaces(num_face);
                  ch_tab[num_face] = vn;
                }
            }
          else if ( sub_type(Dirichlet_paroi_fixe,la_cl) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                ch_tab[num_face] = 0;
            }
          else if ( sub_type(Dirichlet_paroi_defilante,la_cl) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                ch_tab[num_face] = 0;
            }
        }
      modif_perio_fait_  = 1;
    }
  else
    {
      Cerr << "Le type de Champ_Inc " <<  ch->que_suis_je() << " n'est pas prevu en PolyMAC_old\n";
      exit();
    }
  ch_tab.echange_espace_virtuel();
  Debog::verifier("Domaine_Cl_PolyMAC_old::imposer_cond_lim ch_tab",ch_tab);
}





int Domaine_Cl_PolyMAC_old::nb_faces_sortie_libre() const
{
  exit();
  /*
  int compteur=0;
  for(int cl=0; cl<les_conditions_limites_.size(); cl++)
    {
      if(sub_type(Neumann_sortie_libre, les_conditions_limites_[cl].valeur()))
  {
    const Front_VF& le_bord=ref_cast(Front_VF,les_conditions_limites_[cl]->frontiere_dis());
    compteur+=le_bord.nb_faces();
  }
    }
  return compteur;
  */
  return -1;
}

int Domaine_Cl_PolyMAC_old::nb_bord_periodicite() const
{
  int compteur=0;
  for(int cl=0; cl<les_conditions_limites_.size(); cl++)
    {
      if (sub_type(Periodique,les_conditions_limites_[cl].valeur()))
        compteur++;
    }
  return compteur;
}


int Domaine_Cl_PolyMAC_old::initialiser(double temps)
{
  Domaine_Cl_dis_base::initialiser(temps);

  if (nb_bord_periodicite()>0)
    {
      Cerr<<" La periodicite n'est pas code !!!"<<finl;
      abort();
    }
  return 1;
}

Domaine_PolyMAC_old& Domaine_Cl_PolyMAC_old::domaine_PolyMAC_old()
{
  return ref_cast(Domaine_PolyMAC_old, domaine_dis().valeur());
}

const Domaine_PolyMAC_old& Domaine_Cl_PolyMAC_old::domaine_PolyMAC_old() const
{
  return ref_cast(Domaine_PolyMAC_old, domaine_dis().valeur());
}
