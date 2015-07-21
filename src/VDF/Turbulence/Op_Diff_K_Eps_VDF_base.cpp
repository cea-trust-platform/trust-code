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
// File:        Op_Diff_K_Eps_VDF_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_K_Eps_VDF_base.h>
#include <Modele_turbulence_hyd_K_Eps.h>
#include <Champ_P0_VDF.h>
#include <Fluide_Quasi_Compressible.h>
#include <Navier_Stokes_Turbulent.h>

Implemente_base(Op_Diff_K_Eps_VDF_base,"Op_Diff_K_Eps_VDF_base",Op_Diff_K_Eps_base);

Implemente_instanciable_sans_constructeur(Op_Diff_K_Eps_VDF_Elem,"Op_Diff_K_Eps_VDF_P0_VDF",Op_Diff_K_Eps_VDF_base);

implemente_It_VDF_Elem(Eval_Diff_K_Eps_VDF_Elem)

////  printOn
//

Sortie& Op_Diff_K_Eps_VDF_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Sortie& Op_Diff_K_Eps_VDF_Elem::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Diff_K_Eps_VDF_base::readOn(Entree& s )
{
  return s ;
}

Entree& Op_Diff_K_Eps_VDF_Elem::readOn(Entree& s )
{
  return s ;
}


void Op_Diff_K_Eps_VDF_base::completer()
{
  Operateur_base::completer();
  iter.completer_();
  if(sub_type(Transport_K_Eps,mon_equation.valeur()))
    {
      const Transport_K_Eps& eqn_transport = ref_cast(Transport_K_Eps,mon_equation.valeur());
      if(sub_type( Modele_turbulence_hyd_K_Eps,eqn_transport.modele_turbulence()))
        {
          const Modele_turbulence_hyd_K_Eps& mod_turb = ref_cast(Modele_turbulence_hyd_K_Eps,eqn_transport.modele_turbulence());
          Eval_Diff_K_Eps_VDF_Elem& eval_diff = (Eval_Diff_K_Eps_VDF_Elem&) iter.evaluateur();
          eval_diff.associer_Pr_K_Eps(mod_turb.get_Prandtl_K(),mod_turb.get_Prandtl_Eps());
        }
    }
}
void Op_Diff_K_Eps_VDF_base::associer_diffusivite(const Champ_base& ch_diff)
{
  Eval_Diff_K_Eps_VDF& eval_diff_turb = (Eval_Diff_K_Eps_VDF&) iter.evaluateur();
  eval_diff_turb.associer(ch_diff);
}

const Champ_base& Op_Diff_K_Eps_VDF_base::diffusivite() const
{
  Eval_Diff_K_Eps_VDF& eval_diff_turb = (Eval_Diff_K_Eps_VDF&) iter.evaluateur();
  return eval_diff_turb.diffusivite();
}
// Description:
// complete l'iterateur et l'evaluateur
void Op_Diff_K_Eps_VDF_Elem::associer(const Zone_dis& zone_dis,
                                      const Zone_Cl_dis& zone_cl_dis,
                                      const Champ_Inc& ch_diffuse)
{
  const Champ_P0_VDF& inco = ref_cast(Champ_P0_VDF,ch_diffuse.valeur());
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  iter.associer(zvdf, zclvdf, *this);

  Eval_Diff_K_Eps_VDF_Elem& eval_diff = (Eval_Diff_K_Eps_VDF_Elem&) iter.evaluateur();
  eval_diff.associer_zones(zvdf, zclvdf );          // Evaluateur_VDF::associer_zones
  eval_diff.associer_inconnue(inco );        // Eval_VDF_::associer_inconnue
}


// Description:
// associe le champ de diffusivite_turbulente a l'evaluateur
void Op_Diff_K_Eps_VDF_base::associer_diffusivite_turbulente()
{
  assert(mon_equation.non_nul());
  if(sub_type(Transport_K_Eps_base,mon_equation.valeur()))
    {
      const Transport_K_Eps_base& eqn_transport = ref_cast(Transport_K_Eps_base,mon_equation.valeur());
      //   if(sub_type( Modele_turbulence_hyd_K_Eps,eqn_transport.modele_turbulence()))
      {

        const Mod_turb_hyd_RANS& mod_turb = ref_cast(Mod_turb_hyd_RANS,eqn_transport.modele_turbulence());
        const Champ_Fonc& diff_turb = mod_turb.viscosite_turbulente();
        Eval_Diff_K_Eps_VDF& eval_diff = (Eval_Diff_K_Eps_VDF&) iter.evaluateur();
        eval_diff.associer_diff_turb(diff_turb);
      }
    }
  else
    {
      Cerr<<" erreur dans Op_Diff_K_Eps_VDF_base::associer_diffusivite ... cas non prevu "<<finl;
      exit();
    }
  // association de la masse_volumique (pour le QC)
  {
    Eval_Diff_K_Eps_VDF& eval_diff = (Eval_Diff_K_Eps_VDF&) iter.evaluateur();

    const Fluide_Incompressible& mil = ref_cast(Fluide_Incompressible,mon_equation->milieu());
    const Champ_Don& mvol = mil.masse_volumique();
    //
    eval_diff.associer_mvolumique(mvol);
  }

}

const Champ_Fonc& Op_Diff_K_Eps_VDF_base::diffusivite_turbulente() const
{
  const Eval_Diff_K_Eps_VDF& eval_diff =
    (Eval_Diff_K_Eps_VDF&) iter.evaluateur();
  return eval_diff.diffusivite_turbulente();
}

void Op_Diff_K_Eps_VDF_base::mettre_a_jour_diffusivite() const
{
  assert(mon_equation.non_nul());
}

void Op_Diff_K_Eps_VDF_base::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_VDF_Elem::modifier_pour_Cl(iter.zone(), iter.zone_Cl(), matrice, secmem);
  // si la ldp est non nul on modifie la matrice en consequence

  const Navier_Stokes_Turbulent& eqn_hydr = ref_cast(Navier_Stokes_Turbulent,equation().probleme().equation(0) ) ;
  const Mod_turb_hyd& mod_turb = eqn_hydr.modele_turbulence();
  const Turbulence_paroi& loipar = mod_turb.loi_paroi();



  Nom type_loi = loipar.valeur().que_suis_je();

  if ( !(type_loi.debute_par("negligeable")) )
    {

      const IntVect& tab1=matrice.tab1_;
      DoubleVect& coeff = matrice.coeff_;

      const IntTab& face_voisins = iter.zone().face_voisins();

      const Transport_K_Eps& eqn_k_eps=ref_cast(Transport_K_Eps,equation());
      const DoubleTab& val=eqn_k_eps.inconnue().valeurs();

      const Zone_Cl_dis_base& zone_Cl_dis_base = ref_cast(Zone_Cl_dis_base,eqn_hydr.zone_Cl_dis().valeur());

      const Conds_lim& les_cl = zone_Cl_dis_base.les_conditions_limites();
      int nb_cl=les_cl.size();
      for (int num_cl=0; num_cl<nb_cl; num_cl++)
        {
          //Boucle sur les bords
          const Cond_lim& la_cl = les_cl[num_cl];
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int nbfaces = la_front_dis.nb_faces();
          int ndeb = la_front_dis.num_premiere_face();
          int nfin = ndeb + nbfaces;

          if ((sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))||(sub_type(Dirichlet_paroi_defilante,la_cl.valeur())))
            for (int num_face=ndeb; num_face<nfin; num_face++)

              {

                int elem= face_voisins(num_face,0);
                if (elem==-1) elem= face_voisins(num_face,1);
                int nb_comp=2;
                for (int comp=0; comp<nb_comp; comp++)
                  {
                    // on doit remettre la ligne a l'identite et le secmem a l'inconnue
                    int idiag = tab1[elem*nb_comp+comp]-1;
                    coeff[idiag]=1;
                    // pour les voisins

                    int nbvois = tab1[elem*nb_comp+1+comp] - tab1[elem*nb_comp+comp];
                    for (int k=1; k < nbvois; k++)
                      {
                        coeff[idiag+k]=0;
                      }

                    secmem(elem,comp)=val(elem,comp);

                  }
              }

        }
    }
}

//
// Fonctions inline de la classe Op_Diff_K_Eps_VDF_Elem
//
Op_Diff_K_Eps_VDF_Elem::Op_Diff_K_Eps_VDF_Elem() :
  Op_Diff_K_Eps_VDF_base(It_VDF_Elem(Eval_Diff_K_Eps_VDF_Elem)())
{
}
