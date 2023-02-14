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

#include <Op_Dift_Multiphase_VDF_Face.h>
#include <Viscosite_turbulente_base.h>
#include <VDF_discretisation.h>
#include <Pb_Multiphase.h>

Implemente_instanciable_sans_constructeur(Op_Dift_Multiphase_VDF_Face,"Op_Diff_VDFTURBULENTE_Face",Op_Dift_VDF_Face_base);

Sortie& Op_Dift_Multiphase_VDF_Face::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Dift_Multiphase_VDF_Face::readOn(Entree& is)
{
  //lecture de la correlation de viscosite turbulente
  corr_.typer_lire(equation().probleme(), "viscosite_turbulente", is);

  associer_corr_impl<Type_Operateur::Op_DIFT_MULTIPHASE_FACE, Eval_Dift_Multiphase_VDF_Face>(corr_);

  const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, equation().probleme());
  noms_nu_t_post_.dimensionner(pb.nb_phases()), nu_t_post_.resize(pb.nb_phases());
  for (int i = 0; i < pb.nb_phases(); i++)
    champs_compris_.ajoute_nom_compris(noms_nu_t_post_[i] = Nom("viscosite_turbulente_") + pb.nom_phase(i));

  return is;
}

Op_Dift_Multiphase_VDF_Face::Op_Dift_Multiphase_VDF_Face() : Op_Dift_VDF_Face_base(Iterateur_VDF_Face<Eval_Dift_Multiphase_VDF_Face>())
{
  // declare_support_masse_volumique(1); // poubelle
}

void Op_Dift_Multiphase_VDF_Face::creer_champ(const Motcle& motlu)
{
  Op_Dift_VDF_Face_base::creer_champ(motlu);
  int i = noms_nu_t_post_.rang(motlu);
  if (i >= 0 && nu_t_post_[i].est_nul())
    {
      const VDF_discretisation dis = ref_cast(VDF_discretisation, equation().discretisation());
      Noms noms(1), unites(1);
      noms[0] = noms_nu_t_post_[i];
      Motcle typeChamp = "champ_elem" ;
      dis.discretiser_champ(typeChamp, equation().domaine_dis(), scalaire, noms , unites, 1, 0, nu_t_post_[0]);
      champs_compris_.ajoute_champ(nu_t_post_[i]);
    }
}

void Op_Dift_Multiphase_VDF_Face::completer()
{
  completer_Op_Dift_VDF_base();
  associer_pb<Eval_Dift_Multiphase_VDF_Face>(equation().probleme());

  //si la correlation a besoin du gradient de u, on doit le creer maintenant
  if (corr_.non_nul() && ref_cast(Viscosite_turbulente_base, corr_.valeur()).gradu_required())
    equation().probleme().creer_champ("gradient_vitesse");

  if (nu_t_post_[0].non_nul())
    {
      const int N = ref_cast(Pb_Multiphase, equation().probleme()).nb_phases();
      const int nl = nu_t_post_[0]->valeurs().dimension_tot(0);
      nu_t_.resize(nl,N);
    }
}

void Op_Dift_Multiphase_VDF_Face::mettre_a_jour(double temps)
{
  int N = ref_cast(Pb_Multiphase, equation().probleme()).nb_phases();
  for (int n = 0; n < N; n++)
    if (nu_t_post_[n].non_nul()) //viscosite turbulente : toujours scalaire
      {
        const DoubleTab& rho = equation().milieu().masse_volumique().passe() ;
        DoubleTab& val = nu_t_post_[n]->valeurs();
        int nl = val.dimension(0);
        int cR = (rho.dimension(0) == 1);
        ref_cast(Viscosite_turbulente_base, corr_.valeur()).eddy_viscosity(nu_t_); //remplissage par la correlation
        for (int i = 0; i < nl; i++)  val(i, 0) = rho(!cR * i, n) * nu_t_(i, n);
        nu_t_post_[n].mettre_a_jour(temps);
      }

  set_nut_impl<Type_Operateur::Op_DIFT_MULTIPHASE_FACE, Eval_Dift_Multiphase_VDF_Face>(nu_t_);

  // XXX : la a la fin !!!!
  mettre_a_jour_impl<Type_Operateur::Op_DIFF_FACE,Eval_Dift_Multiphase_VDF_Face>(); // a voir si utile ...
}


