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
// File:        Modele_turbulence_scal_Prandtl.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/38
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_turbulence_scal_Prandtl.h>
#include <Pb_Thermohydraulique_Turbulent_QC.h>
#include <Modifier_pour_QC.h>
#include <Param.h>
#include <Champ_Uniforme.h>
#include <Motcle.h>
#include <Parser_U.h>

Implemente_instanciable_sans_constructeur(Modele_turbulence_scal_Prandtl,"Modele_turbulence_scal_Prandtl",Mod_Turb_scal_diffturb_base);

Modele_turbulence_scal_Prandtl::Modele_turbulence_scal_Prandtl() : LePrdt(0.9) {}

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
Sortie& Modele_turbulence_scal_Prandtl::printOn(Sortie& s ) const
{

  return Mod_Turb_scal_diffturb_base::printOn(s);
}


// Description:
//    Lit les specifications d'un modele de turbulence
//    a partir d'un flot d'entree.
//    Format:
//      {
//      }
//    (il n'y a rien a lire sauf les accolades)
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
// Exception: accolade fermante attendue
// Effets de bord:
// Postcondition:
Entree& Modele_turbulence_scal_Prandtl::readOn(Entree& is )
{
  Mod_Turb_scal_diffturb_base::readOn(is);
  /*
  // GF on  peut forcer le modele a toujours utilise le parser
  //pour l'instant c'est bloquant pour les constituants
  if (definition_fonction==Nom())
  {
  definition_fonction=Nom("nu_t/");
  Nom nprtd(LePrdt);
  definition_fonction+=nprtd;
  }
  */

  // si on  a lu une fonction, on initialise le Parser
  if (definition_fonction!=Nom())
    {
      fonction.setNbVar(2);
      fonction.addVar("alpha");
      fonction.addVar("nu_t");
      fonction.setString(definition_fonction);
      fonction.parseString();
    }

  if (LePrdt_fct!=Nom())
    {
      fonction1.setNbVar(3);
      fonction1.addVar("x");
      fonction1.addVar("y");
      fonction1.addVar("z");
      fonction1.setString(LePrdt_fct);
      fonction1.parseString();
      Cerr << "L'expression du nombre de Prandtl turbulent est " << LePrdt_fct << finl;
    }
  else
    {
      Cerr << "La valeur par defaut du nombre de Prandtl turbulent est " << LePrdt << finl;
    }

  Cerr << "L'expression du nombre de Prandtl turbulent est " << LePrdt_fct << finl;
  return is;
}

void Modele_turbulence_scal_Prandtl::set_param(Param& param)
{
  param.ajouter("Prdt",&LePrdt_fct);
  param.ajouter("Prandt_turbulent_fonction_nu_t_alpha",&definition_fonction);
  Modele_turbulence_scal_base::set_param(param);
}




// Description:
//    Calcule la diffusivite turbulente et
//    la loi de paroi.
// Precondition:
// Parametre: double
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: NON ACCEDE
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Modele_turbulence_scal_Prandtl::mettre_a_jour(double )
{
  calculer_diffusivite_turbulente();
  const Milieu_base& mil=equation().probleme().milieu();
  diviser_par_rho_si_qc(la_diffusivite_turbulente.valeurs(),mil);
  const Turbulence_paroi_scal& lp = loi_paroi();
  if (lp.non_nul())
    {
      loipar->calculer_scal(la_diffusivite_turbulente);
    }
  const Probleme_base& mon_pb = mon_equation->probleme();
  if (sub_type(Pb_Thermohydraulique_Turbulent_QC,mon_pb))
    {
      DoubleTab& alpha_t = la_diffusivite_turbulente.valeurs();
      const DoubleTab& tab_Cp = mon_pb.milieu().capacite_calorifique().valeurs();
      double cp=-1;
      if (tab_Cp.nb_dim()==2)
        {
          cp=tab_Cp(0,0);
          alpha_t*=cp;
        }
      else
        {
          int taille=alpha_t.size();
          for (int i=0; i<taille; i++)
            alpha_t(i)*=tab_Cp(i);
        }
      multiplier_par_rho_si_qc(alpha_t,mil);
    }
  la_diffusivite_turbulente->valeurs().echange_espace_virtuel();
}


// Description:
//    Calcule la diffusivite turbulente.
//    diffusivite_turbulente = viscosite_turbulente / Prdt_turbulent
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Fonc&
//    Signification: la diffusivite turbulente nouvellement calculee
//    Contraintes:
// Exception: les champs diffusivite_turbulente et viscosite_turbulente
//            doivent avoir le meme nombre de valeurs nodales
// Effets de bord:
// Postcondition:
Champ_Fonc& Modele_turbulence_scal_Prandtl::calculer_diffusivite_turbulente()
{
  DoubleTab& alpha_t = la_diffusivite_turbulente.valeurs();
  const DoubleTab& mu_t = la_viscosite_turbulente->valeurs();
  double temps = la_viscosite_turbulente->temps();

  Zone& zone = mon_equation->zone_dis().zone();
  DoubleTab xp;
  zone.calculer_centres_gravite(xp);

  // if (temps != la_diffusivite_turbulente.temps())
  {
    int n= alpha_t.size();
    if (mu_t.size() != n)
      {
        Cerr << "Les DoubleTab des champs diffusivite_turbulente et viscosite_turbulente" << finl;
        Cerr << "doivent avoir le meme nombre de valeurs nodales" << finl;
        exit();
      }


    if (definition_fonction!=Nom())
      {
        // modif VB pour utiliser l'equation qui approche Yakhot : LePrdt = 0.7/Pe-t + 0.85
        // Pe-t est un nombre de Peclet turbulent defini par Pr*(nut/nu)
        // On a alors alpha_t = nut * nut / ( 0.7 alpha + 0.85 nut )

        const Milieu_base& milieu=mon_equation.valeur().milieu();
        const Champ_Don& alpha = milieu.diffusivite();
        if (!alpha.non_nul())
          {
            // GF si cette condition est bloquante, on peut ameliorer en
            // tentant de creer un parser sans la variable alpha, si ok on peut dire que alpha existe , est constant et vaut 1.
            Cerr<<"Erreur dans Modele_turbulence_scal_prandt, l'option Prandt_turbulent_fonction_nu_t_alpha n'est disponible que pour des milieux ayant defini la diffusivite"<<finl;
            exit();
          }
        double d_alpha=0.;
        int is_alpha_unif=sub_type(Champ_Uniforme,alpha.valeur());
        if (is_alpha_unif)
          {
            d_alpha = alpha(0,0);
            fonction.setVar("alpha",d_alpha);
          }
        for (int i=0; i<n; i++)
          {
            if (!is_alpha_unif)
              fonction.setVar("alpha",alpha(i));
            fonction.setVar("nu_t",mu_t[i]);

            alpha_t[i] = fonction.eval();
          }
      }
    else
      for (int i=0; i<n; i++)
        {
          if (LePrdt_fct!=Nom())
            {
              double x = xp(i,0);
              double y = xp(i,1);
              double z = 0;
              if (xp.nb_dim()==3)
                {
                  z = xp(i,2);
                }
              fonction1.setVar("x",x);
              fonction1.setVar("y",y);
              fonction1.setVar("z",z);
              double NbPrandtlCell = fonction1.eval();
              alpha_t[i] = mu_t[i]/NbPrandtlCell;
            }
          else
            {
              alpha_t[i] = mu_t[i]/LePrdt;
            }
        }

    la_diffusivite_turbulente.changer_temps(temps);
  }

  return la_diffusivite_turbulente;
}

