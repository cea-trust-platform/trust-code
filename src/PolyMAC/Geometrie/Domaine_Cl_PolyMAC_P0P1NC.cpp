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

#include <Domaine_Cl_PolyMAC_P0P1NC.h>
#include <Domaine_PolyMAC_P0P1NC.h>
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
#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Debog.h>


Implemente_instanciable(Domaine_Cl_PolyMAC_P0P1NC,"Domaine_Cl_PolyMAC_P0P1NC",Domaine_Cl_dis_base);
//// printOn
//

Sortie& Domaine_Cl_PolyMAC_P0P1NC::printOn(Sortie& os ) const
{
  return os;
}

//// readOn
//

Entree& Domaine_Cl_PolyMAC_P0P1NC::readOn(Entree& is )
{
  return Domaine_Cl_dis_base::readOn(is) ;
}


/////////////////////////////////////////////////////////////////////
//
// Implementation des fonctions de la classe Domaine_Cl_PolyMAC_P0P1NC
//
/////////////////////////////////////////////////////////////////////

/*! @brief etape de discretisation : dimensionnement des tableaux
 *
 */
void Domaine_Cl_PolyMAC_P0P1NC::associer(const Domaine_PolyMAC_P0P1NC& le_dom_PolyMAC_P0P1NC)
{
  //  int nb_elem_Cl  = le_dom_PolyMAC_P0P1NC.nb_elem_Cl();
}

/*! @brief remplissage des tableaux
 *
 */
void Domaine_Cl_PolyMAC_P0P1NC::completer(const Domaine_dis& un_domaine_dis)
{
  modif_perio_fait_ = 0;
  if (sub_type(Domaine_PolyMAC_P0P1NC,un_domaine_dis.valeur()))
    {
      const Domaine_PolyMAC_P0P1NC& le_dom_poly = ref_cast(Domaine_PolyMAC_P0P1NC, un_domaine_dis.valeur());
      remplir_type_elem_Cl(le_dom_poly);
    }
  else
    {
      cerr << "Domaine_Cl_PolyMAC_P0P1NC::completer() prend comme argument un Domaine_PolyMAC_P0P1NC\n";
      exit();
    }
}
/*! @brief appele par remplir_volumes_entrelaces_Cl() : remplissage de type_elem_Cl_
 *
 */
void Domaine_Cl_PolyMAC_P0P1NC::remplir_type_elem_Cl(const Domaine_PolyMAC_P0P1NC& le_dom_PolyMAC_P0P1NC)
{
}

/*! @brief Impose les conditions aux limites a la valeur temporelle "temps" du Champ_Inc
 *
 */
void Domaine_Cl_PolyMAC_P0P1NC::imposer_cond_lim(Champ_Inc& ch, double temps)
{

  Champ_Inc_base& ch_base=ch.valeur();
  DoubleTab& ch_tab = ch_base.valeurs(temps);
  int n, N = ch_tab.line_size();
  if (sub_type(Champ_Inc_P0_base,ch_base)) { /* do nothing */ }
  else if(ch_base.nature_du_champ()==scalaire) { /* do nothing */ }
  else if (sub_type(Champ_Face_PolyMAC_P0P1NC,ch_base))
    {
      Champ_Face_PolyMAC_P0P1NC& ch_face = ref_cast(Champ_Face_PolyMAC_P0P1NC, ch_base);
      const Domaine_VF& mon_dom_VF = ch_face.domaine_vf();
      int ndeb,nfin, num_face;

      for(int i=0; i<nb_cond_lim(); i++)
        {
          const Cond_lim_base& la_cl = les_conditions_limites(i).valeur();
          if (sub_type(Periodique,la_cl)) abort(); //nope
          else if( sub_type(Symetrie,la_cl) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                for (n = 0; n < N; n++)
                  ch_tab(num_face, n) = 0;
            }
          else if ( sub_type(Dirichlet_entree_fluide,la_cl) )
            {
              const Dirichlet_entree_fluide& la_cl_diri = ref_cast(Dirichlet_entree_fluide,la_cl);
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (num_face=ndeb; num_face<nfin; num_face++)
                for (n = 0; n < N; n++)
                  {
                    // WEC : optimisable (pour chaque face recherche le bon temps !)
                    // vn
                    double vn=0;
                    for (int d=0; d<dimension; d++)
                      vn+=mon_dom_VF.face_normales(num_face,d)*la_cl_diri.val_imp_au_temps(temps,num_face-ndeb, N * d + n);
                    vn/=mon_dom_VF.face_surfaces(num_face);
                    ch_tab(num_face, n) = vn;
                  }
            }
          else if ( sub_type(Dirichlet_paroi_fixe,la_cl) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                for (n = 0; n < N; n++)
                  ch_tab(num_face, n) = 0;
            }
          else if ( sub_type(Dirichlet_paroi_defilante,la_cl) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                for (n = 0; n < N; n++)
                  ch_tab(num_face, n) = 0;
            }
        }
      modif_perio_fait_  = 1;
    }
  else
    {
      Cerr << "Le type de Champ_Inc " <<  ch->que_suis_je() << " n'est pas prevu en PolyMAC_P0P1NC\n";
      exit();
    }
  ch_tab.echange_espace_virtuel();
  Debog::verifier("Domaine_Cl_PolyMAC_P0P1NC::imposer_cond_lim ch_tab",ch_tab);
}

int Domaine_Cl_PolyMAC_P0P1NC::nb_faces_sortie_libre() const
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

int Domaine_Cl_PolyMAC_P0P1NC::nb_bord_periodicite() const
{
  int compteur = 0;
  for (const auto &itr : les_conditions_limites_)
    {
      if (sub_type(Periodique, itr.valeur()))
        compteur++;
    }
  return compteur;
}


int Domaine_Cl_PolyMAC_P0P1NC::initialiser(double temps)
{
  Domaine_Cl_dis_base::initialiser(temps);

  if (nb_bord_periodicite()>0)
    {
      Cerr<<" La periodicite n'est pas code !!!"<<finl;
      abort();
    }
  return 1;
}

Domaine_PolyMAC_P0P1NC& Domaine_Cl_PolyMAC_P0P1NC::domaine_PolyMAC_P0P1NC()
{
  return ref_cast(Domaine_PolyMAC_P0P1NC, domaine_dis().valeur());
}

const Domaine_PolyMAC_P0P1NC& Domaine_Cl_PolyMAC_P0P1NC::domaine_PolyMAC_P0P1NC() const
{
  return ref_cast(Domaine_PolyMAC_P0P1NC, domaine_dis().valeur());
}
