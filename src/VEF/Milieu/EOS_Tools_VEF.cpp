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

#include <EOS_Tools_VEF.h>
#include <Domaine.h>
#include <Domaine_VEF.h>
#include <Debog.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Fluide_Dilatable_base.h>
#include <Loi_Etat_Melange_GP_base.h>
#include <Porosites_champ.h>
#include <Champ_P1NC.h>
#include <Check_espace_virtuel.h>
#include <kokkos++.h>
#include <TRUSTArray_kokkos.tpp>
#include <TRUSTTab_kokkos.tpp>

Implemente_instanciable(EOS_Tools_VEF,"EOS_Tools_VEF",EOS_Tools_base);

Sortie& EOS_Tools_VEF::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& EOS_Tools_VEF::readOn(Entree& is)
{
  return is;
}

/*! @brief Associe les domaines a l'EDO
 *
 * @param (Domaine_dis& domaine) domaine
 * @param (Domaine_Cl_dis& domaine_cl) domaine cl
 */
void  EOS_Tools_VEF::associer_domaines(const Domaine_dis& domaine, const Domaine_Cl_dis& domaine_cl)
{
  le_dom = ref_cast(Domaine_VEF,domaine.valeur());
  le_dom_Cl = domaine_cl;
}

/*! @brief Renvoie rho avec la meme discretisation que la vitesse : une valeur par face en VEF
 *
 * @return (DoubleTab&) rho discretise par face
 */
const DoubleTab& EOS_Tools_VEF::rho_discvit() const
{
  return le_fluide_->masse_volumique().valeurs();
}

const DoubleTab& EOS_Tools_VEF::rho_face_n() const
{
  return le_fluide_->rho_n();
}

const DoubleTab& EOS_Tools_VEF::rho_face_np1() const
{
  return le_fluide_->rho_np1();
}

/*! @brief Calcule la moyenne volumique de la grandeur P1NC donnee
 *
 * @return (DoubleTab&) rho discretise par face
 */
double EOS_Tools_VEF::moyenne_vol(const DoubleTab& tab) const
{
  assert(tab.line_size() == 1);
  double x = Champ_P1NC::calculer_integrale_volumique(le_dom.valeur(), tab, FAUX_EN_PERIO);
  // La facon simple de faire serait celle-ci, mais c'est faux a cause de FAUX_EN_PERIO
  //  qui compte deux fois les volumes entrelaces des faces periodiques:
  DoubleVect un;
  le_dom.valeur().creer_tableau_faces(un, RESIZE_OPTIONS::NOCOPY_NOINIT);
  un = 1.;
  double y = Champ_P1NC::calculer_integrale_volumique(le_dom.valeur(), un, FAUX_EN_PERIO);
  return x / y;
}

/*! @brief Div(u) is computed on faces from Div(u) on elements
 *
 */
void EOS_Tools_VEF::divu_discvit(const DoubleTab& DivVelocityElements, DoubleTab& DivVelocityFaces)
{
  assert_espace_virtuel_vect(DivVelocityElements);
  //Corrections pour moyenner div(u) sur les faces
  const DoubleVect& volumes = le_dom->volumes();
  const DoubleVect& volumes_entrelaces = le_dom->volumes_entrelaces();
  const DoubleVect& volumes_entrelaces_Cl = ref_cast(Domaine_Cl_VEF,le_dom_Cl->valeur()).volumes_entrelaces_Cl();

  IntTab& face_voisins = le_dom->face_voisins();
  int premiere_fac_std=le_dom->premiere_face_std();

  //remplissage de div(u) sur les faces
  for (int face=0; face<premiere_fac_std; face++)
    {
      int nb_comp=0;
      DivVelocityFaces(face)=0;
      for (int i=0 ; i<2 ; i++)
        {
          int elem= face_voisins(face,i);
          if (elem!=-1)
            {
              nb_comp++;
              DivVelocityFaces(face) += DivVelocityElements(elem)*(volumes_entrelaces_Cl(face)/volumes(elem));
            }
        }
      DivVelocityFaces(face) /= nb_comp;
    }

  int size = le_dom->nb_faces();
  for (int face=premiere_fac_std; face<size; face++)
    {
      int nb_comp=0;
      DivVelocityFaces(face)=0;
      for (int i=0 ; i<2 ; i++)
        {
          int elem= face_voisins(face,i);
          if (elem!=-1)
            {
              nb_comp++;
              DivVelocityFaces(face) += DivVelocityElements(elem)*(volumes_entrelaces(face)/volumes(elem));
            }
        }
      DivVelocityFaces(face) /= nb_comp;
    }
}

/*! @brief Calcule le second membre de l'equation de continuite pour une discretisation VEF_P1B: div(U) = W = dZ/dT + U.
 *
 * grad(Z)    avec Z=ln(rho)
 *
 * @return rho discretise par face
 */
void EOS_Tools_VEF::secmembre_divU_Z(DoubleTab& tab_W) const
{
  double dt = le_fluide().vitesse()->equation().schema_temps().pas_de_temps();
  const IntTab& face_sommets = le_dom->face_sommets();
  int nb_elem_tot = le_dom->nb_elem_tot();
  int nb_som_tot = le_dom->domaine().nb_som_tot();
  int nb_faces_tot = le_dom->nb_faces_tot();
  const Equation_base& eq = le_fluide().vitesse()->equation();

  // Dimensionnement de tab_dZ
  const Navier_Stokes_std& eqns = ref_cast(Navier_Stokes_std, eq);
  const DoubleVect& pression = eqns.pression().valeurs();

  // ToDo OpenMP: exemple ou DoubleTrav est important pour le GPU:
  DoubleVect tab_dZ = pression;
  DoubleVect tab_rhon_som(nb_som_tot);
  DoubleVect volume_int_som(nb_som_tot);
  DoubleVect tab_rhonp1_som(nb_som_tot);

  const DoubleTab& tab_rhon = le_fluide().loi_etat()->rho_n();
  const DoubleTab& tab_rhonp1 = le_fluide().loi_etat()->rho_np1();
  DoubleTab tab_rhonP1_, tab_rhonp1P1_;
  const DoubleVect& porosite_face = le_fluide().porosite_face();
  const DoubleTab& tab_rhonP1 = modif_par_porosite_si_flag(tab_rhon, tab_rhonP1_, 1, porosite_face);
  const DoubleTab& tab_rhonp1P1 = modif_par_porosite_si_flag(tab_rhonp1, tab_rhonp1P1_, 1, porosite_face);
  const DoubleVect& volumes_entrelaces = le_dom->volumes_entrelaces();
  const Domaine_VEF& zp1b = ref_cast(Domaine_VEF, le_dom.valeur());

  int nfe = le_dom->domaine().nb_faces_elem();
  int nsf = le_dom->nb_som_face();
  const Domaine& dom = le_dom->domaine();

  // calcul de la somme des volumes entrelacees autour d'un sommet
  volume_int_som = 0.;
  CIntTabView face_sommets_v = face_sommets.view_ro();
  CDoubleArrView volumes_entrelaces_v = volumes_entrelaces.view_ro();
  CIntArrView renum_som_perio_v = dom.get_renum_som_perio().view_ro();
  DoubleArrView volume_int_som_v = volume_int_som.view_rw();
  start_gpu_timer();
  Kokkos::parallel_for("EOS_Tools_VEF::secmembre_divU_Z 1", nb_faces_tot, KOKKOS_LAMBDA(
                         const int face)
  {
    for (int som = 0; som < nsf; som++)
      {
        int som_glob = renum_som_perio_v(face_sommets_v(face, som));
        Kokkos::atomic_add(&volume_int_som_v(som_glob), volumes_entrelaces_v(face));
      }
  });
  end_gpu_timer(Objet_U::computeOnDevice, "[KOKKOS]EOS_Tools_VEF::secmembre_divU_Z face loop");

  //discretisation de rho sur les sommets
  tab_rhon_som = 0;
  tab_rhonp1_som = 0;

  CDoubleTabView tab_rhonP1_v = tab_rhonP1.view_ro();
  CDoubleTabView tab_rhonp1P1_v = tab_rhonp1P1.view_ro();
  DoubleArrView tab_rhon_som_v = tab_rhon_som.view_rw();
  DoubleArrView tab_rhonp1_som_v = tab_rhonp1_som.view_rw();
  start_gpu_timer();
  Kokkos::parallel_for("EOS_Tools_VEF::secmembre_divU_Z 2", nb_faces_tot, KOKKOS_LAMBDA(
                         const int face)
  {
    for (int som = 0; som < nsf; som++)
      {
        int som_glob = renum_som_perio_v(face_sommets_v(face, som));
        // double pond = 1./nsf; // version_originale
        double pond = volumes_entrelaces_v(face) / volume_int_som_v(som_glob);
        Kokkos::atomic_add(&tab_rhon_som_v(som_glob), tab_rhonP1_v(face, 0) * pond);
        Kokkos::atomic_add(&tab_rhonp1_som_v(som_glob), tab_rhonp1P1_v(face, 0) * pond);
      }
  });
  end_gpu_timer(Objet_U::computeOnDevice, "[KOKKOS]EOS_Tools_VEF::secmembre_divU_Z face loop");

//Corrections pour test de la moyenne de la derivee de la masse volumique
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tab_dZ=",tab_dZ);
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tab_rhonP1=",tab_rhonP1);
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tab_rhonp1P1=",tab_rhonp1P1);
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z dt=",dt);

  int p_has_elem=zp1b.get_alphaE();
  int p_has_som=zp1b.get_alphaS();
  int p_has_arrete=zp1b.get_alphaA();
  int nb_ar_tot = le_dom->domaine().nb_aretes_tot();

  // ToDo_Kokkos, try to merge all theses kernels for efficiency:
  int decal=0;
  DoubleArrView tab_dZ_v = tab_dZ.view_rw();
  if (p_has_elem)
    {
      CIntTabView elem_faces_v = le_dom->elem_faces().view_ro();
      start_gpu_timer();
      Kokkos::parallel_for("EOS_Tools_VEF::secmembre_divU_Z 3", nb_elem_tot, KOKKOS_LAMBDA(
                             const int elem)
      {
        double rn = 0;
        double rnp1 = 0;
        for (int face = 0; face < nfe; face++)
          {
            rn += tab_rhonP1_v(elem_faces_v(elem, face), 0);
            rnp1 += tab_rhonp1P1_v(elem_faces_v(elem, face), 0);
          }
        tab_dZ_v(elem) = (rnp1 - rn) / (nfe * dt);
      });
      end_gpu_timer(Objet_U::computeOnDevice, "[KOKKOS]EOS_Tools_VEF::secmembre_divU_Z elem loop");
      decal += nb_elem_tot;
    }

  if (p_has_som)
    {
      start_gpu_timer();
      Kokkos::parallel_for("EOS_Tools_VEF::secmembre_divU_Z 4", nb_som_tot, KOKKOS_LAMBDA(
                             const int som)
      {
        tab_dZ_v(decal + som) = ((tab_rhonp1_som_v(som)) - (tab_rhon_som_v(som))) / dt;
      });
      end_gpu_timer(Objet_U::computeOnDevice, "[KOKKOS]EOS_Tools_VEF::secmembre_divU_Z som loop");
      decal += nb_som_tot;
    }

  if (p_has_arrete)
    {
      for (int ar = 0; ar < nb_ar_tot; ar++)
        tab_dZ(decal + ar) = 0;
    }
  tab_dZ.echange_espace_virtuel();
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tab_dZ=",tab_dZ);

  DoubleTabView tab_W_v = tab_W.view_rw();
  decal=0;
  if (p_has_elem)
    {
      double coefdivelem=1;
      CDoubleArrView volumes_v = le_dom->volumes().view_ro();
      start_gpu_timer();
      Kokkos::parallel_for("EOS_Tools_VEF::secmembre_divU_Z 5", nb_elem_tot, KOKKOS_LAMBDA(
                             const int elem)
      {
        tab_W_v(elem, 0) = -coefdivelem * tab_dZ_v(elem) * volumes_v(elem);
      });
      end_gpu_timer(Objet_U::computeOnDevice, "[KOKKOS]EOS_Tools_VEF::secmembre_divU_Z elem loop");
      decal+=nb_elem_tot;
    }
  if (p_has_som)
    {
      double coefdivsom=1;
      CDoubleArrView volumes_controle_v = zp1b.volume_aux_sommets().view_ro();
      start_gpu_timer();
      Kokkos::parallel_for("EOS_Tools_VEF::secmembre_divU_Z 6", nb_som_tot, KOKKOS_LAMBDA(
                             const int som)
      {
        tab_W_v(decal + som, 0) = -coefdivsom * tab_dZ_v(decal + som) * volumes_controle_v(som);
      });
      end_gpu_timer(Objet_U::computeOnDevice, "[KOKKOS]EOS_Tools_VEF::secmembre_divU_Z som loop");
      decal+=nb_som_tot;
    }
  if (p_has_arrete)
    {
      for (int ar = 0; ar < nb_ar_tot; ar++)
        {
          //pour l'instant on n'a pas calcule tab_dZ aux aretes
          assert(tab_dZ(decal + ar) == 0);
          tab_W(decal + ar) = 0;
        }
    }
  tab_W.echange_espace_virtuel();
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tab_W=",tab_W);
}
