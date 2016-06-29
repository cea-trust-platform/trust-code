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
// File:        EqnF22base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/26
//
//////////////////////////////////////////////////////////////////////////////

#include <EqnF22base.h>
#include <EChaine.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Modele_turbulence_hyd_K_Eps_V2.h>
#include <Param.h>

Implemente_base_sans_constructeur(EqnF22base,"EqnF22base",Equation_base);

EqnF22base::EqnF22base()
{
  //initialisation des constantes par defaut :
  // C1 = 1.4 avant car eqnF22 en fonction de (C1-1) ....
  // ici Durbin donne C1=0.4
  C1 = 0.4;
  //
  C2 = 0.3;
  /*
    Noms& nom=champs_compris_.liste_noms_compris();
    nom.dimensionner(1);
    nom[0]="f22";
  */
}
// Description:
//    Imprime le type de l'equation sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
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
Sortie& EqnF22base::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}


// Description:
//  cf Equation_base::readOn(is)
//    Controle dynamique du type du terme source.
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& EqnF22base::readOn(Entree& s )
{
  Equation_base::readOn(s);

  // typage de l'operateur de diffusion.
  if ( sub_type(Fluide_Incompressible,le_fluide.valeur()))
    {
      Fluide_Incompressible& fluide_inc = ref_cast(Fluide_Incompressible,le_fluide.valeur());
      terme_diffusif.associer_diffusivite(fluide_inc.viscosite_cinematique());
    }
  Nom type="Op_DiffF22_";

  Nom disc = discretisation().que_suis_je();
  // les operateurs C_D_Turb_T sont communs aux discretisations VEF et VEFP1B
  if(disc=="VEFPreP1B")
    disc="VEF";
  type+=disc;

  Nom nb_inc;
  if (terme_diffusif.diffusivite().nb_comp() == 1)
    nb_inc = "_";
  else
    nb_inc = "_Multi_inco_";
  type+= nb_inc ;

  Nom type_diff;
  if(sub_type(Champ_Uniforme,terme_diffusif.diffusivite()))
    type_diff="const_";
  else
    type_diff="var_";
  type+= type_diff;

  Nom type_inco=inconnue()->que_suis_je();
  type+=(type_inco.suffix("Champ_"));

  if (axi)
    type+="_Axi";

  terme_diffusif.typer(type);
  terme_diffusif.l_op_base().associer_eqn(*this);
  terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());
  //terme_diffusif->completer();

  return s;
}

void EqnF22base::set_param(Param& param)
{
  param.ajouter("C1",&C1);
  param.ajouter("C2",&C2);
  param.ajouter_non_std("solveur",(this),Param::REQUIRED);
}

int EqnF22base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="solveur")
    {
      Cerr << "Reading and typing of F22 equation solver: " << finl;
      Nom nom_solveur("Solv_");
      Motcle type_solv_sys;
      is >> type_solv_sys;
      nom_solveur+=type_solv_sys;
      Cerr<<"F22 solver name : "<<nom_solveur<<finl;
      if (nom_solveur=="Solv_CHOLESKY" && Process::nproc()>1)
        {
          Cerr << finl;
          Cerr << "Cholesky solver not parallelized." << finl;
          Cerr << "Change your solver." << finl;
          exit();
        }
      solveur.typer(nom_solveur);
      solveur.nommer("f22_equation_solver");
      is >> solveur.valeur();
      return 1;
    }
  else
    {
      Cerr << mot << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard"<< finl;
      exit();
    }
  return -1;
}

// Description:
//    Associe un modele de turbulence K-epsilon_V2 a l'equation.
// Precondition:
// Parametre: Modele_turbulence_hyd_K_Eps& modele
//    Signification: le modele de turbulence K-epsilon a asoocier a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation a un modele de turbulence associe
void EqnF22base::associer_modele_turbulence(const Modele_turbulence_hyd_K_Eps_V2& modele)
{
  Equation_base::associer_pb_base(modele.equation().probleme());
  Equation_base::associer_sch_tps_base(modele.equation().schema_temps());
  Equation_base::associer_zone_dis(modele.equation().zone_dis());
  //AJOUT : zone_cl_dis non initialisee
  la_zone_Cl_dis = modele.equation().zone_Cl_dis();
  associer_milieu_base(modele.equation().milieu());
  mon_modele=modele;
  discretiser();
}

int EqnF22base::preparer_calcul()
{
  Equation_base::preparer_calcul();
  return 1;
}

int EqnF22base::sauvegarder(Sortie& os) const
{
  int bytes=0;
  bytes += Equation_base::sauvegarder(os);
  bytes += le_champ_F22.sauvegarder(os);
  return bytes;
}

int EqnF22base::reprendre(Entree& is)
{
  Equation_base::reprendre(is);
  return 1;
}

void EqnF22base::completer()
{
  const Zone_dis_base& une_zone_dis = zone_dis().valeur();
  //  const Zone& zone=une_zone_dis.zone();

  int n = une_zone_dis.nb_front_Cl();
  la_zone_Cl_dis.valeur().les_conditions_limites().dimensionner(n);
  int nb_clim=0;

  int nb_cond_lim_ = modele_turbulence().eqn_transp_K_Eps().zone_Cl_dis().nb_cond_lim();

  for (int j =0; j<nb_cond_lim_; j++)
    {
      const Frontiere_dis_base& la_fr_dis = une_zone_dis.frontiere_dis(j);
      //      const Frontiere& frontiere=la_fr_dis.frontiere();
      //      const Zone& zone1=frontiere.zone();

      la_zone_Cl_dis.valeur().les_conditions_limites(j) = modele_turbulence().eqn_transp_K_Eps().zone_Cl_dis().les_conditions_limites(j);

      if (la_zone_Cl_dis.valeur().les_conditions_limites(j).valeur().que_suis_je() == "Paroi_fixe")
        {
          EChaine cha("Paroi_fixe");
          cha >> la_zone_Cl_dis.valeur().les_conditions_limites(j);
        }
      else if (la_zone_Cl_dis.valeur().les_conditions_limites(j).valeur().que_suis_je() == "Symetrie")
        {
          EChaine cha("Symetrie");
          cha >> la_zone_Cl_dis.valeur().les_conditions_limites(j);
        }
      else
        {
          Cerr << "Typing of the of the other boundaries conditions" << finl;
          EChaine cha("Symetrie");
          cha >> la_zone_Cl_dis.valeur().les_conditions_limites(j);
        }
      la_zone_Cl_dis.valeur().les_conditions_limites(j)->associer_fr_dis_base(la_fr_dis);
      nb_clim++;
    }

  une_zone_dis.ecrire_noms_bords(Cerr);

  for (int num_cl=0; num_cl<nb_clim; num_cl++)
    {
      la_zone_Cl_dis.valeur().les_conditions_limites(num_cl)->associer_zone_cl_dis_base(la_zone_Cl_dis.valeur());
      //        const Frontiere_dis_base& la_fr_dis = la_zone_Cl_dis.valeur().les_conditions_limites(num_cl).frontiere_dis();
      //        const Frontiere& frontiere=la_fr_dis.frontiere();
      //        const Zone& zone2=frontiere.zone();
      //const Zone& zone2=frontiere.zone();
    }
  verif_Cl();
  Equation_base::completer();
  terme_diffusif.valeur().dimensionner(matF22);
}


// Description:
//    Renvoie le milieu (fluide) associe a l'equation.
// Precondition: un milieu physique fluide doit avoir ete associe
//               a l'equation
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le milieu (fluide) associe a l'equation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Milieu_base& EqnF22base::milieu()
{
  if(!le_fluide.non_nul())
    {
      Cerr << "No fluid has been associated to"
           << "F22 equation." << finl;
      exit();
    }
  return le_fluide.valeur();
}


// Description:
//    Renvoie le milieu (fluide) associe a l'equation.
//    (version const)
// Precondition: un milieu physique fluide doit avoir ete associe
//               a l'equation
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le milieu (fluide) associe a l'equation
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Milieu_base& EqnF22base::milieu() const
{
  if(!le_fluide.non_nul())
    {
      Cerr << "No fluid has been associated to"
           << "F22 equation." << finl;
      exit();
    }
  return le_fluide.valeur();
}


// Description:
//    Associe un milieu physique a l'equation.
// Precondition:
// Parametre: Milieu_base& un_milieu
//    Signification: le milieu physique a associer a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void EqnF22base::associer_milieu_base(const Milieu_base& un_milieu)
{
  le_fluide =  un_milieu;
}


// Description:
//    Renvoie le nombre d'operateurs de l'equation.
//    Ici 2.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre d'operateurs de l'equation
//    Contraintes: toujours egal a 2
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int EqnF22base::nombre_d_operateurs() const
{
  return 1;
}

// Description:
//    Renvoie l'operateur specifie par son index:
//     renvoie terme_diffusif si i = 0
//     renvoie terme_convectif si i = 1
//     exit si i>1
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'index de l'operateur a renvoyer
//    Valeurs par defaut:
//    Contraintes: 0 <= i <= 1
//    Acces: entree
// Retour: Operateur&
//    Signification: l'operateur specifie
//    Contraintes: reference constante
// Exception: l'equation n'a pas plus de 2 operateurs
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Operateur& EqnF22base::operateur(int i) const
{
  switch(i)
    {
    case 0:
      return terme_diffusif;
    default :
      Cerr << "Error for EqnF22base::operateur(int i)" << finl;
      Cerr << "EqnF22base has " << nombre_d_operateurs() <<" operators "<<finl;
      Cerr << "and you are trying to access the " << i <<" th one."<< finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

// Description:
//    Renvoie l'operateur specifie par son index:
//     renvoie terme_diffusif si i = 0
//     renvoie terme_convectif si i = 1
//     exit si i>1
// Precondition:
// Parametre: int i
//    Signification: l'index de l'operateur a renvoyer
//    Valeurs par defaut:
//    Contraintes: 0 <= i <= 1
//    Acces: entree
// Retour: Operateur&
//    Signification: l'operateur specifie
//    Contraintes:
// Exception: l'equation n'a pas plus de 2 operateurs
// Effets de bord:
// Postcondition:
Operateur& EqnF22base::operateur(int i)
{
  switch(i)
    {
    case 0:
      return terme_diffusif;
    default :
      Cerr << "Error for EqnF22base::operateur(int i)" << finl;
      Cerr << "EqnF22base has " << nombre_d_operateurs() <<" operators "<<finl;
      Cerr << "and you are trying to access the " << i <<" th one."<< finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
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
void EqnF22base::discretiser()
{
  Cerr << "F22 equation discretization" << finl;
  discretiser_F22(schema_temps(),zone_dis(),le_champ_F22);
  champs_compris_.ajoute_champ(le_champ_F22);
  Equation_base::discretiser();
}

void EqnF22base::discretiser_F22(const Schema_Temps_base& sch,Zone_dis& z, Champ_Inc& ch) const
{
  int nb_val_temps = 1+0*sch.nb_valeurs_temporelles();
  const Discretisation_base& dis = discretisation();
  dis.discretiser_champ("temperature",z.valeur(),"f22","1/s",1,nb_val_temps,sch.temps_courant(),ch);
}


// Description:
//    Renvoie le nom du domaine d'application de l'equation.
//    Ici "Transport_V2".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Motcle&
//    Signification: le nom du domaine d'application de l'equation
//    Contraintes: toujours egal a "Transport_Keps"
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Motcle& EqnF22base::domaine_application() const
{
  static Motcle domaine = "indetermine";
  return domaine;
}

