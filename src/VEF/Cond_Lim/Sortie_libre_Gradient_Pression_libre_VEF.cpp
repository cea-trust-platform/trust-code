/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Sortie_libre_Gradient_Pression_libre_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Cond_Lim
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////



#include <Sortie_libre_Gradient_Pression_libre_VEF.h>
#include <Navier_Stokes_std.h>
#include <Champ_Uniforme.h>
#include <Milieu_base.h>
#include <Champ_P0_VEF.h>
#include <Discretisation_base.h>
#include <Zone_VEF.h>
#include <distances_VEF.h>

Implemente_instanciable(Sortie_libre_Gradient_Pression_libre_VEF,"Frontiere_ouverte_Gradient_Pression_libre_VEF",Neumann_sortie_libre);


//// printOn
//

Sortie& Sortie_libre_Gradient_Pression_libre_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

//// readOn
//
//
Entree& Sortie_libre_Gradient_Pression_libre_VEF::readOn(Entree& s )
{
  le_champ_front.typer("Champ_front_uniforme");
  le_champ_front.valeurs().resize(1,dimension);
  le_champ_front->fixer_nb_comp(1);
  le_champ_ext.typer("Champ_front_uniforme");
  le_champ_ext.valeurs().resize(1,dimension);
  return s;
}
//
////////////////////////////////////////////////////////////////
//
//           Implementation de fonctions
//
//     de la classe Sortie_libre_Gradient_Pression_libre_VEF
//
////////////////////////////////////////////////////////////////

void Sortie_libre_Gradient_Pression_libre_VEF::completer()
{

  Cerr << "Sortie_libre_Gradient_Pression_libre_VEF::completer()" << finl;
  const Zone_Cl_dis_base& la_zone_Cl = zone_Cl_dis();
  const Equation_base& eqn = la_zone_Cl.equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,eqn);

  //  const Zone_VEF& la_zone_VEF = ref_cast(Zone_VEF,eqn.zone_dis().valeur());

  const Champ_P0_VEF& pression = ref_cast(Champ_P0_VEF,eqn_hydr.pression().valeur());

  pression_interne = pression;

  const Front_VF& le_bord = ref_cast(Front_VF,frontiere_dis());
  int nb_faces_loc = le_bord.nb_faces();

  trace_pression_int.resize(nb_faces_loc);

  Cerr << "Sortie_libre_Gradient_Pression_libre_VEF::completer() ok" << finl;
}

int Sortie_libre_Gradient_Pression_libre_VEF::initialiser(double temps)
{

  int ok=Cond_lim_base::initialiser(temps);

  const Zone_Cl_dis_base& la_zone_Cl = zone_Cl_dis();
  const Equation_base& eqn = la_zone_Cl.equation();
  //      const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,eqn);
  const Zone_VEF& ma_zone_VEF = ref_cast(Zone_VEF,eqn.zone_dis().valeur());
  const IntTab& face_voisins  = ma_zone_VEF.face_voisins();
  //      const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,ma_zone_cl_dis.valeur());
  //      const DoubleVect& volumes_entrelaces_Cl = zone_Cl_VEF.volumes_entrelaces_Cl();
  //      const DoubleTab& face_normale = la_zone_VEF.face_normales();
  const Front_VF& le_bord = ref_cast(Front_VF,frontiere_dis());
  int nb_faces_loc = le_bord.nb_faces();
  int ndeb = le_bord.num_premiere_face();
  int nfin = ndeb + nb_faces_loc;
  int face;
  coeff.resize(nb_faces_loc);
  if (dimension == 2)
    for (face=ndeb; face<nfin ; face++)
      {
        int elem=face_voisins(face,0);
        if (elem != -1)
          coeff[face-ndeb] = distance_2D(face,elem,ma_zone_VEF);
        else
          {
            elem=face_voisins(face,1);
            coeff[face-ndeb] = -distance_2D(face,elem,ma_zone_VEF);
          }
      }
  if (dimension == 3)
    for (face=ndeb; face<nfin ; face++)
      {
        int elem=face_voisins(face,0);
        if (elem != -1)
          coeff[face-ndeb] = distance_3D(face,elem,ma_zone_VEF);
        else
          {
            elem=face_voisins(face,1);
            coeff[face-ndeb] = -distance_3D(face,elem,ma_zone_VEF);
          }
      }
  return ok;
}


void Sortie_libre_Gradient_Pression_libre_VEF::mettre_a_jour(double temps)
{
  Cond_lim_base::mettre_a_jour(temps);

  const Front_VF& le_bord = ref_cast(Front_VF,frontiere_dis());
  int ndeb = le_bord.num_premiere_face();
  int nb_faces_loc = le_bord.nb_faces();
  int nfin = ndeb + nb_faces_loc;

  assert(pression_interne.non_nul());
  for (int face=ndeb; face<nfin; face++)
    {
      trace_pression_int[face-ndeb] = pression_interne->valeur_au_bord(face);
    }

}

double Sortie_libre_Gradient_Pression_libre_VEF::flux_impose(int face) const
{
  const Zone_Cl_dis_base& la_zone_Cl = zone_Cl_dis();
  const Equation_base& eqn = la_zone_Cl.equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,eqn);
  const Zone_VEF& ma_zone_VEF = ref_cast(Zone_VEF,eqn.zone_dis().valeur());
  const IntTab& face_voisins  = ma_zone_VEF.face_voisins();
  //  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,ma_zone_cl_dis.valeur());
  //  const Front_VF& le_bord = ref_cast(Front_VF,frontiere_dis());
  const IntTab& elem_faces    = ma_zone_VEF.elem_faces();
  const DoubleTab& face_normales = ma_zone_VEF.face_normales();
  const Champ_P0_VEF& pre = ref_cast(Champ_P0_VEF,eqn_hydr.pression().valeur());

  double Pimp, diff, grad ;

  int elem1, elem2, face_adj, face_face_adj ;

  double a1 = trace_pression_int[face];
  double a2 = coeff[face];

  double a3 = 0. ;

  elem1 = face_voisins(face,0);

  for (face_adj=0; face_adj<dimension; face_adj++)
    {
      face_face_adj= elem_faces(elem1,face_adj);

      elem2 = face_voisins(face_face_adj,0);
      if (elem2 != elem1)
        {
          diff = pre(elem2) - pre(elem1);

          for (int comp=0; comp < dimension; comp++)
            {
              grad = diff*face_normales(face_face_adj,comp);
              a3 += grad ;
            }
        }
    }

  a3 /= double(dimension) ;
  Pimp  = a1 + a2 * a3 ;
  return Pimp ;

}

double Sortie_libre_Gradient_Pression_libre_VEF::flux_impose(int  , int ) const
{
  Cerr << "Sortie_libre_Gradient_Pression_libre_VEF::flux_impose(int  , int )" << finl;
  Cerr << "On ne sait imposer que la composante normale du gradient" << finl;
  return 0.;
}

double Sortie_libre_Gradient_Pression_libre_VEF::Grad_P_lib_VEF(int face) const
{
  const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
  const Champ_Uniforme& rho=ref_cast(Champ_Uniforme,mil.masse_volumique().valeur());
  double d_rho = rho(0,0);
  if (le_champ_front.valeurs().size()==1)
    return le_champ_front(0,0)/d_rho;
  else if (le_champ_front.valeurs().dimension(1)==1)
    return le_champ_front(face,0)/d_rho;
  else
    Cerr << "Sortie_libre_Gradient_Pression_libre_VEF::Grad_P_lib_VEF() erreur" << finl;
  exit();
  return 0.;
}

int Sortie_libre_Gradient_Pression_libre_VEF::
compatible_avec_discr(const Discretisation_base& discr) const
{
  if (discr.que_suis_je() == "VEF")
    return 1;
  else
    {
      err_pas_compatible(discr);
      return 0;
    }
}

int Sortie_libre_Gradient_Pression_libre_VEF::
compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Hydraulique="Hydraulique";
  Motcle indetermine="indetermine";
  if ( (dom_app==Hydraulique) || (dom_app==indetermine) )
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}


