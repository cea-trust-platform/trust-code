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
// File:        Op_Dift_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_VDF_Elem.h>
#include <Modele_turbulence_scal_base.h>
#include <Mod_turb_hyd_base.h>
#include <Champ_P0_VDF.h>

Implemente_instanciable_sans_constructeur(Op_Dift_VDF_Elem,"Op_Dift_VDF_P0_VDF",Op_Dift_VDF_base);

implemente_It_VDF_Elem(Eval_Dift_VDF_const_Elem)

//// printOn
//

Sortie& Op_Dift_VDF_Elem::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Dift_VDF_Elem::readOn(Entree& s )
{
  return s ;
}



///////////////////////////////////////////////////////
//
//  Fonctions de la classe Op_Dift_VDF_Elem
//
///////////////////////////////////////////////////////


// Description:
// complete l'iterateur et l'evaluateur
void Op_Dift_VDF_Elem::associer(const Zone_dis& zone_dis,
                                const Zone_Cl_dis& zone_cl_dis,
                                const Champ_Inc& ch_diffuse)
{
  const Champ_P0_VDF& inco = ref_cast(Champ_P0_VDF,ch_diffuse.valeur());
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  iter.associer(zvdf, zclvdf, *this);
  Eval_Dift_VDF_const_Elem& eval_diff_turb = (Eval_Dift_VDF_const_Elem&) iter.evaluateur();
  eval_diff_turb.associer_zones(zvdf, zclvdf );
  eval_diff_turb.associer_inconnue(inco );
}


// Description:
// associe le champ de diffusivite a l'evaluateur
void Op_Dift_VDF_Elem::associer_diffusivite(const Champ_base& ch_diff)
{
  Eval_Dift_VDF_const_Elem& eval_diff_turb = (Eval_Dift_VDF_const_Elem&) iter.evaluateur();
  eval_diff_turb.associer(ch_diff);
}

const Champ_base& Op_Dift_VDF_Elem::diffusivite() const
{
  const Eval_Dift_VDF_const_Elem& eval_diff_turb =
    (Eval_Dift_VDF_const_Elem&) iter.evaluateur();
  return eval_diff_turb.diffusivite();
}

void Op_Dift_VDF_Elem::associer_diffusivite_turbulente(const Champ_Fonc& diff_turb)
{
  la_diffusivite_turbulente = diff_turb;
  Evaluateur_VDF& eval = iter.evaluateur();
  Eval_Dift_VDF_const_Elem& eval_diff_turb = (Eval_Dift_VDF_const_Elem&) eval;
  eval_diff_turb.associer_diff_turb(diff_turb);
}


void Op_Dift_VDF_Elem::associer_loipar(const Turbulence_paroi_scal& loi_paroi)
{
  //loipar = loi_paroi;
  Evaluateur_VDF& eval = iter.evaluateur();
  Eval_Dift_VDF_const_Elem& eval_diff_turb = (Eval_Dift_VDF_const_Elem&) eval;
  eval_diff_turb.associer_loipar(loi_paroi);
}

void Op_Dift_VDF_Elem::completer()
{
  Cerr << "Op_Dift_VDF_Elem::completer() "<<equation().que_suis_je() << finl;
  Op_Dift_VDF_base::completer();

  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  if (sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()))
    {
      const Modele_turbulence_scal_base& mod_turb = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
      const Champ_Fonc& alpha_t = mod_turb.diffusivite_turbulente();
      associer_diffusivite_turbulente(alpha_t);


      const Turbulence_paroi_scal& loipar = mod_turb.loi_paroi();
      if (loipar.non_nul())
        associer_loipar(loipar);
      //
      Evaluateur_VDF& eval = iter.evaluateur();
      Eval_Dift_VDF_const_Elem& eval_diff_turb = (Eval_Dift_VDF_const_Elem&) eval;
      eval_diff_turb.associer_modele_turbulence(mod_turb);
    }
  else
    {
      // bizarre mais V2
      const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
      const Champ_Fonc& alpha_t = mod_turb.viscosite_turbulente();
      associer_diffusivite_turbulente(alpha_t);

    }


}

double Op_Dift_VDF_Elem::calculer_dt_stab() const
{
  double dt_stab;
  double coef;
  const Zone_VDF& zone_VDF = iter.zone();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double alpha = (diffusivite().valeurs())(0,0);
  const DoubleVect& alpha_t = la_diffusivite_turbulente->valeurs();
  double tab_rho,Cp;


  // Calcul du pas de temps de stabilite :
  //
  //
  //  - La diffusivite est non constante donc:
  //
  //     dt_stab = Min (1/(2*(diff_lam(i)+diff_turb(i))*coeff(elem))
  //
  //     avec :
  //            coeff =  1/(dx*dx) + 1/(dy*dy) + 1/(dz*dz)
  //
  //            i decrivant l'ensemble des elements du maillage
  //

  coef= -1.e10;
  double alpha_local,h_x,h_y,h_z;

  //quasi compressible
  int is_QC = (diffusivite().le_nom()=="conductivite");


  //Cerr<<" alpha "<<alpha<<finl;
  //Cerr<<" alpha_t "<<alpha_t<<finl;
  //Cerr<<" equation "<<equation().que_suis_je()<<finl;
  //Cerr<<" is_QC "<<is_QC<<finl;
  Debog::verifier("alpha=",alpha);
  Debog::verifier("alpha_t=",alpha_t);

  if (dimension == 2)
    {
      int numfa[4];
      for (int elem=0; elem<zone_VDF.nb_elem(); elem++)
        {

          for (int i=0; i<4; i++)
            numfa[i] = elem_faces(elem,i);
          h_x = zone_VDF.dist_face(numfa[0],numfa[2],0);
          h_y = zone_VDF.dist_face(numfa[1],numfa[3],1);
          //Cerr<<" alpha_t "<< alpha_t <<finl;


          if (is_QC)
            {
              tab_rho = mon_equation->milieu().masse_volumique()(elem);
              const DoubleTab& tab_Cp = mon_equation->milieu().capacite_calorifique().valeurs();
              if (tab_Cp.nb_dim()==2)
                Cp = tab_Cp(0,0);
              else
                Cp = tab_Cp(elem);
              alpha_local = ((alpha+alpha_t(elem))/(tab_rho*Cp))
                            *(1/(h_x*h_x) + 1/(h_y*h_y));
            }
          else
            alpha_local = (alpha+alpha_t(elem))
                          *(1/(h_x*h_x) + 1/(h_y*h_y));


          //alpha_local = (alpha+alpha_t(elem))
          //*(1/(h_x*h_x) + 1/(h_y*h_y));
          //Correction pour Quasi_compressible
          //alpha_local = (alpha+alpha_t(elem)/(tab_rho*Cp))
          //*(1/(h_x*h_x) + 1/(h_y*h_y));


          coef = max(coef,alpha_local);
          //Cerr<<" alpha_t "<< alpha_t(elem) <<finl;
        }
    }

  else if (dimension == 3)
    {
      int numfa[6];
      for (int elem=0; elem<zone_VDF.nb_elem(); elem++)
        {

          for (int i=0; i<6; i++)
            numfa[i] = elem_faces(elem,i);
          h_x = zone_VDF.dist_face(numfa[0],numfa[3],0);
          h_y = zone_VDF.dist_face(numfa[1],numfa[4],1);
          h_z = zone_VDF.dist_face(numfa[2],numfa[5],2);

          if (is_QC)
            {
              tab_rho = mon_equation->milieu().masse_volumique()(elem);
              const DoubleTab& tab_Cp = mon_equation->milieu().capacite_calorifique().valeurs();
              if (tab_Cp.nb_dim()==2)
                Cp = tab_Cp(0,0);
              else
                Cp = tab_Cp(elem);
              alpha_local = ((alpha+alpha_t(elem))/(tab_rho*Cp))
                            *(1/(h_x*h_x) + 1/(h_y*h_y) + 1/(h_z*h_z));
            }
          else
            alpha_local = (alpha+alpha_t(elem))
                          *(1/(h_x*h_x) + 1/(h_y*h_y) + 1/(h_z*h_z));

          //alpha_local = (alpha+alpha_t(elem))
          //*(1/(h_x*h_x) + 1/(h_y*h_y) + 1/(h_z*h_z));

          coef = max(coef,alpha_local);
        }
    }

  coef = Process::mp_max(coef);
  dt_stab = 1/(2*(coef+DMINFLOAT));
  Debog::verifier("Op_Dift_VDF_P0_VDF::calculer_dt_stab dt_stab Fin",dt_stab);
  return dt_stab;
}

//
// Fonctions inline de la classe Op_Dift_VDF_Elem
//
//// Op_Dift_VDF_Elem
//
Op_Dift_VDF_Elem::Op_Dift_VDF_Elem() :
  Op_Dift_VDF_base(It_VDF_Elem(Eval_Dift_VDF_const_Elem)())
{
}
