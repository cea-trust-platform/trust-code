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
// File:        Convection_Diffusion_fraction_massique_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Equations
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_fraction_massique_QC.h>
#include <Fluide_Quasi_Compressible.h>
#include <Loi_Etat_Multi_GP_QC.h>
#include <Probleme_base.h>
#include <DoubleTrav.h>
#include <Param.h>

Implemente_instanciable(Convection_Diffusion_fraction_massique_QC,"Convection_Diffusion_fraction_massique_QC",Convection_Diffusion_fraction_massique_Fluide_Dilatable_base);

// Description:
//    Simple appel a: Convection_Diffusion_fraction_massique_Fluide_Dilatable_base::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& is
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
Sortie& Convection_Diffusion_fraction_massique_QC::printOn(Sortie& is) const
{
  return Convection_Diffusion_fraction_massique_Fluide_Dilatable_base::printOn(is);
}

// Description:
//    Verifie si l'equation a une inconnue et un fluide associe
//    et appelle Convection_Diffusion_fraction_massique_Fluide_Dilatable_base::readOn(Entree&).
// Precondition: l'objet a une inconnue associee
// Precondition: l'objet a un fluide associe
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
Entree& Convection_Diffusion_fraction_massique_QC::readOn(Entree& is)
{
  alias_=inconnue().le_nom();
  Convection_Diffusion_fraction_massique_Fluide_Dilatable_base::readOn(is);
  champs_compris_.ajoute_champ(l_inco_ch);
  l_inco_ch.valeur().add_synonymous(alias_);
  return is;
}

void Convection_Diffusion_fraction_massique_QC::set_param(Param& param)
{
  Convection_Diffusion_fraction_massique_Fluide_Dilatable_base::set_param(param);
  param.ajouter("espece",&mon_espece_);
  param.ajouter("alias",&alias_);
}

int Convection_Diffusion_fraction_massique_QC::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;
      //associe mu_sur_Sc dans la diffusivite
      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      ref_cast_non_const(Champ_base,terme_diffusif.diffusivite()).nommer("mu_sur_Schmidt");
      is >> terme_diffusif;
      //Il faut appeler associer_diffusivite_pour_pas_de_temps et on passe actuellement en
      //parametre mu_sur_Schmidt qu il faut remplacer par nu_sur_Schmidt
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(diffusivite_pour_pas_de_temps());
      return 1;
    }
  else
    return Convection_Diffusion_fraction_massique_Fluide_Dilatable_base::lire_motcle_non_standard(mot,is);
  return 1;
}

const Champ_base& Convection_Diffusion_fraction_massique_QC::diffusivite_pour_pas_de_temps()
{
  return le_fluide->mu_sur_Schmidt();
}

// Description:
//    Associe l inconnue de l equation a la loi d etat,
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Convection_Diffusion_fraction_massique_QC::completer()
{
  Convection_Diffusion_fraction_massique_Fluide_Dilatable_base::completer();
  Fluide_Quasi_Compressible& le_fluideQC=ref_cast(Fluide_Quasi_Compressible,fluide());
  Loi_Etat_Multi_GP_QC& loi_etat = ref_cast_non_const(Loi_Etat_Multi_GP_QC,le_fluideQC.loi_etat().valeur());
  loi_etat.associer_inconnue(l_inco_ch.valeur());
  loi_etat.associer_espece(*this);
}

// Description:
//     Renvoie la derivee en temps de l'inconnue de l'equation.
//     Le calcul est le suivant:
//         d(inconnue)/dt = M^{-1} * (sources - somme(Op_{i}(inconnue))) / rho
// Precondition:
// Parametre: DoubleTab& derivee
//    Signification: le tableau des valeurs de la derivee en temps du champ inconnu
//    Valeurs par defaut:
//    Contraintes: ce parametre est remis a zero des l'entree de la methode
//    Acces: sortie
// Retour: DoubleTab&
//    Signification: le tableau des valeurs de la derivee en temps du champ inconnu
//    Contraintes:
// Exception:
// Effets de bord: des communications (si version parallele) sont generees pas cet appel
// Postcondition:
DoubleTab& Convection_Diffusion_fraction_massique_QC::derivee_en_temps_inco(DoubleTab& derivee)
{
  if (schema_temps().diffusion_implicite())
    {
      // on ne peut  pas pour l'instanrt aller plus vite que la CFL
      double dt=le_schema_en_temps->pas_de_temps();
      double dt_op = operateur(1).calculer_pas_de_temps();
      if(inf_strict(dt_op,dt))
        {
          Cerr<<"diffusion_implicite cannot be achieved for the case dt > dt_op_conv"<<finl;
          Process::exit();
        }
    }

  DoubleTrav secmem(derivee);

  // on commence par retirer phi*div(1 U)
  const DoubleTab& frac_mass = inconnue().valeurs();

  int n = frac_mass.dimension_tot(0);
  DoubleTrav unite(frac_mass);
  unite=1;

  // on change temporairement la zone_cl
  operateur(1).l_op_base().associer_zone_cl_dis(zcl_modif_.valeur());
  operateur(1).ajouter(unite,secmem);
  operateur(1).l_op_base().associer_zone_cl_dis(zone_Cl_dis().valeur());

  for (int i=0; i<n; i++)
    secmem(i)=-secmem(i)*frac_mass(i);

  // suite + standard
  operateur(1).ajouter(secmem);
  les_sources.ajouter(secmem);
  solveur_masse.appliquer(secmem);

  solveur_masse->set_name_of_coefficient_temporel("masse_volumique");
  if (schema_temps().diffusion_implicite())
    {
      derivee=inconnue().valeurs();
      Equation_base::Gradient_conjugue_diff_impl(secmem, derivee);
    }
  else
    {
      DoubleTrav secmem_bis(secmem);
      operateur(0).ajouter(secmem_bis);
      solveur_masse.appliquer(secmem_bis);
      secmem += secmem_bis;
      derivee = secmem;
    }
  solveur_masse->set_name_of_coefficient_temporel("no_coeff");
  return derivee;
}

void Convection_Diffusion_fraction_massique_QC::assembler( Matrice_Morse& matrice, const DoubleTab& inco, DoubleTab& resu)
{

  resu=0;
  const IntVect& tab1= matrice.get_tab1();

  DoubleVect& coeff=matrice.get_set_coeff();

  const DoubleTab& rho=get_champ("masse_volumique").valeurs();
  operateur(0).l_op_base().contribuer_a_avec(inco, matrice );

  operateur(0).ajouter( resu );

  int ndl=rho.dimension(0);
  // on divise par rho chaque ligne
  for (int som=0; som<ndl; som++)
    {
      double inv_rho=1/rho(som);
      for (int k=tab1(som)-1; k<tab1(som+1)-1; k++)
        coeff(k)*=inv_rho;
      resu(som)*=inv_rho;
    }


  // ajout de la convection
  operateur(1).l_op_base().contribuer_a_avec(inco, matrice );
  operateur(1).ajouter(resu );


  // on retire Divu1 *inco

  DoubleTrav unite(inco),divu1(inco);
  unite=1;

  {
    // on change temporairement la zone_cl

    operateur(1).l_op_base().associer_zone_cl_dis(zcl_modif_.valeur());
    operateur(1).ajouter(unite,divu1);

    operateur(1).l_op_base().associer_zone_cl_dis(zone_Cl_dis().valeur());
  }
  for (int i=0; i<ndl; i++)
    {
      resu(i)-=divu1(i)*inco(i);
      matrice(i,i)+=divu1(i);
    }

  les_sources.contribuer_a_avec(inco,matrice);
  les_sources.ajouter(resu);
  int test_op=0;
  {
    char* theValue = getenv("TRUST_TEST_OPERATEUR_IMPLICITE_BLOQUANT");
    if (theValue != NULL) test_op=1;
  }


  if (test_op)
    {
      DoubleTab test(resu);
      DoubleTab test2(resu);
      DoubleTrav resu2(resu);
      derivee_en_temps_inco(resu2);
      solveur_masse.appliquer(test2);
      resu2-=test2;
      Cerr<<" here " <<mp_max_abs_vect(resu2)<<finl;
      matrice.ajouter_multvect(inco,test);
      solveur_masse.appliquer(test);
      const double max_test = mp_max_abs_vect(test);
      Cerr<<"iii "<<max_test<<finl;

      if (max_test>0)
        {

          for (int i=0; i<resu.size(); i++)
            if (dabs(test(i))>1e-5)
              Cerr<<i << " "<<test(i)<<finl;
          //        Cerr<<resu <<finl;
          exit();
        }
    }
  matrice.ajouter_multvect(inco,resu);
  /*
    Cerr<<" Convection_Diffusion_fraction_massique_QC::assembler non code "<<finl;
    Cerr<<"vous ne pouvez pas faire d'implicite avec des fracions massiques "<<finl;
    exit();
  */
}

const Champ_base& Convection_Diffusion_fraction_massique_QC::get_champ(const Motcle& nom) const
{
  try
    {
      return Convection_Diffusion_fraction_massique_Fluide_Dilatable_base::get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }
// a revoir ..... a mon avis
  try
    {
      return probleme().equation(0).get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }
  throw Champs_compris_erreur();

  REF(Champ_base) ref_champ;
  return ref_champ;
}
