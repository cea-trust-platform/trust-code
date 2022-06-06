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

#include <Terme_Source_Th_TdivU_VEF_Face.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Convection_Diffusion_std.h>
#include <Op_Conv_Muscl_VEF_Face.h>
#include <EChaine.h>
#include <Milieu_base.h>
#include <Periodique.h>
#include <Neumann_homogene.h>
#include <Symetrie.h>
#include <Neumann.h>
#include <Neumann_val_ext.h>
#include <Dirichlet_homogene.h>
#include <Echange_impose_base.h>

Implemente_instanciable(Terme_Source_Th_TdivU_VEF_Face,"Source_Th_TdivU_VEF_P1NC",Source_base);



//// printOn
//

Sortie& Terme_Source_Th_TdivU_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Terme_Source_Th_TdivU_VEF_Face::readOn(Entree& s )
{
  return s ;
}

void Terme_Source_Th_TdivU_VEF_Face::associer_pb(const Probleme_base& pb)
{
  int nb_eqn = pb.nombre_d_equations();
  int ok_vit=0;
  int ok_temp=0;
  for (int i=0; i<nb_eqn; i++)
    {
      const Equation_base& eqn = pb.equation(i);
      if  (sub_type(Navier_Stokes_std,eqn))
        {
          ok_vit = 1;
        }
      if  (sub_type(Convection_Diffusion_std,eqn))
        {
          ok_temp = 1;
        }
    }

  if (!ok_vit)
    {
      Cerr << "Error TRUST in " << que_suis_je()  << finl;
      Cerr << "Hydraulic equation not found" << finl;
      exit();
    }
  if (!ok_temp)
    {
      Cerr << "Error TRUST in " << que_suis_je()  << finl;
      Cerr << "Thermal equation not found" << finl;
      exit();
    }
}
void Terme_Source_Th_TdivU_VEF_Face::associer_eqn_t()
{
  eqn_t = equation();
}

void Terme_Source_Th_TdivU_VEF_Face::associer_zones(const Zone_dis& zone_dis,
                                                    const Zone_Cl_dis& zone_Cl_dis)
{
}

void Terme_Source_Th_TdivU_VEF_Face::completer()
{
  Source_base::completer();
  associer_eqn_t();

  zone_cl_mod_=0;
}
void Terme_Source_Th_TdivU_VEF_Face::modifier_zone_cl()
{
  if (zone_cl_mod_) return;
  zone_cl_mod_=1;
  Zone_Cl_dis& ma_zonecl=ref_cast(Zone_Cl_dis,ma_zonecl_);
  ma_zonecl=eqn_t.valeur().zone_Cl_dis();
  zonecl_sa=eqn_t.valeur().zone_Cl_dis().valeur();
  Conds_lim& condlims=ma_zonecl.les_conditions_limites();
  Conds_lim& condlims_sa=zonecl_sa.valeur().les_conditions_limites();
  int nb=condlims.size();

  for (int i=0; i<nb*0; i++)
    {
      condlims[i]= condlims_sa[i];
      //Cerr<<(int)( &condlims[i].valeur())<<" addr "<<(int) &condlims_sa[i].valeur()<<finl;
    }
  for (int i=0; i<nb; i++)
    {
      Cond_lim_base& la_cl=condlims_sa[i].valeur();
      int modif=0;
      Nom new_cl;
      if (sub_type(Periodique,la_cl)||sub_type(Symetrie,la_cl))
        {
          ;
        }
      else if (sub_type(Neumann_homogene,la_cl)||sub_type(Neumann,la_cl)||sub_type(Neumann_val_ext,la_cl)||sub_type(Echange_impose_base,la_cl))
        {
          modif=1;
          new_cl="Frontiere_ouverte T_ext Champ_front_uniforme 1 1";
        }
      else if  (sub_type(Dirichlet_homogene,la_cl)||sub_type(Dirichlet,la_cl))
        {
          modif=1;
          new_cl="Frontiere_ouverte_temperature_imposee Champ_front_uniforme 1 1";
        }
      else
        {
          Cerr<<"surcharge de la cl "<<la_cl.que_suis_je()<<"pas codee dans "<<__FILE__<<finl;
          exit();
        }
      if (modif)
        {
          EChaine cons("Frontiere_ouverte T_ext Champ_front_uniforme 1 1");
          //EChaine cons("symetrie");
          Frontiere_dis_base& frdis=condlims[i].valeur().frontiere_dis();
          cons>>condlims[i];
          Cond_lim_base& cl_m=condlims[i].valeur();

          cl_m.associer_fr_dis_base(frdis);
          cl_m.completer();

        }

    }
}

DoubleTab& Terme_Source_Th_TdivU_VEF_Face::ajouter(DoubleTab& resu) const
{
  Terme_Source_Th_TdivU_VEF_Face& me_non_const=ref_cast_non_const(Terme_Source_Th_TdivU_VEF_Face,*this);
  me_non_const.modifier_zone_cl();
  if(resu.line_size() > 1)
    {
      Cerr << "Error in " << que_suis_je()  << finl;
      Cerr << "The source term Source_Th_TdivU_VEF_P1NC is only dedicated to thermal equation" << finl;
      exit();
    }
  const DoubleTab& temperature=equation().inconnue().valeurs();
  const int nb_faces = temperature.dimension_tot(0);

  // Calcul de TdivU d'apres le schema de convection
  DoubleTab temp(temperature);
  DoubleTab TdivU(resu);
  TdivU=0.;
  temp=1.;
  const Operateur& Op_conv=equation().operateur(1);
  Operateur_base& optype=ref_cast_non_const(Operateur_base,Op_conv.l_op_base());
  // We save the boundary fluxes:
  DoubleTab flux_bords_backup = optype.flux_bords();
  optype.associer_zone_cl_dis(ma_zonecl_);
  Op_conv.ajouter(temp,TdivU);
  for(int face=0; face<nb_faces; face++)
    {
      TdivU[face]*=temperature(face);
    }
  if (mp_max_vect(TdivU)==0)
    {
      Cerr << finl;
      Cerr << "******* Warning *******" << finl;
      Cerr << "You are using source term TdivU into one transport equation." << finl;
      Cerr << "It seems useless cause TdivU=0" << finl;
      Cerr << "May be the advection operator is calculated with the non-conservative formulation." << finl;
    }

  // la partie en TdivU est un bout de l'operateur de convection -> il doit etre multiplie par rhoCp
  const double rhoCp = equation().milieu().capacite_calorifique().valeurs()(0, 0) * equation().milieu().masse_volumique().valeurs()(0, 0);
  TdivU *= rhoCp;
  resu-=TdivU;
  // on remet la bonne zone_cl_dis
  optype.associer_zone_cl_dis(zonecl_sa.valeur());
  // We reset flux_bords
  optype.flux_bords() = flux_bords_backup;
  return resu;
}

DoubleTab& Terme_Source_Th_TdivU_VEF_Face::calculer(DoubleTab& resu) const
{
  resu = 0.;
  return ajouter(resu);
}

void Terme_Source_Th_TdivU_VEF_Face::mettre_a_jour(double temps)
{
  ;
}

