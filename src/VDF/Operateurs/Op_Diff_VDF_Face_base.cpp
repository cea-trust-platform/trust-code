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
// File:        Op_Diff_VDF_Face_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_VDF_Face_base.h>
#include <Fluide_Incompressible.h>
#include <Champ_Inc.h>
#include <Eval_Diff_VDF.h>
#include <Eval_VDF_Face.h>
#include <SFichier.h>

Implemente_base(Op_Diff_VDF_Face_base,"Op_Diff_VDF_Face_base",Op_Diff_VDF_base);



//// printOn
//
Sortie& Op_Diff_VDF_Face_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//
Entree& Op_Diff_VDF_Face_base::readOn(Entree& s )
{
  return s ;
}
double Op_Diff_VDF_Face_base::calculer_dt_stab() const
{
  return Op_Diff_VDF_Face_base::calculer_dt_stab(iter.zone());
}
double Op_Diff_VDF_Face_base::calculer_dt_stab(const Zone_VDF& zone_VDF) const
{
  double dt_stab;
  double coef;
  //  const Zone_VDF & zone_VDF = iter.zone();
  //  const IntTab & elem_faces = zone_VDF.elem_faces();

  if (has_champ_masse_volumique())
    {

      const Champ_base& champ_diffu = diffusivite();
      const DoubleVect& diffu = champ_diffu.valeurs();
      coef=0;
      const int diffu_variable = (diffu.size() == 1) ? 0 : 1;
      const double diffu_constante = (diffu_variable ? 0. : diffu(0));
      const DoubleTab& valeurs_rho = get_champ_masse_volumique().valeurs();
      const int dim = Objet_U::dimension;

      // Si la diffusivite est variable, ce doit etre un champ aux elements.
      assert((!diffu_variable) || diffu.size()==zone_VDF.nb_elem());


      int elem;
      const int nb_elem = zone_VDF.nb_elem();
      for (elem = 0; elem < nb_elem; elem++)
        {
          int i;
          double diflo = 0.;
          for (i = 0; i < dim; i++)
            {
              const double h = zone_VDF.dim_elem(elem, i);
              diflo += 1. / (h * h);
            }
          double alpha = diffu_variable ? diffu(elem) : diffu_constante;
          double rho = valeurs_rho(elem);
          diflo *= alpha / rho;
          coef = max(coef,diflo);
        }
      if (coef==0)
        dt_stab = DMAXFLOAT;
      else
        dt_stab = 0.5/coef;


    }
  else
    {


      const Champ_base& champ_diffu = diffusivite_pour_pas_de_temps();
      const DoubleVect& diffu = champ_diffu.valeurs();

      // Calcul du pas de temps de stabilite :
      //
      //
      //  - La diffusivite n'est pas uniforme donc:
      //
      //     dt_stab = Min (1/(2*diffusivite(elem)*coeff(elem))
      //
      //     avec :
      //            coeff =  1/(dx*dx) + 1/(dy*dy) + 1/(dz*dz)
      //
      //            i decrivant l'ensemble des elements du maillage
      //
      //  Rq: en hydraulique on cherche le Max sur les elements du maillage
      //      initial (comme en thermique) et non le Max sur les volumes de Qdm.

      coef=0;
      double diflo;
      // double h_x,h_y,h_z;
      const int diffu_variable = (diffu.size() == 1) ? 0 : 1;
      const double diffu_constante = (diffu_variable ? 0. : diffu(0));

      // Si la diffusivite est variable, ce doit etre un champ aux elements.
      assert((!diffu_variable) || diffu.size()==zone_VDF.nb_elem());


      for (int elem=0; elem<zone_VDF.nb_elem(); elem++)
        {
          double inv_h=0;
          for (int dir=0; dir<dimension; dir++)
            {
              double h=zone_VDF.dim_elem(elem,dir);
              inv_h+=1/(h*h);
            }
          double alpha = diffu_variable ? diffu(elem) : diffu_constante;
          //diflo = alpha * (1/(h_x*h_x) + 1/(h_y*h_y));
          diflo=alpha*inv_h;
          coef = max(coef,diflo);
        }


      if (coef==0)
        dt_stab = DMAXFLOAT;
      else
        dt_stab = 0.5/coef;
    }
  return Process::mp_min(dt_stab);
}



// Description:
// complete l'iterateur et l'evaluateur
void Op_Diff_VDF_Face_base::associer(const Zone_dis& zone_dis,
                                     const Zone_Cl_dis& zone_cl_dis,
                                     const Champ_Inc& ch_transporte)
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  const Champ_Face& inco = ref_cast(Champ_Face,ch_transporte.valeur());

  iter.associer(zvdf, zclvdf, *this);

  Evaluateur_VDF& eval_diff =  iter.evaluateur();
  eval_diff.associer_zones(zvdf, zclvdf );          // Evaluateur_VDF::associer_zones
  //  (dynamic_cast<Eval_VDF_Face&>(eval_diff)).associer_inconnue(inco );        // Eval_VDF_Face::associer_inconnue
  get_eval_face().associer_inconnue(inco );
}

// Description:
// associe le champ de diffusivite a l'evaluateur
void Op_Diff_VDF_Face_base::associer_diffusivite(const Champ_base& ch_diff)
{
  Eval_Diff_VDF& eval_diff = (Eval_Diff_VDF&) iter.evaluateur();
  eval_diff.associer(ch_diff);          // Eval_Diff_VDF::associer
}

const Champ_base& Op_Diff_VDF_Face_base::diffusivite() const
{
  const Eval_Diff_VDF& eval_diff = (const Eval_Diff_VDF&) iter.evaluateur();
  return eval_diff.get_diffusivite();
}

void Op_Diff_VDF_Face_base::mettre_a_jour(double temps)
{
  Eval_Diff_VDF& eval_diff = ( Eval_Diff_VDF&) iter.evaluateur();
  eval_diff.mettre_a_jour();
}
