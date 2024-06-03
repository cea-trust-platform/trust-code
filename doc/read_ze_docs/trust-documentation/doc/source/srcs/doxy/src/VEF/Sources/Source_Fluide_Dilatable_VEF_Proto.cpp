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

#include <Source_Fluide_Dilatable_VEF_Proto.h>
#include <Neumann_sortie_libre.h>
#include <Discretisation_base.h>
#include <Equation_base.h>
#include <Domaine_Cl_VEF.h>
#include <Milieu_base.h>
#include <Periodique.h>
#include <TRUSTTab.h>
#include <Domaine_VEF.h>
#include <Symetrie.h>


void Source_Fluide_Dilatable_VEF_Proto::associer_domaines_impl(const Domaine_dis& domaine,const Domaine_Cl_dis& domaine_cl)
{
  le_dom = ref_cast(Domaine_VEF,domaine.valeur());
  le_dom_Cl = ref_cast(Domaine_Cl_VEF,domaine_cl.valeur());
}

void Source_Fluide_Dilatable_VEF_Proto::associer_volume_porosite_impl(const Domaine_dis& domaine, DoubleVect& volumes, DoubleVect& porosites)
{
  volumes.ref(ref_cast(Domaine_VF,domaine.valeur()).volumes_entrelaces());
  porosites.ref(le_dom_Cl->equation().milieu().porosite_face());
}

void Source_Fluide_Dilatable_VEF_Proto::ajouter_impl(const Equation_base& eqn,const DoubleVect& g,
                                                     const int dimension, const double rho_m,
                                                     const DoubleTab& tab_rho, DoubleTab& resu) const
{
  const int nb_faces = le_dom->nb_faces(), premiere_face_interne = le_dom->premiere_face_int();
  const DoubleVect& volumes_entrelaces = le_dom->volumes_entrelaces();
  const DoubleVect& porosite_face = le_dom_Cl->equation().milieu().porosite_face();
  const IntTab& face_voisins = le_dom->face_voisins();
  const DoubleTab& face_normales = le_dom->face_normales(), xp = le_dom->xp(), xv = le_dom->xv();

  if (eqn.discretisation().que_suis_je()=="VEF")
    {
      // Boucle faces bord
      for (int num_cl=0 ; num_cl<le_dom->nb_front_Cl() ; num_cl++)
        {
          const Cond_lim& la_cl = le_dom_Cl->les_conditions_limites(num_cl);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
            {
              for (int face=ndeb ; face<nfin ; face++)
                {
                  int elem1 = face_voisins(face,0);
                  if (elem1==-1)  elem1 = face_voisins(face,1);

                  for (int comp=0 ; comp< Objet_U::dimension ; comp++)
                    {
                      double delta_coord = (xv(face,comp) - xp(elem1,comp));
                      for (int i=0 ; i<dimension ; i++)
                        resu(face,i) += (tab_rho(face)-rho_m)*delta_coord*face_normales(face,i)*g(comp);
                    }
                }
            }
          else if (sub_type(Periodique,la_cl.valeur()))
            {
              for (int face=premiere_face_interne ; face<nb_faces; face++)
                {
                  int elem1 = face_voisins(face,0), elem2 = face_voisins(face,1);
                  for (int comp=0 ; comp<dimension ; comp++)
                    {
                      double delta_coord = (xp(elem2,comp) - xp(elem1,comp));
                      for (int i=0 ; i<dimension ; i++)
                        resu(face,i) += (tab_rho(face)-rho_m)*delta_coord*face_normales(face,i)*g(comp);
                    }
                }
            }
        }
      // Boucle faces internes
      for (int face=premiere_face_interne ; face<nb_faces; face++)
        {
          int elem1 = face_voisins(face,0), elem2 = face_voisins(face,1);
          for (int comp=0 ; comp<dimension ; comp++)
            {
              double delta_coord = (xp(elem2,comp) - xp(elem1,comp));
              for (int i=0 ; i<dimension ; i++)
                resu(face,i) += (tab_rho(face)-rho_m)*delta_coord*face_normales(face,i)*g(comp);
            }
        }
    }
  else if (eqn.discretisation().que_suis_je()=="VEFPreP1B")
    {
      // Boucle faces bord
      for (int num_cl=0 ; num_cl<le_dom->nb_front_Cl() ; num_cl++)
        {
          const Cond_lim& la_cl = le_dom_Cl->les_conditions_limites(num_cl);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          if (sub_type(Neumann_sortie_libre,la_cl.valeur())||sub_type(Symetrie,la_cl.valeur())||sub_type(Periodique,la_cl.valeur()))
            {
              for (int face=ndeb ; face<nfin ; face++)
                for (int comp=0 ; comp<dimension ; comp++)
                  resu(face,comp) += (tab_rho(face)-rho_m)*volumes_entrelaces(face)*porosite_face(face)*g(comp);
            }
        }
      // Boucle faces internes
      for (int face=premiere_face_interne ; face<nb_faces; face++)
        for (int comp=0 ; comp<dimension ; comp++)
          resu(face,comp) += (tab_rho(face)-rho_m)*volumes_entrelaces(face)*porosite_face(face)*g(comp);
    }
  else
    {
      Cerr<<"La discretisation "<<eqn.discretisation().que_suis_je()<<" n'est pas reconnue dans Source_Fluide_Dilatable_VEF_Proto"<<finl;
      Process::exit();
    }
}
