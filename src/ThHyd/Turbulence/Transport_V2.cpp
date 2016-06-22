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
// File:        Transport_V2.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#include <Transport_V2.h>
#include <Modele_turbulence_hyd_K_Eps_V2.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
// genralisation
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Transport_V2,"Transport_V2",Equation_base);

Transport_V2::Transport_V2()
{
  /*  Noms& nom=champs_compris_.liste_noms_compris();
      nom.dimensionner(1);
      nom[0]="v2"; */
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
Sortie& Transport_V2::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}


// Description:
//    Lit les specifications d'une equation de transport K-epsilon
//    a partir d'un flot d'entree.
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
Entree& Transport_V2::readOn(Entree& s )
{
  Equation_base::readOn(s);

  // Ajout automatique du terme source si pas instancie dans le jeu ed donnees
  if (les_sources.est_vide())
    {
      Source t;
      Source& so=les_sources.add(t);
      //      const Probleme_base& pb = probleme();
      Cerr<<"Creation and typing of the source term for the Transport_V2 equation :"<<finl;
      Nom typ = "Source_Transport_V2";
      so.typer(typ,*this);
      so->associer_eqn(*this);
    }
  return s;
}

void Transport_V2::set_param(Param& param)
{
  Equation_base::set_param(param);
  param.ajouter_non_std("diffusion",(this));
  param.ajouter_non_std("convection",(this));
  param.ajouter_condition("is_read_diffusion","The diffusion operator must be read, select negligeable type if you want to neglect it.");
  param.ajouter_condition("is_read_convection","The convection operator must be read, select negligeable type if you want to neglect it.");
}

int Transport_V2::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;
      if ( sub_type(Fluide_Incompressible,le_fluide.valeur()))
        {
          Fluide_Incompressible& fluide_inc = ref_cast(Fluide_Incompressible,le_fluide.valeur());
          terme_diffusif.associer_diffusivite(fluide_inc.viscosite_cinematique());
        }
      //  is >> terme_diffusif;
      Nom type="Op_Dift_";
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
        type_diff="";
      else
        type_diff="var_";
      type+= type_diff;

      Nom type_inco=inconnue()->que_suis_je();
      type+=(type_inco.suffix("Champ_"));

      if (axi)
        type+="_Axi";

      terme_diffusif.typer(type);
      terme_diffusif.l_op_base().associer_eqn(*this);
      Cerr << terme_diffusif.valeur().que_suis_je() << finl;
      terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());
      Motcle motbidon;
      is >>  motbidon;
      is >>  motbidon;
      return 1;
    }
  else if (mot=="convection")
    {
      Cerr << "Reading and typing of the convection operator : " << finl;
      const Champ_Inc& inco_equation_associee =modele_turbulence().equation().inconnue();
      associer_Champ_Inconnu(inco_equation_associee);
      terme_convectif.associer_vitesse(inco_equation_associee);
      is >> terme_convectif;
      Cerr << "Lecture de l'operateur de convection fin" << finl;
      return 1;
    }
  else
    return Equation_base::lire_motcle_non_standard(mot,is);
  return 1;
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
void Transport_V2::associer_modele_turbulence(const Modele_turbulence_hyd_K_Eps_V2& modele)
{
  // Cerr << "Transport_K_Eps::associer_modele_turbulence() " << finl;
  Equation_base::associer_pb_base(modele.equation().probleme());
  Equation_base::associer_sch_tps_base(modele.equation().schema_temps());
  Equation_base::associer_zone_dis(modele.equation().zone_dis());
  associer_milieu_base(modele.equation().milieu());
  associer_Champ_Inconnu(modele.equation().inconnue());
  mon_modele=modele;
  liste_modeles_.add_if_not(modele);
  discretiser();
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
Milieu_base& Transport_V2::milieu()
{
  if(!le_fluide.non_nul())
    {
      Cerr << " No fluid has been associated"
           << "to the Transport_V2 equation" << finl;
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
const Milieu_base& Transport_V2::milieu() const
{
  if(!le_fluide.non_nul())
    {
      Cerr << " No fluid has been associated"
           << "to the Transport_V2 equation" << finl;
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
void Transport_V2::associer_milieu_base(const Milieu_base& un_milieu)
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
int Transport_V2::nombre_d_operateurs() const
{
  return 2;
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
const Operateur& Transport_V2::operateur(int i) const
{
  switch(i)
    {
    case 0:
      return terme_diffusif;
    case 1:
      return terme_convectif;
    default :
      Cerr << "Error for Transport_V2::operateur(int i)" << finl;
      Cerr << "Transport_V2 has " << nombre_d_operateurs() <<" operators "<<finl;
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
Operateur& Transport_V2::operateur(int i)
{
  switch(i)
    {
    case 0:
      return terme_diffusif;
    case 1:
      return terme_convectif;
    default :
      Cerr << "Error for Transport_V2::operateur(int i)" << finl;
      Cerr << "Transport_V2 has " << nombre_d_operateurs() <<" operators "<<finl;
      Cerr << "and you are trying to access the " << i <<" th one."<< finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

// Description:
//    Controle le champ inconnue K-epsilon en
//    forcant a zero les valeurs du champ
//    inferieurs a 1.e-10.
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
int Transport_V2::controler_V2()
{
  DoubleTab& V2 = le_champ_V2.valeurs();
  // Cerr << " dans Transport_V2::controler_V2() " << V2 << finl;
  int size=V2.dimension(0);
  int neg=0;
  for (int n=0; n<size; n++)
    {
      if (V2(n) < 1.e-6)
        {
          if (V2(n) < 0.) neg++;
          V2(n) = 1.e-6;
        }
    }
  if (2*neg>size) Cerr << "Warning: More than 50% of elements have negative values for v2 forced to 1.e-10" << finl;

  //Cerr << " dans Transport_V2::controler_V2() " << V2 << finl;
  return 1;
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
void Transport_V2::discretiser()
{
  Cerr << "V2 transport equation discretization" << finl;
  discretiser_V2(schema_temps(),zone_dis(),le_champ_V2);
  champs_compris_.ajoute_champ(le_champ_V2);
  Equation_base::discretiser();
}

void Transport_V2::discretiser_V2(const Schema_Temps_base& sch, Zone_dis& z, Champ_Inc& ch) const
{
  const Discretisation_base& dis = discretisation();
  dis.discretiser_champ("temperature",z.valeur(),"V2","m2/s2",1,sch.nb_valeurs_temporelles(),sch.temps_courant(),ch);
}

// Description:
//    Renvoie le nom du domaine d'application de l'equation.
//    Ici "Transport_Keps".
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
const Motcle& Transport_V2::domaine_application() const
{
  static Motcle domaine = "Transport_V2";
  return domaine;
}
const RefObjU& Transport_V2::get_modele(Type_modele type) const
{
  CONST_LIST_CURSEUR(RefObjU) curseur = liste_modeles_;
  while(curseur)
    {
      const RefObjU&  mod = curseur.valeur();
      if (mod.non_nul())
        if ((sub_type(Mod_turb_hyd_base,mod.valeur())) && (type==TURBULENCE))
          return mod;
      ++curseur;
    }
  return Equation_base::get_modele(type);
}

