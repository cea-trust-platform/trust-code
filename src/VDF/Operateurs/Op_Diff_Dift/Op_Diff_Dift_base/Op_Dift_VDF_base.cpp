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

#include <Eval_Dift_VDF_const.h>
#include <Mod_turb_hyd_base.h>
#include <Eval_Dift_VDF_var.h>
#include <Op_Dift_VDF_base.h>
#include <Champ_Fonc.h>
#include <TRUSTTrav.h>
#include <Motcle.h>
#include <Statistiques.h>

extern Stat_Counter_Id diffusion_counter_;

Implemente_base(Op_Dift_VDF_base,"Op_Dift_VDF_base",Op_Diff_VDF_base);

Sortie& Op_Dift_VDF_base::printOn(Sortie& is) const { return Op_Diff_VDF_base::printOn(is); }
Entree& Op_Dift_VDF_base::readOn(Entree& is) { return Op_Diff_VDF_base::readOn(is); }

void Op_Dift_VDF_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(diffusion_counter_);
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL;
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : equation().inconnue().valeurs();

  if(mat) iter->ajouter_contribution(inco, *mat);
  iter->ajouter(inco, secmem);

  Op_Dift_VDF_base::ajoute_terme_pour_axi_turb(inco,mat,secmem);

  statistiques().end_count(diffusion_counter_);
}

// Ajout du terme supplementaire en V/(R*R) dans le cas des coordonnees axisymetriques
void Op_Dift_VDF_base::ajoute_terme_pour_axi_turb(const DoubleTab& inco, Matrice_Morse* mat, DoubleTab& secmem) const
{
  if (equation().domaine_application() == Motcle("Hydraulique")) // On est dans le cas des equations de Navier_Stokes
    if (Objet_U::bidim_axi == 1)
      {
        const Zone_VDF& zvdf = iter->zone();
        const DoubleTab& xv = zvdf.xv();
        const IntVect& ori = zvdf.orientation();
        const IntTab& face_voisins = zvdf.face_voisins();
        const DoubleVect& volumes_entrelaces = zvdf.volumes_entrelaces();
        int face, nb_faces = zvdf.nb_faces(); //, cst;
        DoubleTrav diffu_tot(zvdf.nb_elem_tot());
        double db_diffusivite;

        const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
        if (sub_type(Mod_turb_hyd_base, modele_turbulence.valeur()))
          {
            const Eval_Diff_VDF& eval = dynamic_cast<const Eval_Diff_VDF&>(iter->evaluateur());
            const Champ_base& ch = eval.get_diffusivite();
            const DoubleVect& tab_diffusivite = ch.valeurs();
            int size = diffu_tot.size_totale();
            if (tab_diffusivite.size() == 1)
              {
                db_diffusivite = tab_diffusivite[0];
                const Eval_Dift_VDF_const& eval_dift = static_cast<const Eval_Dift_VDF_const&>(eval);
                const Champ_Fonc& ch_diff_turb = eval_dift.diffusivite_turbulente();
                const DoubleVect& diffusivite_turb = ch_diff_turb.valeurs();
                for (int i = 0; i < size; i++) diffu_tot[i] = db_diffusivite + diffusivite_turb[i];
              }
            else
              {
                const Eval_Dift_VDF_var& eval_dift = static_cast<const Eval_Dift_VDF_var&>(eval);
                const Champ_Fonc& ch_diff_turb = eval_dift.diffusivite_turbulente();
                const DoubleVect& diffusivite_turb = ch_diff_turb.valeurs();
                for (int i = 0; i < size; i++) diffu_tot[i] = tab_diffusivite[i] + diffusivite_turb[i];
              }
          }
        else
          {
            Cerr << "Method Op_Dift_VDF_base::ajouter_blocs" << finl;
            Cerr << "The type " << equation().que_suis_je() << " of the equation associated " << finl;
            Cerr << "with the current operator " << que_suis_je() << "is not coherent." << finl;
            Cerr << "It must be sub typing of Navier_Stokes_Turbulent" << finl;
            Process::exit();
          }

        for (face = 0; face < nb_faces; face++)
          if (ori(face) == 0)
            {
              const int elem1 = face_voisins(face, 0), elem2 = face_voisins(face, 1);

              if (elem1 == -1) db_diffusivite = diffu_tot(elem2);
              else if (elem2 == -1) db_diffusivite = diffu_tot(elem1);
              else db_diffusivite = 0.5 * (diffu_tot(elem2) + diffu_tot(elem1));

              double r = xv(face, 0);
              if (r >= 1.e-24)
                {
                  if (mat) (*mat)(face, face) += db_diffusivite * volumes_entrelaces(face) / (r * r);
                  secmem(face) -= inco(face) * db_diffusivite * volumes_entrelaces(face) / (r * r);
                }
            }
      }
}
