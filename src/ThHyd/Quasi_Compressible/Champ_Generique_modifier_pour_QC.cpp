/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Champ_Generique_modifier_pour_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_modifier_pour_QC.h>
#include <Postraitement.h>
#include <Modifier_pour_QC.h>
#include <Equation_base.h>
#include <Fluide_Quasi_Compressible.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Champ_Generique_modifier_pour_QC,"modifier_pour_QC",Champ_Gen_de_Champs_Gen);

Champ_Generique_modifier_pour_QC::Champ_Generique_modifier_pour_QC()
{
  diviser_=0;
}

Sortie& Champ_Generique_modifier_pour_QC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Champ_Generique_modifier_pour_QC::readOn(Entree& s )
{
  Champ_Gen_de_Champs_Gen::readOn(s);
  return s ;
}

void Champ_Generique_modifier_pour_QC::set_param(Param& param)
{
  Champ_Gen_de_Champs_Gen::set_param(param);
  param.ajouter_flag("division",&diviser_);
}

void Champ_Generique_modifier_pour_QC::completer(const Postraitement_base& post)
{
  Champ_Gen_de_Champs_Gen::completer(post);
  const Postraitement& mon_post = ref_cast(Postraitement,post);
  mon_milieu_ = mon_post.probleme().equation(0).milieu();
}

const Champ_base& Champ_Generique_modifier_pour_QC::get_champ_without_evaluation(Champ& espace_stockage) const
{
  Champ source_espace_stockage;
  const Champ_base& source = get_source(0).get_champ_without_evaluation(source_espace_stockage);
  Nature_du_champ nature_source = source.nature_du_champ();
  int nb_comp = source.nb_comp();
  Champ_Fonc es_tmp;
  espace_stockage = creer_espace_stockage(nature_source,nb_comp,es_tmp);
  return espace_stockage.valeur();
}
const Champ_base& Champ_Generique_modifier_pour_QC::get_champ(Champ& espace_stockage) const
{
  Champ source_espace_stockage;
  const Champ_base& source = get_source(0).get_champ(source_espace_stockage);
  Nature_du_champ nature_source = source.nature_du_champ();
  int nb_comp = source.nb_comp();
  Champ_Fonc es_tmp;
  espace_stockage = creer_espace_stockage(nature_source,nb_comp,es_tmp);

  DoubleTab& val = espace_stockage.valeurs();
  val = source.valeurs();
  const Fluide_Quasi_Compressible& fluide = ref_cast(Fluide_Quasi_Compressible,mon_milieu_.valeur());
  multiplier_diviser_rho(val,fluide,diviser_);
  val.echange_espace_virtuel();
  return espace_stockage.valeur();
}

//Nomme le champ en tant que source par defaut
//"Modifier_pour_QC_"+nom_champ_source
void Champ_Generique_modifier_pour_QC::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source, nom_champ_source;
      const Noms nom = get_source(0).get_property("nom");
      nom_champ_source = nom[0];
      nom_post_source = "Modifier_pour_QC_";
      nom_post_source +=  nom_champ_source;
      nommer(nom_post_source);
    }
}

