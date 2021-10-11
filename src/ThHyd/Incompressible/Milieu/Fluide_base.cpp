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
// File:        Fluide_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Milieu
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_base.h>
#include <Motcle.h>
#include <Champ.h>
#include <Champ_Uniforme.h>
#include <Champ_Fonc_Tabule.h>
#include <Probleme_base.h>
#include <Discretisation_base.h>
#include <Equation_base.h>
#include <Schema_Temps_base.h>
#include <Param.h>
#include <Champ_Fonc_MED.h>
#include <EChaine.h>

Implemente_instanciable_sans_constructeur(Fluide_base,"Fluide_base",Milieu_base);
// XD fluide_base milieu_base fluide_base -1 Basic class for fluids.
// XD attr indice field_base indice 1 Refractivity of fluid.
// XD attr kappa field_base kappa 1 Absorptivity of fluid (m-1).


Fluide_base::Fluide_base() { }

// Description:
//    Ecrit les proprietes du fluide sur un flot de sortie.
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
Sortie& Fluide_base::printOn(Sortie& os) const
{
  os << "{" << finl;
  os << "kappa "<<coeff_absorption_ << finl;
  os << "indice "<<indice_refraction_ << finl;
  os << "longueur_rayo "<<longueur_rayo_ << finl;
  os << "mu " << mu << finl;
  os << "beta_co " << beta_co << finl;
  Milieu_base::ecrire(os);
  os << "}" << finl;
  return os;
}


// Description:
//   Lit les caracteristiques du fluide a partir d'un flot
//   d'entree.
//   Format:
//     Fluide_base
//     {
//      Mu type_champ bloc de lecture de champ
//      Rho Champ_Uniforme 1 vrel
//      [Cp Champ_Uniforme 1 vrel]
//      [Lambda type_champ bloc de lecture de champ]
//      [Beta_th type_champ bloc de lecture de champ]
//      [Beta_co type_champ bloc de lecture de champ]
//     }
// cf Milieu_base::readOn
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
Entree& Fluide_base::readOn(Entree& is)
{
  Milieu_base::readOn(is);
  return is;
}

void Fluide_base::set_param(Param& param)
{
  Milieu_base::set_param(param);
  //La lecture de rho n est pas specifiee obligatoire ici car ce
  //champ ne doit pas etre lu pour un fluide dilatable
  param.ajouter("mu",&mu,Param::REQUIRED);
  param.ajouter("beta_co",&beta_co);
  param.ajouter("kappa",&coeff_absorption_);
  param.ajouter("indice",&indice_refraction_);
}

void Fluide_base::creer_champs_non_lus()
{
  Milieu_base::creer_champs_non_lus();
  creer_nu();
}

void Fluide_base::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  const Zone_dis_base& zone_dis=pb.equation(0).zone_dis();
  // mu rho nu  revoir
  double temps=pb.schema_temps().temps_courant();
  if (mu.non_nul())
    if (sub_type(Champ_Fonc_MED,mu.valeur()))
      {
        Cerr<<" on convertit le champ_fonc_med en champ_don"<<finl;
        Champ_Don mu_prov;
        dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",1,temps,mu_prov);
        mu_prov.affecter_(mu.valeur());
        mu.detach();
        nu.detach();
        dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",1,temps,mu);

        mu.valeur().valeurs()=mu_prov.valeur().valeurs();

      }
  if (!mu.non_nul())
    {
      dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",1,temps,mu);
      dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",1,temps,nu);
    }
  if (mu.non_nul())
    {
      dis.nommer_completer_champ_physique(zone_dis,"viscosite_dynamique","kg/m/s",mu.valeur());
      champs_compris_.ajoute_champ(mu.valeur());
    }
  if (sub_type(Champ_Fonc_Tabule,mu.valeur()))
    {
      dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",1,temps,nu);
    }
  if (!nu.non_nul())
    dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",1,temps,nu);

  if (nu.non_nul())
    {
      dis.nommer_completer_champ_physique(zone_dis,"viscosite_cinematique","m2/s",nu.valeur());
      champs_compris_.ajoute_champ(nu.valeur());
    }
  if (beta_co.non_nul())
    {
      dis.nommer_completer_champ_physique(zone_dis,"dilatabilite_solutale",".",beta_co.valeur());
      champs_compris_.ajoute_champ(beta_co.valeur());
    }

  Milieu_base::discretiser(pb,dis);
}
// Description:
//    Verifie que les champs lus l'ont ete correctement.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: la masse volumique (rho) n'est pas strictement positive
// Exception: la masse volumique (rho) n'est pas de type Champ_Uniforme
// Exception: la viscosite (mu) n'est pas strictement positive
// Exception: l'une des proprietes (rho ou mu) du fluide n'a pas ete definie
// Exception: la capacite calorifique (Cp) n'est pas strictement positive
// Exception: la capacite calorifique (Cp) n'est pas de type Champ_Uniforme
// Exception: la conductivite (lambda) n'est pas strictement positive
// Exception: toutes les proprietes du fluide anisotherme n'ont pas ete definies
// Effets de bord:
// Postcondition:
void Fluide_base::verifier_coherence_champs(int& err,Nom& msg)
{
  msg="";
  if (rho.non_nul())
    {
      if (mp_min_vect(rho.valeurs()) <= 0)
        {
          msg += "The density rho is not striclty positive. \n";
          err = 1;
        }
    }
  else
    {
      msg += "The density rho has not been specified. \n";
      err = 1;
    }
  if  (  (Cp.non_nul()) && ( (lambda.non_nul()) && (beta_th.non_nul()) ) ) // Fluide anisotherme
    {
//      if (mp_min_vect(Cp.valeurs()) <= 0)
//        {
//          msg += "The heat capacity Cp is not striclty positive. \n";
//          err = 1;
//        }
//      if (mp_min_vect(lambda.valeurs()) <= 0)
//        {
//          msg += "The conductivity lambda is not positive. \n";
//          err = 1;
//        }
      if (sub_type(Champ_Uniforme,Cp.valeur()))
        {
          if  (Cp(0,0) <= 0)
            {
              msg += "The heat capacity Cp is not striclty positive. \n";
              err = 1;
            }
        }
      else
        {
          msg += "The heat capacity Cp is not of type Champ_Uniforme. \n";
          err = 1;
        }
      if (sub_type(Champ_Uniforme,lambda.valeur()))
        {
          if (lambda(0,0) < 0)
            {
              msg += "The conductivity lambda is not positive. \n";
              err = 1;
            }
        }

    }
  if  ( ( (Cp.non_nul()) || (beta_th.non_nul()) ) && (!lambda.non_nul()) )
    {
      msg += " Physical properties for an anisotherm case : \n";
      msg += "the conductivity lambda has not been specified. \n";
      err = 2;
    }
  if  ( ( (lambda.non_nul()) || (beta_th.non_nul()) ) && (!Cp.non_nul()) )
    {
      msg += " Physical properties for an anisotherm case : \n";
      msg += "the heat capacity Cp has not been specified. \n";
      err = 2;
    }
  if ( ( (lambda.non_nul()) || (Cp.non_nul()) ) && (!beta_th.non_nul()) )
    {
      msg += " Physical properties for an anisotherm case : \n";
      msg += "the thermal expansion coefficient beta_th has not been specified. \n";
      err = 2;
    }

  // Test de la coherence des proprietees radiatives du fluide incompressible
  // (pour un milieu semi transparent
  if ( (coeff_absorption_.non_nul()) && (!indice_refraction_.non_nul()) )
    {
      msg += " Physical properties for semi tranparent radiation case : \n";
      msg += "Refraction index has not been specfied while it has been done for absorption coefficient. \n";
      err = 1;
    }
  if ( (!coeff_absorption_.non_nul()) && (indice_refraction_.non_nul()) )
    {
      msg += " Physical properties for semi tranparent radiation case : \n";
      msg += "Absorption coefficient has not been specfied while it has been done for refraction index. \n";
      err = 1;
    }

  if ( (coeff_absorption_.non_nul()) && indice_refraction_.non_nul() )
    {
      if (sub_type(Champ_Uniforme,coeff_absorption_.valeur()))
        {
          if (coeff_absorption_(0,0) <= 0)
            {
              msg += "The absorption coefficient kappa is not striclty positive. \n";
              err = 1;
            }
        }
    }
  Milieu_base::verifier_coherence_champs(err,msg);
}


// Description:
//    Si l'objet reference par nu et du type Champ_Uniforme
//     type nu en "Champ_Uniforme" et le remplit
//     avec mu(0,0)/rho(0,0).
//    Sinon n efait rien.
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
void Fluide_base::creer_nu()
{
  assert(mu.non_nul());
  assert(rho.non_nul());
  nu=mu;
  if (sub_type(Champ_Uniforme, mu.valeur()) && !sub_type(Champ_Uniforme, rho.valeur()))
    nu.valeurs().resize(rho.valeurs().dimension_tot(0), rho.valeurs().line_size());
  nu->nommer("nu");
}

void Fluide_base::calculer_nu()
{
  const DoubleTab& tabmu = mu.valeurs();
  const DoubleTab& tabrho = rho.valeurs();
  DoubleTab& tabnu = nu.valeurs();

  int cRho = sub_type(Champ_Uniforme, rho.valeur()), cMu = sub_type(Champ_Uniforme, mu.valeur()),
      i, j, n, Nl = tabnu.dimension_tot(0), N = tabnu.line_size();
  /* valeurs : mu / rho */
  for (i = j = 0; i < Nl; i++) for (n = 0; n < N; n++, j++) tabnu.addr()[j] = tabmu.addr()[cMu ? n : j] / tabrho.addr()[cRho ? n : j];
}


bool Fluide_base::initTimeStep(double dt)
{
  if (!equation_.size() || !e_int_auto_) return true; //pas d'equation associee ou pas de e_int a gerer
  const Schema_Temps_base& sch = equation_.begin()->second->schema_temps(); //on recupere le schema en temps par la 1ere equation
  Champ_Inc_base& ch = ref_cast(Champ_Inc_base, e_int.valeur());
  for (int i = 1; i <= sch.nb_valeurs_futures(); i++)
    ch.changer_temps_futur(sch.temps_futur(i), i), ch.futur(i) = ch.valeurs();
  return true;
}

// Description:
//    Effectue une mise a jour en temps du milieu,
//    et donc de ses parametres caracteristiques.
//    Les champs uniformes sont recalcules pour le
//    nouveau temps specifie, les autres sont mis a
//    par un appel a CLASSE_DU_CHAMP::mettre_a_jour(double temps).
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Fluide_base::mettre_a_jour(double temps)
{
  Milieu_base::mettre_a_jour(temps);
  if (beta_co.non_nul())
    beta_co.mettre_a_jour(temps);
  mu.mettre_a_jour(temps);
  calculer_nu();
  nu.valeur().changer_temps(temps);
  nu->valeurs().echange_espace_virtuel();
  if (e_int_auto_) e_int.mettre_a_jour(temps);

  // Mise a jour des proprietes radiatives du fluide incompressible
  // (Pour un fluide incompressible semi transparent).

  if (coeff_absorption_.non_nul() && indice_refraction_.non_nul())
    {
      coeff_absorption_.mettre_a_jour(temps);
      indice_refraction_.mettre_a_jour(temps);

      // Mise a jour de longueur_rayo
      longueur_rayo_.mettre_a_jour(temps);

      if (sub_type(Champ_Uniforme,kappa().valeur()))
        {
          longueur_rayo()->valeurs()(0,0)=1/(3*kappa()(0,0));
          longueur_rayo()->valeurs().echange_espace_virtuel();
        }
      else
        {
          DoubleTab& l_rayo = longueur_rayo_.valeurs();
          const DoubleTab& K = kappa().valeurs();
          for (int i=0; i<kappa().nb_valeurs_nodales(); i++)
            l_rayo[i] = 1/(3*K[i]);
          l_rayo.echange_espace_virtuel();
        }
    }
}


// Description:
//    Initialise les parametres du fluide.
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
// Postcondition: les parametres du fluide sont initialises
int Fluide_base::initialiser(const double& temps)
{
  Cerr << "Fluide_base::initialiser()" << finl;
  Milieu_base::initialiser(temps);
  mu.initialiser(temps);

  if (beta_co.non_nul())
    beta_co.initialiser(temps);
  calculer_nu();
  nu->valeurs().echange_espace_virtuel();
  nu->changer_temps(temps);

  // Initialisation des proprietes radiatives du fluide incompressible
  // (Pour un fluide incompressible semi transparent).

  if (coeff_absorption_.non_nul() && indice_refraction_.non_nul())
    {
      Cerr<<"Semi transparent fluid properties initialization."<<finl;
      coeff_absorption_.initialiser(temps);
      indice_refraction_.initialiser(temps);

      // Initialisation de longueur_rayo
      longueur_rayo_.initialiser(temps);
      if (sub_type(Champ_Uniforme,kappa().valeur()))
        {
          longueur_rayo()->valeurs()(0,0)=1/(3*kappa()(0,0));
        }
      else
        {
          DoubleTab& l_rayo = longueur_rayo_.valeurs();
          const DoubleTab& K = kappa().valeurs();
          for (int i=0; i<kappa().nb_valeurs_nodales(); i++)
            l_rayo[i] = 1/(3*K[i]);
        }
    }
  return 1;
}

int Fluide_base::is_rayo_semi_transp() const
{
  return int(indic_rayo_ == SEMITRANSP);
}

int Fluide_base::is_rayo_transp() const
{
  return int(indic_rayo_ == TRANSP);
}

void Fluide_base::reset_type_rayo()
{
  indic_rayo_=NONRAYO;
}
void Fluide_base::fixer_type_rayo()
{
  if ((coeff_absorption_.non_nul()) && (indice_refraction_.non_nul()))
    indic_rayo_ = SEMITRANSP;
  else
    indic_rayo_ = TRANSP;

}

int Fluide_base::longueur_rayo_is_discretised()
{
  return longueur_rayo_.non_nul();
}

void Fluide_base::set_h0_T0(double h0, double T0)
{
  T0_ = T0;
  h0_ = h0;
}

void Fluide_base::creer_e_int() const
{
  Champ_Inc e_int_inc;
  const Equation_base& eq = equation("temperature");
  eq.discretisation().discretiser_champ("champ_elem", eq.zone_dis(), "energie_interne", "J/kg", 1,
                                        eq.inconnue()->nb_valeurs_temporelles(), eq.inconnue()->temps(), e_int_inc);
  e_int_inc.associer_eqn(eq), e_int_inc->init_champ_calcule(*this, calculer_e_int);
  e_int = e_int_inc;
  e_int.mettre_a_jour(eq.inconnue()->temps());
  e_int_auto_ = 1;
}

void Fluide_base::calculer_e_int(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Fluide_base& fl = ref_cast(Fluide_base, obj);
  const Champ_base& ch_T = fl.equation("temperature").inconnue().valeur(), &ch_Cp = fl.capacite_calorifique();
  const DoubleTab& T = ch_T.valeurs(), &Cp = ch_Cp.valeurs();
  int i, Ni = T.dimension_tot(0), Nb = bval.dimension_tot(0), n, N = fl.id_composite >= 0 ? 1 : Cp.dimension(1),
         n0 = max(fl.id_composite, 0), cCp = Cp.dimension_tot(0) == 1;

  for (i = 0; i < Ni; i++) for (n = 0; n < N; n++) val(i, n) = fl.h0_ + Cp(!cCp * i, n) * (T(i, n0 + n) - fl.T0_);
  DoubleTab bT = ch_T.valeur_aux_bords(), bCp;
  if (ch_Cp.a_une_zone_dis_base()) bCp = ch_Cp.valeur_aux_bords();
  else bCp.resize(bval.dimension_tot(0), N), ch_Cp.valeur_aux(ref_cast(Zone_VF, ch_T.zone_dis_base()).xv_bord() , bCp);
  for (i = 0; i < Nb; i++) for (n = 0; n < N; n++) bval(i, n) = fl.h0_ + bCp(i, n) * (bT(i, n0 + n) - fl.T0_);

  DoubleTab& der_T = deriv[ch_T.le_nom().getString()];
  for (der_T.resize(Ni, N), i = 0; i < Ni; i++) for (n = 0; n < N; n++) der_T(i, n) = Cp(!cCp * i, n);
}

const Champ_base& Fluide_base::energie_interne() const
{
  if (!e_int.non_nul()) creer_e_int();
  return e_int;
}

Champ_base& Fluide_base::energie_interne()
{
  if (!e_int.non_nul()) creer_e_int();
  return e_int;
}


const Champ_base& Fluide_base::enthalpie() const
{
  if (!h.non_nul() && !e_int.non_nul()) creer_e_int();
  return h.non_nul() ? h : e_int;
}

Champ_base& Fluide_base::enthalpie()
{
  if (!h.non_nul() && !e_int.non_nul()) creer_e_int();
  return h.non_nul() ? h : e_int;
}
