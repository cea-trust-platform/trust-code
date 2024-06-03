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

#include <Source_QC_QDM_Gen.h>
#include <Equation_base.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Modifier_pour_fluide_dilatable.h>
#include <Fluide_Quasi_Compressible.h>
#include <TRUSTTrav.h>

Implemente_instanciable(Source_QC_QDM_Gen,"Source_QC_QDM_Gen_QC",Source_base);

Sortie& Source_QC_QDM_Gen::printOn(Sortie& os) const { return os; }

Entree& Source_QC_QDM_Gen::readOn(Entree& is)
{
  Nom typ_complet;
  is >> typ_complet;
  readOn_spec( is,typ_complet);
  Cerr<<" Source_QC_QDM_Gen Pas teste encore.... on s'arrete "<<finl;
  Process::exit();
  return is;
}

// Desciption: methode appele par les classes filles
// pour typer le terme source
Entree& Source_QC_QDM_Gen::readOn_spec(Entree& is,Nom& typ)
{
  source_incompressible.typer_direct(typ);
  source_incompressible->associer_eqn(mon_equation.valeur());
  is >> source_incompressible.valeur();
  return is;
}

DoubleTab& Source_QC_QDM_Gen::ajouter(DoubleTab& resu) const
{
  DoubleTrav trav(resu);
  source_incompressible.ajouter(trav);
  // multiplication par rho de trav
  multiplier_par_rho_si_dilatable(trav,equation().milieu());
  resu+=trav;
  return resu;
}
/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 *     Mise a jour en temps du terme source.
 *
 * @param (double) le pas de temps de mise a jour
 */
void Source_QC_QDM_Gen::mettre_a_jour(double t)
{
  source_incompressible->mettre_a_jour(t);
}

/*! @brief Met a jour les references internes a l'objet Source_QC_QDM_Gen.
 *
 * Appelle 2 methodes virtuelles pures protegees:
 *        Source_QC_QDM_Gen::associer_domaines(const Domaine_dis& ,const Domaine_Cl_dis&)
 *        Source_QC_QDM_Gen::associer_pb(const Probleme_base&)
 *
 */
void Source_QC_QDM_Gen::completer()
{
  Source_base::completer();
  source_incompressible->completer();
}

/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 *     Mise a jour en temps du terme source.
 *
 * @param (double) le pas de temps de mise a jour
 */
int Source_QC_QDM_Gen::impr(Sortie& os) const
{
  return source_incompressible->impr(os);
}
DoubleTab&  Source_QC_QDM_Gen::calculer(DoubleTab& resu) const
{
  resu=0.;
  ajouter(resu);
  return resu;
}
void  Source_QC_QDM_Gen::associer_domaines(const Domaine_dis& z ,const Domaine_Cl_dis& zcl )
{
  source_incompressible->associer_domaines_public(z,zcl);
}
void  Source_QC_QDM_Gen::associer_pb(const Probleme_base& pb )
{
  if (!sub_type(Fluide_Quasi_Compressible,pb.equation(0).milieu()))
    {
      Cerr<<que_suis_je() <<" n'est a utiliser qu'en Quasi Compressible" <<finl;
      Process::exit();
    }
}

void Source_QC_QDM_Gen::creer_champ(const Motcle& motlu)
{
  source_incompressible->creer_champ(motlu);
}
const Champ_base& Source_QC_QDM_Gen::get_champ(const Motcle& nom) const
{
  return source_incompressible->get_champ(nom);
}
void Source_QC_QDM_Gen::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  source_incompressible->get_noms_champs_postraitables(nom,opt);
}
