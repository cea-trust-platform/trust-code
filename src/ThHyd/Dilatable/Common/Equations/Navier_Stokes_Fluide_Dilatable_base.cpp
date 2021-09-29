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
// File:        Navier_Stokes_Fluide_Dilatable_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Equations
// Version:     /main/47
//
//////////////////////////////////////////////////////////////////////////////

#include <Navier_Stokes_Fluide_Dilatable_base.h>
#include <Fluide_Dilatable_base.h>
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
#include <Discret_Thyd.h>
#include <Operateur_Diff_base.h>
#include <Param.h>

Implemente_base_sans_constructeur(Navier_Stokes_Fluide_Dilatable_base,"Navier_Stokes_Fluide_Dilatable_base",Navier_Stokes_std);

Navier_Stokes_Fluide_Dilatable_base::Navier_Stokes_Fluide_Dilatable_base() : cumulative_(0) {}

Sortie& Navier_Stokes_Fluide_Dilatable_base::printOn(Sortie& is) const
{
  return Navier_Stokes_std::printOn(is);
}

Entree& Navier_Stokes_Fluide_Dilatable_base::readOn(Entree& is)
{
  Navier_Stokes_std::readOn(is);
  divergence.set_description((Nom)"Mass flow rate=Integral(rho*u*ndS) [kg.s-1]");
  return is;
}

int Navier_Stokes_Fluide_Dilatable_base::impr(Sortie& os) const
{
  {
    const DoubleTab& vit=vitesse().valeurs();

    const Fluide_Dilatable_base& fluide_dil=ref_cast(Fluide_Dilatable_base,fluide());
    DoubleTab mass_flux(vit);
    const DoubleTab& rho=fluide_dil.rho_face_np1();
    rho_vitesse(rho,vit,mass_flux);
    DoubleTab array;
    array.copy(div().valeurs(), Array_base::NOCOPY_NOINIT); // init structure uniquement
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
    operateur_divergence().ajouter(mass_flux, array);
    double LocalMassFlowRateError = mp_max_abs_vect(array); // max|sum(rho*u*ndS)|

    os << "-------------------------------------------------------------------"<< finl;
    os << "Cell balance mass flow rate control for the problem " << probleme().le_nom() << " : " << finl;
    os << "Absolute value : " << LocalMassFlowRateError << " kg/s" << finl; ;

    // On divise array par vol(i)
    operateur_divergence().volumique(array);

    // On divise par un rho moyen
    double rho_moyen = mp_moyenne_vect(rho);
    double dt = probleme().schema_temps().pas_de_temps();
    double bilan_massique_relatif = mp_max_abs_vect(array) * dt / rho_moyen;
    os << "Relative value : " << bilan_massique_relatif << finl; // =max|LocalMassFlowRateError/(rho_moyen*Vol(i)/dt)|

    // Calculation as OpenFOAM: http://foam.sourceforge.net/docs/cpp/a04190_source.html
    // It is relative errors (normalized by the volume/dt)
    double TotalMass = rho_moyen * probleme().domaine().zone(0).volume_total();
    double local = LocalMassFlowRateError / ( TotalMass / dt );
    double global = mp_somme_vect(array) / ( TotalMass / dt );
    cumulative_ += global;

    os << "time step continuity errors : sum local = " << local << ", global = " << global << ", cumulative = " << cumulative_ << finl;

    if (local>0.01)
      {
        Cerr << "The mass balance is too bad (relative value>1%)." << finl;
        Cerr << "Please check and lower the convergence value of the pressure solver." << finl;
        Process::exit();
      }
  }
  return Navier_Stokes_std::impr(os);
}

// Description:
//     cf Equation_base::preparer_calcul()
//     Assemblage du solveur pression et
//     initialisation de la pression.
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
int Navier_Stokes_Fluide_Dilatable_base::preparer_calcul()
{
  return Navier_Stokes_std::preparer_calcul();
}

// Description:
//    Complete l'equation base,
//    associe la pression a l'equation,
//    complete la divergence, le gradient et le solveur pression.
//    Ajout de 2 termes sources: l'un representant la force centrifuge
//    dans le cas axi-symetrique,l'autre intervenant dans la resolution
//    en 2D axisymetrique
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
// Postcondition:
void Navier_Stokes_Fluide_Dilatable_base::completer()
{
  Cerr<<"Navier_Stokes_std::completer"<<finl;
  Navier_Stokes_std::completer();
  Cerr<<"Unknown field type : " << inconnue()->que_suis_je() << finl;
  Cerr<<"Unknown field name : " << inconnue()->le_nom() << finl;
  Cerr<<"Equation type : " << inconnue()->equation().que_suis_je() << finl;
}

const Champ_Don& Navier_Stokes_Fluide_Dilatable_base::diffusivite_pour_transport()
{
  return le_fluide->viscosite_dynamique();
}

const Champ_base& Navier_Stokes_Fluide_Dilatable_base::diffusivite_pour_pas_de_temps()
{
  return le_fluide->viscosite_cinematique();
}

const Champ_base& Navier_Stokes_Fluide_Dilatable_base::vitesse_pour_transport()
{
  return la_vitesse;
}

const Champ_base& Navier_Stokes_Fluide_Dilatable_base::get_champ(const Motcle& nom) const
{
  if (nom=="rho_u")
    return rho_la_vitesse().valeur();
  try
    {
      return Navier_Stokes_std::get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }

  try
    {
      return milieu().get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }
  throw Champs_compris_erreur();
}

bool Navier_Stokes_Fluide_Dilatable_base::initTimeStep(double dt)
{
  DoubleTab& tab_vitesse=inconnue().valeurs();
  Fluide_Dilatable_base& fluide_dil=ref_cast(Fluide_Dilatable_base,le_fluide.valeur());
  const DoubleTab& tab_rho = fluide_dil.rho_discvit();
  DoubleTab& rhovitesse = rho_la_vitesse_.valeurs(); // will be filled
  rho_vitesse(tab_rho,tab_vitesse,rhovitesse);

  return  Navier_Stokes_std::initTimeStep(dt);
}

void Navier_Stokes_Fluide_Dilatable_base::discretiser()
{
  Navier_Stokes_std::discretiser();
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  dis.vitesse(schema_temps(), zone_dis(), rho_la_vitesse_);
  rho_la_vitesse_.nommer("rho_u");
  rho_la_vitesse_.valeur().nommer("rho_u");
}

// Description:
// Multiply density by velocity and return density*velocity (mass flux)
DoubleTab& Navier_Stokes_Fluide_Dilatable_base::rho_vitesse(const DoubleTab& tab_rho,const DoubleTab& vit,DoubleTab& rhovitesse) const
{
  int i,j, n = vit.dimension(0), ls = vit.line_size(), dim = Objet_U::dimension;
  if (ls==1)
    {
      for (i=0 ; i<n ; i++)
        rhovitesse(i) = tab_rho(i)*vit(i);
    }
  else
    {
      for (i=0 ; i<n ; i++)
        for (j=0 ; j<dim ; j++)
          rhovitesse(i,j) = tab_rho(i)*vit(i,j);
    }

  rhovitesse.echange_espace_virtuel();
  Debog::verifier("Navier_Stokes_Fluide_Dilatable_base::rho_vitesse : ", rhovitesse);

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
DoubleTab& Navier_Stokes_Fluide_Dilatable_base::derivee_en_temps_inco(DoubleTab& vpoint)
{
  int n = vpoint.dimension(0);
  DoubleTab& press = pression().valeurs();
  DoubleTab& vit = vitesse().valeurs();
  DoubleTab secmem(press);
  REF(Champ_base) gradient_pression;

  try
    {
      gradient_pression = get_champ("gradient_pression");
    }
  catch (Champs_compris_erreur)
    {
      Cerr<<" l'equation ne comprend pas gradient_pression "<<finl;
      Process::exit();
    }

  DoubleTab& gradP=gradient_pression.valeur().valeurs();
  DoubleTrav inc_pre(press);

  if (!tab_W.get_md_vector().non_nul())
    {
      tab_W.copy(secmem, Array_base::NOCOPY_NOINIT); // copie la structure
      // initialisation sinon plantage assert lors du remplissage dans EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1B
      tab_W = 0.;
    }

  DoubleTab rhoU;
  rhoU.copy(vit, Array_base::NOCOPY_NOINIT); // copie la structure

  Fluide_Dilatable_base& fluide_dil=ref_cast(Fluide_Dilatable_base,le_fluide.valeur());
  const DoubleTab& tab_rho_face_n =fluide_dil.rho_face_n();
  const DoubleTab& tab_rho_face_np1=fluide_dil.rho_face_np1();
  const DoubleTab& tab_rho = fluide_dil.rho_discvit(); // rho avec la meme discretisation que la vitesse

  fluide_dil.secmembre_divU_Z(tab_W); //Calcule W=-dZ/dt, 2nd membre de l'equation div(rhoU) = W
  vpoint=0;

  // ajout diffusion (avec la viscosite dynamique)
  if (!schema_temps().diffusion_implicite())
    operateur(0).ajouter(vpoint);

  DoubleTab& rhovitesse = ref_cast_non_const(DoubleTab,rho_la_vitesse().valeurs());
  rho_vitesse(tab_rho,vit,rhovitesse);

  // ajout convection utilise rhovitesse
  if (!schema_temps().diffusion_implicite())
    operateur(1).ajouter(rhovitesse,vpoint);
  else
    {
      DoubleTrav trav(vpoint);
      derivee_en_temps_conv(trav,rhovitesse);
      vpoint=trav;
    }

  // ajout source
  sources().ajouter(vpoint);

  // ajout de gradP
  corriger_derivee_expl(vpoint);
  const Champ_base& rho_vit=get_champ("rho_comme_v");
  ref_cast_non_const(DoubleTab,rho_vit.valeurs())=tab_rho_face_np1;

  if (schema_temps().diffusion_implicite())
    {
      DoubleTrav secmemV(vpoint);
      secmemV=vpoint;
      double dt=schema_temps().pas_de_temps();
      /*
       * secmemV contient M(rhonp1 unp1 -rhon un)/dt
       * M-1 secmemv*dt+rhon un -rhonp1un= rhonp1 (unp1 -unp )
       * dt/rhnhonp1 =(unp1-un)/dt
       * M-1 secmemV/rhonp1 + (rhon  -rhonp1)/rhonp1/dt  un
       *
       * on modifie le solveur masse pour diviser par rhonp1
       * (pratique aussi pour la diffusion implicite)
       */

      solv_masse()->set_name_of_coefficient_temporel("rho_comme_v");
      solv_masse().appliquer(secmemV);
      DoubleTrav dr(tab_rho_face_n);
      for (int i=0; i<dr.size_totale(); i++)
        dr(i)=(tab_rho_face_n(i)/tab_rho_face_np1(i)-1.)/dt;

      // on sert de vpoint pour calculer
      rho_vitesse(dr,vit,vpoint);
      secmemV+=vpoint;
      DoubleTab delta_u(inconnue().futur());

      Gradient_conjugue_diff_impl(secmemV, delta_u ) ;

      /*
       * delta_u=unp1 -un => delta_u + un=unp1
       * (delat_u + un)*rhonp1 = rhonp1 * unp1
       * (delat_u + un)*rhonp1  - rhon * un= rhonp1 * unp1 - rhon * un
       */

      delta_u*=dt;
      delta_u+=vit;
      rho_vitesse(tab_rho_face_np1,delta_u,vpoint);
      vpoint-=rhovitesse;
      vpoint /= dt;
      solv_masse()->set_name_of_coefficient_temporel("no_coeff");
    }
  else
    solv_masse().appliquer(vpoint);

  double dt_ = schema_temps().pas_de_temps();
  double t = schema_temps().temps_courant();

  // on ajoute durho/dt au bord Dirichlet car les solveur masse a mis a zero
  // NOTE : en incompressible le terme est rajoute par modifier_secmem

  const Conds_lim& lescl=zone_Cl_dis().les_conditions_limites();
  const IntTab& face_voisins = zone_dis().valeur().face_voisins();
  int nbcondlim=lescl.size();
  int taille=vpoint.line_size();
  if (taille==1)
    if (orientation_VDF_.size()==0)
      orientation_VDF_.ref(ref_cast(Zone_VF,zone_dis().valeur()).orientation());

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
                  if (n0 == -1) n0 = face_voisins(num_face, 1);

                  // GF en cas de diffsion implicite vpoint!=0 on ignrore l'ancienne valeur
                  vpoint(num_face)=(diri.val_imp(num_face-ndeb,orientation_VDF_(num_face))*tab_rho_face_np1(num_face)-
                                    vit(num_face)*tab_rho_face_n(num_face))/dt_;
                }
            }
          else
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

  // Resolution pression
  vpoint.echange_espace_virtuel();

  // Compute rhoU(n) :
  rho_vitesse(tab_rho_face_n,vit,rhoU);

  // Add source term to vpoint if interfaces
  Probleme_base& prob=probleme();
  DoubleTrav vpoint0(vpoint) ;
  vpoint0 = vpoint ;
  for (int i=0; i<prob.nombre_d_equations(); i++)
    if (sub_type(Transport_Interfaces_base,prob.equation(i)))
      {
        Transport_Interfaces_base& eq_transport = ref_cast(Transport_Interfaces_base,prob.equation(i));
        const int nb = vpoint.dimension(0);
        const int m = vpoint.line_size();
        DoubleTab source_ibc(nb, m);

        //On ajoute un terme source a vpoint pour imposer au fluide la vitesse de l interface
        //source_ibc est local pas postraitable (different cas FT ou le terme source est defini et peut etre postraite)
        eq_transport.modifier_vpoint_pour_imposer_vit(rhoU,vpoint0,vpoint,tab_rho_face_np1,source_ibc,t,dt_);
      }

  secmem = tab_W;
  operator_negate(secmem);
  operateur_divergence().ajouter(rhoU,secmem);
  secmem /= dt_; // (-tabW + Div(rhoU))/dt

  operateur_divergence().ajouter(vpoint, secmem);
  secmem *= -1;
  secmem.echange_espace_virtuel();
  Debog::verifier("Navier_Stokes_Fluide_Dilatable_base::derivee_en_temps_inco, secmem : ", secmem);

  // On ne fait appel qu une seule fois a assembler dans preparer calcul (au lieu de assembler_QC)
  // Correction du second membre d'apres les conditions aux limites :
  assembleur_pression_.modifier_secmem(secmem);
  solveur_pression().resoudre_systeme(matrice_pression_.valeur(),  secmem,inc_pre);

  // On a besoin de l'espace virtuel de la pression pour calculer le gradient plus bas
  // et modifier_solution ne fait pas toujours l'echange_espace_virtuel.
  // On suppose que pression et inc_pre ont leur espace virtuel a jour
  // On fait pression += inc_pre:
  operator_add(press, inc_pre, VECT_ALL_ITEMS);
  assembleur_pression_.modifier_solution(press);

  // Correction de la vitesse en pression
  // M-1 Bt P
  solv_masse().appliquer(gradP);
  vpoint += gradP; // M-1 F

  press.echange_espace_virtuel();
  operateur_gradient().calculer(press, gradP);

  // On conserve Bt P pour la prochaine fois.
  DoubleTrav Mmoins1grad(gradP);
  Mmoins1grad = gradP;
  solv_masse().appliquer(Mmoins1grad);

  // Correction en pression
  vpoint -= Mmoins1grad;

  // vpoint = (rhoU(n+1)-rhoU(n))/dt
  vpoint*=dt_;
  vpoint+=rhoU; // rhoU(n+1)

  // Compute U(n+1):
  if (vpoint.line_size()==1)
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
  vpoint-=vit;
  vpoint/=dt_;

  vpoint.echange_espace_virtuel();
  Debog::verifier("Navier_Stokes_Fluide_Dilatable_base::derivee_en_temps_inco, vpoint : ", vpoint);

  return vpoint;
}

void Navier_Stokes_Fluide_Dilatable_base::assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem)
{
  Cerr << "Navier_Stokes_Fluide_Dilatable_base::assembler is not coded ! You should use assembler_avec_inertie !" << finl;
  Process::exit();
}

void Navier_Stokes_Fluide_Dilatable_base::assembler_avec_inertie( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem)
{
  // avant inertie

  // diffusion en div(mu grad u ) or on veut impliciter en rho * u
  // on divise les contributions par le rho_face associe
  // GF on ajoute apres avoir contribuer pour avoir les bons flux bords

  DoubleTrav rhovitesse(present);

  // Op diff
  operateur(0).l_op_base().contribuer_a_avec(present,mat_morse);
  operateur(0).ajouter(secmem);

  const Fluide_Dilatable_base& fluide_dil=ref_cast(Fluide_Dilatable_base,milieu());
  const DoubleTab& tab_rho_face_np1=fluide_dil.rho_face_np1();
  const DoubleTab& tab_rho_face_n=fluide_dil.rho_face_n();
  const int nb_compo = present.line_size();
  const IntVect& tab1= mat_morse.get_tab1();
  const IntVect& tab2= mat_morse.get_tab2();

  DoubleVect& coeff=mat_morse.get_set_coeff();
  for (int i=0; i<mat_morse.nb_lignes(); i++)
    for (int k=tab1(i)-1; k<tab1(i+1)-1; k++)
      {
        int j=tab2(k)-1;
        double rapport=tab_rho_face_np1(j/nb_compo);
        coeff(k)/=rapport;
      }

  rho_vitesse(tab_rho_face_np1,present,rhovitesse); // rho*U

  // Op conv
  operateur(1).l_op_base().contribuer_a_avec(rhovitesse,mat_morse);
  operateur(1).ajouter(rhovitesse,secmem);

  // sources
  sources().ajouter(secmem);
  sources().contribuer_a_avec(present,mat_morse);

  // on resout en rho u on stocke donc rho u dans present
  rho_vitesse(tab_rho_face_np1,present,ref_cast_non_const(DoubleTab,present));
  mat_morse.ajouter_multvect(present,secmem);

  /*
   * contribution a la matrice de l'inertie :
   * on divisie la diagonale par rhon+1 face
   * on ajoute l'inertiede facon standard
   * on remultiplie la diagonale par rhon+1
   */

  // ajout de l'inertie
  const double& dt=schema_temps().pas_de_temps();
  solv_masse().ajouter_masse(dt,mat_morse,0);

  rho_vitesse(tab_rho_face_n,inconnue().passe(),rhovitesse);
  solv_masse().ajouter_masse(dt,secmem,rhovitesse,0);

  // blocage_cl faux si dirichlet u!=0 !!!!!! manque multiplication par rho
  for (int op=0; op< nombre_d_operateurs(); op++)
    operateur(op).l_op_base().modifier_pour_Cl(mat_morse,secmem);

  /*
   * correction finale pour les dirichlets
   * on ne doit pas imposer un+1 mais rho_un+1 => on multiplie dons le resu par rho_face_np1
   */
  const Conds_lim& lescl=zone_Cl_dis().les_conditions_limites();
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
              if (present.line_size()==1)
                secmem(num_face)*=tab_rho_face_np1(num_face);
              else
                for (int dir=0; dir<Objet_U::dimension; dir++)
                  secmem(num_face,dir)*=tab_rho_face_np1(num_face);
            }
        }
    }
}
