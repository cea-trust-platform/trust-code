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
// File:        Convection_Diffusion_fraction_massique_MB_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Equations
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_fraction_massique_MB_QC.h>
#include <Probleme_base.h>
#include <Discret_Thyd.h>
#include <Domaine.h>
#include <Avanc.h>
#include <Debog.h>
#include <Frontiere_dis_base.h>
#include <EcritureLectureSpecial.h>
#include <Loi_Etat_Melange_Binaire.h>
#include <Navier_Stokes_QC.h>
#include <DoubleTrav.h>
#include <Neumann_sortie_libre.h>
#include <Matrice_Morse.h>
#include <Param.h>
#include <Op_Conv_negligeable.h>

Implemente_instanciable_sans_constructeur(Convection_Diffusion_fraction_massique_MB_QC,"Convection_Diffusion_fraction_massique_MB_QC",Convection_Diffusion_std);

Convection_Diffusion_fraction_massique_MB_QC::Convection_Diffusion_fraction_massique_MB_QC()
{}

// Description:
//    Simple appel a: Convection_Diffusion_std::printOn(Sortie&)
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
Sortie& Convection_Diffusion_fraction_massique_MB_QC::printOn(Sortie& is) const
{
  return Convection_Diffusion_std::printOn(is);
}

// Description:
//    Verifie si l'equation a une inconnue et un fluide associe
//    et appelle Convection_Diffusion_std::readOn(Entree&).
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
Entree& Convection_Diffusion_fraction_massique_MB_QC::readOn(Entree& is)
{
  assert(l_inco_ch.non_nul());
  assert(le_fluide.non_nul());
  Convection_Diffusion_std::readOn(is);

  terme_convectif.set_fichier("Convection_fraction_massique_MB");
  terme_convectif.set_description((Nom)"Convective flux =Integral(-rho*Y*u*ndS) [kg/s] if SI units used");
  terme_diffusif.set_fichier("Diffusion_fraction_massique_MB");
  terme_diffusif.set_description((Nom)"Diffusive flux=Integral(rho*D*grad(Y)*ndS) [kg/s] if SI units used");


  //On modifie le nom ici pour que le champ puisse etre reconnu si une sonde de fraction_massique est demandee
  if (le_fluide->type_fluide()=="Melange_Binaire")
    l_inco_ch->nommer("fraction_massique");
  else
    {
      Cerr << "Error in your data file !" << finl;
      Cerr << "The equation " << que_suis_je() << " should only be used with the EOS Melange_Binaire";
      Process::exit();
    }

  champs_compris_.ajoute_champ(l_inco_ch);

  return is;
}

void Convection_Diffusion_fraction_massique_MB_QC::set_param(Param& param)
{
  Convection_Diffusion_std::set_param(param);
}

int Convection_Diffusion_fraction_massique_MB_QC::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      /*
       * The diffusive term is div(rho*D*grad(Y))
       * recall that mu_sur_Sc = rho*D and nu_sur_Sc = D
       */

      Cerr << "Reading and typing of the diffusion operator : " << finl;
      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      ref_cast_non_const(Champ_base,terme_diffusif.diffusivite()).nommer("mu_sur_Schmidt");
      is >> terme_diffusif;
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(diffusivite_pour_pas_de_temps());
      return 1;
    }
  else
    return Convection_Diffusion_std::lire_motcle_non_standard(mot,is);

}

void Convection_Diffusion_fraction_massique_MB_QC::associer_milieu_base(const Milieu_base& un_milieu)
{
  const Fluide_Quasi_Compressible& un_fluideQC = ref_cast(Fluide_Quasi_Compressible,un_milieu);
  associer_fluide(un_fluideQC);
}

const Champ_Don& Convection_Diffusion_fraction_massique_MB_QC::diffusivite_pour_transport()
{
  // rho * D
  return le_fluide->mu_sur_Schmidt();
}

const Champ_base& Convection_Diffusion_fraction_massique_MB_QC::diffusivite_pour_pas_de_temps()
{
  // D
  return le_fluide->nu_sur_Schmidt();
}

const Champ_base& Convection_Diffusion_fraction_massique_MB_QC::vitesse_pour_transport()
{
  // we need rho * u and not u
  const Probleme_base& pb = probleme();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,pb.equation(0));
  const Champ_Inc& vitessetransportante = eqn_hydr.rho_la_vitesse() ;

  return vitessetransportante;
}


// Description:
//    Tout ce qui ne depend pas des autres problemes eventuels.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Convection_Diffusion_fraction_massique_MB_QC::preparer_calcul()
{
  Convection_Diffusion_std::preparer_calcul();
  // remplissage de la zone cl modifiee avec 1 partout au bord...
  zcl_modif_=(zone_Cl_dis());

  Conds_lim& condlims=zcl_modif_.valeur().les_conditions_limites();
  int nb=condlims.size();
  for (int i=0; i<nb; i++)
    {
      // pour chaque condlim on recupere le champ_front et on met 1
      // meme si la cond lim est un flux (dans ce cas la convection restera
      // nullle.)
      DoubleTab& T=condlims[i].valeur().champ_front().valeurs();
      T=1.;
      if (sub_type(Neumann_sortie_libre,condlims[i].valeur()))
        ref_cast(Neumann_sortie_libre,condlims[i].valeur()).tab_ext()=1;
    }
  zcl_modif_.les_conditions_limites().set_modifier_val_imp(0);

  return 1;
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
void Convection_Diffusion_fraction_massique_MB_QC::completer()
{
  assert(le_fluide->loi_etat().valeur().que_suis_je() == "Loi_Etat_Melange_Binaire");
  Equation_base::completer();
}

// Description:
//    Discretise l'equation.
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
// Postcondition: l'equation est discretisee
void Convection_Diffusion_fraction_massique_MB_QC::discretiser()
{
  int nb_valeurs_temp = schema_temps().nb_valeurs_temporelles();
  double temps = schema_temps().temps_courant();
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());

  Cerr << "Massic fraction equation discretization for isothermal binary mixture" << finl;
  // fraction_massique is discretized like the temperature
  dis.discretiser_champ("temperature",zone_dis(),"fraction_massique","sans_dimension", 1,nb_valeurs_temp,temps,l_inco_ch);
  Equation_base::discretiser();
  Cerr << "Convection_Diffusion_fraction_massique_MB_QC::discretiser() ok" << finl;
}

// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Fluide_Dilatable_base upcaste en Milieu_base)
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Fluide_Dilatable_base upcaste en Milieu_base
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Milieu_base& Convection_Diffusion_fraction_massique_MB_QC::milieu() const
{
  return fluide();
}

// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Fluide_Dilatable_base upcaste en Milieu_base)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Fluide_Dilatable_base upcaste en Milieu_base
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Milieu_base& Convection_Diffusion_fraction_massique_MB_QC::milieu()
{
  return fluide();
}

// Description:
//    Renvoie le fluide incompressible associe a l'equation.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Fluide_Dilatable_base&
//    Signification: le fluide incompressible associe a l'equation
//    Contraintes: reference constante
// Exception: pas de fluide associe a l'eqaution
// Effets de bord:
//// Postcondition: la methode ne modifie pas l'objet
const Fluide_Dilatable_base& Convection_Diffusion_fraction_massique_MB_QC::fluide() const
{
  if (!le_fluide.non_nul())
    {
      Cerr << "You forgot to associate the fluid to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_fluide.valeur();
}

// Description:
//    Renvoie le fluide incompressible associe a l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Fluide_Dilatable_base&
//    Signification: le fluide incompressible associe a l'equation
//    Contraintes:
// Exception: pas de fluide associe a l'eqaution
// Effets de bord:
// Postcondition:
Fluide_Dilatable_base& Convection_Diffusion_fraction_massique_MB_QC::fluide()
{
  assert(le_fluide.non_nul());
  return le_fluide.valeur();
}

void Convection_Diffusion_fraction_massique_MB_QC::calculer_div_rho_u(DoubleTab& Div) const
{
  // No convective operator:
  if (sub_type(Op_Conv_negligeable,operateur(1).l_op_base()))
    {
      Div=0;
      return;
    }

  DoubleTrav unite(inconnue().valeurs());
  unite=1;
  ref_cast_non_const(Operateur_base,operateur(1).l_op_base()).associer_zone_cl_dis(zcl_modif_.valeur());

  operateur(1).ajouter(unite,Div);
  ref_cast_non_const(Operateur_base,operateur(1).l_op_base()).associer_zone_cl_dis(zone_Cl_dis().valeur());

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
DoubleTab& Convection_Diffusion_fraction_massique_MB_QC::derivee_en_temps_inco(DoubleTab& derivee)
{
  derivee=0;
  derivee_en_temps_inco_sans_solveur_masse(derivee);
  if (!schema_temps().diffusion_implicite())
    {
      solveur_masse.appliquer(derivee);
      derivee.echange_espace_virtuel();
    }
  return derivee;
}

DoubleTab& Convection_Diffusion_fraction_massique_MB_QC::derivee_en_temps_inco_sans_solveur_masse(DoubleTab& derivee)
{
  /*
   * The equation is like that :
   *
   * d(rho Y)/dt + div( rho*u*Y ) = div( rho*D*grad(Y) )
   *
   * With a non conservative formulation we write:
   * rho d(Y)/dt + Y d(rho)/dt + Y div (rho*u) + rho*u grad(Y) = div( rho*D*grad(Y) )
   *
   * Using the mass equation, we get :
   * rho d(Y)/dt + rho*u grad(Y) = div( rho*D*grad(Y) )
   *
   * divide by rho =>
   * d(Y)/dt = div( rho*D*grad(Y) ) / rho - u grad(Y)
   *
   * This is what we code here with derivee = d(Y)/dt
   */

  const Schema_Temps_base& sch=schema_temps();
  int diffusion_implicite=sch.diffusion_implicite();
  la_zone_Cl_dis.les_conditions_limites().set_modifier_val_imp(0);

  /*
   * FIRST TERM : diffusive
   * derivee = div( rho*D*grad(Y) )
   */
  if (!diffusion_implicite)
    operateur(0).ajouter(derivee);

  la_zone_Cl_dis.les_conditions_limites().set_modifier_val_imp(1);
  derivee.echange_espace_virtuel(); // XXX : here or later ?

  // On divise derivee  par rho
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  int n = tab_rho.dimension(0);
  for (int som=0 ; som<n ; som++)
    derivee(som) /= tab_rho(som);

  derivee.echange_espace_virtuel();

  // TODO : FIXME
  // WE SHOULD ADD THE SOURCE TERM ...
  /*
   *
   */

  /*
   * SECOND TERM : convective
   * = - u grad(Y) = [ Y div (rho*u) - div( rho*u*Y ) ] / rho
   */
  DoubleTrav convection(derivee);

  // Add Y div (rho*u)
  const DoubleTab& Y=inconnue().valeurs();
  calculer_div_rho_u(convection);

  for (int som=0 ; som<n ; som++)
    convection(som) *= (-Y(som));

  // Add convection operator: - div( rho*u*Y )
  operateur(1).ajouter(convection);

  // Divide by rho
  for (int som=0 ; som<n ; som++)
    convection(som) /= tab_rho(som);

  /*
   * TOTAL TERM : diffusive + convective
   */
  derivee+=convection;

  if (diffusion_implicite)
    {
      const DoubleTab& Tfutur=inconnue().futur();
      DoubleTrav secmem(derivee);
      secmem=derivee;
      solv_masse().appliquer(secmem);
      derivee=(Tfutur);
      solveur_masse->set_name_of_coefficient_temporel("masse_volumique");
      Equation_base::Gradient_conjugue_diff_impl( secmem, derivee ) ;
      solveur_masse->set_name_of_coefficient_temporel("no_coeff");
    }
  return derivee;
}


void Convection_Diffusion_fraction_massique_MB_QC::assembler( Matrice_Morse& matrice,const DoubleTab& inco, DoubleTab& resu)
{
  int test_op=0;
  {
    char* theValue = getenv("TRUST_TEST_OPERATEUR_IMPLICITE");
    if (theValue != NULL) test_op=2;
  }
  {
    char* theValue = getenv("TRUST_TEST_OPERATEUR_IMPLICITE_BLOQUANT");
    if (theValue != NULL) test_op=1;
  }

  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  int n = tab_rho.dimension(0);
  // ajout diffusion (avec rho, D et Y)
  operateur(0).l_op_base().contribuer_a_avec(inco, matrice );

  const IntVect& tab1= matrice.get_tab1();
  DoubleVect& coeff=matrice.get_set_coeff();
  DoubleVect coeff_diffusif(coeff);

  // on calcule les coefficients de l'op de convection on obtient les coeff de div (rho*u*Y)
  coeff=0;

  operateur(1).l_op_base().contribuer_a_avec(inco, matrice );

  // on calcule div(rho * u)
  DoubleTrav derivee2(resu);
  calculer_div_rho_u(derivee2);

  for (int som=0 ; som<n ; som++)
    {
      double inv_rho = 1. / tab_rho(som);
      for (int k=tab1(som)-1; k<tab1(som+1)-1; k++)
        coeff(k)= (coeff(k)*inv_rho+coeff_diffusif(k)*inv_rho);

      matrice(som,som)+=derivee2(som)*inv_rho;
    }

  // TODO : FIXME
  // WE SHOULD ADD THE SOURCE TERM ...
  /*
   *
   */

  // on a la matrice approchee on recalcule le residu;
  resu=0;
  derivee_en_temps_inco_sans_solveur_masse(resu);
  matrice.ajouter_multvect(inco,resu);

  if (test_op)
    {
      DoubleTrav diff(resu);
      DoubleTrav conv(resu);
      operateur(0).l_op_base().contribuer_au_second_membre(diff);
      operateur(1).l_op_base().contribuer_au_second_membre(conv);

      for (int som=0 ; som<n ; som++)
        {

          double inv_rho=1./tab_rho(som);
          diff(som)=resu(som)-conv(som)*inv_rho-diff(som)*inv_rho;
        }
      solv_masse().appliquer(diff);
      double err=mp_max_abs_vect(diff);
      Cerr << que_suis_je() << " erreur assemblage " << err << finl;

      if (err>1e-5)
        {
          {
            DoubleVect& diff_=diff;
            Cerr<<" size "<< diff_.size()<<finl;
            for (int i=0; i<diff_.size(); i++)
              if (dabs(diff_(i))>1e-5)
                {
                  Cerr<<i << " "<< diff_(i)<< " "<<finl;
                }
          }
          if (test_op==1)
            {
              Cerr<<" pb max case "<<imin_array(diff)<<" ou " <<imax_array(diff)<<finl;
              exit();
            }
        }
    } // end if (test_op)
}

int Convection_Diffusion_fraction_massique_MB_QC::sauvegarder(Sortie& os) const
{
  int bytes=0;
  bytes += Equation_base::sauvegarder(os);
  // en mode ecriture special seul le maitre ecrit
  int a_faire,special;
  EcritureLectureSpecial::is_ecriture_special(special,a_faire);

  return bytes;
}

// Description:
//     Effectue une reprise a partir d'un flot d'entree.
//     Appelle Equation_base::reprendre()
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception: la reprise a echoue
// Effets de bord:
// Postcondition:
int Convection_Diffusion_fraction_massique_MB_QC::reprendre(Entree& is)
{
  Equation_base::reprendre(is);
  return 1;

}

// Description:
//    Impression des flux sur les bords sur un flot de sortie.
//    Appelle Equation_base::impr(Sortie&)
// Precondition: Sortie&
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Convection_Diffusion_fraction_massique_MB_QC::impr(Sortie& os) const
{
  return Equation_base::impr(os);
}

// Description:
//    Renvoie le nom du domaine d'application de l'equation.
//    Ici "Thermique".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Motcle&
//    Signification: le nom du domaine d'application de l'equation
//    Contraintes: toujours egal a "Thermique"
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Motcle& Convection_Diffusion_fraction_massique_MB_QC::domaine_application() const
{
  static Motcle domaine ="Fraction_massique";
  return domaine;
}

// Description:
//    Associe un fluide de type Fluide_Quasi_Compressible a l'equation.
// Precondition:
// Parametre: Fluide_Dilatable_base& un_fluide
//    Signification: le milieu a associer a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation a un milieu associe
void Convection_Diffusion_fraction_massique_MB_QC::associer_fluide(const Fluide_Dilatable_base& un_fluide)
{
  assert(sub_type(Fluide_Quasi_Compressible,un_fluide));
  le_fluide = ref_cast(Fluide_Quasi_Compressible,un_fluide);
}
