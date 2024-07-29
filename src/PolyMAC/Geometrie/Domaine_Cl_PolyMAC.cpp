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

#include <Dirichlet_entree_fluide_leaves.h>
#include <Champ_front_softanalytique.h>
#include <Dirichlet_paroi_defilante.h>
#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Dirichlet_paroi_fixe.h>
#include <Discretisation_base.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Champ_Face_PolyMAC.h>
#include <Dirichlet_homogene.h>
#include <Champ_Inc_P0_base.h>
#include <Domaine_PolyMAC.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Matrice_Morse.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Debog.h>

Implemente_instanciable(Domaine_Cl_PolyMAC, "Domaine_Cl_PolyMAC", Domaine_Cl_dis_base);

Sortie& Domaine_Cl_PolyMAC::printOn(Sortie& os) const { return os; }

Entree& Domaine_Cl_PolyMAC::readOn(Entree& is) { return Domaine_Cl_dis_base::readOn(is); }

void Domaine_Cl_PolyMAC::completer(const Domaine_dis& )
{
  modif_perio_fait_ = 0;
}

void Domaine_Cl_PolyMAC::imposer_cond_lim(Champ_Inc& ch, double temps)
{

  Champ_Inc_base& ch_base = ch.valeur();
  DoubleTab& ch_tab = ch_base.valeurs(temps);
  int n, N = ch_tab.line_size();

  if (sub_type(Champ_Inc_P0_base, ch_base)) { /* Do nothing */ }
  else if (ch_base.nature_du_champ() == scalaire) { /* Do nothing */ }
  else if (sub_type(Champ_Face_PolyMAC_P0P1NC, ch_base) || sub_type(Champ_Face_PolyMAC, ch_base))
    {
      Champ_Face_base& ch_face = ref_cast(Champ_Face_base, ch_base);
      const Domaine_VF& mon_dom_VF = ch_face.domaine_vf();
      int ndeb, nfin, num_face;

      for (int i = 0; i < nb_cond_lim(); i++)
        {
          const Cond_lim_base& la_cl = les_conditions_limites(i).valeur();
          if (sub_type(Periodique, la_cl))
            {
              if (N > 1) Process::exit("Periodique CLS not yet supported for multiphase pbs.");

              if (modif_perio_fait_ == 0)
                {
                  // On fait en sorte que le champ ait la meme valeur
                  // sur deux faces de periodicite qui sont en face l'une de l'autre
                  const Periodique& la_cl_perio = ref_cast(Periodique, la_cl);
                  const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  int voisine;
                  double moy;
                  for (num_face = ndeb; num_face < nfin; num_face++)
                    {
                      voisine = la_cl_perio.face_associee(num_face - ndeb) + ndeb;
                      if (ch_tab[num_face] != ch_tab[voisine])
                        {
                          moy = 0.5 * (ch_tab[num_face] + ch_tab[voisine]);
                          ch_tab[num_face] = moy;
                          ch_tab[voisine] = moy;
                        }
                    }
                  // Il ne faut pas le faire a la premiere cl mais une fois toutes les cl faites une fois, cas multi perio avec ci non perio
                  // init = 1;
                }
            }
          else if (sub_type(Symetrie, la_cl))
            {
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face = ndeb; num_face < nfin; num_face++)
                for (n = 0; n < N; n++)
                  ch_tab(num_face, n) = 0;
            }
          else if (sub_type(Dirichlet_entree_fluide, la_cl))
            {
              const Dirichlet_entree_fluide& la_cl_diri = ref_cast(Dirichlet_entree_fluide, la_cl);
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (num_face = ndeb; num_face < nfin; num_face++)
                for (n = 0; n < N; n++)
                  {
                    // WEC : optimisable (pour chaque face recherche le bon temps !)
                    // vn
                    double vn = 0;
                    for (int d = 0; d < dimension; d++)
                      vn += mon_dom_VF.face_normales(num_face, d) * la_cl_diri.val_imp_au_temps(temps,num_face-ndeb, N * d + n);
                    vn /= mon_dom_VF.face_surfaces(num_face);
                    ch_tab(num_face, n) = vn;
                  }
            }
          else if (sub_type(Dirichlet_paroi_fixe, la_cl))
            {
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face = ndeb; num_face < nfin; num_face++)
                for (n = 0; n < N; n++)
                  ch_tab(num_face, n) = 0;
            }
          else if (sub_type(Dirichlet_paroi_defilante, la_cl))
            {
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face = ndeb; num_face < nfin; num_face++)
                for (n = 0; n < N; n++)
                  ch_tab(num_face, n) = 0;
            }
        }
      modif_perio_fait_ = 1;
    }
  else
    {
      Cerr << "Le type de Champ_Inc " << ch->que_suis_je() << " n'est pas prevu en PolyMAC family " << finl;
      Process::exit();
    }
  ch_tab.echange_espace_virtuel();
  Debog::verifier("Domaine_Cl_PolyMAC::imposer_cond_lim ch_tab", ch_tab);
}

int Domaine_Cl_PolyMAC::nb_faces_sortie_libre() const
{
  Process::exit();
  return -1000000;
}

int Domaine_Cl_PolyMAC::nb_bord_periodicite() const
{
  int compteur = 0;
  for (int cl = 0; cl < les_conditions_limites_.size(); cl++)
    {
      if (sub_type(Periodique, les_conditions_limites_[cl].valeur()))
        compteur++;
    }
  return compteur;
}

int Domaine_Cl_PolyMAC::initialiser(double temps)
{
  Domaine_Cl_dis_base::initialiser(temps);

  if (nb_bord_periodicite() > 0)
    {
      Cerr << " La periodicite n'est pas code !!!" << finl;
      Process::exit();
    }
  return 1;
}

Domaine_VF& Domaine_Cl_PolyMAC::domaine_vf()
{
  return ref_cast(Domaine_VF, domaine_dis().valeur());
}

const Domaine_VF& Domaine_Cl_PolyMAC::domaine_vf() const
{
  return ref_cast(Domaine_VF, domaine_dis().valeur());
}
