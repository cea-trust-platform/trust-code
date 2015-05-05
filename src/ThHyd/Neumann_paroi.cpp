/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Neumann_paroi.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Neumann_paroi.h>
#include <Motcle.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Convection_Diffusion_Concentration.h>

Implemente_instanciable(Neumann_paroi,"Neumann_paroi",Neumann);
Implemente_instanciable(Neumann_paroi_adiabatique,"Neumann_Paroi_adiabatique",Neumann_homogene);
Implemente_instanciable(Neumann_paroi_flux_nul,"Paroi",Neumann_paroi_adiabatique);


// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
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
Sortie& Neumann_paroi::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
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
Sortie& Neumann_paroi_adiabatique::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
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
Sortie& Neumann_paroi_flux_nul::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//    Simple appel a: Cond_lim_base::readOn(Entree& )
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& s
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Neumann_paroi::readOn(Entree& s )
{
  return Cond_lim_base::readOn(s) ;
}

// Description:
//    Simple appel a: Neumann_homogene::readOn(Entree& )
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& s
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Neumann_paroi_adiabatique::readOn(Entree& s )
{
  return Neumann_homogene::readOn(s) ;
}

// Description:
//    Simple appel a: Neumann_homogene::readOn(Entree& )
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& s
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Neumann_paroi_flux_nul::readOn(Entree& s )
{
  return Neumann_homogene::readOn(s) ;
}


// Description:
//    Renvoie un booleen indiquant la compatibilite des conditions
//    aux limites avec l'equation specifiee en parametre.
//    Des CL de type Neumann_paroi sont compatibles
//    avec une equation dont le domaine est la Thermique
//    ou bien indetermine.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation avec laquelle il faut verifier la compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: valeur booleenne,
//                   1 si les CL sont compatibles avec l'equation
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Neumann_paroi::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Thermique="Thermique";
  Motcle Diphasique="diphasique_moyenne";
  Motcle indetermine="indetermine";
  Motcle Concentration="Concentration";

  if ( (dom_app==Thermique) || (dom_app==Diphasique) || (dom_app==indetermine) || (dom_app==Concentration))
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}

void Neumann_paroi::verifie_ch_init_nb_comp()
{
  if (le_champ_front.non_nul())
    {
      const Equation_base& eq = zone_Cl_dis().equation();
      const int nb_comp = le_champ_front.valeur().nb_comp();
      eq.verifie_ch_init_nb_comp(eq.inconnue(),nb_comp);
    }
}

// Description:
//    Renvoie un booleen indiquant la compatibilite des conditions
//    aux limites avec l'equation specifiee en parametre.
//    Des CL de type Neumann_paroi sont compatibles
//    avec une equation dont le domaine est la Thermique
//    ou bien indetermine.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation avec laquelle il faut verifier la compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: valeur booleenne,
//                   1 si les CL sont compatibles avec l'equation
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Neumann_paroi_adiabatique::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Thermique="Thermique";
  Motcle Diphasique="Diphasique_moyenne";
  Motcle Thermique_H="Thermique_H";
  Motcle indetermine="indetermine";
  if ( (dom_app==Thermique) || (dom_app==Diphasique) || (dom_app==Thermique_H) || (dom_app==indetermine) )
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}

// Description:
//    Renvoie un booleen indiquant la compatibilite des conditions
//    aux limites avec l'equation specifiee en parametre.
//    Des CL de type Neumann_paroi_flux_nul sont compatibles
//    avec une equation dont le domaine est la Concentration, la Fraction_massique
//    le Transport_Keps, le Transport_Keps_V2, le Transport_V2, le Diphasique_moyenne
//    ou bien indetermine.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation avec laquelle il faut verifier la compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: valeur booleenne,
//                   1 si les CL sont compatibles avec l'equation
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Neumann_paroi_flux_nul::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Concentration="Concentration";
  Motcle Fraction_massique="Fraction_massique";
  Motcle K_Eps ="Transport_Keps";
  Motcle K_Eps_V2 ="Transport_Keps_V2";
  Motcle V2 ="Transport_V2";
  Motcle Diphasique="Diphasique_moyenne";
  Motcle indetermine="indetermine";
  if ( (dom_app==Concentration) || (dom_app==K_Eps) || (dom_app==K_Eps_V2) || (dom_app==V2) || (dom_app==Diphasique) || (dom_app==indetermine) || (dom_app==Fraction_massique))
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}

double Neumann_paroi::flux_impose(int i) const
{

  if (!sub_type(Convection_Diffusion_Concentration,ma_zone_cl_dis->equation()))
    {
      const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
      Nom nom_pb=ma_zone_cl_dis->equation().probleme().que_suis_je();
      double d_Cp;
      double d_rho;
      const Champ_Don& rho=mil.masse_volumique();
      if (rho.non_nul())
        {
          const Champ_Don& Cp =mil.capacite_calorifique();

          if (sub_type(Champ_Uniforme,rho.valeur()))
            {
              d_rho= rho(0,0);
            }
          else
            {
              d_rho= rho.valeur()(i);
            }
          if (sub_type(Champ_Uniforme,Cp.valeur()))
            {
              d_Cp= Cp(0,0);
            }
          else
            {
              d_Cp= Cp.valeur()(i);
            }
          if ((nom_pb.debute_par("Probleme_Interface")|| nom_pb==Nom("Probleme_Thermo_Front_Tracking"))||!sub_type(Champ_Uniforme,rho.valeur()))
            {
              // Pour le front tracking, on ne divise pas par Rho*Cp
              // ni pour QC
              d_rho=1.;
              d_Cp=1.;
            }
        }
      else
        {
          d_rho=1.;
          d_Cp=1.;

        }
      //  Cout << "Rho = " << d_rho << finl;
      //  Cout << "Cp = " << d_Cp << finl;
      //   Cout << "val imp = " << le_champ_front(0,0) << finl;
      if (le_champ_front.valeurs().size()==1)
        return le_champ_front(0,0)/(d_rho*d_Cp);
      else if (le_champ_front.valeurs().dimension(1)==1)
        return le_champ_front(i,0)/(d_rho*d_Cp);
      else
        Cerr << "Neumann_paroi::flux_impose erreur" << finl;
      exit();
      return 0.;
    }
  else
    {
      if (le_champ_front.valeurs().size()==1)
        return le_champ_front(0,0);
      else if (le_champ_front.valeurs().dimension(1)==1)
        return le_champ_front(i,0);
      else
        Cerr << "Neumann_paroi::flux_impose erreur" << finl;
      exit();
      return 0.;

    }
}

double Neumann_paroi::flux_impose(int i,int j) const
{

  if (!sub_type(Convection_Diffusion_Concentration,ma_zone_cl_dis->equation()))
    {
      const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
      Nom nom_pb=ma_zone_cl_dis->equation().probleme().que_suis_je();
      double d_Cp;
      double d_rho;
      const Champ_Don& rho=mil.masse_volumique();
      const Champ_Don& Cp =mil.capacite_calorifique();

      if (sub_type(Champ_Uniforme,rho.valeur()))
        {
          d_rho= rho(0,0);
        }
      else
        {
          d_rho= rho.valeur()(i);
        }
      if (sub_type(Champ_Uniforme,Cp.valeur()))
        {
          d_Cp= Cp(0,0);
        }
      else
        {
          d_Cp= Cp.valeur()(i);
        }

      if (nom_pb.debute_par("Probleme_Interface") || nom_pb==Nom("Probleme_Thermo_Front_Tracking"))
        {
          // Pour le front tracking, on ne divise pas par Rho*Cp
          d_rho=1.;
          d_Cp=1.;
        }
      if (le_champ_front.valeurs().dimension(0)==1)
        return le_champ_front(0,j)/(d_rho*d_Cp);
      else
        return le_champ_front(i,j)/(d_rho*d_Cp);
    }
  else
    {
      if (le_champ_front.valeurs().dimension(0)==1)
        return le_champ_front(0,j);
      else
        return le_champ_front(i,j);
    }

}

void Neumann_paroi::mettre_a_jour(double temps)
{
  Cond_lim_base::mettre_a_jour(temps);
  le_champ_front.mettre_a_jour(temps);
}
