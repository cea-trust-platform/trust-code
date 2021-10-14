/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Terme_Source_Decroissance_Radioactive_P0_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Sources
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Decroissance_Radioactive_P0_CoviMAC.h>
#include <Equation_base.h>
#include <Zone_Cl_dis.h>
#include <Zone_CoviMAC.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Synonyme_info.h>
#include <Probleme_base.h>

Implemente_instanciable_sans_constructeur(Terme_Source_Decroissance_Radioactive_P0_CoviMAC,"Decroissance_Radioactive_P0_CoviMAC",Source_base);
Add_synonym(Terme_Source_Decroissance_Radioactive_P0_CoviMAC,"radioactive_decay_P0_CoviMAC");

//// printOn
//

Sortie& Terme_Source_Decroissance_Radioactive_P0_CoviMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je();
}

//// readOn
//

Entree& Terme_Source_Decroissance_Radioactive_P0_CoviMAC::readOn(Entree& s )
{
  double lambda_tmp;
  s >> nb_groupes;
  Cerr << "Nombre de groupes a lire : " << nb_groupes << finl;
  for (int i = 0; i < nb_groupes; i++)
    {
      s >> lambda_tmp;
      Cerr << "lambda lu : " << lambda_tmp << finl;
      lambda.push_back(lambda_tmp);
    }

  return s ;
}

void Terme_Source_Decroissance_Radioactive_P0_CoviMAC::completer()
{
  Source_base::completer();
  const int N = equation().inconnue().valeurs().line_size();
  if (N != nb_groupes)
    {
      Cerr << "Terme_Source_Decroissance_Radioactive_P0_CoviMAC : inconsistency between the number of radioactive decay constants ( " << nb_groupes << " ) and the number of components of the unknown of the equation ( " << N << " )" << finl;
      Process::exit();
    }
}

void Terme_Source_Decroissance_Radioactive_P0_CoviMAC::associer_zones(const Zone_dis& zone_dis,
                                                                      const Zone_Cl_dis& zone_Cl_dis)
{
  Cerr << " Terme_Source_Decroissance_Radioactive_P0_CoviMAC::associer_zones " << finl ;
  la_zone_CoviMAC = ref_cast(Zone_CoviMAC, zone_dis.valeur());
}

void Terme_Source_Decroissance_Radioactive_P0_CoviMAC::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Zone_VF& zone = la_zone_CoviMAC.valeur();
  const DoubleTab& inco = equation().inconnue().valeurs();
  const int ne = zone.nb_elem(), N = inco.line_size();

  for (auto &&n_m : matrices) if (n_m.first == "concentration")
      {
        Matrice_Morse& mat = *n_m.second, mat2;
        IntTrav sten(0, 2);
        sten.set_smart_resize(1);
        for (int e = 0; e < ne; e++) for (int n = 0; n < N; n++) sten.append_line(N * e + n, N * e + n);
        tableau_trier_retirer_doublons(sten);
        Matrix_tools::allocate_morse_matrix(inco.size_totale(), equation().probleme().get_champ(n_m.first.c_str()).valeurs().size_totale(), sten, mat2);
        mat.nb_colonnes() ? mat += mat2 : mat = mat2;
      }
}

void Terme_Source_Decroissance_Radioactive_P0_CoviMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_VF& zone = la_zone_CoviMAC.valeur();
  const DoubleVect& pe = zone.porosite_elem(), &ve = zone.volumes();
  const DoubleTab& c = equation().inconnue().valeurs();
  Matrice_Morse *Mc = matrices.count("concentration") ? matrices.at("concentration") : NULL;
  const int N = c.line_size();

  for (int e = 0; e < zone.nb_elem(); e++) for (int l = 0; l < N; l++)
      {
        const double fac = pe(e) * ve(e) * lambda[l];
        secmem(e, l) -= fac * c(e, l);
        if (Mc) (*Mc)(N * e + l, N * e + l) += fac;
      }
}
