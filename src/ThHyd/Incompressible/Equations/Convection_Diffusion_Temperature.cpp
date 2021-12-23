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
// File:        Convection_Diffusion_Temperature.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Equations
// Version:     /main/48
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Temperature.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Fluide_Ostwald.h>
#include <Discret_Thyd.h>
#include <Frontiere_dis_base.h>
#include <Param.h>

#include <Transport_Interfaces_base.h>
#include <Fluide_base.h>
#include <DoubleTrav.h>
#include <IntTrav.h>
#include <SFichier.h>
#include <Zone_VF.h>
#include <Matrice_Morse.h>
#include <Champ_Uniforme.h>
#include <Matrice_Morse_Sym.h>
#include <Statistiques.h>

extern Stat_Counter_Id assemblage_sys_counter_;

Implemente_instanciable_sans_constructeur(Convection_Diffusion_Temperature,"Convection_Diffusion_Temperature",Convection_Diffusion_std);
Implemente_vect(RefObjU);

Convection_Diffusion_Temperature::Convection_Diffusion_Temperature()
{
//  champs_compris_.ajoute_nom_compris("temperature_paroi");
  champs_compris_.ajoute_nom_compris("gradient_temperature");
  champs_compris_.ajoute_nom_compris("h_echange_");
  champs_compris_.ajoute_nom_compris("temperature_residu");
  eta = 1.0;
  is_penalized = 0;
  tag_indic_pena_global = -1;
  indic_pena_global = 0;
  indic_face_pena_global = 0;
  choix_pena = 0;
}
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
Sortie& Convection_Diffusion_Temperature::printOn(Sortie& is) const
{
  return Convection_Diffusion_std::printOn(is);
}

// Description:
//    Verifie si l'equation a une temperature et un fluide associe
//    cf Convection_Diffusion_std::readOn(Entree&).
// Precondition: l'objet a une temperature associee
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
Entree& Convection_Diffusion_Temperature::readOn(Entree& is)
{
  assert(la_temperature.non_nul());
  assert(le_fluide.non_nul());
  Convection_Diffusion_std::readOn(is);
  //Nom unite;
  //if (dimension+bidim_axi==2) unite="[W/m]";
  //else unite="[W]";
  Nom num=inconnue().le_nom(); // On prevoir le cas d'equation de scalaires passifs
  num.suffix("temperature");
  Nom nom="Convection_chaleur";
  nom+=num;
  terme_convectif.set_fichier(nom);
  //terme_convectif.set_description((Nom)"Convective heat transfer rate=Integral(-rho*cp*T*u*ndS) "+unite);
  terme_convectif.set_description((Nom)"Convective heat transfer rate=Integral(-rho*cp*T*u*ndS) [W] if SI units used");
  nom="Diffusion_chaleur";
  nom+=num;
  terme_diffusif.set_fichier(nom);
  //terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) "+unite);
  terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) [W] if SI units used");
  solveur_masse->set_name_of_coefficient_temporel("rho_cp_comme_T");
  return is;
}

void Convection_Diffusion_Temperature::set_param(Param& param)
{
  Convection_Diffusion_std::set_param(param);
  param.ajouter_non_std("penalisation_L2_FTD",(this));
}

int Convection_Diffusion_Temperature::lire_motcle_non_standard(const Motcle& un_mot, Entree& is)
{
  if (un_mot=="penalisation_L2_FTD")
    {
      is_penalized = 1;
      eta = 1.e-12;
      Cerr << " Equation_base :: penalisation_L2_FTD" << finl;
      Cerr << "Equation_base: modele penalisation ibc = "<< choix_pena << finl;
      Nom mot, nom_equation;
      int dim;
      is >> mot;
      Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
      if (mot == accolade_ouverte)
        {
          is >> mot;
          while (mot != accolade_fermee)
            {
              nom_equation = mot;

              Cerr << "penalisation_L2_FTD equation: "<< nom_equation << finl;
              const Transport_Interfaces_base& eq = ref_cast(Transport_Interfaces_base,probleme().get_equation_by_name(nom_equation));
              ref_penalisation_L2_FTD.add(eq);

              is >> dim;
              DoubleTab tab = DoubleTab();
              tab.resize(dim);
              for ( int i = 0 ; i<dim ; i++)
                {
                  is >> tab(i);
                  Cerr << "Lecture de la valeur de la penalisation suivant la direction " << i+1 <<" :" << tab(i) << finl;
                }
              tab_penalisation_L2_FTD.add(tab);
              is >> mot ;
            }
        }
      else
        {
          Cerr << "Erreur a la lecture des parametres de la penalisation L2 " << finl;
          Cerr << "On attendait : " << accolade_ouverte << finl;
          exit();
        }
      const Zone_VF& zone_vf = ref_cast(Zone_VF, zone_dis().valeur());
      zone_vf.zone().creer_tableau_elements(indic_pena_global);
      zone_vf.creer_tableau_faces(indic_face_pena_global);
    }
  else
    return Convection_Diffusion_std::lire_motcle_non_standard(un_mot,is);
  return 1;
}

// Description:
//    Associe un milieu physique a l'equation,
//    le milieu est en fait caste en Fluide_base ou en Fluide_Ostwald.
// Precondition:
// Parametre: Milieu_base& un_milieu
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//                 doit pourvoir etre force au type "Fluide_base"
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: les proprietes physiques du fluide ne sont pas toutes specifiees
// Effets de bord:
// Postcondition:
void Convection_Diffusion_Temperature::associer_milieu_base(const Milieu_base& un_milieu)
{
  if (sub_type(Fluide_Ostwald, un_milieu))
    {
      const Fluide_Ostwald& un_fluide = ref_cast(Fluide_Ostwald,un_milieu);
      if  ((un_fluide.conductivite().non_nul()) &&
           ((un_fluide.capacite_calorifique().non_nul()) && (un_fluide.beta_t().non_nul())))
        associer_fluide(un_fluide);
      else
        {
          Cerr << "Vous n'avez pas defini toutes les proprietes physiques du fluide " << finl;
          Cerr << "necessaires pour resoudre l'equation d'energie " << finl;
          Cerr << "Verifier que vous avez defini: la conductivite (lambda)"<< finl;
          Cerr << "                                  la capacite calorifique (Cp)"<< finl;
          Cerr << "                                  le coefficient de dilatation thermique (beta_th)"<< finl;
          exit();
        }
    }
  else if (sub_type(Fluide_base,un_milieu))
    {
      const Fluide_base& un_fluide = ref_cast(Fluide_base,un_milieu);
      if  ((un_fluide.conductivite().non_nul()) &&
           ((un_fluide.capacite_calorifique().non_nul()) && (un_fluide.beta_t().non_nul())))
        associer_fluide(un_fluide);
      else
        {
          Cerr << "Vous n'avez pas defini toutes les proprietes physiques du fluide " << finl;
          Cerr << "necessaires pour resoudre l'equation d'energie " << finl;
          Cerr << "Verifier que vous avez defini: la conductivite (lambda)"<< finl;
          Cerr << "                                  la capacite calorifique (Cp)"<< finl;
          Cerr << "                                  le coefficient de dilatation thermique (beta_th)"<< finl;

          exit();
        }
    }
  else
    {
      if (Process::je_suis_maitre())
        Cerr << "le fluide considere n'est un sous fluide de Fluide_base, \n"
             << "ni un sous fluide de Fluide_Ostwald. " << finl;
    }
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
void Convection_Diffusion_Temperature::discretiser()
{
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  if (Process::je_suis_maitre())
    Cerr << "Energy equation discretization" << finl;
  dis.temperature(schema_temps(), zone_dis(), la_temperature);
  champs_compris_.ajoute_champ(la_temperature);

  Equation_base::discretiser();
  if (Process::je_suis_maitre())
    Cerr << "Convection_Diffusion_Temperature::discretiser() ok" << finl;
}


// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Fluide_base upcaste en Milieu_base)
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Fluide_base upcaste en Milieu_base
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Milieu_base& Convection_Diffusion_Temperature::milieu() const
{
  return fluide();
}


// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Fluide_base upcaste en Milieu_base)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Fluide_base upcaste en Milieu_base
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Milieu_base& Convection_Diffusion_Temperature::milieu()
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
// Retour: Fluide_base&
//    Signification: le fluide incompressible associe a l'equation
//    Contraintes: reference constante
// Exception: pas de fluide associe a l'eqaution
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Fluide_base& Convection_Diffusion_Temperature::fluide() const
{
  if(!le_fluide.non_nul())
    {
      Cerr << "A fluid has not been associated to "
           << "the Convection_Diffusion_Temperature equation" << finl;
      exit();
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
// Retour: Fluide_base&
//    Signification: le fluide incompressible associe a l'equation
//    Contraintes:
// Exception: pas de fluide associe a l'eqaution
// Effets de bord:
// Postcondition:
Fluide_base& Convection_Diffusion_Temperature::fluide()
{
  if(!le_fluide.non_nul())
    {
      Cerr << "A fluid has not been associated to"
           << "the Convection_Diffusion_Temperature equation" << finl;
      exit();
    }
  return le_fluide.valeur();
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
int Convection_Diffusion_Temperature::impr(Sortie& os) const
{
  return Equation_base::impr(os);
}



inline int string2int(const char* digit, int& result)
{
  result = 0;

  //--- Convert each digit char and add into result.
  while (*digit >= '0' && *digit <='9')
    {
      result = (result * 10) + (*digit - '0');
      digit++;
    }

  //--- Check that there were no non-digits at end.
  if (*digit != 0)
    {
      return 0;
    }

  return 1;
}

void Convection_Diffusion_Temperature::creer_champ(const Motcle& motlu)
{
  Convection_Diffusion_std::creer_champ(motlu);

  Motcle nom_mot(motlu),temp_mot(nom_mot);
  /*  if (motlu == "temperature_paroi")
      {
        if (!temperature_paroi.non_nul())
          {
            const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
            dis.t_paroi(zone_dis(),zone_Cl_dis(),*this,temperature_paroi);
            champs_compris_.ajoute_champ(temperature_paroi);
          }
      } */
  if (motlu == "gradient_temperature")
    {
      if (!gradient_temperature.non_nul())
        {
          const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
          dis.grad_T(zone_dis(),zone_Cl_dis(),la_temperature,gradient_temperature);
          champs_compris_.ajoute_champ(gradient_temperature);
        }
    }
  if (nom_mot.debute_par("H_ECHANGE"))
    {
      if (!h_echange.non_nul())
        {
          const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
          temp_mot.suffix("H_ECHANGE_");
          int temperature;
          string2int(temp_mot,temperature);
          dis.h_conv(zone_dis(),zone_Cl_dis(),la_temperature,h_echange,nom_mot,temperature);
          champs_compris_.ajoute_champ(h_echange);
        }
    }
}

const Champ_base& Convection_Diffusion_Temperature::get_champ(const Motcle& nom) const
{


  /*  if (nom=="temperature_paroi")
      {
        double temps_init = schema_temps().temps_init();
        Champ_Fonc_base& ch_tp=ref_cast_non_const(Champ_Fonc_base,temperature_paroi.valeur());
        if (((ch_tp.temps()!=la_temperature->temps()) || (ch_tp.temps()==temps_init)) && ((la_temperature->mon_equation_non_nul())))
          ch_tp.mettre_a_jour(la_temperature->temps());
        return champs_compris_.get_champ(nom);
      } */
  if (nom=="gradient_temperature")
    {
      double temps_init = schema_temps().temps_init();
      Champ_Fonc_base& ch_gt=ref_cast_non_const(Champ_Fonc_base,gradient_temperature.valeur());
      if (((ch_gt.temps()!=la_temperature->temps()) || (ch_gt.temps()==temps_init)) && (la_temperature->mon_equation_non_nul()))
        ch_gt.mettre_a_jour(la_temperature->temps());
      return champs_compris_.get_champ(nom);
    }
  if (h_echange.non_nul())
    if (nom==h_echange.valeur().le_nom())
      {
        double temps_init = schema_temps().temps_init();
        Champ_Fonc_base& ch_hconv=ref_cast_non_const(Champ_Fonc_base,h_echange.valeur());
        if (((ch_hconv.temps()!=la_temperature->temps()) || (ch_hconv.temps()==temps_init)) && (la_temperature->mon_equation_non_nul()))
          {
            ch_hconv.mettre_a_jour(la_temperature->temps());

          }
        return champs_compris_.get_champ(nom);
      }

  try
    {
      return Convection_Diffusion_std::get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }

  throw Champs_compris_erreur();
  REF(Champ_base) ref_champ;

  return ref_champ;
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
const Motcle& Convection_Diffusion_Temperature::domaine_application() const
{
  static Motcle domaine = "Thermique";
  return domaine;
}

DoubleTab& Convection_Diffusion_Temperature::derivee_en_temps_inco(DoubleTab& derivee)
{
  if (!is_penalized)
    {
      return derivee_en_temps_inco_eq_base(derivee);
    }
  else
    {
      const double rhoCp = le_fluide->capacite_calorifique().valeurs()(0, 0) * le_fluide->masse_volumique().valeurs()(0, 0);
      // Specific code if temperature equation is penalized
      derivee=inconnue().valeurs();
      //   Mise en place d'une methode de mise en place d'une zone fantome
      mise_en_place_zone_fantome(derivee);
      DoubleTab& inc=inconnue().valeurs();
      inc = derivee;

      DoubleTrav secmem(derivee);

      if (nombre_d_operateurs()>1)
        {
          derivee_en_temps_conv(secmem, derivee);
          secmem *= rhoCp;
        }

      // Transport des ibcs et de la couche limite
      DoubleTrav secmem_conv_vr(derivee);
      //on advecte la variable avec la vitesse ibc
      transport_ibc(secmem_conv_vr, derivee);
      //on retranche cette advection
      secmem -= secmem_conv_vr;

      DoubleTab derivee_conv;
      derivee_conv.copy(secmem, Array_base::COPY_INIT);

      les_sources.ajouter(secmem);

      filtrage_si_appart_ibc(derivee_conv,secmem);
      secmem.echange_espace_virtuel();

      if (calculate_time_derivative())
        {
          // Store dI/dt(n) = M-1 secmem :
          derivee_en_temps().valeurs()=secmem;
          solveur_masse.appliquer(derivee_en_temps().valeurs());
          schema_temps().modifier_second_membre((*this),secmem); // Change secmem for some schemes (eg: Adams_Bashforth)
        }

      solveur_masse.appliquer(secmem);
      Equation_base::Gradient_conjugue_diff_impl(secmem, derivee);

      //  penalisation de la temperature
      penalisation_L2(derivee);
      return derivee;
    }
}

DoubleTab& Convection_Diffusion_Temperature::derivee_en_temps_inco_eq_base(DoubleTab& derivee)
{
  derivee=0;
  DoubleTrav secmem(derivee);

  const double rhoCp = le_fluide->capacite_calorifique().valeurs()(0, 0) * le_fluide->masse_volumique().valeurs()(0, 0);

  // secmem = sum(operators) + sources + equation specific terms
  if (schema_temps().diffusion_implicite())
    {
      // Add convection operator only if equation has one
      derivee = inconnue().valeurs();
      if (nombre_d_operateurs()>1)
        {
          derivee_en_temps_conv(secmem, derivee);
          secmem *= rhoCp;
        }
    }
  else
    {
      // Add all explicit operators
      for(int i=0; i<nombre_d_operateurs(); i++)
        if(operateur(i).l_op_base().get_decal_temps()!=1)
          {
            DoubleTrav secmem_tmp(secmem);
            operateur(i).ajouter(secmem_tmp);
            if (i == 1) secmem_tmp *= rhoCp;
            secmem += secmem_tmp;
          }
    }
  les_sources.ajouter(secmem);

  if (calculate_time_derivative())
    {
      // Store dI/dt(n) = M-1 secmem :
      derivee_en_temps().valeurs()=secmem;
      solveur_masse.appliquer(derivee_en_temps().valeurs());
      schema_temps().modifier_second_membre((*this),secmem); // Change secmem for some schemes (eg: Adams_Bashforth)
    }

  corriger_derivee_expl(secmem); // Add specific term for an equation (eg: -gradP for Navier Stokes)

  if (implicite_==0)
    {
      solveur_masse.appliquer(secmem); // M-1 * secmem
      if (schema_temps().diffusion_implicite())
        {
          // Solve: (1/dt + M-1*A)*dI = M-1 * secmem
          // where A is the diffusion
          Equation_base::Gradient_conjugue_diff_impl(secmem, derivee);
        }
      else
        {
          derivee = secmem;
          derivee.echange_espace_virtuel();
        }
      corriger_derivee_impl(derivee);  // Solve specific implicit term for an equation (eg: pressure for Navier Stokes)
    }
  else if (implicite_>0)
    {
      // TRUST support notices that this part has never been covered...
      //implicite
      // M dU/dt + AU* = f -BUn;
      // U* = Un+dt dU/dt
      // (M/dt + A) U* = f -BUn + M/dt Un
      //
      double dt=schema_temps().pas_de_temps();
      for(int i=0; i<nombre_d_operateurs(); i++)
        {
          //boucle sur les operateurs
          Operateur_base& op=operateur(i).l_op_base();
          if(!op.get_matrice().non_nul())
            op.set_matrice().typer("Matrice_Morse");
          if(op.get_decal_temps()==1)
            {
              //if (op.set_matrice().valeur().nb_lignes()<2)
              {
                Matrice_Morse& matrice=ref_cast(Matrice_Morse,op.set_matrice().valeur());
                op.dimensionner(matrice);
                sys_invariant_=0;
              }
              if(!sys_invariant_)
                {
                  Matrice_Morse& matrice=ref_cast(Matrice_Morse, op.set_matrice().valeur());
                  op.contribuer_a_avec(inconnue().valeurs(), matrice);
                  solv_masse().ajouter_masse(dt, op.set_matrice().valeur());

                  if(
                    (op.get_solveur()->que_suis_je()=="Solv_Cholesky")
                    ||
                    (op.get_solveur()->que_suis_je()=="Solv_GCP")
                  )
                    {
                      Matrice_Morse_Sym new_mat(matrice);
                      new_mat.set_est_definie(1);
                      op.set_matrice()=new_mat;
                      ref_cast_non_const(SolveurSys,op.get_solveur())->reinit();
                    }
                }
            }
        }
      if(implicite_==1)
        {
          // Un seul operateur implicite.
          // On suppose que c'est le premier (la diffusion !!)
          Operateur_base& op=operateur(0).l_op_base();
          Matrice_Base& matrice=op.set_matrice().valeur();
          // DoubleTrav secmem(derivee);
          secmem=derivee;
          solv_masse().ajouter_masse(dt, secmem, inconnue().valeurs());
          op.contribuer_au_second_membre(secmem );
          op.set_solveur().resoudre_systeme(matrice,
                                            secmem,
                                            derivee
                                           );
          solv_masse().corriger_solution(derivee,inconnue().valeurs());

          derivee-=inconnue().valeurs();
          derivee/=dt;

          //Sert uniquement a calculer les flux sur les bords quand la diffusion est implicitee !
          DoubleTab resu;
          resu=derivee;
          operateur(0).calculer(inconnue().valeurs(), resu);
        }
      else
        {
          // plusieurs operateurs implicites ...
          Cerr << "Must be coded ... " << finl;
          exit();
        }
    }
  else
    {
      Cerr << "Error in Equation_base::derivee_en_temps_inco" << finl;
      Cerr << "implicite_ = " << implicite_ << " has not been initialized!" << finl;
      Cerr << "May be " << que_suis_je() << "::completer() method doesn't call Equation_base::completer()" << finl;
      Cerr << "Contact TRUST support." << finl;
      Process::exit();
    }
  return derivee;
}

// ajoute les contributions des operateurs et des sources
void Convection_Diffusion_Temperature::assembler(Matrice_Morse& matrice, const DoubleTab& inco, DoubleTab& resu)
{
  const double rhoCp = le_fluide->capacite_calorifique().valeurs()(0, 0) * le_fluide->masse_volumique().valeurs()(0, 0);

  // Test de verification de la methode contribuer_a_avec
  for (int op=0; op<nombre_d_operateurs(); op++)
    operateur(op).l_op_base().tester_contribuer_a_avec(inco, matrice);

  // Contribution des operateurs et des sources:
  // [Vol/dt+A]Inco(n+1)=somme(residu)+Vol/dt*Inco(n)
  // Typiquement: si Op=flux(Inco) alors la matrice implicite A contient une contribution -dflux/dInco
  // Exemple: Op flux convectif en VDF:
  // Op=T*u*S et A=-d(T*u*S)/dT=-u*S
  const Discretisation_base::type_calcul_du_residu& type_codage=probleme().discretisation().codage_du_calcul_du_residu();
  if (type_codage==Discretisation_base::VIA_CONTRIBUER_AU_SECOND_MEMBRE)
    {
      Cerr << "VIA_CONTRIBUER_AU_SECOND_MEMBRE pas code pour " << que_suis_je() << ":assembler" << finl;
      Process::exit();
      // // On calcule somme(residu) par contribuer_au_second_membre (typiquement CL non implicitees)
      // // Cette approche necessite de coder 3 methodes (contribuer_a_avec, contribuer_au_second_membre et ajouter pour l'explicite)
      // sources().contribuer_a_avec(inco,matrice);
      // statistiques().end_count(assemblage_sys_counter_,0,0);
      // sources().ajouter(resu);
      // statistiques().begin_count(assemblage_sys_counter_);
      // matrice.ajouter_multvect(inco, resu); // Add source residual first
      // for (int op = 0; op < nombre_d_operateurs(); op++)
      //   {
      //     operateur(op).l_op_base().contribuer_a_avec(inco, matrice);
      //     operateur(op).l_op_base().contribuer_au_second_membre(resu);
      //   }
    }
  else if (type_codage==Discretisation_base::VIA_AJOUTER)
    {
      // On calcule somme(residu) par somme(operateur)+sources+A*Inco(n)
      // Cette approche necessite de coder seulement deux methodes (contribuer_a_avec et ajouter)
      // Donc un peu plus couteux en temps de calcul mais moins de code a ecrire/maintenir
      for (int op=0; op<nombre_d_operateurs(); op++)
        {
          Matrice_Morse mat(matrice);
          mat.get_set_coeff() = 0.0;
          operateur(op).l_op_base().contribuer_a_avec(inco, mat);
          if (op == 1) mat *= rhoCp; // la derivee est multipliee par rhoCp pour la convection
          matrice += mat;
          statistiques().end_count(assemblage_sys_counter_, 0, 0);
          {
            DoubleTab resu_tmp(resu);
            resu_tmp = 0.;
            operateur(op).ajouter(inco, resu_tmp);
            if (op == 1) resu_tmp *= rhoCp;
            resu += resu_tmp;
          }
          statistiques().begin_count(assemblage_sys_counter_);
        }
      sources().contribuer_a_avec(inco,matrice);
      statistiques().end_count(assemblage_sys_counter_,0,0);
      sources().ajouter(resu);
      statistiques().begin_count(assemblage_sys_counter_);
      matrice.ajouter_multvect(inco, resu); // Ajout de A*Inco(n)
      // PL (11/04/2018): On aimerait bien calculer la contribution des sources en premier
      // comme dans le cas VIA_CONTRIBUER_AU_SECOND_MEMBRE mais le cas Canal_perio_3D (keps
      // periodique plante: il y'a une erreur de periodicite dans les termes sources du modele KEps...
    }
  else
    {
      Cerr << "Unknown value in Equation_base::assembler for " << (int)type_codage << finl;
      Process::exit();
    }
}

int Convection_Diffusion_Temperature::verifier_tag_indicatrice_pena_glob()
{
  //Les ibcs ont elles ete modifiees ?
  int maj = 0;
  int tag_all = 0;
  for ( int w = 0; w<ref_penalisation_L2_FTD.size() ; ++w)
    {
      Transport_Interfaces_base& nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[w].valeur());
      tag_all += nom_eq.get_mesh_tag();
    }
  if (tag_all == tag_indic_pena_global) maj = 1;
  return maj;
}

int Convection_Diffusion_Temperature::mise_a_jour_tag_indicatrice_pena_glob()
{
  //Les ibcs ont elles ete modifiees ?
  int maj = 0;
  int tag_all = 0;
  for ( int w = 0; w<ref_penalisation_L2_FTD.size() ; ++w)
    {
      Transport_Interfaces_base& nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[w].valeur());
      tag_all += nom_eq.get_mesh_tag();
    }
  if (tag_all != tag_indic_pena_global)
    {
      tag_indic_pena_global = tag_all;
      maj = 1;
    }
  return maj;
}

void Convection_Diffusion_Temperature::set_indic_pena_globale()
{
  //  Set de la fonction characteristique ibc globlale (element et faces)
  int maj = verifier_tag_indicatrice_pena_glob();
  if (maj == 0)
    {
      calcul_indic_pena_global(indic_pena_global, indic_face_pena_global);
      maj = mise_a_jour_tag_indicatrice_pena_glob();
      assert(maj == 1);
    }
}

void Convection_Diffusion_Temperature::transport_ibc(DoubleTrav& secmem_conv_vr, DoubleTab& inco_conv_vr)
{
  secmem_conv_vr = 0.;
  const double rhoCp = le_fluide->capacite_calorifique().valeurs()(0, 0) * le_fluide->masse_volumique().valeurs()(0, 0);

  //fonction characteristique globale ibc au temps courant
  set_indic_pena_globale();

  // on calcule l indicatrice epaisse
  const Zone_VF& zone_vf = ref_cast(Zone_VF, zone_dis().valeur());
  const IntTab& faces_elem = zone_vf.face_voisins();
  IntTrav indic_pena_global_fat(indic_pena_global);
  for (int i_face = 0; i_face < indic_face_pena_global.size(); i_face++)
    {
      if (indic_face_pena_global(i_face) > 0.)
        {
          const int voisin_0 = faces_elem(i_face, 0);
          if (voisin_0 >= 0) indic_pena_global_fat(voisin_0) = 1;
          const int voisin_1 = faces_elem(i_face, 1);
          if (voisin_1 >= 0) indic_pena_global_fat(voisin_1) = 1;
        }
    }
  // post-traitement particulier pour les coins
  const IntTab& elem_faces = zone_vf.elem_faces();
  const int nb_faces_elem = elem_faces.dimension(1);
  const int nb_elem  = indic_pena_global.dimension(0);
  for (int i_elem = 0; i_elem < nb_elem;  i_elem++)
    {
      if (indic_pena_global_fat(i_elem) == 0)
        {
          int coeff = 0;
          for (int i_face = 0; i_face < nb_faces_elem; i_face++)
            {
              const int face = elem_faces(i_elem, i_face);
              const int voisin = faces_elem(face, 0) + faces_elem(face, 1) - i_elem;
              if (voisin >= 0)
                {
                  if (indic_pena_global_fat(voisin) != 0)
                    {
                      ++coeff;
                    } ;
                }
            }
          if (coeff > 1) indic_pena_global_fat(i_elem) = 1;
        }
    }

  // Si on veut un traitement local a chaque indicatrice (necessaire si on utilise vitesse imposee)
  // IntTrav indic_pena_fat(indic_pena_global);
  // for (int w = 0; w < ref_penalisation_L2_FTD.size(); w++)
  //   {
  //     Transport_Interfaces_base & nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[w].valeur());
  //     // on calcul le filtre local a l ibc
  //     const DoubleTab & indicatrice_face = nom_eq.get_compute_indicatrice_faces().valeurs();
  //     indic_pena_fat = 0;
  //     for (int i_face = 0; i_face < indicatrice_face.size(); i_face++)
  //         {
  //           if (indicatrice_face(i_face) > 0.)
  //             {
  //               const int voisin_0 = faces_elem(i_face, 0);
  //               if (voisin_0 >= 0) indic_pena_fat(voisin_0) = 1;
  //               const int voisin_1 = faces_elem(i_face, 1);
  //               if (voisin_1 >= 0) indic_pena_fat(voisin_1) = 1;
  //             }
  //         }
  //     indic_pena_global_fat += indic_pena_fat;
  //   }

  // on convecte le champ de variable avec la vitesse fluide pour les faces ibc
  if ( nombre_d_operateurs() > 1 )
    {
      derivee_en_temps_conv(secmem_conv_vr, inco_conv_vr);
      secmem_conv_vr *= rhoCp;
    }

  //on ne garde la contribution que pour les cellules dont indicatrice fat est <> 0
  for (int i_elem = 0; i_elem < nb_elem;  i_elem++)
    {
      if (indic_pena_global_fat(i_elem) == 0) secmem_conv_vr(i_elem) = 0.;
    }
}

void Convection_Diffusion_Temperature::mise_en_place_zone_fantome(DoubleTab& solution)
{
  int maj = verifier_tag_indicatrice_pena_glob();
  if (maj == 1) return;

  // garde-fou
  if (schema_temps().facteur_securite_pas() > 1.0)
    {
      Cerr << "Convection_Diffusion_Temperature::mise_en_place_zone_fantome Facteur securite doit etre <= 1"<<finl;
      exit();
    }

  // fonction characteristique globale ibc penalisees actuelle
  IntTrav indicatrice_totale;
  IntTrav indicatrice_face_totale ;
  calcul_indic_pena_global(indicatrice_totale,indicatrice_face_totale);
  // etat des lieux actuel
  const int nb_elem  = indicatrice_totale.dimension(0);
  int x=0;
  int j=0;
  for (int k_elem =0 ; k_elem <nb_elem; ++k_elem)  ((indicatrice_totale(k_elem)!=0) ? ++x : ++j);

  //traitement cellules fantomes (if any)
  const Zone_VF& zone_vf = ref_cast(Zone_VF, zone_dis().valeur());
  const IntTab& elem_faces = zone_vf.elem_faces();
  const IntTab& faces_elem = zone_vf.face_voisins();
  const int nb_faces_elem = elem_faces.dimension(1);
  int k=0; //nb cellules fantomes ibc -> fluide
  int k_cor=0; //nb cellules fantomes (ibc -> fluide) corrigees
  int k_cor2=0; //nb cellules fantomes (fluide -> ibc) corrigees
  int u=0; //nb cellules fluide -> ibc
  for (int i_elem = 0; i_elem < nb_elem; i_elem++)
    {
      //cellules fantomes (actuellement fluide et precedement ibc) => on modifie solution
      double somme_inc = 0.;
      double coeff = 0.;
      if (indic_pena_global(i_elem) != 0 && indicatrice_totale(i_elem) == 0)
        {
          ++k;
          // boucle sur les faces pour determiner la valeur de la variable pour les cellules voisines strictement fluide
          IntTrav stok_vois(nb_faces_elem);
          for (int i_face = 0; i_face < nb_faces_elem; i_face++)
            {
              const int face = elem_faces(i_elem, i_face);
              const int voisin = faces_elem(face, 0) + faces_elem(face, 1) - i_elem;
              if (voisin >= 0) // le voisin existe
                {
                  if ((indic_pena_global(voisin) == indicatrice_totale(voisin)) &&  (indic_pena_global(voisin) == indicatrice_totale(i_elem))) // strictement fluide aux deux pas de temps
                    {
                      somme_inc += solution(voisin);
                      ++coeff;
                    }
                  else     // On sauve le numero du voisin
                    {
                      stok_vois(i_face) = voisin;
                    }
                }
            }
          // Si on n a pas trouve de cellules fluide, on regarde les voisins des voisins
          if (coeff == 0.)
            {
              for (int i_vois = 0; i_vois < nb_faces_elem; i_vois++)
                {
                  for (int ii_face = 0; ii_face < nb_faces_elem; ii_face++)
                    {
                      const int face_v = elem_faces(stok_vois(i_vois), ii_face);
                      const int voisin_v = faces_elem(face_v, 0) + faces_elem(face_v, 1) - stok_vois(i_vois);
                      if ((voisin_v >= 0) && (voisin_v != i_elem) )// le voisin existe et pas i_elem
                        {
                          if ((indic_pena_global(voisin_v) == indicatrice_totale(voisin_v)) &&  (indic_pena_global(voisin_v) == indicatrice_totale(i_elem))) // strictement fluide aux deux pas de temps
                            {
                              somme_inc += solution(voisin_v);
                              ++coeff;
                            }
                        }
                    }
                }
            }
          // definition d une nouvelle valleur de la variable pour la cellule fantome
          if (coeff > 0.)
            {
              solution(i_elem) = somme_inc / coeff;
              k_cor += 1;
            }
        }
      //cellules actuellement ibc et precedement fluide
      if (indic_pena_global(i_elem) == 0 && indicatrice_totale(i_elem) != 0)
        {
          ++u;
          // boucle sur les faces pour determiner la valeur de la variable pour les cellules voisines strictement ibc
          IntTrav stok_vois(nb_faces_elem);
          for (int i_face = 0; i_face < nb_faces_elem; i_face++)
            {
              const int face = elem_faces(i_elem, i_face);
              const int voisin = faces_elem(face, 0) + faces_elem(face, 1) - i_elem;
              if (voisin >= 0) // le voisin existe
                {
                  // strictement ibc aux deux pas de temps (la meme) car solution a ete modifiee ci-dessus
                  if ((indic_pena_global(voisin) == indicatrice_totale(voisin)) && (indic_pena_global(voisin) == indicatrice_totale(i_elem)))
                    {
                      somme_inc += solution(voisin);
                      ++coeff;
                    }
                  else     // On sauve le numero du voisin
                    {
                      stok_vois(i_face) = voisin;
                    }
                }
            }
          // Si on n a pas trouve de cellules ibc, on regarde les voisins des voisins
          if (coeff == 0.)
            {
              for (int i_vois = 0; i_vois < nb_faces_elem; i_vois++)
                {
                  for (int ii_face = 0; ii_face < nb_faces_elem; ii_face++)
                    {
                      const int face_v = elem_faces(stok_vois(i_vois), ii_face);
                      const int voisin_v = faces_elem(face_v, 0) + faces_elem(face_v, 1) - stok_vois(i_vois);
                      if ((voisin_v >= 0) && (voisin_v != i_elem) )// le voisin existe et pas i_elem
                        {
                          if ((indic_pena_global(voisin_v) == indicatrice_totale(voisin_v)) && (indic_pena_global(voisin_v) == indicatrice_totale(i_elem))) // strictement ibc aux deux pas de temps (la meme) car solution a ete modifiee ci-dessus
                            {
                              somme_inc += solution(voisin_v);
                              ++coeff;
                            }
                        }
                    }
                }
            }
          // definition d une nouvelle valleur de la variable pour la cellule fantome
          if (coeff > 0.)
            {
              solution(i_elem) = somme_inc / coeff;
              k_cor2 += 1;
            }
          else
            {
              solution(i_elem) = tab_penalisation_L2_FTD[indicatrice_totale(i_elem)-1](0);
              k_cor2 += 1;
            }
        }
    }
  solution.echange_espace_virtuel();
  //     Debog::verifier("Convection_Diffusion_Temperature::mise_en_place_zone_fantome solution ",solution);
  Cout <<"Mise_en_place_zone_fantome : fluide -> ibc : "<<u<< " elem. (dont "<<k_cor2<<" corrigees) et ibc -> fluide : ";
  Cout<<k<<" elem.(dont "<<k_cor<<" corrigees)"<<finl;
  Cout <<"             nb elem. ibc : "<<x<<" et nb elem fluide : "<<j<<" sur un total de : "<<nb_elem<<finl;
  if ((u != k_cor2) || (k !=k_cor))
    {
      Cerr <<"Mise_en_place_zone_fantome : Les cellules fantomes ne sont pas toutes corrigees"<<finl;
      exit();
    }

  indic_pena_global = indicatrice_totale;
  indic_face_pena_global = indicatrice_face_totale;
  maj = mise_a_jour_tag_indicatrice_pena_glob();
  assert(maj == 1);
}

DoubleTab& Convection_Diffusion_Temperature::filtrage_si_appart_ibc(DoubleTab& u_conv, DoubleTab& u)
{
  // caclul de la fonction caracteristique globale des ibc pour le calcul de T_voisinage
  set_indic_pena_globale();

  for (int j = 0 ; j< indic_pena_global.size() ; ++j)
    {
      if ( indic_pena_global(j) != 0) u(j) = u_conv(j);
    }
  u.echange_espace_virtuel();
  //   Debog::verifier("Convection_Diffusion_Temperature::filtrage_si_appart_ibc u ",u);
  return u;
}

void Convection_Diffusion_Temperature::calcul_indic_pena_global(IntTab& indicatrice_totale, IntTab& indicatrice_face_totale)
{
  // fonction characteristique globale ibc penalisees
  indicatrice_totale.copy(indic_pena_global);
  indicatrice_face_totale.copy(indic_face_pena_global);
  indicatrice_totale = 0;
  indicatrice_face_totale = 0;
  int x=0;
  int xf=0;
  const int nb_elem  = indicatrice_totale.dimension_tot(0);
  const int nfaces = indicatrice_face_totale.dimension_tot(0);
  // boucle sur les ibc
  for ( int w = 0 ; w<ref_penalisation_L2_FTD.size() ; ++w)
    {
      Transport_Interfaces_base& nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[w].valeur());
      const DoubleTab& indicatrice = nom_eq.get_update_indicatrice().valeurs();
      // fonction characteristique (0 ou 1) pour l'ensemble des ibc
      for (int k_elem =0 ; k_elem <nb_elem; ++k_elem)
        {
          if (indicatrice(k_elem)>0.)
            {
              if(indicatrice_totale(k_elem) != 0)
                {
                  Cerr<<"calcul_indic_pena_global : Attention: les elements des indicatrices se chevauchent "<<finl;
                  exit();
                }
              indicatrice_totale(k_elem) = w+1;
              ++x;
            }
        }
      // fonction characteristique (numero ibc) pour l'ensemble des ibc
      const Zone_VF& zone_vf = ref_cast(Zone_VF, zone_dis().valeur());
      const IntTab& face_voisins = zone_vf.face_voisins();
      for (int i = 0; i < nfaces ; i++)
        {
          const int elem0 = face_voisins(i, 0);
          const int elem1 = face_voisins(i, 1);
          double indi = 0.;
          if (elem0 >= 0) indi = indicatrice(elem0);
          if (elem1 >= 0) indi += indicatrice(elem1);
          if(indi > 0.)
            {
              if(indicatrice_face_totale(i) !=  0)
                {
                  Cerr<<"calcul_indic_pena_global : Attention: les faces des indicatrices se chevauchent "<<finl;
                  exit();
                }
              indicatrice_face_totale(i) = w+1;
              ++xf;
            }
        }
    }
  Cout<<"Calcul_indic_pena_global : Nb d elements de l indicatrice globale des IBCs: "<<x;
  Cout<<"  et nb de faces : "<<xf<<finl;
  indicatrice_totale.echange_espace_virtuel();
  indicatrice_face_totale.echange_espace_virtuel();
  //      Debog::verifier("Convection_Diffusion_Temperature::calcul_indic_pena_global indicatrice_totale ",indicatrice_totale);
  //      Debog::verifier("Convection_Diffusion_Temperature::calcul_indic_pena_global indicatrice_face_totale ",indicatrice_face_totale);
}

DoubleTab& Convection_Diffusion_Temperature::penalisation_L2(DoubleTab& u)
{
  // garde-fou
  assert(ref_penalisation_L2_FTD.size() != 0);
  assert(ref_penalisation_L2_FTD.size() == tab_penalisation_L2_FTD.size());

  const double& dt = schema_temps().pas_de_temps();
  DoubleTab u_old(u);

  // caclul de la fonction caracteristique globale des ibc pour le calcul de T_voisinage
  set_indic_pena_globale();

  //calcul de T_voisinage pour tous les elemnts
  const Zone_VF& zone_vf = ref_cast(Zone_VF, zone_dis().valeur());
  const IntTab& elem_faces = zone_vf.elem_faces();
  const IntTab& faces_elem = zone_vf.face_voisins();
  const int nb_faces_elem = elem_faces.dimension(1);
  const DoubleTab& inc=inconnue().valeurs();
  // inconnue doit etre scalaire
  assert(inc.line_size() == 1);
  DoubleTrav t_voisinage(inc);
  DoubleTrav u_voisinage(u);
  const int nb_elem  = u.dimension_tot(0);
  for (int i_elem = 0; i_elem < nb_elem; i_elem++)
    {
      double somme = 0.;
      double somme_deriv = 0.;
      double coeff = 0.;

      if( indic_pena_global(i_elem) != 0)
        {
          // boucle sur les faces de l element
          for (int i_face = 0; i_face < nb_faces_elem; i_face++)
            {
              const int face = elem_faces(i_elem, i_face);
              const int voisin = faces_elem(face, 0) + faces_elem(face, 1) - i_elem;
              if (voisin >= 0)
                {
                  if (indic_pena_global(voisin) == 0)
                    {
                      somme += inc(voisin);
                      somme_deriv += u(voisin);
                      coeff += 1.;
                    }
                }
            }
        }

      if (coeff > 0.)
        {
          t_voisinage(i_elem) = somme / coeff;
          u_voisinage(i_elem) = somme_deriv / coeff;
        }
      else
        {
          // cas pas de voisin dans le fluide
          t_voisinage(i_elem) = inc(i_elem);
          u_voisinage(i_elem) = u(i_elem);
        }
    }
  t_voisinage.echange_espace_virtuel();
  u_voisinage.echange_espace_virtuel();

  //determination du champ global de penalisation L2
  DoubleTrav pena_glob(inc); //initialise a zero
  DoubleTrav tab_(inc); //initialise a zero
  DoubleTrav denom(inc); //initialise a zero

  //boucle ibc
  for ( int i = 0 ; i < ref_penalisation_L2_FTD.size() ; ++i)
    {
      Transport_Interfaces_base& nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[i].valeur());
      const DoubleTab& tab = tab_penalisation_L2_FTD[i];
      //verification dimensions tab t obstacle = nb dim de l inconnu
      if ( tab.dimension(0) != inc.line_size() )
        {
          Cerr << " penalisation_L2: Les champs de temperature impose et calcule n'ont pas les memes dimensions" <<finl;
          Cerr << " Dimension du champ de temperature impose "<< tab.dimension(0) << finl;
          Cerr << " Dimension du champ de temperature calcule par TRUST "<<inc.line_size() <<finl;
          Cerr << " Nombre d'elements stockes au dans l'iconnue " <<inc.size() <<finl;
          Process::exit();
        }
      const DoubleTab& indicatrice = nom_eq.get_update_indicatrice().valeurs();

      DoubleTrav pena_loc(indicatrice);
      // Determination du terme de penalisation pour l ibc courante
      // boucle sur les elements
      for (int j = 0; j<nb_elem ; ++j)
        {
          //calcul de la fonction characteristique Ksi de l ibc courante -> pena_loc
          if (indicatrice(j) > 0.)
            {
              double coeff_p=1.;
              pena_loc(j) = coeff_p;
              //calcul du denominateur du terme de penalisation ( Sigma_ibc Ksi_ibc(j) )
              denom(j) += coeff_p;

              //determination valeur a imposer T_ref
              double tref_j = 0.;
              switch (choix_pena)
                {
                case 0 :
                  // approximation de T_ref par T_ref = T_imp
                  tref_j = tab(0);
                  break;
                case 1 :
                  // approximation de T_ref par T_ref = T+dt*derivee
                  tref_j = inc(j) + dt*u_old(j);
                  break;
                case 2 :
                  // approximation de T_imp par T_ref= tau*T_imp + (1-tau)*T_voisin
                  // approximation de T_voisin par T_voisin= 1/n * somme(1,n)(T_environnant+dt*derivee_environnant)
                  tref_j = (indicatrice(j)*tab(0)+(1.-indicatrice(j))*(t_voisinage(j)+dt*u_voisinage(j)));
                  break;
                case 3 :
                  // approximation de T_imp par T_ref= [(1-tau)*T_voisin + (1/2)*T_imp] / (3/2 -tau)
                  // approximation de T_voisin par T_voisin= 1/n * somme(1,n)(T_environnant+dt*derivee_environnant)
                  tref_j = ((1.-indicatrice(j))*(t_voisinage(j)+dt*u_voisinage(j))+0.5*tab(0))/(1.5-indicatrice(j));
                  break;
                case 4 :
                  // approximation de T_imp par T_ref= T_imp si tau=1 sinon T_ref=T_voisin
                  if (indicatrice(j) == 1.)
                    {
                      tref_j = tab(0);
                    }
                  else
                    {
                      tref_j = (t_voisinage(j)+dt*u_voisinage(j));
                    }
                  break;
                default:
                  Cerr << "Penalisation_L2: choix_pena invalide "<< choix_pena <<finl;
                  exit();
                  break;
                }
              // sauvegarde de Sigma_ibc Ksi_ibc(j) Tref_ibc pour post-traitement
              tab_(j) += tref_j;
              // penalisation_global = Sigma_ibc Ksi_ibc(j)*(Tref_ibc - inc) /  eta;
              pena_loc(j) *= (tref_j-inc(j));
              pena_loc(j) /= eta;
              pena_glob(j)  += pena_loc(j);
            }
        }
    }

  //  Realisation de la penalisation L2 de la derivee en temps: (dT_t_* + pena_glob) / ( 1 + (dt*Sigma_ibc Ksi_ibc(j)) / eta )
  // Tref est la moyenne arithmetique de Tref_j lorsque 0<eta<<1
  for (int j = 0 ; j<nb_elem ; ++j)
    {
      u(j) = u_old(j) + pena_glob(j);
      u(j) /= (1. + denom(j)*dt/eta);
    }
  u.echange_espace_virtuel();

  //  Sauvegarde des flux
  ecrire_fichier_pena_th(u_old,u,tab_,denom);

  //   Debog::verifier("Convection_Diffusion_Temperature::penalisation_L2 u ",u);
  return u;
}

void ouvrir_fichier_pena_th(SFichier& os, const Nom& type, const int& flag, const Transport_Interfaces_base& equation)
{
  // flag nul on n'ouvre pas le fichier
  if (flag==0)
    return ;
  Nom fichier=Objet_U::nom_du_cas();
  fichier+="_";
  fichier+=type;
  fichier+="_";
  fichier+=equation.le_nom();
  fichier+=".out";
  const Schema_Temps_base& sch=equation.probleme().schema_temps();
  const int& precision=sch.precision_impr();
  // On cree le fichier a la premiere impression avec l'en tete
  if (sch.nb_impr()==1 && !equation.probleme().reprise_effectuee())
    {
      os.ouvrir(fichier);
      SFichier& fic=os;
      Nom espace="\t\t";
      fic << (Nom)"# Impression puissance thermique";
      fic << " interface " << equation.le_nom();
      fic << " en W" << "." << finl;
      fic << finl << "# Temps";

      Nom ch=espace;
      ch+="P";
      fic << ch << finl;
    }
  // Sinon on l'ouvre
  else
    {
      os.ouvrir(fichier,ios::app);
    }
  os.precision(precision);
  os.setf(ios::scientific);
}

void Convection_Diffusion_Temperature::ecrire_fichier_pena_th(DoubleTab& u_old, DoubleTab& u, DoubleTab& tref, DoubleTab& denom)
{

  if (le_schema_en_temps->limpr())
    {

      const Zone_VF& zone_vf = ref_cast(Zone_VF, zone_dis().valeur());
      const DoubleVect& vol_maille = zone_vf.volumes();
      const Fluide_base& fluide_inc = ref_cast(Fluide_base, milieu());
      const DoubleTab& tab_rho = fluide_inc.masse_volumique().valeurs();
      const double rho = tab_rho(0,0);
      const DoubleTab& tab_cp = fluide_inc.capacite_calorifique().valeur().valeurs();
      const double cp = tab_cp(0,0);

      //  Methode pour calculer le flux total sur les ibc

      //Calcul de T_n+1 apres penalisation
      const DoubleTab& inc=inconnue().valeurs();
      const int nb_elem  = u.dimension(0);
      const double& dt = schema_temps().pas_de_temps();
      DoubleTab tkp1(inc);
      for (int k = 0 ; k<nb_elem ; ++k) tkp1(k) += u(k)*dt;

      //Terme de penalisation:       Sigma_ibc Ksi_ibc * ( T_n+1 - Tref_ibc) / eta
      DoubleTrav F(inc);
      for (int j = 0; j<nb_elem; ++j)  F(j) = (tref(j)-denom(j)*tkp1(j))/eta;

      // boucle sur les IBC

      DoubleTrav filtre(nb_elem);
      DoubleTrav filtre_glob(nb_elem);
      const double temps_flux = le_schema_en_temps->temps_courant();
      Nom espace=" \t";

      for ( int i = 0 ; i < ref_penalisation_L2_FTD.size() ; ++i) //boucle sur le nombre d'ibc
        {
          Transport_Interfaces_base& nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[i].valeur());
          const DoubleTab& indicatrice = nom_eq.get_update_indicatrice().valeurs();
          double Flux_pena= 0.;
          double Flux_pena_old= 0.;
          double Flux_temp_interne= 0.;
          //Filtre ibc (1) / non ibc (0)
          filtre = 0.;
          for ( int j = 0 ; j<nb_elem ; ++j)
            {
              if (indicatrice(j) > 0.)
                {
                  filtre(j) = 1.;
                  filtre_glob(j)=1.;
                }
              //bilans
              //Attention rho et cp constants
              Flux_pena_old += u_old(j) * filtre(j) *rho *cp *vol_maille(j);
              Flux_pena += F(j) * filtre(j) * vol_maille(j) *rho *cp;
              Flux_temp_interne += filtre(j)*rho*cp*vol_maille(j)*u(j);
            }

          // Ajout des differents processeurs en //
          Flux_pena_old = mp_sum(Flux_pena_old);
          Flux_pena = mp_sum(Flux_pena);
          Flux_temp_interne = mp_sum(Flux_temp_interne);

          //ecriture
          if (Process::je_suis_maitre())
            {
              SFichier FTE;
              ouvrir_fichier_pena_th(FTE,"Puissance_penalisation_thermique_ibc",1,nom_eq);
              FTE << temps_flux;
              FTE << espace << Flux_pena;
              FTE << finl;

              SFichier FTI;
              ouvrir_fichier_pena_th(FTI,"Puissance_thermique_ibc",1,nom_eq);
              FTI << temps_flux;
              FTI << espace << Flux_pena_old;
              FTI << finl;

              SFichier FTTI;
              ouvrir_fichier_pena_th(FTTI,"Derivee_temps_temperature_ibc",1,nom_eq);
              FTTI << temps_flux;
              FTTI << espace << Flux_temp_interne;
              FTTI << finl;
            }
        }

      //bilan Derivee_temps_temperature_fluide
      //Attention rho et cp constants
      double Flux_temp_externe= 0.;
      for (int j = 0; j<nb_elem; ++j)
        {
          Flux_temp_externe += (1.-filtre_glob(j))  *rho *cp *vol_maille(j) * u(j); //fluide
        }

      // Ajout des differents processeurs en //
      Flux_temp_externe = mp_sum(Flux_temp_externe); //fluide

      //ecriture
      if (Process::je_suis_maitre())
        {
          SFichier fichier;
          Nom fichier_nom=Objet_U::nom_du_cas();
          fichier_nom+="_";
          fichier_nom+="Derivee_temps_temperature_fluide.out";
          if (le_schema_en_temps->nb_impr()==1 && !probleme().reprise_effectuee())
            {
              fichier.ouvrir(fichier_nom);
            }
          else
            {
              fichier.ouvrir(fichier_nom,ios::app);
            }
          //if (fichier)
          {
            (fichier) <<temps_flux<<" \t"<<Flux_temp_externe<<finl;

          }
        }
    }
}

const Champ_Don& Convection_Diffusion_Temperature::diffusivite_pour_transport() const
{
  return milieu().conductivite();
}

const Champ_base& Convection_Diffusion_Temperature::diffusivite_pour_pas_de_temps() const
{
  return milieu().diffusivite();
}
