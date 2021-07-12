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
// File:        Convection_Diffusion_Chaleur_Fluide_Dilatable_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Common
// Version:     /main/52
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Chaleur_Fluide_Dilatable_base.h>
#include <Fluide_Dilatable_base.h>
#include <Probleme_base.h>
#include <Discret_Thyd.h>
#include <Domaine.h>
#include <Avanc.h>
#include <EcritureLectureSpecial.h>
#include <Champ_Uniforme.h>
#include <Matrice_Morse.h>
#include <Navier_Stokes_std.h>
#include <DoubleTrav.h>
#include <Neumann_sortie_libre.h>
#include <Op_Conv_negligeable.h>

Implemente_base(Convection_Diffusion_Chaleur_Fluide_Dilatable_base,"Convection_Diffusion_Chaleur_Fluide_Dilatable_base",Convection_Diffusion_std);

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
Sortie& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::printOn(Sortie& is) const
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
Entree& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::readOn(Entree& is)
{
  assert(l_inco_ch.non_nul());
  assert(le_fluide.non_nul());
  Convection_Diffusion_std::readOn(is);
  terme_convectif.set_fichier("Convection_chaleur");
  terme_convectif.set_description((Nom)"Convective heat transfer rate=Integral(-rho*cp*T*u*ndS) [W] if SI units used");
  terme_diffusif.set_fichier("Diffusion_chaleur");
  terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) [W] if SI units used");
  //On modifie le nom ici pour que le champ puisse etre reconnu si une sonde d enthalpie est demandee
  if (le_fluide->type_fluide()=="Gaz_Reel") l_inco_ch->nommer("enthalpie");
  return is;
}

void Convection_Diffusion_Chaleur_Fluide_Dilatable_base::set_param(Param& param)
{
  Convection_Diffusion_std::set_param(param);
}

int Convection_Diffusion_Chaleur_Fluide_Dilatable_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  return Convection_Diffusion_std::lire_motcle_non_standard(mot,is);
}

// Description:
//    Associe un milieu physique a l'equation,
//    le milieu est en fait caste en Fluide_Dilatable_base ou en Fluide_Ostwald.
// Precondition:
// Parametre: Milieu_base& un_milieu
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//                 doit pourvoir etre force au type "Fluide_Dilatable_base"
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: les proprietes physiques du fluide ne sont pas toutes specifiees
// Effets de bord:
// Postcondition:
void Convection_Diffusion_Chaleur_Fluide_Dilatable_base::associer_milieu_base(const Milieu_base& un_milieu)
{
  const Fluide_Dilatable_base& un_fluide = ref_cast(Fluide_Dilatable_base,un_milieu);
  associer_fluide(un_fluide);
}

const Champ_Don& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::diffusivite_pour_transport()
{
  return milieu().conductivite();
}

const Champ_base& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::diffusivite_pour_pas_de_temps()
{
  return milieu().diffusivite();
}

const Champ_base& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::vitesse_pour_transport()
{
  const Probleme_base& pb = probleme();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,pb.equation(0));
  const Champ_Inc& vitessetransportante = eqn_hydr.rho_la_vitesse(); // rho * u
  return vitessetransportante;
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
void Convection_Diffusion_Chaleur_Fluide_Dilatable_base::discretiser()
{
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  Cerr << "Energy equation discretization " << finl;
  dis.temperature(schema_temps(), zone_dis(), l_inco_ch);
  champs_compris_.ajoute_champ(l_inco_ch);
  Equation_base::discretiser();
  Cerr << "Convection_Diffusion_Chaleur_Fluide_Dilatable_base::discretiser() ok" << finl;
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
const Milieu_base& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::milieu() const
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
Milieu_base& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::milieu()
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
// Postcondition: la methode ne modifie pas l'objet
const Fluide_Dilatable_base& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::fluide() const
{
  if (!le_fluide.non_nul())
    {
      Cerr << "You forgot to associate the fluid to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_fluide.valeur();
}

void Convection_Diffusion_Chaleur_Fluide_Dilatable_base::associer_fluide(const Fluide_Dilatable_base& un_fluide)
{
  le_fluide = un_fluide;
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
Fluide_Dilatable_base& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::fluide()
{
  assert(le_fluide.non_nul());
  return le_fluide.valeur();
}

void Convection_Diffusion_Chaleur_Fluide_Dilatable_base::calculer_div_u(DoubleTab& Div) const
{
  // No convective operator:
  if (sub_type(Op_Conv_negligeable,operateur(1).l_op_base()))
    {
      Div=0;
      return;
    }

  // Compute Div(u) with convection operator
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
DoubleTab& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::derivee_en_temps_inco(DoubleTab& derivee)
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

DoubleTab& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::derivee_en_temps_inco_sans_solveur_masse(DoubleTab& derivee)
{
  // Compute derivee= Div(lambda*grad(T))+S)/(rho*Cp) - ugrad(T)
  const Schema_Temps_base& sch=schema_temps();
  int diffusion_implicite=sch.diffusion_implicite();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  int n = tab_rho.dimension(0);
  la_zone_Cl_dis.les_conditions_limites().set_modifier_val_imp(0);
  // Calcul de derivee=Div(lambda*gradient(temperature))
  if (le_fluide->type_fluide()=="Gaz_Parfait")
    {
      if (!diffusion_implicite)
        operateur(0).ajouter(derivee);
    }
  else
    {
      operateur(0).ajouter(le_fluide->temperature(),derivee);
      if (diffusion_implicite)
        {
          Cerr << "Error: diffusion implicit not implemented in Convection_Diffusion_Chaleur_Fluide_Dilatable_base" << finl;
          Process::exit();
        }
    }

  la_zone_Cl_dis.les_conditions_limites().set_modifier_val_imp(1);
  derivee.echange_espace_virtuel();

  // On ajoute le terme source a derivee
  les_sources.ajouter(derivee);

  // on stocke rho_cp
  // On divise derivee par rho*Cp dans le cas d'un gaz parfait, par rho dans le cas d'un gaz reel
  if (le_fluide->type_fluide()=="Gaz_Parfait")
    {
      le_fluide->update_rho_cp(schema_temps().temps_courant());
      const DoubleTab& rhoCp = get_champ("rho_cp_comme_T").valeurs();
      for (int som=0 ; som<n ; som++)
        derivee(som) /= rhoCp(som);
    }
  else
    {
      for (int som=0 ; som<n ; som++)
        derivee(som) /= tab_rho(som);
    }
  derivee.echange_espace_virtuel();

  // We need to compute convection = -ugrad(T) = ( T*div([rho*]u) - div([rho*]u*T) )[/rho]
  DoubleTrav convection(derivee);

  // Add T*div([rho*]u*1)
  const DoubleTab& T=inconnue().valeurs();
  calculer_div_u_ou_div_rhou(convection);

  for (int som=0 ; som<n ; som++)
    convection(som) *= (-T(som));

  // Add convection operator: -div([rho*]u*T)
  operateur(1).ajouter(convection);

  // Divide by volume mass if necessary:
  if (is_generic())
    for (int som=0 ; som<n ; som++) convection(som) /= tab_rho(som);

  derivee+=convection;

  if (diffusion_implicite)
    {
      const DoubleTab& Tfutur=inconnue().futur();
      DoubleTrav secmem(derivee);
      secmem=derivee;
      solv_masse().appliquer(secmem);
      derivee=(Tfutur);
      solveur_masse->set_name_of_coefficient_temporel("rho_cp_comme_T");
      Equation_base::Gradient_conjugue_diff_impl( secmem, derivee ) ;
      solveur_masse->set_name_of_coefficient_temporel("no_coeff");
    }
  return derivee;
}

// derivee_en_temps_inco non std donc assembler non plus
void Convection_Diffusion_Chaleur_Fluide_Dilatable_base::assembler( Matrice_Morse& matrice,const DoubleTab& inco, DoubleTab& resu)
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

  //ajout diffusion (avec la conductivite et la temperature)
  operateur(0).l_op_base().contribuer_a_avec(inco, matrice );
  les_sources.contribuer_a_avec(inco,matrice);
  const IntVect& tab1= matrice.get_tab1();
  DoubleVect& coeff=matrice.get_set_coeff();
  DoubleVect coeff_diffusif(coeff);
  // on calcule les coefficients de l'op de convection on obtient les coeff de div(p*u*T)
  // il faudrait multiplier par cp puis divisier par rho cp on le fera d'un coup...
  coeff=0;
  operateur(1).l_op_base().contribuer_a_avec(inco, matrice );

  // on calcule div(rhou)
  DoubleTrav derivee2(resu);
  calculer_div_u_ou_div_rhou(derivee2);

  int som;
  if (le_fluide->type_fluide()=="Gaz_Parfait")
    {
      le_fluide->update_rho_cp(schema_temps().temps_courant());
      const DoubleTab& rhoCp = get_champ("rho_cp_comme_T").valeurs();

      for (som=0 ; som<n ; som++)
        {
          double inv_rho = 1. / tab_rho(som);

          if (!is_generic()) inv_rho = 1.;

          double rapport = 1. / rhoCp(som);

          // il faut multiplier toute la ligne de la matrice par rapport
          for (int k=tab1(som)-1; k<tab1(som+1)-1; k++)
            coeff(k)= (coeff(k)*inv_rho+coeff_diffusif(k)*rapport);

          // ajout de Tdiv(rhou )/rho
          matrice(som,som)+=derivee2(som)*inv_rho;
        }
    }
  else
    {
      Cerr<<"The implicit algorithm is available only for perfect gas."<<finl;
      Process::exit();
    }

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
      les_sources.ajouter(diff);
      double Cp=-5;
      int is_cp_unif=sub_type(Champ_Uniforme,le_fluide->capacite_calorifique().valeur());
      const DoubleTab& tab_cp =le_fluide->capacite_calorifique().valeurs();
      if (is_cp_unif)
        Cp=tab_cp(0,0);
      for (som=0 ; som<n ; som++)
        {
          if (!is_cp_unif) Cp=tab_cp(som);

          double inv_rho=1./tab_rho(som);

          if (!is_generic()) inv_rho=1;

          double rapport=1./(tab_rho(som)*Cp);

          diff(som)=resu(som)-conv(som)*inv_rho-diff(som)*rapport;
        }
      solv_masse().appliquer(diff);
      double err=mp_max_abs_vect(diff);
      Cerr<<que_suis_je()<<" erreur assemblage "<<err<<finl;;

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
              Process::exit();
            }
        }
    }
}

int Convection_Diffusion_Chaleur_Fluide_Dilatable_base::sauvegarder(Sortie& os) const
{
  int bytes=0;
  bytes += Equation_base::sauvegarder(os);
  // en mode ecriture special seul le maitre ecrit
  int a_faire,special;
  EcritureLectureSpecial::is_ecriture_special(special,a_faire);

  if (a_faire)
    {
      Nom ident_Pth("pression_thermo");
      ident_Pth += probleme().domaine().le_nom();
      double temps = inconnue().temps();
      ident_Pth += Nom(temps,"%e");
      os << ident_Pth<<finl;
      os << "constante"<<finl;
      os << le_fluide->pression_th();
      os << flush ;
      Cerr << "Saving thermodynamic pressure at time : " <<  Nom(temps,"%e") << finl;
    }
  return bytes;
}

// Description:
//     Effectue une reprise a partir d'un flot d'entree.
//     Appelle Equation_base::reprendre()
//     et reprend l'inconnue de la chaleur et la pression thermodynamique
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception: la reprise a echoue, identificateur de la pression non trouve
// Effets de bord:
// Postcondition:
int Convection_Diffusion_Chaleur_Fluide_Dilatable_base::reprendre(Entree& is)
{
  if (le_fluide->type_fluide() != "Gaz_Parfait") l_inco_ch->nommer("enthalpie");
  Equation_base::reprendre(is);
  double temps = schema_temps().temps_courant();
  Nom ident_Pth("pression_thermo");
  ident_Pth += probleme().domaine().le_nom();
  ident_Pth += Nom(temps,probleme().reprise_format_temps());
  avancer_fichier(is, ident_Pth);
  double pth;
  is>>pth;
  le_fluide->set_pression_th(pth);

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
int Convection_Diffusion_Chaleur_Fluide_Dilatable_base::impr(Sortie& os) const
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
const Motcle& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::domaine_application() const
{
  static Motcle domaine = "Thermique_H";
  if (le_fluide->type_fluide()=="Gaz_Parfait")
    {
      domaine = "Thermique";
    }
  return domaine;
}

int Convection_Diffusion_Chaleur_Fluide_Dilatable_base::preparer_calcul()
{
  Convection_Diffusion_std::preparer_calcul();
  return 1;
}

// Description:
// remplissage de la zone cl modifiee avec 1 partout au bord...
int Convection_Diffusion_Chaleur_Fluide_Dilatable_base::remplir_cl_modifiee()
{
  zcl_modif_=(zone_Cl_dis());
  Conds_lim& condlims=zcl_modif_.valeur().les_conditions_limites();
  int nb=condlims.size();
  // pour chaque condlim on recupere le champ_front et on met 1
  // meme si la cond lim est un flux (dans ce cas la convection restera nullle.)
  for (int i=0; i<nb; i++)
    {
      DoubleTab& T=condlims[i].valeur().champ_front().valeurs();
      T=1.;
      if (sub_type(Neumann_sortie_libre,condlims[i].valeur()))
        ref_cast(Neumann_sortie_libre,condlims[i].valeur()).tab_ext()=1;
    }
  zcl_modif_.les_conditions_limites().set_modifier_val_imp(0);
  return 1;
}
