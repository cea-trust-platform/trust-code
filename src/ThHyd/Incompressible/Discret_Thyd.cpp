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

#include <Solveur_Implicite_Base.h>
#include <Schema_Implicite_base.h>
#include <Schema_Temps_base.h>
#include <Discret_Thyd.h>
#include <Milieu_base.h>
#include <Champ_Fonc.h>
#include <Champ_Inc.h>
#include <Zone_dis.h>
#include <Zone_VF.h>

Implemente_base(Discret_Thyd,"Discret_Thyd",Discret_Thermique);

Sortie& Discret_Thyd::printOn(Sortie& s) const { return s; }

Entree& Discret_Thyd::readOn(Entree& s) { return s ; }

void Discret_Thyd::vitesse(const Schema_Temps_base& sch, Zone_dis& z, Champ_Inc& ch, int nb_comp) const
{
  Cerr << "Velocity discretization" << finl;
  discretiser_champ("vitesse", z.valeur(), "vitesse", "m/s", dimension * nb_comp, sch.nb_valeurs_temporelles(), sch.temps_courant(), ch);
}

void Discret_Thyd::translation(const Schema_Temps_base& sch, Zone_dis& z, Champ_Fonc& ch) const
{
  Cerr << "Translation discretization" << finl;
  discretiser_champ("vitesse", z.valeur(), "translation", "m/s", dimension, sch.temps_courant(), ch);
}

void Discret_Thyd::entcor(const Schema_Temps_base& sch, Zone_dis& z, Champ_Fonc& ch) const
{
  Cerr << "Entcor discretization" << finl;
  discretiser_champ("vitesse", z.valeur(), "entcor", "m/s", dimension, sch.temps_courant(), ch);

}

void Discret_Thyd::pression(const Schema_Temps_base& sch, Zone_dis& z, Champ_Inc& ch) const
{
  Cerr << "Pressure discretization" << finl;
  discretiser_champ("pression", z.valeur(), "pression", "Pa.m3/kg", 1,
                    sub_type(Schema_Implicite_base, sch) ? ref_cast(Schema_Implicite_base, sch).solveur()->nb_valeurs_temporelles_pression() : 1, sch.temps_courant(), ch);
}

void Discret_Thyd::pression_en_pa(const Schema_Temps_base& sch, Zone_dis& z, Champ_Inc& ch) const
{
  Cerr << "Pressure pa discretization" << finl;
  discretiser_champ("pression", z.valeur(), "pression_pa", "Pa", 1,
                    sub_type(Schema_Implicite_base, sch) ? ref_cast(Schema_Implicite_base, sch).solveur()->nb_valeurs_temporelles_pression() : 1, sch.temps_courant(), ch);
}

void Discret_Thyd::divergence_U(const Schema_Temps_base& sch, Zone_dis& z, Champ_Inc& ch) const
{
  Cerr << "Velocity divergence discretization" << finl;
  discretiser_champ("divergence_vitesse", z.valeur(), "divergence_U", "m3/s", 1, 1, sch.temps_courant(), ch);
}

void Discret_Thyd::gradient_P(const Schema_Temps_base& sch, Zone_dis& z, Champ_Inc& ch) const
{
  Cerr << "Pressure gradient discretization" << finl;
  discretiser_champ("gradient_pression", z.valeur(), "gradient_pression", "m/s2", dimension, 1, sch.temps_courant(), ch);
}

/*! @brief NE FAIT RIEN, provoque une erreur A surcharger dans les classes derivees.
 *
 * @param (Schema_Temps_base&)
 * @param (Champ_Inc&)
 * @param (Champ_Fonc&)
 */
void Discret_Thyd::creer_champ_vorticite(const Schema_Temps_base&, const Champ_Inc&, Champ_Fonc&) const
{
  Cerr << "Discret_Thyd::creer_champ_vorticite() does nothing" << finl;
  Cerr << que_suis_je() << "needs to overload it !" << finl;
  Process::exit();
}

/*! @brief NE FAIT RIEN, provoque une erreur A surcharger dans les classes derivees.
 *
 * @param (Schema_Temps_base&)
 * @param (Champ_Inc&)
 * @param (Champ_Fonc&)
 */
void Discret_Thyd::critere_Q(const Zone_dis&, const Zone_Cl_dis&, const Champ_Inc&, Champ_Fonc&) const
{
  // pour le VDF, on a besoin de la Zone_Cl_dis, mais pas pour le VEF
  // -->> on passe quand meme l argument mais on n en fait rien!!!
  Cerr << "Discret_Thyd::critere_Q() does nothing" << finl;
  Cerr << que_suis_je() << " needs to overload it !" << finl;
  Process::exit();
}

/*! @brief NE FAIT RIEN, provoque une erreur A surcharger dans les classes derivees.
 *
 * @param (Schema_Temps_base&)
 * @param (Champ_Inc&)
 * @param (Champ_Fonc&)
 */
void Discret_Thyd::diametre_hydraulique_elem(const Zone_dis& z, const Schema_Temps_base& sch, Champ_Fonc& ch) const
{
  Cerr << "Hydraulic diameter elem field discretization" << finl;
  const Zone_VF& zone_VF = ref_cast(Zone_VF, z.valeur());
  discretiser_champ("champ_elem", zone_VF, "diametre_hydraulique", "m", 1, sch.temps_courant(), ch);
  Champ_Fonc_base& ch_fonc = ref_cast(Champ_Fonc_base, ch.valeur());
  ch_fonc.valeurs().ref(zone_VF.diametre_hydraulique_elem());
}

void Discret_Thyd::diametre_hydraulique_face(const Zone_dis& z, const Schema_Temps_base& sch, Champ_Fonc& ch) const
{
  Cerr << "Hydraulic diameter face field discretization" << finl;
  const Zone_VF& zone_VF = ref_cast(Zone_VF, z.valeur());
  discretiser_champ("champ_face", zone_VF, "diametre_hydraulique_face", "m", 1, sch.temps_courant(), ch);
  Champ_Fonc_base& ch_fonc = ref_cast(Champ_Fonc_base, ch.valeur());
  ch_fonc.valeurs().ref(zone_VF.diametre_hydraulique_face());
}

void Discret_Thyd::section_passage(const Zone_dis& z, const Zone_Cl_dis& zcl, const Schema_Temps_base& sch, Champ_Fonc& ch) const
{
  Cerr << "Section passage field discretization" << finl;
  const Zone_VF& zone_VF = ref_cast(Zone_VF, z.valeur());
  discretiser_champ("champ_face", zone_VF, "section_passage", "m2", dimension, sch.temps_courant(), ch);
  Champ_Fonc_base& ch_fonc = ref_cast(Champ_Fonc_base, ch.valeur());
  DoubleVect& tab = ch_fonc.valeurs();
  const DoubleVect& spf = zcl->equation().milieu().section_passage_face();
  tab.inject_array(spf);
  //tab = zone_VF.section_passage_face();
}

void Discret_Thyd::y_plus(const Zone_dis&, const Zone_Cl_dis&, const Champ_Inc&, Champ_Fonc&) const
{
  // pour le VDF, on a besoin de la Zone_Cl_dis, mais pas pour le VEF
  // -->> on passe quand meme l argument mais on n en fait rien!!!
  Cerr << "Discret_Thyd::y_plus() does nothing" << finl;
  Cerr << que_suis_je() << " needs to overload it !" << finl;
  Process::exit();
}

void Discret_Thyd::distance_paroi_globale(const Schema_Temps_base&, Zone_dis&, Champ_Fonc&) const
{
  Cerr << "Discret_Thyd::distance_paroi() does nothing" << finl;
  Cerr << que_suis_je() << " needs to overload it !" << finl;
  Process::exit();
}

void Discret_Thyd::grad_T(const Zone_dis& z, const Zone_Cl_dis& zcl, const Champ_Inc& eqn, Champ_Fonc& ch) const
{
  Cerr << "Discret_Thyd::grad_T() does nothing" << finl;
  Cerr << que_suis_je() << " needs to overload it !" << finl;
  Process::exit();
}

void Discret_Thyd::h_conv(const Zone_dis& z, const Zone_Cl_dis& zcl, const Champ_Inc& eqn, Champ_Fonc& ch, Motcle& nom, int temp_ref) const
{
  Cerr << "Discret_Thyd::h_conv() does nothing" << finl;
  Cerr << que_suis_je() << " needs to overload it !" << finl;
  Process::exit();
}

/*! @brief NE FAIT RIEN, provoque une erreur A surcharger dans les classes derivees.
 *
 * @param (Zone_dis&)
 * @param (Fluide_Ostwald&)
 * @param (Champ_Inc&)
 * @param (Champ_Inc&)
 */
void Discret_Thyd::proprietes_physiques_fluide_Ostwald(const Zone_dis&, Fluide_Ostwald&, const Navier_Stokes_std&, const Champ_Inc&) const
{
  Cerr << "Discret_Thyd::proprietes_physiques_fluide_Ostwald() does nothing" << finl;
  Cerr << que_suis_je() << " needs to overload it !" << finl;
  Process::exit();
}

void Discret_Thyd::grad_u(const Zone_dis&, const Zone_Cl_dis&, const Champ_Inc&, Champ_Fonc&) const
{
  Cerr << "\nDiscret_Thyd::grad_u() does nothing" << finl;
  Cerr << que_suis_je() << " needs to overload it !" << finl;
  Process::exit();
}

void Discret_Thyd::concentration(const Schema_Temps_base& sch, Zone_dis& z, Champ_Inc& ch, int nb_constituants, const Nom nom_champ) const
{
  Cerr << "Concentration discretization " << finl;
  discretiser_champ("temperature", z.valeur(), nom_champ, "%", nb_constituants, sch.nb_valeurs_temporelles(), sch.temps_courant(), ch);
  ch->nommer(nom_champ);
  if (nb_constituants > 1)
    {
      ch->fixer_nature_du_champ(multi_scalaire);
      Noms noms(nb_constituants);
      for (int i = 0; i < nb_constituants; i++)
        {
          noms[i] = nom_champ;
          Nom param(i);
          noms[i] += param;
          ch->fixer_nom_compo(i, noms[i]);
        }
    }
}
