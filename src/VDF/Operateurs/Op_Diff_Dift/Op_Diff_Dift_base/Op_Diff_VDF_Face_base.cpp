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

#include <Op_Diff_VDF_Face_base.h>
#include <Check_espace_virtuel.h>
#include <Statistiques.h>

extern Stat_Counter_Id diffusion_counter_;

Implemente_base(Op_Diff_VDF_Face_base,"Op_Diff_VDF_Face_base",Op_Diff_VDF_base);

Sortie& Op_Diff_VDF_Face_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Diff_VDF_Face_base::readOn(Entree& s ) { return s ; }


void Op_Diff_VDF_Face_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL, mat2;
  Op_VDF_Face::dimensionner(iter.zone(), iter.zone_Cl(), mat2);
  mat->nb_colonnes() ? *mat += mat2 : *mat = mat2;

}

void Op_Diff_VDF_Face_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(diffusion_counter_);
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse* mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL;
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : equation().inconnue().valeur().valeurs();

  assert_invalide_items_non_calcules(secmem, 0.);
  if(mat) iter.ajouter_contribution(inco, *mat);
  iter.ajouter(inco,secmem);

  if (equation().domaine_application() == Motcle("Hydraulique"))
    // On est dans le cas des equations de Navier_Stokes
    {
      // Ajout du terme supplementaire en V/(R*R) dans le cas des coordonnees axisymetriques
      if(Objet_U::bidim_axi == 1)
        {
          const Zone_VDF& zvdf=iter.zone();
          const DoubleTab& xv=zvdf.xv();
          const IntVect& ori=zvdf.orientation();
          const IntTab& face_voisins=zvdf.face_voisins();
          const DoubleVect& volumes_entrelaces=zvdf.volumes_entrelaces();
          int face, nb_faces=zvdf.nb_faces();//, cst;
          DoubleTrav diffu_tot(zvdf.nb_elem_tot());
          double db_diffusivite;
          Nom nom_eq=equation().que_suis_je();
          if ((nom_eq == "Navier_Stokes_standard")||(nom_eq == "Navier_Stokes_QC")||(nom_eq == "Navier_Stokes_FT_Disc"))
            {
              const Eval_Diff_VDF& eval=dynamic_cast<const Eval_Diff_VDF&> (iter.evaluateur());
              const Champ_base& ch_diff=eval.get_diffusivite();
              const DoubleTab& tab_diffusivite=ch_diff.valeurs();
              if (tab_diffusivite.size() == 1)
                diffu_tot = tab_diffusivite(0,0);
              else
                diffu_tot = tab_diffusivite;

              for(face=0; face<nb_faces; face++)
                if(ori(face)==0)
                  {
                    int elem1=face_voisins(face,0);
                    int elem2=face_voisins(face,1);
                    if(elem1==-1)
                      db_diffusivite=diffu_tot(elem2);
                    else if (elem2==-1)
                      db_diffusivite=diffu_tot(elem1);
                    else
                      db_diffusivite=0.5*(diffu_tot(elem2)+diffu_tot(elem1));
                    double r= xv(face,0);
                    if(r>=1.e-24)
                      {
                        if(mat) (*mat)(face,face)+=db_diffusivite*volumes_entrelaces(face)/(r*r);
                        secmem(face) -=inco(face)*db_diffusivite*volumes_entrelaces(face)/(r*r);

                      }
                  }
            }
          else if (equation().que_suis_je() == "Navier_Stokes_Interface_avec_trans_masse" ||
                   equation().que_suis_je() == "Navier_Stokes_Interface_sans_trans_masse" ||
                   equation().que_suis_je() == "Navier_Stokes_Front_Tracking" ||
                   equation().que_suis_je() == "Navier_Stokes_Front_Tracking_BMOL")
            {
              // Voir le terme source axi dans Interfaces/VDF
            }
          else
            {
              Cerr << "Probleme dans Op_Diff_VDF_base::contribuer_a_avec  avec le type de l'equation" << finl;
              Cerr << "on n'a pas prevu d'autre cas que Navier_Stokes_std" << finl;
              exit();
            }
        }
    }
  statistiques().end_count(diffusion_counter_);

}

double Op_Diff_VDF_Face_base::calculer_dt_stab() const { return Op_Diff_VDF_Face_base::calculer_dt_stab(iter.zone()); }

double Op_Diff_VDF_Face_base::calculer_dt_stab(const Zone_VDF& zone_VDF) const
{
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
  double coef=0;
  const Champ_base& ch_diffu = has_champ_masse_volumique() ? diffusivite() : diffusivite_pour_pas_de_temps();
  const DoubleTab& diffu = ch_diffu.valeurs();
  const double Cdiffu = sub_type(Champ_Uniforme, ch_diffu);

  // Si la diffusivite est variable, ce doit etre un champ aux elements.
  assert(Cdiffu || diffu.size() == zone_VDF.nb_elem());
  const int nb_elem = zone_VDF.nb_elem();
  for (int elem = 0; elem < nb_elem; elem++)
    {
      double diflo = 0.;
      for (int i = 0; i < dimension; i++)
        {
          const double h = zone_VDF.dim_elem(elem, i);
          diflo += 1. / (h * h);
        }
      const int k = Cdiffu ? 0 : elem;
      double alpha = diffu(k, 0);
      for (int ncomp = 1; ncomp < diffu.dimension(1); ncomp++)
        alpha = std::max(alpha, diffu(k, ncomp));

      diflo *= alpha;
      if (has_champ_masse_volumique())
        {
          const DoubleTab& rho = get_champ_masse_volumique().valeurs();
          diflo/= rho(elem);
        }
      coef = std::max(coef,diflo);
    }

  double dt_stab = (coef==0 ? DMAXFLOAT : 0.5/coef);
  return Process::mp_min(dt_stab);
}

