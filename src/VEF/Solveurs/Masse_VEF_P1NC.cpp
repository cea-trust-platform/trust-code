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

#include <Masse_VEF_P1NC.h>
#include <Domaine_Cl_VEF.h>
#include <Domaine_VEF.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Equation_base.h>
#include <Matrice_Morse.h>
#include <Milieu_base.h>
#include <Device.h>

Implemente_instanciable(Masse_VEF_P1NC,"Masse_VEF_P1NC",Solveur_Masse_base);


//     printOn()
/////

Sortie& Masse_VEF_P1NC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Masse_VEF_P1NC::readOn(Entree& s)
{
  return s ;
}



///////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Masse_VEF_P1NC
//
//////////////////////////////////////////////////////////////


DoubleTab& Masse_VEF_P1NC::appliquer_impl(DoubleTab& tab_sm) const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = ref_cast(Domaine_Cl_VEF,le_dom_Cl_VEF.valeur());
  const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();
  const DoubleVect& volumes_entrelaces = domaine_VEF.volumes_entrelaces();
  int nfa = domaine_VEF.nb_faces();
  int num_std = domaine_VEF.premiere_face_std();
  int num_int = domaine_VEF.premiere_face_int();
  int nbcomp = tab_sm.line_size();
  int dimension = Objet_U::dimension;

  if (nfa != tab_sm.dimension(0))
    {
      Cerr << "erreur dans Masse_VEF_P1NC : ";
      Cerr << "nombre de faces :  " << nfa
           << " taille du second membre : " << tab_sm.dimension(0) << finl;
      exit();
    }
  // On traite les faces standard qui ne portent pas de conditions aux limites
  bool kernelOnDevice = tab_sm.checkDataOnDevice();
  const double * porosite_face_addr = mapToDevice(equation().milieu().porosite_face(), "", kernelOnDevice);
  const double * volumes_entrelaces_addr = mapToDevice(volumes_entrelaces, "", kernelOnDevice);
  double * sm_addr = computeOnTheDevice(tab_sm, "", kernelOnDevice);
  start_gpu_timer();
  #pragma omp target teams distribute parallel for if (kernelOnDevice && computeOnDevice)
  for (int face=num_std; face<nfa; face++)
    for (int comp=0; comp<nbcomp; comp++)
      sm_addr[face*nbcomp+comp] /= (volumes_entrelaces_addr[face]*porosite_face_addr[face]);
  end_gpu_timer(kernelOnDevice, "Face loop (std) in Masse_VEF_P1NC::appliquer_impl");
  // On traite les faces non standard
  // les faces des bord sont des faces non standard susceptibles de porter des C.L
  // les faces internes non standard ne portent pas de C.L

  // On traite les conditions aux limites
  CIntTabView face_voisins = domaine_VEF.face_voisins().view_ro();
  CDoubleTabView normales = domaine_VEF.face_normales().view_ro();
  CDoubleArrView volumes_entrelaces_Cl = domaine_Cl_VEF.volumes_entrelaces_Cl().view_ro();
  CDoubleArrView porosite_face = equation().milieu().porosite_face().view_ro();
  DoubleTabView sm = tab_sm.view_rw();
  start_timer();
  for (int n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();
      if ( (sub_type(Dirichlet,la_cl.valeur()))
           ||
           (sub_type(Dirichlet_homogene,la_cl.valeur()))
         )
        // Pour les faces de Dirichlet on met sm a 0
        Kokkos::parallel_for(__KERNEL_NAME__,
                             Kokkos::RangePolicy<>(num1, num2), KOKKOS_LAMBDA(
                               const int face)
        {
          for (int comp = 0; comp < nbcomp; comp++)
            sm(face, comp) = 0;
        });
      else if ((sub_type(Symetrie,la_cl.valeur()))&&(domaine_Cl_VEF.equation().inconnue()->nature_du_champ()==vectoriel))
        {
          Kokkos::parallel_for(__KERNEL_NAME__,
                               Kokkos::RangePolicy<>(num1, num2), KOKKOS_LAMBDA(
                                 const int face)
          {
            double psc=0;
            double surf=0;
            for (int comp=0; comp<nbcomp; comp++)
              {
                psc+=sm(face,comp)*normales(face,comp);
                surf+=normales(face,comp)*normales(face,comp);
              }
            psc/=surf;
            for(int comp=0; comp<dimension; comp++)
              {
                sm(face,comp)-=psc*normales(face,comp);
                sm(face,comp) /= (volumes_entrelaces_Cl(face)*
                                  porosite_face(face));
              }
          });
        }
      else
        Kokkos::parallel_for(__KERNEL_NAME__,
                             Kokkos::RangePolicy<>(num1, num2), KOKKOS_LAMBDA(
                               const int face)
        {
          int elem = face_voisins(face,0);
          if (elem == -1) elem = face_voisins(face,1);
          for (int comp=0; comp<nbcomp; comp++)
            sm(face,comp) /= (volumes_entrelaces_Cl(face)*porosite_face(face));
        });
    }
  end_timer(Objet_U::computeOnDevice, "Masse_VEF_P1NC::appliquer_impl BC");

  kernelOnDevice = tab_sm.checkDataOnDevice();
  const double * volumes_entrelaces_Cl_addr = mapToDevice(domaine_Cl_VEF.volumes_entrelaces_Cl(), "", kernelOnDevice);
  start_gpu_timer();
  #pragma omp target teams distribute parallel for if (kernelOnDevice && computeOnDevice)
  for (int face=num_int; face<num_std; face++)
    for (int comp=0; comp<nbcomp; comp++)
      sm_addr[face*nbcomp+comp] /= (volumes_entrelaces_Cl_addr[face]*porosite_face_addr[face]);
  end_gpu_timer(kernelOnDevice, "Face loop (non-std) in Masse_VEF_P1NC::appliquer_impl");
  //tab_sm.echange_espace_virtuel();
  //Debog::verifier("Masse_VEF_P1NC::appliquer, tab_sm=",tab_sm);
  return tab_sm;
}


//
void Masse_VEF_P1NC::associer_domaine_dis_base(const Domaine_dis_base& le_dom_dis_base)
{
  le_dom_VEF = ref_cast(Domaine_VEF, le_dom_dis_base);
}

void Masse_VEF_P1NC::associer_domaine_cl_dis_base(const Domaine_Cl_dis_base& le_dom_Cl_dis_base)
{
  le_dom_Cl_VEF = ref_cast(Domaine_Cl_VEF, le_dom_Cl_dis_base);
}


Matrice_Base& Masse_VEF_P1NC::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  if (penalisation||(le_dom_Cl_VEF.valeur().equation().inconnue()->nature_du_champ()!=vectoriel))
    return Solveur_Masse_base::ajouter_masse(dt,matrice,penalisation);
  // Sinon on modifie temporairement la nature du champ pour que appliquer_impl ne projette pas sur n.
  Champ_Inc_base& inco=ref_cast_non_const( Champ_Inc_base,le_dom_Cl_VEF.valeur().equation().inconnue().valeur());
  inco.fixer_nature_du_champ(multi_scalaire);
  Solveur_Masse_base::ajouter_masse(dt,matrice,penalisation);
  inco.fixer_nature_du_champ(vectoriel);
  return matrice;


}


DoubleTab& Masse_VEF_P1NC::ajouter_masse(double dt, DoubleTab& x, const DoubleTab& y, int penalisation) const
{
  if (penalisation||(le_dom_Cl_VEF.valeur().equation().inconnue()->nature_du_champ()!=vectoriel))
    return Solveur_Masse_base::ajouter_masse(dt,x,y,penalisation);

  // Sinon on modifie temporairement la nature du champ pour que appliquer_impl ne projette pas sur n.
  Champ_Inc_base& inco=ref_cast_non_const( Champ_Inc_base,le_dom_Cl_VEF.valeur().equation().inconnue().valeur());
  inco.fixer_nature_du_champ(multi_scalaire);
  Solveur_Masse_base::ajouter_masse(dt,x,y,penalisation);
  inco.fixer_nature_du_champ(vectoriel);
  return x;
}

DoubleTab& Masse_VEF_P1NC::corriger_solution(DoubleTab& x, const DoubleTab& y, int incr) const
{
  //  assert(penalisation_==1);

  return Solveur_Masse_base::corriger_solution( x,  y, incr) ;
}
