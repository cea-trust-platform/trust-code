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
//////////////////////////////////////////////////////////////////////////////
//
// File:        EDO_Pression_th_VEF_Gaz_Parfait.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/VEF
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <EDO_Pression_th_VEF_Gaz_Parfait.h>
#include <Fluide_Quasi_Compressible.h>
#include <Zone_VEF.h>

#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Loi_Etat_GP_QC.h>

#include <Dirichlet.h>
#include <Neumann_sortie_libre.h>
#include <Frontiere_ouverte_rho_u_impose.h>
#include <Champ_P1NC.h>

Implemente_instanciable(EDO_Pression_th_VEF_Gaz_Parfait,"EDO_Pression_th_VEF_Gaz_Parfait",EDO_Pression_th_VEF);

Sortie& EDO_Pression_th_VEF_Gaz_Parfait::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& EDO_Pression_th_VEF_Gaz_Parfait::readOn(Entree& is)
{
  return is;
}


// Description:
//    Resoud l'EDO
// Precondition:
// Parametre: double Pth_n
//    Signification: La pression a l'etape precedente
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: double
//    Signification: La nouvelle valeur de la pression
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double EDO_Pression_th_VEF_Gaz_Parfait::resoudre(double Pth_n)
{

  int traitPth = le_fluide_->getTraitementPth();

  if (traitPth==2)  // Pth constant
    return Pth_n;

  double present = le_fluide_->vitesse()->equation().schema_temps().temps_courant();
  double dt      = le_fluide_->vitesse()->equation().schema_temps().pas_de_temps();
  double futur   = present+dt;
  const DoubleTab& tempnp1 = le_fluide_->inco_chaleur()->valeurs(futur);    // T(n+1)
  const DoubleTab& tempn = le_fluide_->inco_chaleur()->valeurs(present);    // T(n)
  int nb_faces = la_zone->nb_faces();
  double Pth=0;

  const Zone_VEF& zone_vef = ref_cast(Zone_VEF, la_zone.valeur());

  if (traitPth == 0)   // EDO
    {

      const DoubleTab& tab_rho = le_fluide_->masse_volumique().valeurs();       // n+1/2

      const double rho_moy = Champ_P1NC::calculer_integrale_volumique(zone_vef, tab_rho, FAUX_EN_PERIO);
      DoubleVect tmp(tab_rho); // copie de rho
      assert(tmp.size() == nb_faces);
      const double invdt = 1. / dt;
      for (int i = 0; i < nb_faces; i++)
        tmp[i] *= (tempnp1[i] - tempn[i]) / tempnp1[i] * invdt;
      double S = Champ_P1NC::calculer_integrale_volumique(zone_vef, tmp, FAUX_EN_PERIO);

      //////////////////
      S /= rho_moy;
      //    double Pth = (Pth_n + S*dt/V);
      //double Pth = Pth_n/(1- S*dt);
      Pth = Pth_n/(1- S*dt);
      //test pour faire apparaitre la difference sur Pth
      //Pth *= 1e10;
    }

  else if (traitPth == 1)   // Conservation masse (WEC mars 2008)
    {

      // On veut Masse(n+1) - Masse(n) = dt * Debits_aux_bords
      // or Masse = Pth / R * sum(Vi/Ti) en volume
      // et Debit = Pth / R * sum(Sj.Uj/Tj) aux faces Dirichlet

      // Donc si on note masse_n = sum(Vi/Ti(n))
      // debit_u_imp = sum(Sj.Uj(impose)/Tj(n+1)) pour les vitesses imposees
      // et debit_rho_u_imp = sum(Sj.Uj(impose)/Tj(n+1)) pour les rho_u imposes
      //                      ou U(impose) = (rho.U)(impose) / rho(Pth(n),T(n+1))
      // il faut Pth(n+1)*masse_np1 - Pth(n)*masse_n = Pth(n+1) * debit_u_imp * dt
      //                                             + Pth(n) * debit_rho_u_imp * dt

      // Attention il faudrait prendre en compte les porosites dans les integrales !!!

      double debit_u_imp=0,debit_rho_u_imp=0;

      // On veut que rho_np1 soit calcule avec T(n+1) et Pth_n pour les CLs en rho_u impose
      le_fluide_->calculer_masse_volumique();

      // Calcul de masse_n et masse_np1
      DoubleVect tmp;
      tmp.copy(tempn, ArrOfDouble::NOCOPY_NOINIT); // copier uniquement la structure
      for (int i = 0; i < nb_faces; i++)
        tmp[i] = 1. / tempn[i];
      const double masse_n = Champ_P1NC::calculer_integrale_volumique(zone_vef, tmp, FAUX_EN_PERIO);
      for (int i = 0; i < nb_faces; i++)
        tmp[i] = 1. / tempnp1[i];
      const double masse_np1 = Champ_P1NC::calculer_integrale_volumique(zone_vef, tmp, FAUX_EN_PERIO);

      // Calcul de debit_u_imp et debit_rho_u_imp
      for (int n_bord=0; n_bord<la_zone->nb_front_Cl(); n_bord++)
        {
          const Cond_lim_base& la_cl = la_zone_Cl->les_conditions_limites(n_bord).valeur();
          if (sub_type(Dirichlet, la_cl))
            {
              const Dirichlet& diri=ref_cast(Dirichlet,la_cl);
              const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb = la_front_dis.num_premiere_face();
              int nfin = ndeb + la_front_dis.nb_faces();
              for (int face=ndeb; face<nfin; face++)
                {
                  double debit_v=0;
                  for (int d=0; d<dimension; d++)
                    debit_v += la_zone->face_normales(face,d)*diri.val_imp_au_temps(futur,face-ndeb,d)/tempnp1(face);
                  int n0 = la_zone->face_voisins(face,0);
                  if (n0 == -1)
                    debit_v=-debit_v;
                  if (sub_type(Frontiere_ouverte_rho_u_impose, la_cl))
                    debit_rho_u_imp+=debit_v;
                  else
                    debit_u_imp+=debit_v;
                }
            }
          else if (sub_type(Neumann_sortie_libre, la_cl))
            {
              Cerr<<la_cl.que_suis_je()<<" est incompatible avec le traitement conservation_masse."<<finl;
              exit();
            }
        }
      // On fait la somme sur les procs
      debit_u_imp=mp_sum(debit_u_imp);
      debit_rho_u_imp=mp_sum(debit_rho_u_imp);

      // Calcul de Pth(n+1)
      Pth = Pth_n * (masse_n - dt*debit_rho_u_imp) / (masse_np1 + dt*debit_u_imp);
    }

  return Pth;
}
