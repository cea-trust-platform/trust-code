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
// File:        Navier_Stokes_QC_impl.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/47
//
//////////////////////////////////////////////////////////////////////////////

#include <Navier_Stokes_QC_impl.h>
#include <Navier_Stokes_std.h>
#include <Fluide_Quasi_Compressible.h>
#include <Debog.h>
#include <Dirichlet.h>
#include <Zone_VF.h>
#include <Schema_Temps_base.h>
#include <Schema_Temps.h>
#include <DoubleTrav.h>
#include <Transport_Interfaces_base.h>
#include <Probleme_base.h>
#include <Matrice_Morse.h>
#include <Domaine.h>

// Description:
// Multiply density by velocity and return density*velocity (mass flux)
DoubleTab& Navier_Stokes_QC_impl::rho_vitesse_impl(const DoubleTab& tab_rho,const DoubleTab& vitesse,DoubleTab& rhovitesse) const
{
  int i,j, n = vitesse.dimension(0);
  if (vitesse.nb_dim()==1)
    {
      for (i=0 ; i<n ; i++)
        {
          rhovitesse(i) = tab_rho(i)*vitesse(i);
        }
    }
  else
    {
      for (i=0 ; i<n ; i++)
        {
          for (j=0 ; j<Objet_U::dimension ; j++)
            {
              rhovitesse(i,j) = tab_rho(i)*vitesse(i,j);
            }
        }
    }
  rhovitesse.echange_espace_virtuel();
  Debog::verifier("Navier_Stokes_QC::rho_vitesse : ", rhovitesse);

  return rhovitesse;
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
DoubleTab& Navier_Stokes_QC_impl::derivee_en_temps_inco(Navier_Stokes_std& eqn,DoubleTab& vpoint, Fluide_Incompressible& le_fluide,const Matrice& matrice_pression_,Assembleur& assembleur_pression_ ,int diffusion_implicite)
{
  int n = vpoint.dimension(0);
  DoubleTab& pression=eqn.pression().valeurs();
  DoubleTab& vitesse=eqn.vitesse().valeurs();
  DoubleTab secmem(pression);
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
  DoubleTrav inc_pre(pression);


  if (!tab_W.get_md_vector().non_nul())
    {
      tab_W.copy(secmem, Array_base::NOCOPY_NOINIT); // copie la structure
      // initialisation sinon plantage assert lors du remplissage dans EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1B
      tab_W = 0.;
    }
  //Correction pour terme source
  DoubleTab rhoU;
  rhoU.copy(vitesse, Array_base::NOCOPY_NOINIT); // copie la structure

  Fluide_Quasi_Compressible& fluide_QC=ref_cast(Fluide_Quasi_Compressible,le_fluide);
  const DoubleTab& tab_rho_face_n =fluide_QC.rho_face_n();
  const DoubleTab& tab_rho_face_np1=fluide_QC.rho_face_np1();
  //renvoie rho avec la meme discretisation que la vitesse
  //ainsi que dZ/dt et Z(n+1/2)  avec Z=ln(rho)
  const DoubleTab& tab_rho = fluide_QC.rho_discvit();

  fluide_QC.secmembre_divU_Z(tab_W);
  vpoint=0;

  //Resolution vitesse
  //ajout diffusion (avec la viscosite dynamique)
  if (!diffusion_implicite)
    eqn.operateur(0).ajouter(vpoint);

  vpoint.echange_espace_virtuel();

  DoubleTab& rhovitesse = ref_cast_non_const(DoubleTab,eqn.rho_la_vitesse().valeurs());
  rho_vitesse_impl(tab_rho,vitesse,rhovitesse);

  //ajout convection utilise rhovitesse
  if (!diffusion_implicite)
    eqn.operateur(1).ajouter(rhovitesse,vpoint);
  else
    {
      DoubleTrav trav(vpoint);
      eqn.derivee_en_temps_conv(trav,rhovitesse);
      vpoint=trav;
    }
  vpoint.echange_espace_virtuel();

  //ajout source
  eqn.sources().ajouter(vpoint);
  vpoint.echange_espace_virtuel();
  // ajout de gradP
  eqn.corriger_derivee_expl(vpoint);
  assert(diffusion_implicite==eqn.schema_temps().diffusion_implicite());
  const Champ_base& rho_vit=eqn.get_champ("rho_comme_v");
  ref_cast_non_const(DoubleTab,rho_vit.valeurs())=tab_rho_face_np1;
  if (diffusion_implicite)
    {
      DoubleTrav secmemV(vpoint);
      secmemV=vpoint;
      double dt=eqn.schema_temps().pas_de_temps();
      // secmemV contient M(rhonp1 unp1 -rhon un)/dt
      //  M-1 secmemv*dt+rhon un -rhonp1un= rhonp1 (unp1 -unp )
      // %/ dt/rhnhonp1 =(unp1-un)/dt
      //M-1 secmemV/rhonp1 + (rhon  -rhonp1)/rhonp1/dt  un
      // on modifie le solveur masse pour divier par rhonp1 (pratique aussi pour la diffusion implicite)
      eqn.solv_masse()->set_name_of_coefficient_temporel("rho_comme_v");
      eqn.solv_masse().appliquer(secmemV);
      DoubleTrav dr(tab_rho_face_n);
      for (int i=0; i<dr.size_totale(); i++)
        dr(i)=(tab_rho_face_n(i)/tab_rho_face_np1(i)-1.)/dt;
      // on sert de vpoint pour calculer
      rho_vitesse_impl(dr,vitesse,vpoint);
      secmemV+=vpoint;
      DoubleTab delta_u(eqn.inconnue().futur());

      eqn.Gradient_conjugue_diff_impl(secmemV, delta_u ) ;
      // delta_u=unp1 -un
      // delta_u+ un=unp1
      // (delat_u+un)*rhonp1 =rhonp1 unp1
      // (delat_u+un)*rhonp1 -rhon un=rhonp1 unp1_rhonun
      delta_u*=dt;
      delta_u+=vitesse;
      rho_vitesse_impl(tab_rho_face_np1,delta_u,vpoint);
      vpoint-=rhovitesse;
      vpoint /= dt;
      eqn.solv_masse()->set_name_of_coefficient_temporel("no_coeff");
    }
  else
    eqn.solv_masse().appliquer(vpoint);

  double dt_ = eqn.schema_temps().pas_de_temps();
  double t = eqn.schema_temps().temps_courant();

  // on ajoute durho/dt au bord Dirichlet car les solveur masse a mis a zero
  // en incompressible le terme est rajoute par modifier_secmem

  const Conds_lim& lescl=eqn.zone_Cl_dis().les_conditions_limites();
  const IntTab& face_voisins = eqn.zone_dis().valeur().face_voisins();
  int nbcondlim=lescl.size();
  int taille=vpoint.nb_dim();
  if (taille==1)
    {
      if (orientation_VDF_.size()==0)
        orientation_VDF_.ref(ref_cast(Zone_VF,eqn.zone_dis().valeur()).orientation());
    }
  for (int icl=0; icl<nbcondlim; icl++)
    {
      const Cond_lim_base& la_cl_base = lescl[icl].valeur();
      if (sub_type(Dirichlet,la_cl_base))
        {

          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl_base.frontiere_dis());
          const Dirichlet& diri=ref_cast(Dirichlet,la_cl_base);
          int ndeb = la_front_dis.num_premiere_face();
          int nfin = ndeb + la_front_dis.nb_faces();

          if (taille==1)
            {
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  int n0 = face_voisins(num_face, 0);

                  if (n0 == -1)
                    n0 = face_voisins(num_face, 1);
                  // GF en cas de diffsion implicite vpoint!=0 on ignrore l'ancienne valeur
                  // assert(vpoint(num_face)==0);
                  vpoint(num_face)=(diri.val_imp(num_face-ndeb,orientation_VDF_(num_face))*tab_rho_face_np1(num_face)- vitesse(num_face)*tab_rho_face_n(num_face))/dt_;
                }
            }
          else
            {
              for (int num_face=ndeb; num_face<nfin; num_face++)
                for (int jj=0; jj<Objet_U::dimension; jj++)
                  {
                    // GF en cas de diffsion implicite vpoint!=0 on ignrore l'ancienne valeur
                    //assert(vpoint(num_face,jj)==0);
                    vpoint(num_face,jj)=(tab_rho_face_np1(num_face)*diri.val_imp(num_face-ndeb,jj)
                                         -tab_rho_face_n(num_face)*vitesse(num_face,jj))/dt_;
                  }
            }
        }
    }

  //Resolution pression
  vpoint.echange_espace_virtuel();

  // Compute rhoU(n) :
  rho_vitesse_impl(tab_rho_face_n,vitesse,rhoU);

  // Add source term to vpoint if interfaces
  Probleme_base& probleme=eqn.probleme();
  DoubleTrav vpoint0(vpoint) ;
  vpoint0 = vpoint ;
  for (int i=0; i<probleme.nombre_d_equations(); i++)
    if (sub_type(Transport_Interfaces_base,probleme.equation(i)))
      {
        Transport_Interfaces_base& eq_transport = ref_cast(Transport_Interfaces_base,probleme.equation(i));
        const int nb = vpoint.dimension(0);
        const int nbdim1 = (vpoint.nb_dim() == 1);
        const int m = (nbdim1 ? 0 : vpoint.dimension(1));
        DoubleTab source_ibc(nb);
        if (m!=0)
          source_ibc.resize(nb,m);

        //On ajoute un terme source a vpoint pour imposer au fluide la vitesse de l interface
        //source_ibc est local pas postraitable (different cas FT ou le terme source est defini et peut etre postraite)
        eq_transport.modifier_vpoint_pour_imposer_vit(rhoU,vpoint0,vpoint,tab_rho_face_np1,source_ibc,t,dt_);
      }

  secmem = tab_W;
  operator_negate(secmem);
  secmem.echange_espace_virtuel();
  eqn.operateur_divergence().ajouter(rhoU,secmem);
  secmem /= dt_; // (-tabW + Div(rhoU))/dt

  eqn.operateur_divergence().ajouter(vpoint, secmem);
  secmem *= -1;
  secmem.echange_espace_virtuel();

  //On ne fait appel qu une seule fois a assembler dans preparer calcul (au lieu de assembler_QC)
  // Correction du second membre d'apres les conditions aux limites :
  assembleur_pression_.modifier_secmem(secmem);

  eqn.solveur_pression().resoudre_systeme(matrice_pression_.valeur(),
                                          secmem,inc_pre);
  // On a besoin de l'espace virtuel de la pression pour calculer le gradient plus bas
  // et modifier_solution ne fait pas toujours l'echange_espace_virtuel.
  // On suppose que pression et inc_pre ont leur espace virtuel a jour
  // On fait pression += inc_pre:
  operator_add(pression, inc_pre, VECT_ALL_ITEMS);

  assembleur_pression_.modifier_solution(pression);


  // Correction de la vitesse en pression
  // M-1 Bt P
  eqn.solv_masse().appliquer(gradP);
  vpoint += gradP; // M-1 F

  pression.echange_espace_virtuel();
  eqn.operateur_gradient().calculer(pression, gradP);
  // On conserve Bt P pour la prochaine fois.
  DoubleTrav Mmoins1grad(gradP);
  Mmoins1grad = gradP;
  eqn.solv_masse().appliquer(Mmoins1grad);

  // Correction en pression
  vpoint -= Mmoins1grad;

  vpoint.echange_espace_virtuel();
  // vpoint=(rhoU(n+1)-rhoU(n))/dt
  vpoint*=dt_;
  vpoint+=rhoU; // rhoU(n+1)

  // Compute U(n+1):
  if (vpoint.nb_dim()==1)
    {
      for (int i=0 ; i<n ; i++)
        vpoint(i) /= tab_rho_face_np1(i);
    }
  else
    {
      for (int i=0 ; i<n ; i++)
        for (int j=0 ; j<Objet_U::dimension ; j++)
          vpoint(i,j) /= tab_rho_face_np1(i);
    }
  // Compute (U(n+1)-U(n))/dt :
  vpoint-=vitesse;
  vpoint/=dt_;

  vpoint.echange_espace_virtuel();
  Debog::verifier("Navier_Stokes_QC::derivee_en_temps_inco, vpoint : ", vpoint);

  return vpoint;

}

//void calculer_rho_np1_vitesse(const Equation_base& eqn,const DoubleTab& vitesse,DoubleTab& resu);
void Navier_Stokes_QC_impl::assembler_impl(const Navier_Stokes_std& eqn,Matrice_Morse& matrice, const DoubleTab& present, DoubleTab& resu)
{
  Cerr<<"  Navier_Stokes_QC_impl::assembler_impl ne doit pas etre utilise. COntatctez la maintenance."<<finl;
  Process::exit();
}
void Navier_Stokes_QC_impl::assembler_avec_inertie_impl(const Navier_Stokes_std& eqn,Matrice_Morse& matrice, const DoubleTab& present, DoubleTab& resu)
{

  // assemblage special NS_QC
  // avant inertie

  // diffusion en div(mu grad u ) or on veut impliciter en rho u
  // on divise les contributions par le rho_face associe
  // GF on ajoute apres avoir contribuer pour avoir les bons flux bords
  DoubleTrav rhovitesse(present);
  eqn.operateur(0).l_op_base().contribuer_a_avec(present,matrice);

  eqn.operateur(0).ajouter(resu);
  const Fluide_Quasi_Compressible& fluide_QC=ref_cast(Fluide_Quasi_Compressible,eqn.milieu());
  const DoubleTab& tab_rho_face_np1=fluide_QC.rho_face_np1();
  const DoubleTab& tab_rho_face_n=fluide_QC.rho_face_n();
  int nb_compo = 1;
  if (present.nb_dim() == 2) nb_compo = present.dimension(1);
  const IntVect& tab1= matrice.get_tab1();
  const IntVect& tab2= matrice.get_tab2();
  DoubleVect& coeff=matrice.get_set_coeff();
  for (int i=0; i<matrice.nb_lignes(); i++)
    {
      for (int k=tab1(i)-1; k<tab1(i+1)-1; k++)
        {
          int j=tab2(k)-1;
          double rapport=tab_rho_face_np1(j/nb_compo);
          coeff(k)/=rapport;
        }
    }
  // calculer_rho_np1_vitesse(eqn,present,rhovitesse);
  rho_vitesse_impl(tab_rho_face_np1,present,rhovitesse);

  eqn.operateur(1).l_op_base().contribuer_a_avec(rhovitesse,matrice);
  eqn.operateur(1).ajouter(rhovitesse,resu);
  eqn.sources().ajouter(resu);
  eqn.sources().contribuer_a_avec(present,matrice);
  // on resout en rho u on stocke donc rho u dans present
  // calculer_rho_np1_vitesse(eqn,present,present);
  rho_vitesse_impl(tab_rho_face_np1,present,ref_cast_non_const(DoubleTab,present));
  matrice.ajouter_multvect(present,resu);

  // contribution a la matrice de l'inertie
  // en attenddant de faire mieux
  // on divisie la diagonale par rhon+1 face
  // on ajoute l'inertiede facon standard
  // on remultiplie la diagonale par rhon+1
  // ajout de l'inertie
  const double& dt=eqn.schema_temps().pas_de_temps();
  eqn.solv_masse().ajouter_masse(dt,matrice,0);

  // calculer_rho_n_vitesse(eqn,passe,rhovitesse);
  rho_vitesse_impl(tab_rho_face_n,eqn.inconnue().passe(),rhovitesse);
  eqn.solv_masse().ajouter_masse(dt,resu,rhovitesse,0);


  // blocage_cl faux si dirichlet u!=0 !!!!!! manque multiplication par rho
  for (int op=0; op< eqn.nombre_d_operateurs(); op++)
    eqn.operateur(op).l_op_base().modifier_pour_Cl(matrice,resu);
  // correction finale pour les dirichlets
  // on ne doit pas imposer un+1 mais rho_un+1
  // on multiplie dons le resu par rho_face_np1
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
              if (present.nb_dim()==1)
                resu(num_face)*=tab_rho_face_np1(num_face);
              else
                for (int dir=0; dir<Objet_U::dimension; dir++)
                  resu(num_face,dir)*=tab_rho_face_np1(num_face);
            }
        }
    }

}
int Navier_Stokes_QC_impl::impr_impl(const Navier_Stokes_std& eqn ,Sortie& os) const
{
  {
    const DoubleTab& vitesse=eqn.vitesse().valeurs();

    const Fluide_Quasi_Compressible& fluide_QC=ref_cast(Fluide_Quasi_Compressible,eqn.fluide());
    DoubleTab mass_flux(vitesse);
    const DoubleTab& rho=fluide_QC.rho_face_np1();
    rho_vitesse_impl(rho,vitesse,mass_flux);
    DoubleTab array;
    array.copy(eqn.div().valeurs(), Array_base::NOCOPY_NOINIT); // init structure uniquement
    if (tab_W.get_md_vector().non_nul())
      {
        // tab_W initialise si on passe dans
        operator_egal(array, tab_W ); //, VECT_REAL_ITEMS);
        array*=-1;
      }
    else
      {
        // remarque B.M.: certaines implementations de cette methode ne font pas echange espace virtuel:
        fluide_QC.secmembre_divU_Z(array);
        array*=-1;
      }

    array.echange_espace_virtuel();
    eqn.operateur_divergence().ajouter(mass_flux, array);
    double LocalMassFlowRateError = mp_max_abs_vect(array); // max|sum(rho*u*ndS)|
    os << "-------------------------------------------------------------------"<< finl;
    os << "Cell balance mass flow rate control for the problem " << eqn.probleme().le_nom() << " : " << finl;
    os << "Absolute value : " << LocalMassFlowRateError << " kg/s" << finl; ;
    eqn.operateur_divergence().volumique(array); // On divise array par vol(i)
    // On divise par un rho moyen
    double rho_moyen = mp_moyenne_vect(rho);
    double dt = eqn.probleme().schema_temps().pas_de_temps();
    double bilan_massique_relatif = mp_max_abs_vect(array) * dt / rho_moyen;
    os << "Relative value : " << bilan_massique_relatif << finl; // =max|LocalMassFlowRateError/(rho_moyen*Vol(i)/dt)|

    // Calculation as OpenFOAM: http://foam.sourceforge.net/docs/cpp/a04190_source.html
    // It is relative errors (normalized by the volume/dt)
    double TotalMass = rho_moyen * eqn.probleme().domaine().zone(0).volume_total();
    double local = LocalMassFlowRateError / ( TotalMass / dt );
    double global = mp_somme_vect(array) / ( TotalMass / dt );
    cumulative_ += global;
    os << "time step continuity errors : sum local = " << local << ", global = " << global << ", cumulative = " << cumulative_ << finl;

    // Nouveau 1.6.8, arret si bilans de masse mauvais
    // Commente car plusieurs cas tests plantent en implicite, peut etre un probleme
    // du calcul du bilan massique en implicite
    if (local>0.01)
      {
        Cerr << "The mass balance is too bad (relative value>1%)." << finl;
        Cerr << "Please check and lower the convergence value of the pressure solver." << finl;
        Process::exit();
      }
  }
  return 1;
}
