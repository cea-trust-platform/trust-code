/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Terme_Source_Decroissance_Radioactive_Elem_PolyMAC.h>
#include <Domaine_VF.h>

#include <Equation_base.h>
#include <Synonyme_info.h>
#include <Probleme_base.h>
#include <Matrix_tools.h>
#include <Array_tools.h>

Implemente_instanciable(Terme_Source_Decroissance_Radioactive_Elem_PolyMAC, "Decroissance_Radioactive_Elem_PolyMAC|Decroissance_Radioactive_Elem_PolyMAC_P0P1NC", Source_base);
Add_synonym(Terme_Source_Decroissance_Radioactive_Elem_PolyMAC, "Decroissance_Radioactive_Elem_PolyMAC_P0");

Add_synonym(Terme_Source_Decroissance_Radioactive_Elem_PolyMAC, "radioactive_decay_Elem_PolyMAC_P0P1NC");
Add_synonym(Terme_Source_Decroissance_Radioactive_Elem_PolyMAC, "radioactive_decay_Elem_PolyMAC_P0");
Add_synonym(Terme_Source_Decroissance_Radioactive_Elem_PolyMAC, "radioactive_decay_Elem_PolyMAC");
// XD radioactive_decay source_base radioactive_decay 0 Radioactive decay source term of the form $-\lambda_i c_i$, where $0 \leq i \leq N$, N is the number of component of the constituent, $c_i$ and $\lambda_i$ are the concentration and the decay constant of the i-th component of the constituant.
// XD attr val list val 0 n is the number of decay constants to read (int), and val1, val2... are the decay constants (double)

Sortie& Terme_Source_Decroissance_Radioactive_Elem_PolyMAC::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Terme_Source_Decroissance_Radioactive_Elem_PolyMAC::readOn(Entree& s)
{
  double lambda_tmp;
  int nb_groupes;
  s >> nb_groupes;
  Cerr << "Nombre de groupes a lire : " << nb_groupes << finl;
  for (int i = 0; i < nb_groupes; i++)
    {
      s >> lambda_tmp;
      Cerr << "lambda lu : " << lambda_tmp << finl;
      lambda_.push_back(lambda_tmp);
    }

  const int N = equation().inconnue().valeurs().line_size(), ng = (int)lambda_.size();
  if (N != ng)
    {
      Cerr << "Terme_Source_Decroissance_Radioactive_Elem_PolyMAC : inconsistency between the number of radioactive decay constants ( " << ng
           << " ) and the number of components of the unknown of the equation ( " << N << " )" << finl;
      Process::exit();
    }
  return s ;
}

void Terme_Source_Decroissance_Radioactive_Elem_PolyMAC::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const DoubleTab& inco = equation().inconnue().valeurs();
  const int ne = equation().domaine_dis().nb_elem(), N = inco.line_size();
  std::string nom_inco = equation().inconnue().le_nom().getString();

  for (auto &&n_m : matrices)
    if (n_m.first == nom_inco)
      {
        Matrice_Morse& mat = *n_m.second, mat2;
        IntTab sten(0, 2);

        for (int e = 0; e < ne; e++)
          for (int n = 0; n < N; n++)
            sten.append_line(N * e + n, N * e + n);
        tableau_trier_retirer_doublons(sten);
        Matrix_tools::allocate_morse_matrix(inco.size_totale(), equation().probleme().get_champ(n_m.first.c_str()).valeurs().size_totale(), sten, mat2);
        mat.nb_colonnes() ? mat += mat2 : mat = mat2;
      }
}

void Terme_Source_Decroissance_Radioactive_Elem_PolyMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const DoubleVect& pe = equation().milieu().porosite_elem(), &ve = domaine.volumes();
  const DoubleTab& c = equation().inconnue().valeurs();
  std::string nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *Mc = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;
  const int N = c.line_size(), ne = domaine.nb_elem();

  for (int e = 0; e < ne; e++)
    for (int l = 0; l < N; l++)
      {
        const double fac = pe(e) * ve(e) * lambda_[l];
        secmem(e, l) -= fac * c(e, l);
        if (Mc)
          (*Mc)(N * e + l, N * e + l) += fac;
      }
}
