/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Masse_Multiphase.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Equations
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Masse_Multiphase.h>
#include <Pb_Multiphase.h>
#include <Discret_Thyd.h>
#include <Zone_VF.h>
#include <Domaine.h>
#include <Avanc.h>
#include <Debog.h>
#include <Frontiere_dis_base.h>
#include <EcritureLectureSpecial.h>
#include <DoubleTrav.h>
#include <Matrice_Morse.h>
#include <Param.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <EChaine.h>

Implemente_instanciable_sans_constructeur(Masse_Multiphase,"Masse_Multiphase",Convection_Diffusion_std);

Masse_Multiphase::Masse_Multiphase()
{
  /*  Noms& nom=champs_compris_.liste_noms_compris();
      nom.dimensionner(1);
      nom[0]="fraction_massique";
  */
}


Sortie& Masse_Multiphase::printOn(Sortie& is) const
{
  return Equation_base::printOn(is);
}

Entree& Masse_Multiphase::readOn(Entree& is)
{
  assert(l_inco_ch.non_nul());
  assert(le_fluide.non_nul());
  terme_convectif.associer_eqn(*this), evanescence.associer_eqn(*this);
  // const Pb_Multiphase &pb = ref_cast(Pb_Multiphase, probleme());
  // for (int i = 0; i < pb.nb_phases(); i++)
  //   champs_compris_.ajoute_nom_compris(Nom("alpha_") + pb.nom_phase(i));

  Equation_base::readOn(is);

  if (!evanescence.non_nul())
    {
      EChaine eva("{ homogene { alpha_res 0 } }");
      eva >> evanescence;
    }
  champs_compris_.ajoute_champ(l_inco_ch);
  return is;
}

void Masse_Multiphase::set_param(Param& param)
{
  Equation_base::set_param(param);
  param.ajouter_non_std("convection",(this));
  param.ajouter_non_std("evanescence|vanishing",(this));
  param.ajouter_condition("is_read_convection","The convection operator must be read, select negligeable type if you want to neglect it.");
}

int Masse_Multiphase::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="evanescence") is >> evanescence;
  else return Convection_Diffusion_std::lire_motcle_non_standard(mot, is);
  return 1;
}

const Operateur& Masse_Multiphase::operateur(int i) const
{
  if (i) Cerr << "Masse_Multiphase : wrong operator number " << i << finl, Process::exit();
  return terme_convectif;
}

Operateur& Masse_Multiphase::operateur(int i)
{
  if (i) Cerr << "Masse_Multiphase : wrong operator number " << i << finl, Process::exit();
  return terme_convectif;
}

/* l'evanescence passe en dernier */
void Masse_Multiphase::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  Equation_base::dimensionner_blocs(matrices, semi_impl);
  evanescence.valeur().dimensionner_blocs(matrices, semi_impl);
}

void Masse_Multiphase::assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  Equation_base::assembler_blocs_avec_inertie(matrices, secmem, semi_impl);
  evanescence.valeur().ajouter_blocs(matrices, secmem, semi_impl);
}

// Description:
//    Associe un milieu physique a l'equation,
//    le milieu est en fait caste en Fluide_Incompressible
// Precondition:
// Parametre: Milieu_base& un_milieu
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//                 doit pourvoir etre force au type "Fluide_Incompressible"
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: les proprietes physiques du fluide ne sont pas toutes specifiees
// Effets de bord:
// Postcondition:
void Masse_Multiphase::associer_milieu_base(const Milieu_base& un_milieu)
{
  const Fluide_Incompressible& un_fluide = ref_cast(Fluide_Incompressible,un_milieu);
  associer_fluide(un_fluide);
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
void Masse_Multiphase::completer()
{
  Equation_base::completer();
  terme_convectif.valeur().check_multiphase_compatibility();
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
void Masse_Multiphase::discretiser()
{
  int nb_valeurs_temp = schema_temps().nb_valeurs_temporelles();
  double temps = schema_temps().temps_courant();
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  Cerr << "Volume fraction discretization" << finl;
  //On utilise temperature pour la directive car discretisation identique
  const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, probleme());
  dis.discretiser_champ("temperature",zone_dis(),"alpha","sans_dimension", pb.nb_phases(),nb_valeurs_temp,temps,l_inco_ch);
  l_inco_ch.valeur().fixer_nature_du_champ(pb.nb_phases() == 1 ? scalaire : pb.nb_phases() == dimension ? vectoriel : multi_scalaire); //pfft
  Equation_base::discretiser();
  for (int i = 0; i < pb.nb_phases(); i++)
    l_inco_ch.valeur().fixer_nom_compo(i, Nom("alpha_") + pb.nom_phase(i));

  Cerr << "Masse_Multiphase::discretiser() ok" << finl;
}

// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Fluide_Incompressible upcaste en Milieu_base)
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Fluide_Incompressible upcaste en Milieu_base
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Milieu_base& Masse_Multiphase::milieu() const
{
  return fluide();
}


// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Fluide_Incompressible upcaste en Milieu_base)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Fluide_Incompressible upcaste en Milieu_base
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Milieu_base& Masse_Multiphase::milieu()
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
// Retour: Fluide_Incompressible&
//    Signification: le fluide incompressible associe a l'equation
//    Contraintes: reference constante
// Exception: pas de fluide associe a l'eqaution
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Fluide_Incompressible& Masse_Multiphase::fluide() const
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
// Retour: Fluide_Incompressible&
//    Signification: le fluide incompressible associe a l'equation
//    Contraintes:
// Exception: pas de fluide associe a l'eqaution
// Effets de bord:
// Postcondition:
Fluide_Incompressible& Masse_Multiphase::fluide()
{
  assert(le_fluide.non_nul());
  return le_fluide.valeur();
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
const Motcle& Masse_Multiphase::domaine_application() const
{
  static Motcle domaine ="Fraction_volumique";
  return domaine;
}

// Description:
//    Associe un fluide de type Fluide_Incompressible a l'equation.
// Precondition:
// Parametre: Fluide_Incompressible& un_fluide
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
void Masse_Multiphase::associer_fluide(const Fluide_Incompressible& un_fluide)
{
  assert(sub_type(Fluide_Incompressible,un_fluide));
  le_fluide = ref_cast(Fluide_Incompressible,un_fluide);
}

void Masse_Multiphase::calculer_champ_conserve(const Champ_Inc_base& ch, double t, DoubleTab& val, DoubleTab& bval, tabs_t& deriv, int val_only)
{
  const Champ_base& ch_rho = ch.equation().milieu().masse_volumique();
  const Champ_Inc_base& ch_alpha = ch.equation().inconnue(), *pch_rho = sub_type(Champ_Inc_base, ch_rho) ? &ref_cast(Champ_Inc_base, ch_rho) : NULL;
  const DoubleTab& alpha = ch_alpha.valeurs(t), &rho = ch_rho.valeurs(t);
  int i, j, nl = val.dimension_tot(0), n, N = val.line_size(), cR = sub_type(Champ_Uniforme, ch_rho);

  /* valeurs du champ */
  for (i = j = 0; i < nl; i++) for (n = 0; n < N; n++, j++) val.addr()[j] = alpha.addr()[j] * rho.addr()[cR ? n : j];
  if (val_only) return;

  /* valeur aux bords */
  /* on ne peut utiliser valeur_aux_bords que si ch_rho a une zone_dis_base */
  ch_rho.a_une_zone_dis_base() ? bval = ch_rho.valeur_aux_bords() : ch_rho.valeur_aux(ref_cast(Zone_VF, ch.zone_dis_base()).xv_bord(), bval);
  tab_multiply_any_shape(bval, ch_alpha.valeur_aux_bords());

  /* derivees */
  DoubleTab& d_a = deriv["alpha"]; //derivee en alpha : rho
  for (d_a.resize(nl, N), i = j = 0; i < nl; i++) for (n = 0; n < N; n++, j++) d_a.addr()[j] = rho.addr()[cR ? n : j];
  if (pch_rho) for (auto &&d_c : pch_rho->derivees()) //derivees en les dependances de rho
      deriv[d_c.first] = d_c.second, tab_multiply_any_shape(deriv[d_c.first], alpha);
}
