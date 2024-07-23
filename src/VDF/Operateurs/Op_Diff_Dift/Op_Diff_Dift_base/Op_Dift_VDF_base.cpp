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

#include <Eval_Dift_Multiphase_VDF.h>
#include <Modele_turbulence_hyd_base.h>
#include <Op_Dift_VDF_base.h>
#include <Eval_Dift_VDF.h>
#include <Statistiques.h>
#include <Champ_Fonc.h>
#include <TRUSTTrav.h>
#include <Motcle.h>

extern Stat_Counter_Id diffusion_counter_;

Implemente_base(Op_Dift_VDF_base,"Op_Dift_VDF_base",Op_Diff_VDF_base);

Sortie& Op_Dift_VDF_base::printOn(Sortie& is) const { return Op_Diff_VDF_base::printOn(is); }
Entree& Op_Dift_VDF_base::readOn(Entree& is) { return Op_Diff_VDF_base::readOn(is); }

void Op_Dift_VDF_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(diffusion_counter_);
  iter_->ajouter_blocs(matrices,secmem,semi_impl);

  // On ajoute des termes si axi ...
  Op_Dift_VDF_base::ajoute_terme_pour_axi_turb(matrices, secmem, semi_impl);

  statistiques().end_count(diffusion_counter_);
}

// Ajout du terme supplementaire en V/(R*R) dans le cas des coordonnees axisymetriques
void Op_Dift_VDF_base::ajoute_terme_pour_axi_turb(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (equation().domaine_application() == Motcle("Hydraulique")) // On est dans le cas des equations de Navier_Stokes
    {
      const std::string& nom_inco = equation().inconnue().le_nom().getString();
      Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;
      const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : equation().inconnue().valeurs();

      if (Objet_U::bidim_axi == 1)
        {
          const Domaine_VDF& zvdf = iter_->domaine();
          const DoubleTab& xv = zvdf.xv();
          const IntVect& ori = zvdf.orientation();
          const IntTab& face_voisins = zvdf.face_voisins();
          const DoubleVect& volumes_entrelaces = zvdf.volumes_entrelaces();
          int face, nb_faces = zvdf.nb_faces(); //, cst;
          double db_diffusivite;

          const Eval_Diff_VDF& eval = dynamic_cast<const Eval_Diff_VDF&>(iter_->evaluateur());
          const Champ_base& ch = eval.get_diffusivite();
          const DoubleTab& tab_diffusivite = ch.valeurs();
          const int N = tab_diffusivite.dimension(1);
          DoubleTrav diffu_tot(zvdf.nb_elem_tot(), N);

          int size = diffu_tot.dimension_tot(0);
          int cM = (tab_diffusivite.dimension(0) == 1);

          const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
          if (is_turb()) // Cas turbulence multiphase
            {
              const Eval_Dift_Multiphase_VDF& eval_dift = dynamic_cast<const Eval_Dift_Multiphase_VDF&>(iter_->evaluateur()) ;
              const DoubleTab& diffusivite_turb = eval_dift.tab_nu_t() ;
              const DoubleTab& alpharho = equation().probleme().equation(1).champ_conserve().passe();
              assert(diffusivite_turb.nb_dim()==2);

              for (int i = 0; i < size; i++)
                for (int n=0; n<N; n++)
                  diffu_tot(i, n) = tab_diffusivite(!cM*i, n) + alpharho(i, n)*diffusivite_turb(i,n);
            }
          else if (sub_type(Modele_turbulence_hyd_base, modele_turbulence.valeur()))
            {
              const Eval_Dift_VDF& eval_dift = static_cast<const Eval_Dift_VDF&>(eval);
              const Champ_Fonc& ch_diff_turb = eval_dift.diffusivite_turbulente();
              const DoubleVect& diffusivite_turb = ch_diff_turb.valeurs();

              for (int i = 0; i < size; i++) diffu_tot[i] = tab_diffusivite[!cM*i] + diffusivite_turb[i];
            }
          else
            {
              Cerr << "Method Op_Dift_VDF_base::ajoute_terme_pour_axi_turb" << finl;
              Cerr << "The type " << equation().que_suis_je() << " of the equation associated " << finl;
              Cerr << "with the current operator " << que_suis_je() << "is not coherent." << finl;
              Cerr << "It must be sub typing of Navier_Stokes_Turbulent" << finl;
              Process::exit();
            }

          for (face = 0; face < nb_faces; face++)
            for (int n = 0; n < N; n++)
              if (ori(face) == 0)
                {
                  const int elem1 = face_voisins(face, 0), elem2 = face_voisins(face, 1);

                  if (elem1 == -1) db_diffusivite = diffu_tot(elem2, n);
                  else if (elem2 == -1) db_diffusivite = diffu_tot(elem1, n);
                  else db_diffusivite = 0.5 * (diffu_tot(elem2, n) + diffu_tot(elem1, n));

                  double r = xv(face, 0);
                  if (r >= 1.e-24)
                    {
                      if (mat) (*mat)(N * face + n, N * face + n) += db_diffusivite * volumes_entrelaces(face) / (r * r);
                      secmem(face, n) -= inco(face, n) * db_diffusivite * volumes_entrelaces(face) / (r * r);
                    }
                }
        }
    }
}
