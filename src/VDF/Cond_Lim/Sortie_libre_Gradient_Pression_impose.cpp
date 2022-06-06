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

#include <Sortie_libre_Gradient_Pression_impose.h>
#include <Navier_Stokes_std.h>
#include <Champ_Uniforme.h>
#include <Milieu_base.h>
#include <Champ_P0_VDF.h>
#include <Zone_VDF.h>
#include <Discretisation_base.h>

Implemente_instanciable(Sortie_libre_Gradient_Pression_impose,"Frontiere_ouverte_Gradient_Pression_impose",Neumann_sortie_libre);

//// printOn
//

Sortie& Sortie_libre_Gradient_Pression_impose::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

//// readOn
//

Entree& Sortie_libre_Gradient_Pression_impose::readOn(Entree& s )
{
  s >> le_champ_front;
  le_champ_ext.typer("Champ_front_uniforme");
  le_champ_ext.valeurs().resize(1,dimension);
  return s;
}

////////////////////////////////////////////////////////////////
//
//           Implementation de fonctions
//
//     de la classe Sortie_libre_Gradient_Pression_impose
//
////////////////////////////////////////////////////////////////

void Sortie_libre_Gradient_Pression_impose::completer()
{
  Cerr << "Sortie_libre_Gradient_Pression_impose::completer()" << finl;
  const Zone_Cl_dis_base& la_zone_Cl = zone_Cl_dis();
  const Equation_base& eqn = la_zone_Cl.equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,eqn);
  const Zone_VDF& zone_vdf = ref_cast(Zone_VDF,eqn.zone_dis().valeur());
  const Champ_P0_VDF& pression = ref_cast(Champ_P0_VDF,eqn_hydr.pression().valeur());
  const IntTab& face_voisins  = zone_vdf.face_voisins();
  const DoubleVect& volumes_entrelaces = zone_vdf.volumes_entrelaces();
  const DoubleVect& face_surfaces = zone_vdf.face_surfaces();

  la_zone_VDF = zone_vdf;
  pression_interne = pression;

  const Front_VF& le_bord = ref_cast(Front_VF,frontiere_dis());
  int nb_faces_loc = le_bord.nb_faces();
  int ndeb = le_bord.num_premiere_face();
  int face;

  coeff.resize(nb_faces_loc);
  trace_pression_int.resize(nb_faces_loc);

  for (face=ndeb; face<ndeb+nb_faces_loc; face++)
    if (face_voisins(face,0) != -1)
      coeff[face-ndeb] = volumes_entrelaces[face]/face_surfaces[face];
    else
      coeff[face-ndeb] = - volumes_entrelaces[face]/face_surfaces[face];

  Cerr << "Sortie_libre_Gradient_Pression_impose::completer() ok" << finl;
}

void Sortie_libre_Gradient_Pression_impose::mettre_a_jour(double temps)
{
  //Cerr<<"Sortie_libre_Gradient_Pression_impose::mettre_a_jour"<<finl;
  Cond_lim_base::mettre_a_jour(temps);

  const Front_VF& le_bord = ref_cast(Front_VF,frontiere_dis());
  int ndeb = le_bord.num_premiere_face();
  int nb_faces_loc = le_bord.nb_faces();

  assert(pression_interne.non_nul());
  for (int face=ndeb; face<ndeb+nb_faces_loc; face++)
    trace_pression_int[face-ndeb] = pression_interne->valeur_au_bord(face);
}

double Sortie_libre_Gradient_Pression_impose::flux_impose(int face) const
{
  if (le_champ_front.valeurs().size()==1)
    return (trace_pression_int[face] + coeff[face]*le_champ_front(0,0));
  else if (le_champ_front.valeurs().line_size()==1)
    return (trace_pression_int[face] + coeff[face]*le_champ_front(face,0));
  else
    Cerr << "Sortie_libre_Gradient_Pression_impose::flux_impose erreur" << finl;
  exit();
  return 0.;
}

double Sortie_libre_Gradient_Pression_impose::flux_impose(int  , int ) const
{
  Cerr << "Sortie_libre_Gradient_Pression_impose::flux_impose(int  , int )" << finl;
  Cerr << "On ne sait imposer que la composante normale du gradient" << finl;
  return 0.;
}

double Sortie_libre_Gradient_Pression_impose::grad_P_imp(int face) const
{
  const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
  if (sub_type(Champ_Uniforme,mil.masse_volumique().valeur()))
    {
      const Champ_Uniforme& rho=ref_cast(Champ_Uniforme,mil.masse_volumique().valeur());
      double d_rho = rho(0,0);
      if (le_champ_front.valeurs().size()==1)
        return le_champ_front(0,0)/d_rho;
      else if (le_champ_front.valeurs().line_size()==1)
        return le_champ_front(face,0)/d_rho;
      else
        Cerr << "Sortie_libre_Gradient_Pression_impose::grad_P_imp() erreur" << finl;
    }
  else
    {
      //quasi compressible
      const DoubleTab& tab_rho = mil.masse_volumique().valeurs();
      int elem = la_zone_VDF->face_voisins(face,0);
      if (elem==-1)
        elem = la_zone_VDF->face_voisins(face,1);
      double d_rho = tab_rho(elem);
      if (le_champ_front.valeurs().size()==1)
        return le_champ_front(0,0)/d_rho;
      else if (le_champ_front.valeurs().line_size()==1)
        return le_champ_front(face,0)/d_rho;
      else
        Cerr << "Sortie_libre_Gradient_Pression_impose::grad_P_imp() erreur" << finl;
    }
  exit();
  return 0.;
}

int Sortie_libre_Gradient_Pression_impose::
compatible_avec_discr(const Discretisation_base& discr) const
{
  if (discr.que_suis_je() == "VDF")
    return 1;
  else if (discr.que_suis_je() == "VDF_Interface" || discr.que_suis_je() == "VDF_Front_Tracking")
    return 1;
  else
    {
      err_pas_compatible(discr);
      return 0;
    }
}

int Sortie_libre_Gradient_Pression_impose::
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


