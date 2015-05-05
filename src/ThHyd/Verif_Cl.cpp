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
// File:        Verif_Cl.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Verif_Cl.h>
#include <Zone_Cl_dis.h>
#include <Les_Cl.h>
#include <Motcle.h>
#include <Frontiere_dis_base.h>


// Description:
//    Teste la compatibilite des conditions aux limites
//    thermiques et hydrauliques.
//    La liste des compatibilites est la suivante:
//    -----------------------------------------------------------------------
//    Hydraulique                      |       Thermique
//    -----------------------------------------------------------------------
//    Entree_fluide_vitesse_imposee ===> Entree_fluide_temperature_imposee
//    =================================> Neumann_sortie_libre
//    -----------------------------------------------------------------------
//    Dirichlet_paroi_fixe |
//    Dirichlet_paroi_defilante =======> Neumann_paroi_adiabatique
//    =================================> Neumann_paroi
//    =================================> Echange_global_impose
//    =================================> Echange_externe_impose
//    =================================> Scalaire_impose_paroi
//    -----------------------------------------------------------------------
//    Neumann_sortie_libre ============> Entree_fluide_temperature_imposee
//    =================================> Neumann_sortie_libre
//    -----------------------------------------------------------------------
//    Symetrie ========================> Neumann_paroi_adiabatique
//    =================================> Neumann_paroi
//    =================================> Echange_global_impose
//    =================================> Echange_externe_impose
//    =================================> Scalaire_impose_paroi
//    =================================> Symetrie
//    -----------------------------------------------------------------------
//    Periodique ======================> Periodique
// Precondition:
// Parametre: Zone_Cl_dis& zone_Cl_hydr
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: Zone_Cl_dis& zone_Cl_th
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:entree
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception: nombres de conditions aux limites differents
// Exception: conditions aux limites hydraulique et thermique incompatibles
// Effets de bord:
// Postcondition:
int tester_compatibilite_hydr_thermique(const Zone_Cl_dis& zone_Cl_hydr, const Zone_Cl_dis& zone_Cl_th)
{

  int nb_Cl = zone_Cl_hydr.nb_cond_lim();

  if (zone_Cl_th.nb_cond_lim() != nb_Cl)
    {
      Cerr << "Les deux objets de type Zone_Cl_dis n'ont pas" << finl;
      Cerr << "le meme nombre de conditions aux limites" << finl;
      Process::exit();
    }

  for (int num_Cl=0; num_Cl<nb_Cl; num_Cl++)
    {
      const Cond_lim& la_cl_hydr = zone_Cl_hydr.les_conditions_limites(num_Cl);
      const Cond_lim& la_cl_th = zone_Cl_th.les_conditions_limites(num_Cl);
      if (sub_type(Entree_fluide_vitesse_imposee,la_cl_hydr.valeur()))
        {
          if ( (sub_type(Entree_fluide_temperature_imposee,la_cl_th.valeur()))
               || (sub_type(Neumann_sortie_libre,la_cl_th.valeur())) )
            ;
          else if ( (sub_type(Entree_fluide_T_h_imposee,la_cl_th.valeur()))
                    || (sub_type(Neumann_sortie_libre,la_cl_th.valeur())) )
            ;
          else
            {
              Cerr << "Les conditions aux limites en hydraulique et en thermique" << finl;
              Cerr << "ne sont pas toutes compatibles bord " <<la_cl_th.frontiere_dis().le_nom()<< finl;
              Cerr<<la_cl_hydr.valeur().que_suis_je()<<" "<<la_cl_th.valeur().que_suis_je()<<finl;
              Process::exit();
            }
        }
      else if ( (sub_type(Dirichlet_paroi_fixe,la_cl_hydr.valeur())) ||
                (sub_type(Dirichlet_paroi_defilante,la_cl_hydr.valeur()))  )
        {
          if ( (sub_type(Neumann_paroi_adiabatique,la_cl_th.valeur()))
               || (sub_type(Neumann_paroi,la_cl_th.valeur()))
               || (sub_type(Echange_global_impose,la_cl_th.valeur()))
               || (sub_type(Echange_externe_impose,la_cl_th.valeur()))
               || (sub_type(Scalaire_impose_paroi,la_cl_th.valeur()))
             )
            {
              ;
            }
          else
            {
              Cerr << "Les conditions aux limites en hydraulique et en thermique" << finl;
              Cerr << "ne sont pas toutes compatibles bord " <<la_cl_th.frontiere_dis().le_nom()<< finl;
              Cerr<<la_cl_hydr.valeur().que_suis_je()<<" "<<la_cl_th.valeur().que_suis_je()<<finl;
              Process::exit();
            }
        }
      else if (sub_type(Neumann_sortie_libre,la_cl_hydr.valeur()))
        {
          if ( (sub_type(Entree_fluide_temperature_imposee,la_cl_th.valeur()))
               || (sub_type(Neumann_sortie_libre,la_cl_th.valeur())) )
            {
              ;
            }
          else
            {
              Cerr << "Les conditions aux limites en hydraulique et en thermique" << finl;
              Cerr << "ne sont pas toutes compatibles bord " <<la_cl_th.frontiere_dis().le_nom()<< finl;
              Cerr<<la_cl_hydr.valeur().que_suis_je()<<" "<<la_cl_th.valeur().que_suis_je()<<finl;
              Process::exit();
            }
        }
      else if (sub_type(Symetrie,la_cl_hydr.valeur()))
        {
          if ( (sub_type(Neumann_paroi_adiabatique,la_cl_th.valeur()))
               || (sub_type(Neumann_paroi,la_cl_th.valeur()))
               || (sub_type(Echange_global_impose,la_cl_th.valeur()))
               || (sub_type(Echange_externe_impose,la_cl_th.valeur()))
               || (sub_type(Scalaire_impose_paroi,la_cl_th.valeur()))
               || (sub_type(Symetrie,la_cl_th.valeur())) )
            {
              ;
            }
          else
            {
              Cerr << "Les conditions aux limites en hydraulique et en thermique" << finl;
              Cerr << "ne sont pas toutes compatibles bord " <<la_cl_th.frontiere_dis().le_nom()<< finl;
              Cerr<<la_cl_hydr.valeur().que_suis_je()<<" "<<la_cl_th.valeur().que_suis_je()<<finl;
              Process::exit();
            }
        }
      else if (sub_type(Periodique,la_cl_hydr.valeur()))
        {
          if (sub_type(Periodique,la_cl_th.valeur()))
            {
              ;
            }
          else
            {
              Cerr << "Les conditions aux limites en hydraulique et en thermique" << finl;
              Cerr << "ne sont pas toutes compatibles bord " <<la_cl_th.frontiere_dis().le_nom()<< finl;
              Cerr<<la_cl_hydr.valeur().que_suis_je()<<" "<<la_cl_th.valeur().que_suis_je()<<finl;
              Process::exit();
            }
        }
    }
  return 1;
}


// Description:
//    Teste la compatibilite des conditions aux limites
//    concentration et hydrauliques.
//    La liste des compatibilites est la suivante:
//    -----------------------------------------------------------------------
//    Hydraulique                      |       Concentration
//    -----------------------------------------------------------------------
//    Entree_fluide_vitesse_imposee ===> Entree_fluide_concentration_imposee
//    =================================> Neumann_sortie_libre
//    -----------------------------------------------------------------------
//    Dirichlet_paroi_fixe |
//    Dirichlet_paroi_defilante =======> Neumann_paroi_flux_nul
//    -----------------------------------------------------------------------
//    Neumann_sortie_libre ============> Neumann_sortie_libre
//    =================================> Entree_fluide_concentration_imposee
//    -----------------------------------------------------------------------
//    Symetrie ========================> Symetrie
//    =================================> Neumann_paroi_flux_nul
//    -----------------------------------------------------------------------
//    Periodique ======================> Periodique
// Precondition:
// Parametre: Zone_Cl_dis& zone_Cl_hydr
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: Zone_Cl_dis& zone_Cl_co
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception: nombres de conditions aux limites differents
// Exception: conditions aux limite hydraulique et concentration incompatible
// Effets de bord:
// Postcondition:
int tester_compatibilite_hydr_concentration(const Zone_Cl_dis& zone_Cl_hydr,
                                            const Zone_Cl_dis& zone_Cl_co)
{

  int nb_Cl = zone_Cl_hydr.nb_cond_lim();

  if (zone_Cl_co.nb_cond_lim() != nb_Cl)
    {
      Cerr << "Les deux objets de type Zone_Cl_dis n'ont pas" << finl;
      Cerr << "le meme nombre de conditions aux limites" << finl;
      Process::exit();
    }

  for (int num_Cl=0; num_Cl<nb_Cl; num_Cl++)
    {
      const Cond_lim& la_cl_hydr = zone_Cl_hydr.les_conditions_limites(num_Cl);
      const Cond_lim& la_cl_co = zone_Cl_co.les_conditions_limites(num_Cl);

      if (sub_type(Entree_fluide_vitesse_imposee,la_cl_hydr.valeur()))
        {
          if ( (sub_type(Entree_fluide_concentration_imposee,la_cl_co.valeur()))
               || (sub_type(Neumann_sortie_libre,la_cl_co.valeur()))  )
            ;
          else
            {
              Cerr << "Les conditions aux limites en hydraulique et en concentration" << finl;
              Cerr << "ne sont pas toutes compatibles" << finl;
              Process::exit();
            }
        }
      else if ( (sub_type(Dirichlet_paroi_fixe,la_cl_hydr.valeur())) ||
                (sub_type(Dirichlet_paroi_defilante,la_cl_hydr.valeur()))  )
        {
          if ((!sub_type(Neumann_paroi_flux_nul,la_cl_co.valeur())) && (!sub_type(Neumann_paroi,la_cl_co.valeur()))
              && (!sub_type(Scalaire_impose_paroi,la_cl_co.valeur())))
            {
              Cerr << "Les conditions aux limites en hydraulique et en concentration" << finl;
              Cerr << "ne sont pas toutes compatibles" << finl;
              Process::exit();
            }
        }
      else if (sub_type(Neumann_sortie_libre,la_cl_hydr.valeur()))
        {
          if ( (sub_type(Neumann_sortie_libre,la_cl_co.valeur())) ||
               (sub_type(Entree_fluide_concentration_imposee,la_cl_co.valeur())) )
            {
              ;
            }
          else
            {
              Cerr << "Les conditions aux limites en hydraulique et en concentration" << finl;
              Cerr << "ne sont pas toutes compatibles" << finl;
              Process::exit();
            }
        }
      else if (sub_type(Symetrie,la_cl_hydr.valeur()))
        {
          if ( (sub_type(Symetrie,la_cl_co.valeur()))
               || (sub_type(Neumann_paroi_flux_nul,la_cl_co.valeur())) )
            {
              ;
            }
          else
            {
              Cerr << "Les conditions aux limites en hydraulique et en concentration" << finl;
              Cerr << "ne sont pas toutes compatibles" << finl;
              Process::exit();
            }
        }
      else if (sub_type(Periodique,la_cl_hydr.valeur()))
        {
          if (sub_type(Periodique,la_cl_co.valeur()))
            {
              ;
            }
          else
            {
              Cerr << "Les conditions aux limites en hydraulique et en concentration" << finl;
              Cerr << "ne sont pas toutes compatibles" << finl;
              Process::exit();
            }
        }
    }

  return 1;
}




// Description:
//    Teste la compatibilite des conditions aux limites
//    hydrauliques et turbulentes.
//    La liste des compatibilites est la suivante:
//    -----------------------------------------------------------------------
//    Hydraulique                      |       Turbulence
//    -----------------------------------------------------------------------
//    Dirichlet_paroi_fixe  ou
//    Dirichlet_paroi_defilante =======> Neumann_paroi_flux_nul
//    =================================> Dirichlet_paroi_fixe (pour bas_reynolds)
//    -----------------------------------------------------------------------
//    Symetrie ========================> Symetrie
//    =================================> Neumann_paroi_flux_nul
//    -----------------------------------------------------------------------
//    Neumann_sortie_libre ============> Neumann_sortie_libre
//    =================================> Entree_fluide_K_Eps_impose
//    -----------------------------------------------------------------------
//    Entree_fluide_vitesse_imposee ===> Entree_fluide_K_Eps_impose
//    =================================> Neumann_sortie_libre
//    =================================> Symetrie
//    -----------------------------------------------------------------------
//    Periodique ======================> Periodique
//    -----------------------------------------------------------------------
// Precondition:
// Parametre: Zone_Cl_dis& zone_Cl_hydr
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: Zone_Cl_dis& zone_Cl_turb
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception: nombres de conditions aux limites differents
// Exception: conditions aux limite hydraulique et turbulentes incompatible
// Effets de bord:
// Postcondition:
int tester_compatibilite_hydr_turb(const Zone_Cl_dis& zone_Cl_hydr, const Zone_Cl_dis& zone_Cl_turb)
{

  int nb_Cl = zone_Cl_hydr.nb_cond_lim();

  if (zone_Cl_turb.nb_cond_lim() != nb_Cl)
    {
      Cerr << "The two objects of type Zone_Cl_dis have not" << finl;
      Cerr << "the same number of boundary conditions." << finl;
      Process::exit();
    }

  for (int num_Cl=0; num_Cl<nb_Cl; num_Cl++)
    {
      const Cond_lim& la_cl_turb = zone_Cl_turb.les_conditions_limites(num_Cl);
      const Cond_lim& la_cl_hydr = zone_Cl_hydr.les_conditions_limites(num_Cl);

      // on teste si une des deux CL est periodique
      // et s'il y en a une, on verifie que les CL des deux eq sont periodiques
      if ( (sub_type(Periodique,la_cl_hydr.valeur()) || sub_type(Periodique,la_cl_turb.valeur())) &&
           ( ! ( sub_type(Periodique,la_cl_hydr.valeur()) && sub_type(Periodique,la_cl_turb.valeur()) ) ) )
        {
          message_erreur( la_cl_hydr, la_cl_turb, num_Cl);
        }

      // hyd symetrie et turb (symetrie ou Neumann_paroi_flux_nul("paroi"))
      if ( sub_type(Symetrie,la_cl_hydr.valeur())  &&
           ! ( sub_type(Symetrie,la_cl_turb.valeur()) ||
               sub_type(Neumann_paroi_flux_nul,la_cl_turb.valeur()) ) )
        {
          message_erreur( la_cl_hydr, la_cl_turb, num_Cl);
        }

      // hyd (paroi_fixe ou defilante) et turb (paroi ou paroi_fixe)
      if ( (sub_type(Dirichlet_paroi_fixe,la_cl_hydr.valeur()) || sub_type(Dirichlet_paroi_defilante,la_cl_hydr.valeur())) &&
           ! ( sub_type(Neumann_paroi_flux_nul,la_cl_turb.valeur()) || sub_type(Dirichlet_paroi_fixe,la_cl_turb.valeur()) ) )
        {
          message_erreur( la_cl_hydr, la_cl_turb, num_Cl);
        }

      // hyd (sortie_libre) et turb (sortie_libre ou k_eps_impose)
      if ( sub_type(Neumann_sortie_libre,la_cl_hydr.valeur()) &&
           ! ( sub_type(Neumann_sortie_libre,la_cl_turb.valeur()) ||
               sub_type(Entree_fluide_K_Eps_impose,la_cl_turb.valeur()) ) )
        {
          message_erreur( la_cl_hydr, la_cl_turb, num_Cl);
        }

      // hyd (Entree_fluide_vitesse_imposee ou Frontiere_ouverte_vitesse_imposee)
      // et turb (Entree_fluide_K_Eps_impose ou Frontiere_ouverte_K_Eps_impose)
      if ( sub_type(Entree_fluide_vitesse_imposee,la_cl_hydr.valeur()) &&
           ! ( sub_type(Neumann_sortie_libre,la_cl_turb.valeur()) ||
               sub_type(Entree_fluide_K_Eps_impose,la_cl_turb.valeur()) ||
               sub_type(Symetrie,la_cl_turb.valeur()) ) )
        {
          message_erreur( la_cl_hydr, la_cl_turb, num_Cl);
        }

    }
  return 1;

}


// Description:
//    Affiche un message d'erreur pour la fonction precedente
// Precondition:
// Parametre: Zone_Cl_dis& zone_Cl_hydr
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: Zone_Cl_dis& zone_Cl_turb
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int num_Cl
//    Signification: numero de la CL
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Effets de bord:
// Postcondition:
int message_erreur(const Cond_lim& la_cl_hydr, const Cond_lim& la_cl_turb, int& num_Cl)
{
  Cerr << "The hydraulic and turbulent boundary conditions are not consitent !!" << finl;
  Cerr << "Boundary conditions number " << num_Cl << " \"" << la_cl_turb.frontiere_dis().le_nom() << "\" have been assigned to : " << finl;
  Cerr << la_cl_hydr.valeur().que_suis_je() << " and " << la_cl_turb.valeur().que_suis_je() << " !! " << finl;
  Process::exit();
  return 1;
}

