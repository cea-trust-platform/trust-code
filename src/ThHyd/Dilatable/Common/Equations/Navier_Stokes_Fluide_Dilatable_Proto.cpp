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
// File:        Navier_Stokes_Fluide_Dilatable_Proto.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Equations
// Version:     /main/47
//
//////////////////////////////////////////////////////////////////////////////

#include <Navier_Stokes_Fluide_Dilatable_Proto.h>
#include <Transport_Interfaces_base.h>
#include <Fluide_Dilatable_base.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <Dirichlet.h>
#include <TRUSTTrav.h>
#include <Zone_VF.h>
#include <Domaine.h>
#include <Debog.h>
#include <Statistiques.h>

extern Stat_Counter_Id assemblage_sys_counter_;
extern Stat_Counter_Id source_counter_;

Navier_Stokes_Fluide_Dilatable_Proto::Navier_Stokes_Fluide_Dilatable_Proto() : cumulative_(0) { }

// Multiply density by velocity and return density*velocity (mass flux)
DoubleTab& Navier_Stokes_Fluide_Dilatable_Proto::rho_vitesse_impl(const DoubleTab& tab_rho,const DoubleTab& vit,DoubleTab& rhovitesse) const
{
  const int n = vit.dimension(0), ncomp = vit.line_size();

  for (int i=0 ; i<n ; i++)
    for (int j=0 ; j<ncomp ; j++) rhovitesse(i,j) = tab_rho(i,0)*vit(i,j);

  rhovitesse.echange_espace_virtuel();
  Debog::verifier("Navier_Stokes_Fluide_Dilatable_Proto::rho_vitesse : ", rhovitesse);
  return rhovitesse;
}

int Navier_Stokes_Fluide_Dilatable_Proto::impr_impl(const Navier_Stokes_std& eqn,Sortie& os) const
{
  const Fluide_Dilatable_base& fluide_dil = ref_cast(Fluide_Dilatable_base,eqn.fluide());
  const DoubleTab& vit = eqn.vitesse().valeurs(), rho = fluide_dil.rho_face_np1();
  DoubleTab mass_flux(vit);
  rho_vitesse_impl(rho,vit,mass_flux);

  DoubleTab array;
  array.copy(eqn.div().valeurs(), Array_base::NOCOPY_NOINIT); // init structure uniquement
  if (tab_W.get_md_vector().non_nul())
    {
      operator_egal(array, tab_W ); //, VECT_REAL_ITEMS); // initialise
      array*=-1;
    }
  else
    {
      // remarque B.M.: certaines implementations de cette methode ne font pas echange espace virtuel:
      fluide_dil.secmembre_divU_Z(array);
      array*=-1;
    }

  array.echange_espace_virtuel();
  eqn.operateur_divergence().ajouter(mass_flux, array);
  double LocalMassFlowRateError = mp_max_abs_vect(array); // max|sum(rho*u*ndS)|

  os << "-------------------------------------------------------------------"<< finl;
  os << "Cell balance mass flow rate control for the problem " << eqn.probleme().le_nom() << " : " << finl;
  os << "Absolute value : " << LocalMassFlowRateError << " kg/s" << finl; ;

  // On divise array par vol(i)
  eqn.operateur_divergence().volumique(array);

  // On divise par un rho moyen
  double rho_moyen = mp_moyenne_vect(rho), dt = eqn.probleme().schema_temps().pas_de_temps();
  double bilan_massique_relatif = mp_max_abs_vect(array) * dt / rho_moyen;
  os << "Relative value : " << bilan_massique_relatif << finl; // =max|LocalMassFlowRateError/(rho_moyen*Vol(i)/dt)|

  // Calculation as OpenFOAM: http://foam.sourceforge.net/docs/cpp/a04190_source.html
  // It is relative errors (normalized by the volume/dt)
  double TotalMass = rho_moyen * eqn.probleme().domaine().zone(0).volume_total();
  double local = LocalMassFlowRateError / ( TotalMass / dt ), global = mp_somme_vect(array) / ( TotalMass / dt );
  cumulative_ += global;

  os << "time step continuity errors : sum local = " << local << ", global = " << global << ", cumulative = " << cumulative_ << finl;

  if (local > 0.01)
    {
      Cerr << "The mass balance is too bad (relative value > 1%)." << finl;
      Cerr << "Please check and lower the convergence value of the pressure solver." << finl;
      Process::exit();
    }
  return 1;
}

// Description:
//    Calcule la derivee en temps de l'inconnue vitesse,
//    i.e. l'acceleration dU/dt et la renvoie.
//    Appelle Equation_base::derivee_en_temps_inco(DoubleTab& )
//    Calcule egalement la pression.
// Precondition:
// Parametre: DoubleTab& vpoint
//    Signification: le tableau des valeurs de l'acceleration dU/dt
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: DoubleTab&
//    Signification: le tableau des valeurs de l'acceleration (derivee de la vitesse)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Navier_Stokes_Fluide_Dilatable_Proto::derivee_en_temps_inco_impl(Navier_Stokes_std& eqn,DoubleTab& vpoint)
{
  const Fluide_Dilatable_base& fluide_dil=ref_cast(Fluide_Dilatable_base,eqn.milieu());
  DoubleTab& press = eqn.pression().valeurs(), vit = eqn.vitesse().valeurs();
  DoubleTab secmem(press);
  DoubleTrav inc_pre(press);

  if (!tab_W.get_md_vector().non_nul())
    {
      tab_W.copy(secmem, Array_base::NOCOPY_NOINIT); // copie la structure
      // initialisation sinon plantage assert lors du remplissage dans EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1B
      tab_W = 0.;
    }

  DoubleTab rhoU;
  rhoU.copy(vit, Array_base::NOCOPY_NOINIT); // copie la structure

  // Get champ gradP
  REF(Champ_base) gradient_pression;

  try
    {
      gradient_pression = eqn.get_champ("gradient_pression");
    }
  catch (Champs_compris_erreur)
    {
      Cerr<<" l'equation ne comprend pas gradient_pression "<<finl;
      Process::exit();
    }

  DoubleTab& gradP=gradient_pression.valeur().valeurs();
  DoubleTrav Mmoins1grad(gradP);

  // We use the incremental pressure-projection algorithm (Chorin)

  // Step 1 : prepare operators and solve for a provisional velocity u*
  prepare_and_solve_u_star(eqn,fluide_dil,rhoU, vpoint);

  // Step 2 : solve the poisson equation for the pressure increment (Pi = P^n+1 - P^n)
  // Attention the matrix has a constant coefficient as the variable of NS is rhoU and not U !!!
  solve_pressure_increment(eqn,fluide_dil,rhoU,secmem,inc_pre,vpoint);

  // Step 3 : compute P^n+1 & compute the correct velocity u^n+1
  correct_and_compute_u_np1(eqn,fluide_dil,rhoU,Mmoins1grad,inc_pre,gradP,vpoint);

  return vpoint;
}

void Navier_Stokes_Fluide_Dilatable_Proto::assembler_avec_inertie_impl(const Navier_Stokes_std& eqn, Matrice_Morse& mat_morse,
                                                                       const DoubleTab& present, DoubleTab& secmem)
{
  // ******   avant inertie   ******
  // diffusion en div(mu grad u ) or on veut impliciter en rho * u => on divise les contributions par le rho_face associe
  // GF on ajoute apres avoir contribuer pour avoir les bons flux bords
  DoubleTrav rhovitesse(present);

  // Op diff
  eqn.operateur(0).l_op_base().contribuer_a_avec(present,mat_morse);
  eqn.operateur(0).ajouter(secmem);

  const Fluide_Dilatable_base& fluide_dil=ref_cast(Fluide_Dilatable_base,eqn.milieu());
  const DoubleTab& tab_rho_face_np1 = fluide_dil.rho_face_np1(), tab_rho_face_n=fluide_dil.rho_face_n();
  const int nb_compo = present.line_size();
  const IntVect& tab1 = mat_morse.get_tab1(), tab2 = mat_morse.get_tab2();

  DoubleVect& coeff = mat_morse.get_set_coeff();
  for (int i=0; i<mat_morse.nb_lignes(); i++)
    for (int k=tab1(i)-1; k<tab1(i+1)-1; k++)
      {
        int j = tab2(k)-1;
        double rapport = tab_rho_face_np1(j/nb_compo);
        coeff(k) /= rapport;
      }

  rho_vitesse_impl(tab_rho_face_np1,present,rhovitesse); // rho*U

  // Op conv
  eqn.operateur(1).l_op_base().contribuer_a_avec(rhovitesse,mat_morse);
  eqn.operateur(1).ajouter(rhovitesse,secmem);

  // sources
  eqn.sources().ajouter(secmem);
  eqn.sources().contribuer_a_avec(present,mat_morse);

  // on resout en rho u on stocke donc rho u dans present
  rho_vitesse_impl(tab_rho_face_np1,present,ref_cast_non_const(DoubleTab,present));
  mat_morse.ajouter_multvect(present,secmem);

  /*
   * contribution a la matrice de l'inertie :
   * on divisie la diagonale par rhon+1 face
   * on ajoute l'inertie de facon standard
   * on remultiplie la diagonale par rhon+1
   */

  // ajout de l'inertie
  const double dt=eqn.schema_temps().pas_de_temps();
  eqn.solv_masse().ajouter_masse(dt,mat_morse,0);

  rho_vitesse_impl(tab_rho_face_n,eqn.inconnue().passe(),rhovitesse);
  eqn.solv_masse().ajouter_masse(dt,secmem,rhovitesse,0);

  // blocage_cl faux si dirichlet u!=0 !!!!!! manque multiplication par rho
  for (int op=0; op< eqn.nombre_d_operateurs(); op++) eqn.operateur(op).l_op_base().modifier_pour_Cl(mat_morse,secmem);

  /*
   * correction finale pour les dirichlets
   * on ne doit pas imposer un+1 mais rho_un+1 => on multiplie dons le resu par rho_face_np1
   */
  const Conds_lim& lescl=eqn.zone_Cl_dis().les_conditions_limites();
  int nbcondlim=lescl.size();

  for (int icl=0; icl<nbcondlim; icl++)
    {
      const Cond_lim_base& la_cl_base = lescl[icl].valeur();
      if (sub_type(Dirichlet,la_cl_base))
        {

          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl_base.frontiere_dis());
          int ndeb = la_front_dis.num_premiere_face();
          int nfin = ndeb + la_front_dis.nb_faces();
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              if (present.line_size()==1) secmem(num_face)*=tab_rho_face_np1(num_face);
              else
                for (int dir=0; dir<Objet_U::dimension; dir++) secmem(num_face,dir)*=tab_rho_face_np1(num_face);
            }
        }
    }
}


void Navier_Stokes_Fluide_Dilatable_Proto::assembler_blocs_avec_inertie(const Navier_Stokes_std& eqn, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl)
{
  statistiques().begin_count(assemblage_sys_counter_);
  const std::string& nom_inco = eqn.inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco)?matrices.at(nom_inco):NULL;
  const DoubleTab& present = eqn.inconnue();

  // ******   avant inertie   ******
  // diffusion en div(mu grad u ) or on veut impliciter en rho * u => on divise les contributions par le rho_face associe
  // GF on ajoute apres avoir contribuer pour avoir les bons flux bords
  DoubleTrav rhovitesse(present);

  // Op diff
  eqn.operateur(0).l_op_base().ajouter_blocs(matrices, secmem, semi_impl);

  const Fluide_Dilatable_base& fluide_dil=ref_cast(Fluide_Dilatable_base,eqn.milieu());
  const DoubleTab& tab_rho_face_np1 = fluide_dil.rho_face_np1(), tab_rho_face_n=fluide_dil.rho_face_n();
  const int nb_compo = present.line_size();
  const IntVect& tab1 = mat->get_tab1(), tab2 = mat->get_tab2();

  DoubleVect& coeff = mat->get_set_coeff();
  for (int i=0; i<mat->nb_lignes(); i++)
    for (int k=tab1(i)-1; k<tab1(i+1)-1; k++)
      {
        int j = tab2(k)-1;
        double rapport = tab_rho_face_np1(j/nb_compo);
        coeff(k) /= rapport;
      }

  rho_vitesse_impl(tab_rho_face_np1,present,rhovitesse); // rho*U

  // Op conv
  eqn.operateur(1).l_op_base().ajouter_blocs(matrices, secmem, {{nom_inco,rhovitesse}});
  statistiques().end_count(assemblage_sys_counter_);

  // sources
  statistiques().begin_count(source_counter_);
  for (int i = 0; i < eqn.sources().size(); i++)
    eqn.sources()(i).valeur().ajouter_blocs(matrices, secmem, semi_impl);
  statistiques().end_count(source_counter_);

  statistiques().begin_count(assemblage_sys_counter_);
  // on resout en rho u on stocke donc rho u dans present
  rho_vitesse_impl(tab_rho_face_np1,present,ref_cast_non_const(DoubleTab,present));
  mat->ajouter_multvect(present,secmem);

  DoubleTab tmp(secmem);
  tmp = 0.0;
  eqn.operateur_gradient().valeur().ajouter_blocs(matrices, tmp, semi_impl);
  secmem-=tmp;

  /*
   * contribution a la matrice de l'inertie :
   * on divisie la diagonale par rhon+1 face
   * on ajoute l'inertie de facon standard
   * on remultiplie la diagonale par rhon+1
   */

  // ajout de l'inertie
  const double dt=eqn.schema_temps().pas_de_temps();
  eqn.solv_masse().ajouter_masse(dt,*mat,0);
  rho_vitesse_impl(tab_rho_face_n,eqn.inconnue().passe(),rhovitesse);
  eqn.solv_masse().ajouter_masse(dt,secmem,rhovitesse,0);

  // blocage_cl faux si dirichlet u!=0 !!!!!! manque multiplication par rho
  for (int op=0; op< eqn.nombre_d_operateurs(); op++) eqn.operateur(op).l_op_base().modifier_pour_Cl(*mat,secmem);

  /*
   * correction finale pour les dirichlets
   * on ne doit pas imposer un+1 mais rho_un+1 => on multiplie dons le resu par rho_face_np1
   */
  const Conds_lim& lescl=eqn.zone_Cl_dis().les_conditions_limites();
  int nbcondlim=lescl.size();

  for (int icl=0; icl<nbcondlim; icl++)
    {
      const Cond_lim_base& la_cl_base = lescl[icl].valeur();
      if (sub_type(Dirichlet,la_cl_base))
        {

          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl_base.frontiere_dis());
          int ndeb = la_front_dis.num_premiere_face();
          int nfin = ndeb + la_front_dis.nb_faces();
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              if (present.line_size()==1) secmem(num_face)*=tab_rho_face_np1(num_face);
              else
                for (int dir=0; dir<Objet_U::dimension; dir++) secmem(num_face,dir)*=tab_rho_face_np1(num_face);
            }
        }
    }
  secmem.echange_espace_virtuel();
  statistiques().end_count(assemblage_sys_counter_);

}


void Navier_Stokes_Fluide_Dilatable_Proto::assembler_impl( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem)
{
  Cerr << "Navier_Stokes_Fluide_Dilatable_Proto::assembler is not coded ! You should use assembler_avec_inertie !" << finl;
  Process::exit();
}

/*
 * ***************
 * Private methods
 * ***************
 */
void Navier_Stokes_Fluide_Dilatable_Proto::prepare_and_solve_u_star(Navier_Stokes_std& eqn,
                                                                    const Fluide_Dilatable_base& fluide_dil,
                                                                    DoubleTab& rhoU, DoubleTab& vpoint)
{
  const DoubleTab& tab_rho_face_n =fluide_dil.rho_face_n(), tab_rho_face_np1=fluide_dil.rho_face_np1();
  const DoubleTab& tab_rho = fluide_dil.rho_discvit(); // rho avec la meme discretisation que la vitesse
  const DoubleTab& vit = eqn.vitesse().valeurs();

  fluide_dil.secmembre_divU_Z(tab_W); //Calcule W=-dZ/dt, 2nd membre de l'equation div(rhoU) = W
  vpoint=0;

  // ajout diffusion (avec la viscosite dynamique)
  if (!eqn.schema_temps().diffusion_implicite()) eqn.operateur(0).ajouter(vpoint);

  DoubleTab& rhovitesse = ref_cast_non_const(DoubleTab,eqn.rho_la_vitesse().valeurs());
  rho_vitesse_impl(tab_rho,vit,rhovitesse);

  // ajout convection utilise rhovitesse
  if (!eqn.schema_temps().diffusion_implicite()) eqn.operateur(1).ajouter(rhovitesse,vpoint);
  else
    {
      DoubleTrav trav(vpoint);
      eqn.derivee_en_temps_conv(trav,rhovitesse);
      vpoint = trav;
    }

  // ajout source
  eqn.sources().ajouter(vpoint);

  // ajout de gradP
  eqn.corriger_derivee_expl(vpoint);

  const Champ_base& rho_vit=eqn.get_champ("rho_comme_v");
  ref_cast_non_const(DoubleTab,rho_vit.valeurs())=tab_rho_face_np1;

  if (eqn.schema_temps().diffusion_implicite())
    {
      DoubleTrav secmemV(vpoint);
      secmemV = vpoint;
      double dt = eqn.schema_temps().pas_de_temps();
      /*
       * secmemV contient M(rhonp1 unp1 -rhon un)/dt
       * M-1 secmemv*dt+rhon un -rhonp1un= rhonp1 (unp1 -unp )
       * dt/rhnhonp1 =(unp1-un)/dt
       * M-1 secmemV/rhonp1 + (rhon  -rhonp1)/rhonp1/dt  un
       *
       * on modifie le solveur masse pour diviser par rhonp1
       * (pratique aussi pour la diffusion implicite)
       */

      eqn.solv_masse()->set_name_of_coefficient_temporel("rho_comme_v");
      eqn.solv_masse().appliquer(secmemV);
      DoubleTrav dr(tab_rho_face_n);
      for (int i=0; i<dr.size_totale(); i++) dr(i)=(tab_rho_face_n(i)/tab_rho_face_np1(i)-1.)/dt;

      // on sert de vpoint pour calculer
      rho_vitesse_impl(dr,vit,vpoint);
      secmemV += vpoint;

      DoubleTab delta_u(eqn.inconnue().futur());
      eqn.Gradient_conjugue_diff_impl(secmemV, delta_u ) ;

      /*
       * delta_u=unp1 -un => delta_u + un=unp1
       * (delat_u + un)*rhonp1 = rhonp1 * unp1
       * (delat_u + un)*rhonp1  - rhon * un= rhonp1 * unp1 - rhon * un
       */

      delta_u *= dt;
      delta_u += vit;
      rho_vitesse_impl(tab_rho_face_np1,delta_u,vpoint);
      vpoint -= rhovitesse;
      vpoint /= dt;
      eqn.solv_masse()->set_name_of_coefficient_temporel("no_coeff");
    }
  else eqn.solv_masse().appliquer(vpoint);

  update_vpoint_on_boundaries(eqn,fluide_dil,vpoint);

} /* END prepare_and_solve_u_star */

void Navier_Stokes_Fluide_Dilatable_Proto::update_vpoint_on_boundaries(const Navier_Stokes_std& eqn,
                                                                       const Fluide_Dilatable_base& fluide_dil,
                                                                       DoubleTab& vpoint)
{
  // on ajoute durho/dt au bord Dirichlet car les solveur masse a mis a zero
  // NOTE : en incompressible le terme est rajoute par modifier_secmem
  const double dt_ = eqn.schema_temps().pas_de_temps();
  const DoubleTab& tab_rho_face_n =fluide_dil.rho_face_n(), tab_rho_face_np1=fluide_dil.rho_face_np1();
  const DoubleTab& vit = eqn.vitesse().valeurs();
  const Conds_lim& lescl=eqn.zone_Cl_dis().les_conditions_limites();
  const IntTab& face_voisins = eqn.zone_dis().valeur().face_voisins();
  const int nbcondlim = lescl.size(), taille = vpoint.line_size();

  if (taille==1)
    if (orientation_VDF_.size() == 0)
      orientation_VDF_.ref(ref_cast(Zone_VF,eqn.zone_dis().valeur()).orientation());

  for (int icl=0; icl<nbcondlim; icl++)
    {
      const Cond_lim_base& la_cl_base = lescl[icl].valeur();
      if (sub_type(Dirichlet,la_cl_base))
        {
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl_base.frontiere_dis());
          const Dirichlet& diri=ref_cast(Dirichlet,la_cl_base);
          const int ndeb = la_front_dis.num_premiere_face(), nfin = ndeb + la_front_dis.nb_faces();

          if (taille==1) // VDF //
            {
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  int n0 = face_voisins(num_face, 0);
                  if (n0 == -1) n0 = face_voisins(num_face, 1);

                  // GF en cas de diffsion implicite vpoint!=0 on ignrore l'ancienne valeur
                  vpoint(num_face)=(diri.val_imp(num_face-ndeb,orientation_VDF_(num_face))*tab_rho_face_np1(num_face)-
                                    vit(num_face)*tab_rho_face_n(num_face))/dt_;
                }
            }
          else // VEF //
            {
              for (int num_face=ndeb; num_face<nfin; num_face++)
                for (int jj=0; jj<Objet_U::dimension; jj++)
                  {
                    // GF en cas de diffsion implicite vpoint!=0 on ignrore l'ancienne valeur
                    vpoint(num_face,jj)=(tab_rho_face_np1(num_face)*diri.val_imp(num_face-ndeb,jj)
                                         -tab_rho_face_n(num_face)*vit(num_face,jj))/dt_;
                  }
            }
        }
    }

} /* END update_vpoint_on_boundaries */

void Navier_Stokes_Fluide_Dilatable_Proto::solve_pressure_increment(Navier_Stokes_std& eqn,
                                                                    const Fluide_Dilatable_base& fluide_dil,
                                                                    DoubleTab& rhoU, DoubleTab& secmem,
                                                                    DoubleTab& inc_pre, DoubleTab& vpoint)
{
  const DoubleTab& tab_rho_face_n =fluide_dil.rho_face_n(), tab_rho_face_np1=fluide_dil.rho_face_np1();
  const DoubleTab& vit = eqn.vitesse().valeurs();
  const double dt_ = eqn.schema_temps().pas_de_temps(),t = eqn.schema_temps().temps_courant();

  // Resolution pression
  vpoint.echange_espace_virtuel();

  // Compute rhoU(n) :
  rho_vitesse_impl(tab_rho_face_n,vit,rhoU);

  // Add source term to vpoint if interfaces
  Probleme_base& prob=eqn.probleme();
  DoubleTrav vpoint0(vpoint) ;
  vpoint0 = vpoint ;
  for (int i=0; i<prob.nombre_d_equations(); i++)
    if (sub_type(Transport_Interfaces_base,prob.equation(i)))
      {
        Transport_Interfaces_base& eq_transport = ref_cast(Transport_Interfaces_base,prob.equation(i));
        const int nb = vpoint.dimension(0), m = vpoint.line_size();
        DoubleTab source_ibc(nb,m);

        //On ajoute un terme source a vpoint pour imposer au fluide la vitesse de l interface
        //source_ibc est local pas postraitable (different cas FT ou le terme source est defini et peut etre postraite)
        eq_transport.modifier_vpoint_pour_imposer_vit(rhoU,vpoint0,vpoint,tab_rho_face_np1,source_ibc,t,dt_);
      }

  secmem = tab_W;
  operator_negate(secmem);
  eqn.operateur_divergence().ajouter(rhoU,secmem);
  secmem /= dt_; // (-tabW + Div(rhoU))/dt

  eqn.operateur_divergence().ajouter(vpoint, secmem);
  secmem *= -1;
  secmem.echange_espace_virtuel();
  Debog::verifier("Navier_Stokes_Fluide_Dilatable_base::derivee_en_temps_inco, secmem : ", secmem);

  // On ne fait appel qu une seule fois a assembler dans preparer calcul (au lieu de assembler_QC)
  // Correction du second membre d'apres les conditions aux limites :
  eqn.assembleur_pression().modifier_secmem(secmem);
  eqn.solveur_pression().resoudre_systeme(eqn.matrice_pression().valeur(),secmem,inc_pre);

} /* END  solve_pressure_increment */

void Navier_Stokes_Fluide_Dilatable_Proto::correct_and_compute_u_np1(Navier_Stokes_std& eqn,
                                                                     const Fluide_Dilatable_base& fluide_dil,
                                                                     DoubleTab& rhoU,DoubleTab& Mmoins1grad,
                                                                     DoubleTab& inc_pre,DoubleTab& gradP,
                                                                     DoubleTab& vpoint)
{
  const DoubleTab& tab_rho_face_np1=fluide_dil.rho_face_np1();
  const DoubleTab& vit = eqn.vitesse().valeurs();
  DoubleTab& press = eqn.pression().valeurs();
  const double dt_ = eqn.schema_temps().pas_de_temps();

  // On a besoin de l'espace virtuel de la pression pour calculer le gradient plus bas
  // et modifier_solution ne fait pas toujours l'echange_espace_virtuel.
  // On suppose que pression et inc_pre ont leur espace virtuel a jour
  // On fait pression += inc_pre:
  operator_add(press, inc_pre, VECT_ALL_ITEMS);
  eqn.assembleur_pression().modifier_solution(press);

  // Correction de la vitesse en pression : M-1 Bt P
  eqn.solv_masse().appliquer(gradP);
  vpoint += gradP; // M-1 F

  press.echange_espace_virtuel();
  eqn.operateur_gradient().calculer(press, gradP);

  // On conserve Bt P pour la prochaine fois.
  Mmoins1grad = gradP;
  eqn.solv_masse().appliquer(Mmoins1grad);

  // Correction en pression
  vpoint -= Mmoins1grad;

  // vpoint = (rhoU(n+1)-rhoU(n))/dt
  vpoint *= dt_;
  vpoint += rhoU; // rhoU(n+1)

  // Compute U(n+1):
  const int n = vpoint.dimension(0), ncomp = vpoint.line_size();
  for (int i=0 ; i<n ; i++)
    for (int j=0 ; j< ncomp; j++) vpoint(i,j) /= tab_rho_face_np1(i);

  // Compute (U(n+1)-U(n))/dt :
  vpoint -= vit;
  vpoint /= dt_;

  vpoint.echange_espace_virtuel();
  Debog::verifier("Navier_Stokes_Fluide_Dilatable_base::derivee_en_temps_inco, vpoint : ", vpoint);

} /* END correct_and_compute_u_np1 */

