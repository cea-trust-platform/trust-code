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

#include <Op_Dift_Multiphase_VDF_Elem.h>
#include <Op_Dift_Multiphase_VDF_Face.h>
#include <Viscosite_turbulente_base.h>
#include <Transport_turbulent_base.h>
#include <VDF_discretisation.h>
#include <Pb_Multiphase.h>

Implemente_instanciable_sans_constructeur(Op_Dift_Multiphase_VDF_Elem,"Op_Diff_VDFTURBULENTE_P0_VDF",Op_Dift_VDF_Elem_base);

Sortie& Op_Dift_Multiphase_VDF_Elem::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Dift_Multiphase_VDF_Elem::readOn(Entree& is)
{
  //lecture de la correlation de transport turbulente
  corr_.typer_lire(equation().probleme(), "transport_turbulent", is);

  associer_corr_impl<Type_Operateur::Op_DIFT_MULTIPHASE_ELEM, Eval_Dift_Multiphase_VDF_Elem>(corr_);

  const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, equation().probleme());
  noms_d_t_post_.dimensionner(pb.nb_phases()), d_t_post_.resize(pb.nb_phases());
  for (int i = 0; i < pb.nb_phases(); i++)
    champs_compris_.ajoute_nom_compris(noms_d_t_post_[i] = Nom("diffusivite_turbulente_") + pb.nom_phase(i));

  return is;
}

Op_Dift_Multiphase_VDF_Elem::Op_Dift_Multiphase_VDF_Elem() : Op_Dift_VDF_Elem_base(Iterateur_VDF_Elem<Eval_Dift_Multiphase_VDF_Elem>()) { }

void Op_Dift_Multiphase_VDF_Elem::creer_champ(const Motcle& motlu)
{
  Op_Dift_VDF_Elem_base::creer_champ(motlu);
  int i = noms_d_t_post_.rang(motlu);
  if (i >= 0 && d_t_post_[i].est_nul())
    {
      const VDF_discretisation dis = ref_cast(VDF_discretisation, equation().discretisation());
      Noms noms(1), unites(1);
      noms[0] = noms_d_t_post_[i];
      Motcle typeChamp = "champ_elem" ;
      dis.discretiser_champ(typeChamp, equation().domaine_dis(), scalaire, noms , unites, 1, 0, d_t_post_[0]);
      champs_compris_.ajoute_champ(d_t_post_[i]);
    }
}

void Op_Dift_Multiphase_VDF_Elem::completer()
{
  assert(corr_.non_nul());
  completer_Op_Dift_VDF_base();
  associer_pb<Eval_Dift_Multiphase_VDF_Elem>(equation().probleme());

  //si la correlation a besoin du gradient de u, on doit le creer maintenant
  if (ref_cast(Transport_turbulent_base, corr_.valeur()).gradu_required())
    equation().probleme().creer_champ("gradient_vitesse");

  // on initialise d_t_ a 0 avec la bonne structure //
  d_t_ = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue()->valeurs();
  d_t_ = 0.;
  set_nut_impl<Type_Operateur::Op_DIFT_MULTIPHASE_ELEM, Eval_Dift_Multiphase_VDF_Elem>(d_t_);
}

void Op_Dift_Multiphase_VDF_Elem::mettre_a_jour(double temps)
{
  Op_Dift_VDF_Elem_base::mettre_a_jour(temps);

  const Operateur_base& op_qdm = equation().probleme().equation(0).operateur(0).l_op_base();
  if (!sub_type(Op_Dift_Multiphase_VDF_Face, op_qdm))
    Process::exit(que_suis_je() + ": no turbulent momentum diffusion found!");
  const Correlation& corr_visc = ref_cast(Op_Dift_Multiphase_VDF_Face, op_qdm).correlation();
  if (!sub_type(Viscosite_turbulente_base, corr_visc.valeur()))
    Process::exit(que_suis_je() + ": no turbulent viscosity correlation found!");

  // on calcule d_t_
  d_t_ = 0.; // XXX : pour n'avoir pas la partie laminaire
  //un "simple" appel a la correlation!
  ref_cast(Transport_turbulent_base, corr_.valeur()).modifier_mu(ref_cast(Convection_Diffusion_std, equation()), ref_cast(Viscosite_turbulente_base, corr_visc.valeur()), d_t_);
  set_nut_impl<Type_Operateur::Op_DIFT_MULTIPHASE_ELEM, Eval_Dift_Multiphase_VDF_Elem>(d_t_);

  int N = ref_cast(Pb_Multiphase, equation().probleme()).nb_phases();
  for (int n = 0; n < N; n++)
    if (d_t_post_[n].non_nul()) //di turbulente : toujours scalaire
      {
        DoubleTab& val = d_t_post_[n]->valeurs();
        int nl = val.dimension(0);
        for (int i = 0; i < nl; i++) val(i, 0) =  d_t_(i, n);
        d_t_post_[n].mettre_a_jour(temps);
      }
}

double Op_Dift_Multiphase_VDF_Elem::calculer_dt_stab() const
{
  const Domaine_VDF& domaine_VDF = iter->domaine();
  double dt_stab, coef = -1.e10;
  const IntTab& elem_faces = domaine_VDF.elem_faces();
  const DoubleTab& lambda = alpha_(), &diffu = diffusivite_pour_pas_de_temps().valeurs();
  const int cL = (lambda.dimension(0) == 1), cD = (diffu.dimension(0) == 1), nb_elem = domaine_VDF.nb_elem(), dim = Objet_U::dimension;

  ArrOfInt numfa(2 * dim);
  for (int elem = 0; elem < nb_elem; elem++)
    {
      double moy = 0.;
      for (int i = 0; i < 2 * dim; i++)
        numfa[i] = elem_faces(elem, i);

      for (int d = 0; d < dim; d++)
        {
          const double hd = domaine_VDF.dist_face(numfa[d], numfa[dim + d], d);
          moy += 1. / (hd * hd);
        }

      double diff_physique = lambda(!cL * elem, 0), diff_turbulent = d_t_(elem, 0);

      for (int ncomp = 1; ncomp < lambda.line_size(); ncomp++)
        diff_physique = std::max(diff_physique, lambda(!cL * elem, ncomp));
      for (int ncomp = 1; ncomp < d_t_.line_size(); ncomp++)
        diff_turbulent = std::max(diff_turbulent, d_t_(elem, ncomp));

      const double alpha_local = (diff_physique + diff_turbulent) / (lambda(!cL * elem, 0) / diffu(!cD * elem, 0)) * moy;
      coef = std::max(coef, alpha_local);
    }

  coef = Process::mp_max(coef);
  dt_stab = 0.5 / (coef + DMINFLOAT);

  return dt_stab;
}

