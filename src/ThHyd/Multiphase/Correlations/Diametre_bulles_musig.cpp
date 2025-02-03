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

#include <Diametre_bulles_musig.h>
#include <Pb_Multiphase.h>

#include <Discret_Thyd.h>
#include <Champ_base.h>
#include <Milieu_MUSIG.h>

Implemente_instanciable(Diametre_bulles_musig, "Diametre_bulles_musig", Correlation_base);

Sortie& Diametre_bulles_musig::printOn(Sortie& os) const { return os; }

Entree& Diametre_bulles_musig::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("diametre", &d_bulle_, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);

  Pb_Multiphase& pb = ref_cast(Pb_Multiphase, pb_.valeur());
  const Milieu_MUSIG *milMusig = sub_type(Milieu_MUSIG, pb.milieu()) ? &ref_cast(Milieu_MUSIG, pb.milieu()) : nullptr;
  if (!milMusig) Process::exit(que_suis_je() + " : MUSIG not found !");

  int N = pb.nb_phases();
  const Discret_Thyd& dis = ref_cast(Discret_Thyd, pb.discretisation());
  Noms noms(N), unites(N);
  noms[0] = "diametre_bulles";
  unites[0] = "m";
  Motcle typeChamp = "champ_elem";
  const Domaine_dis_base& z = pb.domaine_dis();
  dis.discretiser_champ(typeChamp, z, scalaire, noms, unites, N, 0, diametres_);

  champs_compris_.ajoute_champ(diametres_);

  // Print des informations des diametres de la methode MUSIG (phase dispersee)
  // Pas obligatoire mais permet de verifier les donnees sur les diametres
  if (je_suis_maitre())
    {
      for (int k = 0; k < N; k++)
        {
          if (milMusig->has_dispersed_gas(k)) cout << " Gaz_Dispersee - Classe N° " << k << " diametre Inf=" << milMusig->get_Diameter_Inf(k) << " diametre Sup=" << milMusig->get_Diameter_Sup(k) << " diametre Sauter=" << milMusig->get_Diameter_Sauter(k) << endl;
          if (milMusig->has_dispersed_liquid(k)) cout << " Liquide_Dispersee - Classe N° " << k << " diametre Inf=" << milMusig->get_Diameter_Inf(k) << " diametre Sup=" << milMusig->get_Diameter_Sup(k) << " diametre Sauter=" << milMusig->get_Diameter_Sauter(k) << endl;
        }
    }

  // Stockage des diametres de Sauter (phase dispersee)
  DoubleTab& tab_diametres = diametres_->valeurs();
  for (int i = 0; i < tab_diametres.dimension_tot(0); i++)
    for (int k = 0; k < N; k++)
      {
        tab_diametres(i, k) = d_bulle_;
        if (milMusig->has_dispersed_gas(k)) tab_diametres(i, k) = milMusig->get_Diameter_Sauter(k);
        if (milMusig->has_dispersed_liquid(k)) tab_diametres(i, k) = milMusig->get_Diameter_Sauter(k);
      }

  return is;
}

bool Diametre_bulles_musig::has_champ(const Motcle& nom, OBS_PTR(Champ_base) &ref_champ) const
{
  if (nom == "diametre_bulles")
    return champs_compris_.has_champ(nom, ref_champ);

  return false; /* rien trouve */
}

bool Diametre_bulles_musig::has_champ(const Motcle& nom) const
{
  if (nom == "diametre_bulles")
    return true;

  return false; /* rien trouve */
}

const Champ_base& Diametre_bulles_musig::get_champ(const Motcle& nom) const
{
  if (nom == "diametre_bulles")
    return champs_compris_.get_champ(nom);

  throw std::runtime_error(std::string("Field ") + nom.getString() + std::string(" not found !"));
}
