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

#include <Discretisation_tools.h>
#include <Check_espace_virtuel.h>
#include <Champ_base.h>
#include <Domaine_VF.h>
#include <TRUSTTrav.h>
#include <Device.h>
#include <Debog.h>

void Discretisation_tools::nodes_to_cells(const Champ_base& Hn, Champ_base& He)
{
  ToDo_Kokkos("critical for EF but not used in TRUST...");
  const DoubleTab& tabHn=Hn.valeurs();
  DoubleTab& tabHe=He.valeurs();
  const Domaine_dis_base& domaine_dis_base=He.domaine_dis_base();

  assert(tabHe.dimension_tot(0)==domaine_dis_base.nb_elem_tot());
  assert(tabHn.dimension_tot(0)==domaine_dis_base.nb_som_tot());

  const IntTab& les_elems=domaine_dis_base.domaine().les_elems();
  int nb_som_elem=les_elems.dimension(1);
  int nb_elem=domaine_dis_base.nb_elem();

  tabHe=0;
  for (int ele=0; ele<nb_elem; ele++)
    {
      for (int s=0; s<nb_som_elem; s++)
        {
          int sglob=les_elems(ele,s);
          tabHe(ele)+=tabHn(sglob);
        }
    }
  double inv_nb_som_elem=1./(nb_som_elem);
  tabHe*=inv_nb_som_elem;
  tabHe.echange_espace_virtuel();
}
void Discretisation_tools::cells_to_nodes(const Champ_base& He, Champ_base& Hn)
{
  ToDo_Kokkos("critical for EF");
  DoubleTab& tabHn=Hn.valeurs();
  const DoubleTab& tabHe=He.valeurs();
  Debog::verifier("elno entreee",tabHe);
  assert_espace_virtuel_vect(tabHe);
  const Domaine_dis_base& domaine_dis_base=He.domaine_dis_base();

  assert(tabHe.dimension_tot(0)==domaine_dis_base.nb_elem_tot());
  assert(tabHn.dimension_tot(0)==domaine_dis_base.nb_som_tot());

  const IntTab& les_elems=domaine_dis_base.domaine().les_elems();
  const DoubleVect& volumes=ref_cast(Domaine_VF,domaine_dis_base).volumes();

  tabHn=0;
  DoubleTrav vsom(tabHn.dimension_tot(0));
  int nb_som_elem=les_elems.dimension(1);
  int nb_elem_tot=domaine_dis_base.nb_elem_tot();
  int N = tabHn.line_size();

  for (int e = 0; e < nb_elem_tot; e++)
    for (int s = 0, sglob; s < nb_som_elem && (sglob = les_elems(e, s)) >= 0; s++)
      {
        for (int n = 0; n < N; n++)
          tabHn(sglob, n) += tabHe(e, n) * volumes(e);
        vsom(sglob) += volumes(e);
      }

  for (int s = 0; s < domaine_dis_base.nb_som(); s++)
    for (int n = 0; n < N; n++)
      tabHn(s, n) /= vsom(s);

  tabHn.echange_espace_virtuel();
  Debog::verifier("elno sortie",tabHn);
}
void Discretisation_tools::faces_to_cells(const Champ_base& Hf, Champ_base& He)
{
  ToDo_Kokkos("critical but seems not used by TRUST...");
  const DoubleTab& tabHf=Hf.valeurs();
  DoubleTab& tabHe=He.valeurs();
  const Domaine_dis_base& domaine_dis_base=He.domaine_dis_base();
  const Domaine_VF& domaine_vf= ref_cast(Domaine_VF,domaine_dis_base);
  assert(tabHe.dimension_tot(0)==domaine_dis_base.nb_elem_tot());
  assert(tabHf.dimension_tot(0)==domaine_vf.nb_faces_tot());

  const IntTab& elem_faces=domaine_vf.elem_faces();
  int nb_face_elem=elem_faces.dimension(1);
  int nb_elem=domaine_dis_base.nb_elem();

  tabHe=0;
  for (int ele=0; ele<nb_elem; ele++)
    {
      for (int s=0; s<nb_face_elem; s++)
        {

          tabHe(ele)+=tabHf(elem_faces(ele,s));
        }
    }
  double inv_nb_face_elem=1./(nb_face_elem);
  tabHe*=inv_nb_face_elem;
  tabHe.echange_espace_virtuel();
}

// Exemple of a dynamic kernel (can run on host or device)
template<typename ExecSpace>
void cells_to_faces_kernel(const Domaine_VF& domaine_vf, const DoubleTab& tabHe, DoubleTab& tab_vol_tot, DoubleTab& tabHf)
{
  static constexpr bool kernelOnDevice = !std::is_same<ExecSpace, Kokkos::DefaultHostExecutionSpace>::value;
  int nb_elem_tot  = domaine_vf.nb_elem_tot();
  int nb_face_elem = domaine_vf.elem_faces().dimension(1);
  auto elem_faces  = domaine_vf.elem_faces().template view_ro<ExecSpace>();
  auto volumes     = domaine_vf.volumes().template view_ro<ExecSpace>();
  auto He_v        = static_cast<const ArrOfDouble&>(tabHe).template view_ro<ExecSpace>();
  auto vol_tot     = static_cast<ArrOfDouble&>(tab_vol_tot).template view_rw<ExecSpace>();
  auto Hf_v        = static_cast<ArrOfDouble&>(tabHf).template view_rw<ExecSpace>();
  Kokkos::MDRangePolicy<ExecSpace, Kokkos::Rank<2>> policy({0, 0}, {nb_elem_tot, nb_face_elem});
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(const int ele, const int s)
  {
    int face = elem_faces(ele, s);
    double volume_elem = volumes(ele);
    Kokkos::atomic_add(&Hf_v(face), He_v(ele) * volume_elem);
    Kokkos::atomic_add(&vol_tot(face), volume_elem);
  });
  end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
}

void Discretisation_tools::cells_to_faces(const Champ_base& He, Champ_base& Hf)
{
  DoubleTab& tabHf=Hf.valeurs();
  const DoubleTab& tabHe=He.valeurs();
  Debog::verifier("element_face entree",tabHe);
  assert_espace_virtuel_vect(tabHe);
  const Domaine_dis_base& domaine_dis_base=He.domaine_dis_base();
  const Domaine_VF& domaine_vf= ref_cast(Domaine_VF,domaine_dis_base);
  // en realite on fait P1B vers face
  //assert(tabHe.dimension_tot(0)==domaine_dis_base.nb_elem_tot());
  assert(tabHf.dimension_tot(0)==domaine_vf.nb_faces_tot());
  const DoubleVect& volumes_entrelaces=domaine_vf.volumes_entrelaces();

  tabHf=0;

  // TODO : FIXME
  // XXX : codage pas coherent... a voir : volumes ou volumes_entrelaces ???
  if (tabHe.line_size() == 1)
    {
      DoubleTrav tab_vol_tot(tabHf);
      // Lancement de ce kernel multi-discretisation et copie de donnees selon conditions (les deux tableaux doivent etre deja sur le device)
      bool kernelOnDevice = tabHf.checkDataOnDevice(tabHe);
      if (kernelOnDevice)
        cells_to_faces_kernel<Kokkos::DefaultExecutionSpace>(domaine_vf, tabHe, tab_vol_tot, tabHf);
      else
        cells_to_faces_kernel<Kokkos::DefaultHostExecutionSpace>(domaine_vf, tabHe, tab_vol_tot, tabHf);

      // Hf /= vol_tot
      tab_divide_any_shape(tabHf, tab_vol_tot, VECT_REAL_ITEMS);
    }
  else
    {
      int nb_face_elem=domaine_vf.elem_faces().dimension(1);
      int nb_elem_tot=domaine_dis_base.nb_elem_tot();
      const IntTab& elem_faces = domaine_vf.elem_faces();
      const DoubleVect& volumes = domaine_vf.volumes();
      ToDo_Kokkos("critical");
      // TODO : factorize these cases ...
      if (tabHf.nb_dim()==1)
        {
          // VDF
          double coeffb=1;
          double coeffi=2;
          assert(domaine_vf.que_suis_je()=="Domaine_VDF");
          for (int ele=0; ele<nb_elem_tot; ele++)
            for (int s=0; s<nb_face_elem; s++)
              {
                int face=elem_faces(ele,s);
                for (int r = 0; r < domaine_vf.dimension; r++)
                  {
                    // Change of basis N.K.N, with N the normal of the face, and K the tensorial coefficient to get the value of the diffusivity
                    // on the direction of the surface normal.
                    double normOnSurf = domaine_vf.face_normales(face, r) / domaine_vf.face_surfaces(face);
                    tabHf(face) += tabHe(ele, r) * normOnSurf * normOnSurf * volumes(ele);
                  }
              }
          for (int f=0; f<domaine_vf.premiere_face_int(); f++)
            tabHf(f)/=volumes_entrelaces(f)*coeffb;
          for (int f=domaine_vf.premiere_face_int(); f<domaine_vf.nb_faces(); f++)
            tabHf(f)/=volumes_entrelaces(f)*coeffi;
        }
      else
        {
          double coeffb=nb_face_elem;
          double coeffi=coeffb;
          int nb_comp=tabHf.dimension(1);
          for (int ele=0; ele<nb_elem_tot; ele++)
            for (int s=0; s<nb_face_elem; s++)
              {
                int face=elem_faces(ele,s);
                for (int comp=0; comp<nb_comp; comp++)
                  tabHf(face,comp)+=tabHe(ele,comp)*volumes(ele);
              }

          for (int f=0; f<domaine_vf.premiere_face_int(); f++)
            for (int comp=0; comp<nb_comp; comp++)
              tabHf(f,comp)/=volumes_entrelaces(f)*coeffb;
          for (int f=domaine_vf.premiere_face_int(); f<domaine_vf.nb_faces(); f++)
            for (int comp=0; comp<nb_comp; comp++)
              tabHf(f,comp)/=volumes_entrelaces(f)*coeffi;
        }
    }
  tabHf.echange_espace_virtuel();
  Debog::verifier("element_face sortie",tabHf);
}

void Discretisation_tools::cells_to_faces(const Domaine_VF& domaine_vf, const DoubleTab& tab_elem, DoubleTab& tab_face)
{
  const int nb_face_elem = domaine_vf.elem_faces().line_size(), nb_comp = tab_face.line_size();

  assert(tab_elem.dimension_tot(0) == domaine_vf.nb_elem_tot() && tab_face.dimension_tot(0) == domaine_vf.nb_faces_tot());
  assert(tab_elem.line_size() == nb_comp);
  if (domaine_vf.que_suis_je() != "Domaine_VEF")
    Process::exit("Discretisation_tools::cells_to_faces limited to VEF."); // TODO FIXME

  tab_face = 0.;
  int nb_elem_tot = domaine_vf.nb_elem_tot();
  auto elem_faces = domaine_vf.elem_faces().view_ro();
  auto volumes    = domaine_vf.volumes().view_ro();
  auto tab_elem_v = tab_elem.view_ro();
  auto tab_face_v = tab_face.view_rw();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), range_2D({0, 0}, {nb_elem_tot, nb_face_elem}), KOKKOS_LAMBDA(const int ele, const int s)
  {
    int face = elem_faces(ele, s);
    double volume_elem = volumes(ele);
    for (int comp = 0; comp < nb_comp; comp++)
      Kokkos::atomic_add(&tab_face_v(face, comp), tab_elem_v(ele, comp) * volume_elem);
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  CDoubleArrView volumes_entrelaces = domaine_vf.volumes_entrelaces().view_ro();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), range_2D({0, 0}, {domaine_vf.nb_faces(), nb_comp}), KOKKOS_LAMBDA(const int f, const int comp)
  {
    tab_face_v(f, comp) /= volumes_entrelaces(f) * nb_face_elem;
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  // tab_face /= volumes_entrelaces * nb_face_elem :
  // PL: comprends pas, ecarts sur les cas thermal_coupling_on_coincident_domain_jddX avec:
  // tab_face*=nb_face_elem;
  // tab_divide_any_shape(tab_face, volumes_entrelaces, VECT_REAL_ITEMS);
  tab_face.echange_espace_virtuel();
}

void Discretisation_tools::faces_to_cells(const Domaine_VF& domaine_vf, const DoubleTab& tab_face, DoubleTab& tab_elem)
{
  ToDo_Kokkos("critical but seems not used by TRUST");
  const IntTab& elem_faces = domaine_vf.elem_faces();
  const int nb_face_elem = elem_faces.dimension(1), nb_elem = domaine_vf.nb_elem(), nb_comp = tab_face.line_size();;
  assert(tab_elem.dimension_tot(0) == domaine_vf.nb_elem_tot() && tab_face.dimension_tot(0) == domaine_vf.nb_faces_tot());
  assert(tab_elem.line_size() == nb_comp);

  tab_elem = 0;

  for (int ele = 0; ele < nb_elem; ele++)
    for (int comp = 0; comp < nb_comp; comp++)
      for (int s = 0; s < nb_face_elem; s++)
        tab_elem(ele, comp) += tab_face(elem_faces(ele, s), comp);

  double inv_nb_face_elem = 1. / nb_face_elem;
  tab_elem *= inv_nb_face_elem;

  tab_elem.echange_espace_virtuel();
}
