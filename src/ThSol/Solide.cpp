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

#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Champ_Uniforme.h>
#include <Champ_Fonc_MED.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Solide.h>
#include <Param.h>

Implemente_instanciable(Solide,"Solide",Milieu_base);
// XD solide milieu_base solide -1 Solid with cp and/or rho non-uniform.
// XD attr rho field_base rho 1 Density (kg.m-3).
// XD attr cp field_base cp 1 Specific heat (J.kg-1.K-1).
// XD attr lambda field_base lambda_u 1 Conductivity (W.m-1.K-1).
// XD attr user_field field_base user_field 1 user defined field.

/*! @brief Ecrit les caracteristiques du milieu su run flot de sortie.
 *
 * Simple appel a: Milieu_base::printOn(Sortie&)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Solide::printOn(Sortie& os) const { return Milieu_base::printOn(os); }

/*! @brief Lit les caracteristiques du milieu a partir d'un flot d'entree.
 *
 *  cf Milieu_base::readOn
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree& is) le flot d'entree modifie
 */
Entree& Solide::readOn(Entree& is)
{
  champs_don_.add(mon_champ_);
  return Milieu_base::readOn(is);
}

void Solide::set_param(Param& param)
{
  Milieu_base::set_param(param);
  param.ajouter_condition("is_read_rho","Density (rho) has not been read for a Solide type medium.");
  param.ajouter_condition("is_read_Cp","Heat capacity (Cp) has not been read for a Solide type medium.");
  param.ajouter_condition("is_read_lambda","Conductivity (lambda) has not been read for a Solide type medium.");
  param.ajouter_non_std("user_field",(this));
}

/*! @brief Verifie que les champs caracterisant le milieu solide qui on ete lu par readOn(Entree&) sont coherents.
 *
 * @throws la conductivite (lambda) n'est pas strictement positive
 * @throws l'une des proprietes physique du solide: masse volumique (rho),
 * capacite calorifique (Cp) ou conductivite (lambda) n'a pas
 * ete definie.
 */
void Solide::verifier_coherence_champs(int& err,Nom& msg)
{
  msg="";
  if (sub_type(Champ_Uniforme,lambda.valeur()))
    {
      if (lambda(0,0) <= 0)
        {
          msg += "The conductivity lambda is not striclty positive. \n";
          err = 1;
        }
    }

  Milieu_base::verifier_coherence_champs(err,msg);
}

int Solide::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "user_field")
    {
      is >> nom_champ_;
      is >> mon_champ_;
      return 1;
    }
  else return Milieu_base::lire_motcle_non_standard(mot,is);
}

void Solide::discretiser(const Probleme_base& pb, const Discretisation_base& dis)
{
  if (has_hydr_diam())
    {
      Cerr << "WHAT ?? It makes no sense to define diametre_hyd_champ in a Solid medium !!!" << finl;
      Process::exit();
    }

  Milieu_base::discretiser(pb,dis);
  if (mon_champ_.non_nul())
    {
      is_user_defined_ = true;
      const Domaine_dis_base& domaine_dis=pb.equation(0).domaine_dis();
      const double temps = pb.schema_temps().temps_courant();
      if (sub_type(Champ_Fonc_MED,mon_champ_.valeur()))
        {
          Cerr<<"Convert Champ_fonc_MED " << nom_champ_ << " to a Champ_Don ..."<<finl;
          Champ_Don tmp_fld;
          dis.discretiser_champ("champ_elem",domaine_dis,"neant","neant",1,temps,tmp_fld);
          tmp_fld.affecter_(mon_champ_.valeur()); // interpolate ...
          mon_champ_.detach();
          dis.discretiser_champ("champ_elem",domaine_dis,nom_champ_,"neant",1,temps,mon_champ_);
          mon_champ_->valeurs() = tmp_fld->valeurs();
        }
      else if (sub_type(Champ_Uniforme,mon_champ_.valeur())) // blabla ...
        {
          const double val = mon_champ_->valeurs()(0,0);
          mon_champ_.detach();
          dis.discretiser_champ("champ_elem",domaine_dis,nom_champ_,"neant",1,temps,mon_champ_);
          mon_champ_->valeurs() = val;
        }
      else
        dis.nommer_completer_champ_physique(domaine_dis, nom_champ_, "neant", mon_champ_.valeur(), pb);

      champs_compris_.ajoute_champ(mon_champ_.valeur());
    }
}
