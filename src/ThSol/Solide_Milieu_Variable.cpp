/****************************************************************************
* Copyright (c) 2018, CEA
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
// File:        Solide_Milieu_Variable.cpp
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Solide_Milieu_Variable.h>
#include <Champ_Uniforme.h>
#include <Param.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Discretisation_tools.h>
#include <Domaine.h>
#include <Zone_VF.h>

Implemente_instanciable(Solide_Milieu_Variable,"Solide_Milieu_Variable",Milieu_base);
// XD solide_milieu_variable milieu_base solide_milieu_variable -1 Solid with cp and/or rho non-uniform.
//

// Description:
//    Ecrit les caracteristiques du milieu su run flot de sortie.
//    Simple appel a: Milieu_base::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Solide_Milieu_Variable::printOn(Sortie& os) const
{
  return Milieu_base::printOn(os);
}

// Description:
//    Lit les caracteristiques du milieu a partir d'un
//    flot d'entree.
// cf Milieu_base::readOn
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& is
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Solide_Milieu_Variable::readOn(Entree& is)
{
  Milieu_base::readOn(is);
  return is;
}

void Solide_Milieu_Variable::set_param(Param& param)
{
  Milieu_base::set_param(param);
  param.ajouter_condition("is_read_rho","Density (rho) has not been read for a Solide_Milieu_Variable type medium.");
  param.ajouter_condition("is_read_Cp","Heat capacity (Cp) has not been read for a Solide_Milieu_Variable type medium.");
  param.ajouter_condition("is_read_lambda","Conductivity (lambda) has not been read for a Solide_Milieu_Variable type medium.");
}

void Solide_Milieu_Variable::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  Milieu_base::discretiser(pb,dis);
  dis.discretiser_champ("temperature",pb.equation(0).zone_dis(),"rho_cp_comme_T","??",1,0.,rho_cp_comme_T_);
  dis.discretiser_champ("champ_elem",pb.equation(0).zone_dis(),"rho_cp_elem","??",1,0.,rho_cp_elem_);
  champs_compris_.ajoute_champ(rho_cp_comme_T_);
  champs_compris_.ajoute_champ(rho_cp_elem_);
}

void Solide_Milieu_Variable::update_rho_cp(double temps)
{
  rho_cp_elem_.changer_temps(temps);
  rho_cp_elem_.valeur().changer_temps(temps);
  DoubleTab& rho_cp=rho_cp_elem_.valeurs();
  if (sub_type(Champ_Uniforme,rho.valeur()))
    rho_cp=rho.valeurs()(0,0);
  else
    {
      // AB: rho_cp = rho.valeurs() turns rho_cp into a 2 dimensional array with 1 compo. We want to stay mono-dim:
      rho_cp=1;
      tab_multiply_any_shape(rho_cp,rho.valeurs());
    }
  if (sub_type(Champ_Uniforme,Cp.valeur()))
    rho_cp*=Cp.valeurs()(0,0);
  else
    tab_multiply_any_shape(rho_cp,Cp.valeurs());
  rho_cp_comme_T_.changer_temps(temps);
  rho_cp_comme_T_.valeur().changer_temps(temps);
  const MD_Vector& md_som = rho_cp_elem_.zone_dis_base().domaine_dis().domaine().md_vector_sommets();
  const MD_Vector& md_faces = ref_cast(Zone_VF,rho_cp_elem_.zone_dis_base()).md_vector_faces();
  if (rho_cp_comme_T_.valeurs().get_md_vector()==rho_cp_elem_.valeurs().get_md_vector())
    {
      rho_cp_comme_T_.valeurs()= rho_cp;
    }
  else if (rho_cp_comme_T_.valeurs().get_md_vector() == md_som)
    {
      // Cerr << "WARNING: Solide_Milieu_Variable::update_rho_cp(): reprojection of a field from cells to nodes ..." << finl;
      Discretisation_tools::cells_to_nodes(rho_cp_elem_,rho_cp_comme_T_);
    }
  else if (rho_cp_comme_T_.valeurs().get_md_vector() == md_faces)
    {
      // Cerr << "WARNING: Solide_Milieu_Variable::update_rho_cp(): reprojection of a field from cells to faces ..." << finl;
      Discretisation_tools::cells_to_faces(rho_cp_elem_,rho_cp_comme_T_);
    }
  else
    {
      Cerr<< que_suis_je()<<(int)__LINE__<<finl;
      exit();
    }

}
void Solide_Milieu_Variable::mettre_a_jour(double temps)
{
  Milieu_base::mettre_a_jour(temps);
  update_rho_cp(temps);
}
int Solide_Milieu_Variable::initialiser(const double& temps)
{
  Milieu_base::initialiser(temps);
  update_rho_cp(temps);
  return 1;
}

// Description:
//    Verifie que les champs caracterisant le milieu solide
//    qui on ete lu par readOn(Entree&) sont coherents.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: la masse volumique (rho) ou la capacite calorifique (Cp)
//            n'est pas de type Champ_Uniforme
// Exception: la masse volumique (rho) ou la capacite calorifique (Cp)
//            n'est pas strictement positive
// Exception: la conductivite (lambda) n'est pas strictement positive
// Exception: l'une des proprietes physique du solide: masse volumique (rho),
//            capacite calorifique (Cp) ou conductivite (lambda) n'a pas
//            ete definie.
// Effets de bord:
// Postcondition:
void Solide_Milieu_Variable::verifier_coherence_champs(int& err,Nom& msg)
{
  msg="";
  /*  if (! ( (sub_type(Champ_Uniforme,rho.valeur())) &&
          (sub_type(Champ_Uniforme,Cp.valeur())) )   )
    {
      msg += "One of the physical properties rho ou Cp is not of type Champ_Uniforme. \n";
      err = 1;
    }
  else
    {
      if  ( (rho(0,0) <= 0) || (Cp(0,0) <= 0) )
        {
          msg += "One of the physical properties rho ou Cp is not striclty positive. \n";
          err = 1;
        }
    } */
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
