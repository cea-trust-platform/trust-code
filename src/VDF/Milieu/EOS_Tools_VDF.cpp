/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Navier_Stokes_Fluide_Dilatable_base.h>
#include <Source_Masse_Fluide_Dilatable_base.h>
#include <Fluide_Dilatable_base.h>
#include <Check_espace_virtuel.h>
#include <Schema_Temps_base.h>
#include <Champ_Face_VDF.h>
#include <communications.h>
#include <Equation_base.h>
#include <EOS_Tools_VDF.h>
#include <Domaine_VDF.h>
#include <Debog.h>

Implemente_instanciable(EOS_Tools_VDF,"EOS_Tools_VDF",EOS_Tools_base);

Sortie& EOS_Tools_VDF::printOn(Sortie& os) const
{
  return os <<que_suis_je()<< finl;
}

Entree& EOS_Tools_VDF::readOn(Entree& is)
{
  return is;
}

void  EOS_Tools_VDF::associer_domaines(const Domaine_dis_base& dds, const Domaine_Cl_dis_base& domaine_cl)
{
  le_dom = ref_cast(Domaine_VDF,dds);
  le_dom_Cl = domaine_cl;
  Champ_Face_VDF toto;
  toto.associer_domaine_dis_base(dds);
  toto.fixer_nb_comp(1);
  toto.fixer_nb_valeurs_nodales(le_dom->nb_faces());
  tab_rho_face=toto.valeurs();
  tab_rho_face_demi=toto.valeurs();
  tab_rho_face_np1=toto.valeurs();
}

/*! @brief Calcule la moyenne volumique de la grandeur P0 donnee
 *
 * @return (DoubleTab&) rho discretise par face
 */
double EOS_Tools_VDF::moyenne_vol(const DoubleTab& tab) const
{
  int nb_elem=le_dom->nb_elem();
  const DoubleVect& volumes = le_dom->volumes();
  assert(tab.dimension(0)==nb_elem);
  ArrOfDouble sum(2);
  sum = 0;
  for (int elem=0 ; elem<nb_elem ; elem++)
    {
      double v = volumes(elem);
      sum[0] += v;
      sum[1] += v*tab(elem);
    }
  mp_sum_for_each_item(sum);
  return sum[1]/sum[0];
}

void EOS_Tools_VDF::calculer_rho_face_np1(const DoubleTab& tab_rhoP0)
{
  int face, elem, nb_faces_tot = le_dom->nb_faces_tot();
  Debog::verifier("tab_rhoP0",tab_rhoP0);
  int i, nb_comp;
  IntTab& face_voisins = le_dom->face_voisins();
  for (face=0 ; face<nb_faces_tot ; face++)
    {
      nb_comp=0;
      tab_rho_face_np1(face) = 0;
      for (i=0 ; i<2 ; i++)
        {
          elem= face_voisins(face,i);
          if (elem!=-1)
            {
              nb_comp++;
              tab_rho_face_np1(face) += tab_rhoP0(elem);
            }
        }
      tab_rho_face_np1(face) /= nb_comp;
    }

  tab_rho_face_np1.echange_espace_virtuel();
  Debog::verifier("tab_rho_face_np1",tab_rho_face_np1);
  for (face=0 ; face<nb_faces_tot ; face++)
    tab_rho_face_demi(face)=(tab_rho_face_np1(face)+tab_rho_face(face))/2.;
}

/*! @brief Renvoie rho avec la meme discretisation que la vitesse : une valeur par face en VDF
 *
 * @return (DoubleTab&) rho discretise par face
 */
const DoubleTab& EOS_Tools_VDF::rho_discvit() const
{
  return tab_rho_face_demi;
}

/*! @brief Renvoie div(u) avec la meme discretisation que la vitesse : une valeur par face en VDF
 *
 * @return div(u) discretise par face
 */
void EOS_Tools_VDF::divu_discvit(const DoubleTab& secmem1, DoubleTab& secmem2)
{
  assert_espace_virtuel_vect(secmem1);
  int nb_faces_tot = le_dom->nb_faces_tot();
  IntTab& face_voisins = le_dom->face_voisins();
  //remplissage de div(u) sur les faces
  for (int face=0 ; face<nb_faces_tot; face++)
    {
      int nb_comp=0;
      secmem2(face)=0;
      for (int i=0 ; i<2 ; i++)
        {
          int elem= face_voisins(face,i);
          if (elem!=-1)
            {
              nb_comp++;
              secmem2(face) += secmem1(elem);
            }
        }
      secmem2(face) /= nb_comp;
    }
  secmem2.echange_espace_virtuel();
}

/*! @brief Calcule le second membre de l'equation de continuite : div(rhoU) = W = -dZ/dT    avec Z=rho
 *
 * @return rho discretise par face
 */
void EOS_Tools_VDF::secmembre_divU_Z(DoubleTab& tab_W) const
{
  double dt = le_fluide().vitesse().equation().schema_temps().pas_de_temps();
  int elem,nb_elem = le_dom->nb_elem();//,nb_faces = le_dom->nb_faces();
  DoubleVect tab_dZ(nb_elem);
  //DoubleTab tab_gradZ(nb_faces);
  const DoubleTab& tab_rhonP0 = le_fluide().loi_etat()->rho_n();
  const DoubleTab& tab_rhonp1P0 = le_fluide().loi_etat()->rho_np1();
  Debog::verifier("divU tab_rhonP0",tab_rhonP0);
  Debog::verifier("divU tab_rhonp1P0",tab_rhonp1P0);
  const DoubleVect& volumes = le_dom->volumes();

  for (elem=0 ; elem<nb_elem ; elem++)
    tab_dZ(elem) = (tab_rhonp1P0(elem)-tab_rhonP0(elem))/dt;

  // Ajout des termes sources speciaux de l'equation de masse:
  const bool has_mass_flux = (sub_type(Navier_Stokes_Fluide_Dilatable_base, le_fluide().vitesse().equation())) ?
                             ref_cast(Navier_Stokes_Fluide_Dilatable_base, le_fluide().vitesse().equation()).has_source_masse() : false;

  if (has_mass_flux)
    {
      const Source_Masse_Fluide_Dilatable_base& src_mass = ref_cast(Navier_Stokes_Fluide_Dilatable_base, le_fluide().vitesse().equation()).source_masse();
      src_mass.ajouter_projection(le_fluide(), static_cast<DoubleTab&>(tab_dZ));
    }

  for (elem = 0; elem < nb_elem; elem++)
    tab_W(elem) = -tab_dZ(elem) * volumes(elem);
}

void EOS_Tools_VDF::mettre_a_jour(double temps)
{
  int n=tab_rho_face_np1.size_totale();
  for (int i=0; i<n; i++)
    {
      tab_rho_face(i)=tab_rho_face_np1(i);
      tab_rho_face_demi(i)=tab_rho_face_np1(i);
    }
}
