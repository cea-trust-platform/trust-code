/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Echange_global_impose.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Zone_Cl_dis_base.h>
#include <Equation_base.h>

Implemente_instanciable_sans_constructeur(Echange_global_impose,"Paroi_echange_global_impose",Echange_impose_base);

Echange_global_impose::Echange_global_impose()
{
  phi_ext_lu_ = false;
}
// Postcondition:
// Description:
//    Ecrit le type de l'objet sur un flot de sortie
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
Sortie& Echange_global_impose::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//    Simple appel a Echange_impose_base::readOn(Entree&)
//    Lit les specifications des conditions aux limites
//    a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Echange_global_impose::readOn(Entree& s )
{
  phi_ext_lu_ = false;

  // TODO : optional reading of phi_ext
  Echange_impose_base::readOn(s);
  return s ;
}

// Compatible with all discretisations
int Echange_global_impose::compatible_avec_discr(const Discretisation_base& discr) const
{
  return 1;
}

void Echange_global_impose::changer_temps_futur(double temps, int i)
{
  Echange_impose_base::changer_temps_futur(temps,i);
  if (phi_ext_lu_)
    {
      derivee_phi_ext_->changer_temps_futur(temps, i);
      phi_ext_->changer_temps_futur(temps, i);
    }
}

int Echange_global_impose::avancer(double temps)
{
  if (phi_ext_lu_)
    {
      derivee_phi_ext_->avancer(temps);
      phi_ext_->avancer(temps);
    }
  return  Echange_impose_base::avancer(temps);
}

int Echange_global_impose::reculer(double temps)
{
  if (phi_ext_lu_)
    {
      derivee_phi_ext_->reculer(temps);
      phi_ext_->reculer(temps);
    }
  return  Echange_impose_base::reculer(temps);
}

void Echange_global_impose::set_temps_defaut(double temps)
{
  Echange_impose_base::set_temps_defaut(temps);
  if (phi_ext_lu_)
    {
      derivee_phi_ext_->set_temps_defaut(temps);
      phi_ext_->set_temps_defaut(temps);
    }
}

void Echange_global_impose::mettre_a_jour(double temps)
{
  Echange_impose_base::mettre_a_jour(temps);
  if (phi_ext_lu_)
    {
      derivee_phi_ext_->mettre_a_jour(temps);
      phi_ext_->mettre_a_jour(temps);
    }
}

int Echange_global_impose::initialiser(double temps)
{
  Echange_impose_base::initialiser(temps);

  if (phi_ext_lu_)
    {
      derivee_phi_ext_->initialiser(temps, zone_Cl_dis().inconnue());
      phi_ext_->initialiser(temps, zone_Cl_dis().inconnue());
      DoubleTab& vals_derphi = derivee_phi_ext_->valeurs();
      vals_derphi = 0.;
      DoubleTab& vals_phi = phi_ext_->valeurs();
      vals_phi = 0.;
    }
  return 1;
}


double Echange_global_impose::champ_exterieur(int i, const Champ_front& champ_ext) const
{
  if (ma_zone_cl_dis->equation().que_suis_je()!="Convection_Diffusion_Concentration")
    {
      const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
      Nom nom_pb=ma_zone_cl_dis->equation().probleme().que_suis_je();
      double d_Cp;
      double d_rho;
      const Champ_base& rho=mil.masse_volumique().valeur();
      if ((nom_pb.debute_par("Probleme_Interface")|| nom_pb==Nom("Probleme_Thermo_Front_Tracking"))||!sub_type(Champ_Uniforme,rho)||nom_pb==Nom("Pb_Conduction"))
        {
          // Pour le front tracking, on ne divise pas par Rho*Cp
          // ni pour QC
          d_rho=1.;
          d_Cp=1.;
        }
      else
        {
          const Champ_Don& Cp =mil.capacite_calorifique();

          if (sub_type(Champ_Uniforme,rho))
            {
              d_rho= rho(0,0);
            }
          else
            {
              d_rho= rho(i);
            }
          if (sub_type(Champ_Uniforme,Cp.valeur()))
            {
              d_Cp= Cp(0,0);
            }
          else
            {
              d_Cp= Cp.valeur()(i);
            }
        }

      if (champ_ext.valeurs().size()==1)
        return champ_ext(0,0)/(d_rho*d_Cp);
      else if (champ_ext.valeurs().dimension(1)==1)
        return champ_ext(i,0)/(d_rho*d_Cp);
      else
        Cerr << "Echange_global_impose::flux_impose erreur" << finl;
      exit();
      return 0.;
    }
  else
    {
      if (champ_ext.valeurs().size()==1)
        return champ_ext(0,0);
      else if (champ_ext.valeurs().dimension(1)==1)
        return champ_ext(i,0);
      else
        Cerr << "Echange_global_impose::flux_impose erreur" << finl;
      exit();
      return 0.;

    }
}

double Echange_global_impose::champ_exterieur(int i,int j, const Champ_front& champ_ext) const
{
  if (ma_zone_cl_dis->equation().que_suis_je()!="Convection_Diffusion_Concentration")
    {
      const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
      Nom nom_pb=ma_zone_cl_dis->equation().probleme().que_suis_je();
      double d_Cp;
      double d_rho;
      const Champ_base& rho=mil.masse_volumique().valeur();
      if ((nom_pb.debute_par("Probleme_Interface")|| nom_pb==Nom("Probleme_Thermo_Front_Tracking"))||!sub_type(Champ_Uniforme,rho)||nom_pb==Nom("Pb_Conduction"))
        {
          // Pour le front tracking, on ne divise pas par Rho*Cp
          // ni pour QC
          d_rho=1.;
          d_Cp=1.;
        }
      else
        {
          const Champ_Don& Cp =mil.capacite_calorifique();

          if (sub_type(Champ_Uniforme,rho))
            {
              d_rho= rho(0,0);
            }
          else
            {
              d_rho= rho(i);
            }
          if (sub_type(Champ_Uniforme,Cp.valeur()))
            {
              d_Cp= Cp(0,0);
            }
          else
            {
              d_Cp= Cp.valeur()(i);
            }
        }
      if (champ_ext.valeurs().dimension(0)==1)
        return champ_ext(0,j)/(d_rho*d_Cp);
      else
        return champ_ext(i,j)/(d_rho*d_Cp);
    }
  else
    {
      if (champ_ext.valeurs().dimension(0)==1)
        return champ_ext(0,j);
      else
        return champ_ext(i,j);
    }
}

void Echange_global_impose::completer()
{
  Echange_impose_base::completer();
  if (phi_ext_lu_)
    {
      derivee_phi_ext_.typer("Ch_front_var_instationnaire_dep");
      derivee_phi_ext_->fixer_nb_comp(1);
      derivee_phi_ext_->associer_fr_dis_base(frontiere_dis());
      derivee_phi_ext_->fixer_nb_valeurs_temporelles(3);

      phi_ext_.typer("Ch_front_var_instationnaire_dep");
      phi_ext_->fixer_nb_comp(1);
      phi_ext_->associer_fr_dis_base(frontiere_dis());
      phi_ext_->fixer_nb_valeurs_temporelles(3);
    }
}

double Echange_global_impose::derivee_flux_exterieur_imposee(int i,int j) const
{
  return champ_exterieur(i,j,derivee_phi_ext());
}

double Echange_global_impose::derivee_flux_exterieur_imposee(int i) const
{
  return champ_exterieur(i,derivee_phi_ext());
}

double Echange_global_impose::flux_exterieur_impose(int i,int j) const
{
  return champ_exterieur(i,j,phi_ext());
}

double Echange_global_impose::flux_exterieur_impose(int i) const
{
  return champ_exterieur(i,phi_ext());
}
