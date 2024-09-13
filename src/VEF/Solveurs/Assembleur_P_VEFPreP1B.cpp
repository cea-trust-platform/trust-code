/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Assembleur_P_VEFPreP1B.h>
#include <Matrice_Bloc_Sym.h>
#include <Domaine.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Dirichlet_entree_fluide_leaves.h>
#include <Neumann_sortie_libre.h>
#include <Milieu_base.h>
#include <Navier_Stokes_std.h>
#include <TRUSTLists.h>
#include <Op_Grad_VEF_P1B_Face.h>
#include <Op_Div_VEFP1B_Elem.h>
#include <SFichier.h>
#include <Champ_Fonc_P1NC.h>
#include <SolveurPP1B.h>
#include <Check_espace_virtuel.h>
#include <Solv_Petsc.h>
#include <Matrice_Petsc.h>

Implemente_instanciable(Assembleur_P_VEFPreP1B,"Assembleur_P_VEFPreP1B",Assembleur_P_VEF);

// printOn et readOn

Sortie& Assembleur_P_VEFPreP1B::printOn(Sortie& s ) const
{
  return Assembleur_base::printOn(s);
}

Entree& Assembleur_P_VEFPreP1B::readOn(Entree& is )
{
  return Assembleur_base::readOn(is);
}

static double estim_cond(Matrice& A)
{
  int i=A.valeur( ).nb_lignes();
  //Cout << "rang " << i << finl;
  DoubleVect p(A.valeur( ).nb_lignes());
  for(int j=0; j<i; j++)
    p(j)=drand48();
  double normep=sqrt(mp_prodscal(p,p));
  p/=normep;
  DoubleVect r(A.valeur( ).nb_lignes());
  double lmax=1.e-36, lmin=1.e24;
  //double cond=-1;
  i=10000;
  while(i-- )
    {
      //cond=lmax/lmin;
      A.valeur( ).multvect(p,r);
      double pscpr=mp_prodscal(p,r);
      //Cout << "lambda " << i << " : " << pscpr << " , " << lmax/lmin << finl;
      double pscrr=mp_prodscal(r,r);
      double epsilon=-pscpr/pscrr;
      p.ajoute(epsilon,r);
      double normepbis=sqrt(mp_prodscal(p,p));
      //Cout << "normep : " << normep << finl;
      p/=normepbis;
      if(pscpr>lmax)
        lmax=pscpr;
      else if(pscpr<lmin)
        lmin=pscpr;
    }
  //Cout << "lmax " << lmax << " lmin = " << lmin << finl;
  //Cout << p << finl;
  if (A.valeur( ).nb_lignes()<30) A.valeur( ).imprimer_formatte(Cerr);

  return(lmax/lmin);
}

void Assembleur_P_VEFPreP1B::completer(const Equation_base& eqn)
{
  mon_equation=eqn;
  Assembleur_P_VEF::completer(eqn);
  if (domaine_Vef().get_P1Bulle())
    {
      // Pour changer de base et retrouver le P1Bulle
      alpha_=1./Objet_U::dimension;
      beta_=1./(Objet_U::dimension*(Objet_U::dimension+1));
    }
  la_matrice_de_travail_.typer("Matrice_Bloc_Sym");
}

const Domaine_VEF& Assembleur_P_VEFPreP1B::domaine_Vef() const
{
  return ref_cast(Domaine_VEF, le_dom_VEF.valeur());
}

extern void assemblerP0P0(const Domaine_dis_base& z,
                          const Domaine_Cl_dis_base& zcl,
                          Matrice& matrice,
                          const DoubleTab& inverse_quantitee_entrelacee);

extern void assemblerP1P1(const Domaine_dis_base& z,
                          const Domaine_Cl_dis_base& zcl,
                          Matrice& matrice,
                          const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som);

extern void assemblerPaPa(const Domaine_dis_base& z,
                          const Domaine_Cl_dis_base& zcl,
                          Matrice& matrice, const DoubleTab& inverse_quantitee_entrelacee);

extern void assemblerP0P1(const Domaine_dis_base& z,
                          const Domaine_Cl_dis_base& zcl,
                          Matrice& matrice, const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som);

extern void assemblerP0Pa(const Domaine_dis_base& z,
                          const Domaine_Cl_dis_base& zcl,
                          Matrice& matrice,
                          const DoubleTab& inverse_quantitee_entrelacee);

extern  void assemblerP1Pa(const Domaine_dis_base& z,
                           const Domaine_Cl_dis_base& zcl,
                           Matrice& matrice,
                           const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som);

extern void updateP0P0(const Domaine_dis_base& z,
                       const Domaine_Cl_dis_base& zcl,
                       Matrice& matrice,
                       const DoubleTab& inverse_quantitee_entrelacee);

extern void updateP1P1(const Domaine_dis_base& z,
                       const Domaine_Cl_dis_base& zcl,
                       Matrice& matrice,
                       const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som);
extern void modifieP1P1neumann(const Domaine_dis_base& z,
                               const Domaine_Cl_dis_base& zcl,
                               Matrice& matrice,
                               const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som);
extern void updatePaPa(const Domaine_dis_base& z,
                       const Domaine_Cl_dis_base& zcl,
                       Matrice& matrice,
                       const DoubleTab& inverse_quantitee_entrelacee);

extern void updateP0P1(const Domaine_dis_base& z,
                       const Domaine_Cl_dis_base& zcl,
                       Matrice& matrice,
                       const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som);

extern void updateP0Pa(const Domaine_dis_base& z,
                       const Domaine_Cl_dis_base& zcl,
                       Matrice& matrice,
                       const DoubleTab& inverse_quantitee_entrelacee);

extern  void updateP1Pa(const Domaine_dis_base& z,
                        const Domaine_Cl_dis_base& zcl,
                        Matrice& matrice,
                        const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som);

extern int verifier( const Assembleur_P_VEFPreP1B& ass,
                     const Matrice_Bloc_Sym& matrice,
                     const Domaine_VEF& domaine_VEF,
                     const DoubleTab& inverse_quantitee_entrelacee);

int Assembleur_P_VEFPreP1B::assembler(Matrice& la_matrice)
{
  return Assembleur_P_VEF::assembler(la_matrice);
}

int Assembleur_P_VEFPreP1B::assembler_rho_variable(Matrice& la_matrice, const Champ_Don_base& rho)
{
  // On multiplie par la masse volumique aux faces
  if (!sub_type(Champ_Fonc_P1NC, rho))
    {
      Cerr << "La masse volumique n'est pas aux faces dans Assembleur_P_VEFPreP1B::assembler_rho_variable." << finl;
      Process::exit();
    }
  const DoubleVect& volumes_entrelaces=domaine_Vef().volumes_entrelaces();
  const DoubleVect& masse_volumique=rho.valeurs();
  DoubleVect quantitee_entrelacee(volumes_entrelaces);
  int size=quantitee_entrelacee.size_array();
  for (int i=0; i<size; i++)
    quantitee_entrelacee(i)=(volumes_entrelaces(i)*masse_volumique(i));

  // On assemble la matrice
  return assembler_mat(la_matrice,quantitee_entrelacee,1,1);
}

void zero(Matrice_Bloc_Sym& matrice)
{
  int nombre_supports = matrice.ordre();
  for(int i=0; i<nombre_supports; i++)
    for(int j=i; j<nombre_supports; j++)
      {
        Matrice_Bloc& bloc_ij=ref_cast(Matrice_Bloc, matrice.get_bloc(i,j).valeur());
        ref_cast(Matrice_Morse, bloc_ij.get_bloc(0,0).valeur()).clean();
        if(Process::is_parallel())
          {
            ref_cast(Matrice_Morse, bloc_ij.get_bloc(0,1).valeur()).clean();
            ref_cast(Matrice_Morse, bloc_ij.get_bloc(1,0).valeur()).clean();
            ref_cast(Matrice_Morse, bloc_ij.get_bloc(1,1).valeur()).clean();
          }
      }
}



int Assembleur_P_VEFPreP1B::assembler_mat(Matrice& la_matrice,const DoubleVect& quantitee_entrelacee, int incr_pression, int resoudre_en_u)
{
  // On fixe les drapeaux de Assembleur_base
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);

  SolveurSys& solveur_pression = ref_cast(Navier_Stokes_std, mon_equation.valeur()).solveur_pression();
  bool read_matrix = false;
  if (sub_type(Solv_Petsc, solveur_pression.valeur()))
    read_matrix = ref_cast(Solv_Petsc, solveur_pression.valeur()).read_matrix();
  if (read_matrix) // Lecture de la matrice dans un fichier
    {
      la_matrice.typer("Matrice_Petsc");
      //ref_cast(Matrice_Petsc, la_matrice.valeur()).RestoreMatrixFromFile();
    }
  else // Assemblage de la matrice
    {
      const Domaine_VEF& domaine_vef = domaine_Vef();
      Cerr << "Assemblage de la matrice de pression" << (domaine_vef.get_alphaE() ? " P0" : "")
           << (domaine_vef.get_alphaS() ? " P1" : "") << (domaine_vef.get_alphaA() ? " Pa" : "") << " en cours..." << finl;

      // Les decoupages doivent etre de largeur de joint de 2
      // si le support P1 ou Pa est utilise...
      if (Process::is_parallel() &&
          domaine_vef.domaine().nb_joints() &&
          domaine_vef.domaine().joint(0).epaisseur() < 2 &&
          (domaine_vef.get_alphaS() || domaine_vef.get_alphaA()))
        {
          Cerr << "Ghost cells width of " << domaine_vef.domaine().joint(0).epaisseur()
               << " is not enough for assembling VEFPreP1B pressure matrix" << finl;
          Cerr << "for parallel calculation. Partition your mesh with larg_joint option set to 2." << finl;
          Process::exit();
        }

      DoubleTab inverse_quantitee_entrelacee;
      const Domaine_Cl_VEF& domaine_Cl_VEF = le_dom_Cl_VEF.valeur();
      calculer_inv_volume(inverse_quantitee_entrelacee, domaine_Cl_VEF, quantitee_entrelacee);
      int P0 = 0;
      int P1 = P0 + domaine_vef.get_alphaE();
      int Pa = P1 + domaine_vef.get_alphaS();
      int nombre_supports = Pa + domaine_vef.get_alphaA();
      DoubleVect coef_som;
      if (domaine_vef.get_alphaS())
        {
          domaine_vef.domaine().creer_tableau_elements(coef_som);
          for (int elem = 0; elem < coef_som.size_totale(); elem++)
            coef_som[elem] = Op_Grad_VEF_P1B_Face::calculer_coef_som(elem, domaine_Cl_VEF, domaine_vef);
          coef_som.echange_espace_virtuel();
        }

      // Assemblage de la matrice complete selon les supports choisis
      Matrice_Bloc_Sym& la_matrice_bloc_sym_de_travail = ref_cast(Matrice_Bloc_Sym, la_matrice_de_travail_.valeur());
      if (la_matrice_bloc_sym_de_travail.nb_bloc_lignes()==0)
        {
          la_matrice_bloc_sym_de_travail.dimensionner(nombre_supports, nombre_supports);
          if (domaine_vef.get_alphaE())
            assemblerP0P0(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(P0, P0),
                          inverse_quantitee_entrelacee);

          if (domaine_vef.get_alphaS())
            assemblerP1P1(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(P1, P1),
                          inverse_quantitee_entrelacee, coef_som);

          if (domaine_vef.get_alphaE() && domaine_vef.get_alphaS())
            assemblerP0P1(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(P0, P1),
                          inverse_quantitee_entrelacee, coef_som);

          if (domaine_vef.get_alphaA())
            assemblerPaPa(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(Pa, Pa),
                          inverse_quantitee_entrelacee);

          if (domaine_vef.get_alphaS() && domaine_vef.get_alphaA())
            assemblerP1Pa(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(P1, Pa),
                          inverse_quantitee_entrelacee, coef_som);

          if (domaine_vef.get_alphaE() && domaine_vef.get_alphaA())
            assemblerP0Pa(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(P0, Pa),
                          inverse_quantitee_entrelacee);
        }
      // On met a zero la matrice meme si elle a ete correctement construite et remplie dans les methodes
      // assemblerPiPi et on la remplit a nouveau. Pourquoi? Pour avoir une couverture de tests
      // suffisante des methodes updatePiPi en attendant de factoriser correctement les
      // methodes assemblerPiPi et updatePiPi
      zero(la_matrice_bloc_sym_de_travail);
      {
        if (domaine_vef.get_alphaE())
          updateP0P0(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(P0, P0),
                     inverse_quantitee_entrelacee);

        if (domaine_vef.get_alphaS())
          {
            updateP1P1(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(P1, P1),
                       inverse_quantitee_entrelacee, coef_som);
            if (domaine_vef.get_cl_pression_sommet_faible() == 0)
              modifieP1P1neumann(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(P1, P1),
                                 inverse_quantitee_entrelacee, coef_som);
          }
        if (domaine_vef.get_alphaE() && domaine_vef.get_alphaS())
          updateP0P1(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(P0, P1),
                     inverse_quantitee_entrelacee, coef_som);

        if (domaine_vef.get_alphaA())
          updatePaPa(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(Pa, Pa),
                     inverse_quantitee_entrelacee);

        if (domaine_vef.get_alphaS() && domaine_vef.get_alphaA())
          updateP1Pa(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(P1, Pa),
                     inverse_quantitee_entrelacee, coef_som);

        if (domaine_vef.get_alphaE() && domaine_vef.get_alphaA())
          updateP0Pa(domaine_vef, domaine_Cl_VEF, la_matrice_bloc_sym_de_travail.get_bloc(P0, Pa),
                     inverse_quantitee_entrelacee);
      }
      trustIdType ordre_matrice = mp_sum(la_matrice_bloc_sym_de_travail.nb_lignes());
      Cerr << "Order of the matrix = " << ordre_matrice << finl;

      // Methode verifier
      char *theValue = getenv("TRUST_VERIFIE_MATRICE_VEF");
      if (theValue != nullptr) verifier(*this, la_matrice_bloc_sym_de_travail, domaine_vef, inverse_quantitee_entrelacee);

      ////////////////////////////////////////////
      // Changement de base eventuel P0P1->P1Bulle
      ////////////////////////////////////////////
      if (changement_base())
        changer_base_matrice(la_matrice_de_travail_); // A->A~

      /////////////////////////////////////////////////////////////
      // Modification de la matrice si pas de pression de reference
      /////////////////////////////////////////////////////////////
      modifier_matrice(la_matrice_de_travail_);

      // Conversion eventuelle en Matrice_Morse_Sym
      if (ref_cast(Navier_Stokes_std, mon_equation.valeur()).solveur_pression()->supporte_matrice_morse_sym() &&
          domaine_vef.get_alphaE() != nombre_supports) // On n'est pas en P0
        {
          //////////////////////////////////////////////////////////
          // La matrice retournee est une Matrice_Morse_Sym nettoyee
          // si le solveur utilisee supporte ce type de matrice
          // et si on n'est pas en P0 seulement (dans ce cas la, la
          // conversion en Mat_Morse_Sym n'apporte rien, et plante le SSOR
          // car la matrice morse contient alors des parties virtuelles
          // alors qu'il n'y a pas d'items communs et on tombe sur
          // l'assert assert(*tab2_ptr<=n); Deux autres solutions:
          // -Modifier le SSOR pour ne resoudre que la partie reelle
          // -Dans le cas de P0 seul, il faudrait vider VV et RV dans la Mat_Bloc_Sym
          //////////////////////////////////////////////////////////
          la_matrice.typer("Matrice_Morse_Sym");
          ref_cast(Matrice_Bloc_Sym, la_matrice_de_travail_.valeur()).BlocSymToMatMorseSym(
            ref_cast(Matrice_Morse_Sym, la_matrice.valeur()));
          ref_cast(Matrice_Morse_Sym, la_matrice.valeur()).compacte(
            2);// Suppression des coefficients nuls et quasi non nuls
        }
      else
        {
          /////////////////////////////////////////////////////////
          // La matrice retournee est une Matrice_Bloc_Sym nettoyee
          /////////////////////////////////////////////////////////
          la_matrice = la_matrice_de_travail_;
          for (int i = 0; i < nombre_supports; i++)
            for (int j = i; j < nombre_supports; j++)
              {
                Matrice_Bloc_Sym& mat_bloc_sym = ref_cast(Matrice_Bloc_Sym, la_matrice.valeur());
                Matrice_Bloc& bloc_ij = ref_cast(Matrice_Bloc, mat_bloc_sym.get_bloc(i, j).valeur());
                ref_cast(Matrice_Morse, bloc_ij.get_bloc(0, 0).valeur()).compacte(
                  2); // Suppression des coefficients nuls et quasi non nuls
              }
        }
    }

  // Si pas deja fait, on prends un solveur (SolveurPP1B) qui fera les changements de base pour la solution et le second membre
  if (changement_base() && solveur_pression->que_suis_je() != "SolveurPP1B")
    {
      SolveurSys solveur_pression_lu = solveur_pression;
      solveur_pression.typer("SolveurPP1B");
      SolveurPP1B& solveur_pression_PP1B = ref_cast(SolveurPP1B, solveur_pression.valeur());
      solveur_pression_PP1B.associer(*this, solveur_pression_lu);
    }

  //////////////////////////////////////////////////////
  // Affichage eventuel du conditionnement de la matrice
  //////////////////////////////////////////////////////
  char* theValue2 = getenv("TRUST_CONDITIONNEMENT_MATRICE");
  if(theValue2 != nullptr)
    Cout << "Estimation du conditionnement de la matrice: " << estim_cond(la_matrice)<<finl;

  return 1;
}

int Assembleur_P_VEFPreP1B::modifier_secmem(DoubleTab& b)
{
  const Domaine_VEF& le_dom = domaine_Vef();

  // Verification sur le support Pa
  if (le_dom.get_alphaA())
    {
      // Verification sur les aretes que:
      const ArrOfInt& renum_arete_perio=le_dom.get_renum_arete_perio();
      const ArrOfInt& ok_arete=le_dom.get_ok_arete();
      int npa=le_dom.numero_premiere_arete();
      // b n'a pas forcement son espace virtuel a jour
      int nb_aretes=le_dom.domaine().nb_aretes();
      ToDo_Kokkos("critical");
      for(int i=0; i<nb_aretes; i++)
        if(!ok_arete[i] && b(npa+i)!=0.) // Les aretes superflues ont une valeur nulle
          {
            Cerr << "Pb div Aretes, la pression sur l'arete " << i << " (qui est superflue) n'est pas nulle." << finl;
            Process::exit();
          }
        else if ( (renum_arete_perio[i]!=i) && b(npa+i)!=0.) // Les aretes periodiques ont une valeur nulle
          {
            Cerr << "Pb div Aretes Perio, la pression sur l'arete " << i << " (qui est periodique) n'est pas nulle." << finl;
            Process::exit();
          }
    }

  // Verification sur le support P1
  if (le_dom.get_alphaS())
    {
      // Verification que la pression sur les sommets periodiques est nulle
      const Domaine& dom=le_dom.domaine();
      int nps=le_dom.numero_premier_sommet();
      int ns=le_dom.domaine().nb_som();
      CIntArrView renum_som_perio = dom.get_renum_som_perio().view_ro();
      CDoubleArrView b_v = static_cast<const DoubleVect&>(b).view_ro();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), ns, KOKKOS_LAMBDA(
                             const int i)
      {
        int k=renum_som_perio(i);
        if((k!=i)&& b_v(nps+i)!=0.)
          {
            printf("Pb div Som, la pression sur le sommet %ld (qui est periodique) n'est pas nulle.\n",(long)i);
            printf("En terme clair, le second membre n'est pas nul sur un sommet periodique.\n");
            if (b_v(nps+i)!=b_v(nps+k))
              {
                printf("En outre, le second membre n'a pas la meme valeur sur les 2 sommets periodiques.\n");
                printf("b(nps+i)=%f <> b(nps+k)=%f\n",b_v(nps+i),b_v(nps+k));
              }
            Kokkos::abort("Il y'a probabilite que le modele utilise soit mal implemente pour\nune condition de periodicite. Contacter le support TRUST.");
          }
      });
      end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
    }

  if (get_resoudre_en_u())
    {
      const Domaine_VEF& domaine_VEF =  domaine_Vef();
      const Domaine_Cl_VEF& domaine_Cl = le_dom_Cl_VEF.valeur();

      const DoubleVect& porosite_face = domaine_Cl.equation().milieu().porosite_face();

      const int nb_cond_lim = domaine_Cl.nb_cond_lim();

      /**************************/
      /* Recuperation de Gpoint */
      /**************************/
      DoubleTrav Gpoint(equation().inconnue().valeurs());
      //Gpoint=0.; Un DoubleTrav initialise a 0
      int Gpoint_nul = 1; // Drapeau pour economiser potentiellement un echange_espace_virtuel
      for (int cond_lim=0; cond_lim<nb_cond_lim; cond_lim++)
        {
          const Cond_lim_base& cl_base = domaine_Cl.les_conditions_limites(cond_lim).valeur();

          const Front_VF& front_VF = ref_cast(Front_VF,cl_base.frontiere_dis());
          const Champ_front_base& champ_front = cl_base.champ_front();

          const int ndeb = front_VF.num_premiere_face();
          const int nfin = ndeb+front_VF.nb_faces();

          /* Test sur la nature du champ au bord du domaine */
          if (sub_type(Entree_fluide_vitesse_imposee, cl_base) && champ_front.instationnaire())
            {
              Gpoint_nul = 0;
              const DoubleTab& gpoint = champ_front.derivee_en_temps();
              bool ch_unif = (gpoint.nb_dim()==1);
              ToDo_Kokkos("To avoid copy");
              for (int num_face=ndeb; num_face<nfin; num_face++)
                for (int dim=0; dim<Objet_U::dimension; dim++)
                  Gpoint(num_face,dim)=porosite_face(num_face) * (ch_unif ? gpoint(dim) : gpoint(num_face-ndeb,dim));
            }
        }

      //Pour le parallele
      if (!Gpoint_nul) Gpoint.echange_espace_virtuel();

      /******************************/
      /* Fin recuperation de Gpoint */
      /******************************/

      /*********************************/
      /* Modification du second membre */
      /*********************************/

      if (domaine_VEF.get_alphaE()) modifier_secmem_elem(Gpoint,b);
      if (domaine_VEF.get_alphaS()) modifier_secmem_som(Gpoint,b);
      if (domaine_VEF.get_alphaA()) modifier_secmem_aretes(Gpoint,b);

      /**********************************/
      /* Fin modification second membre */
      /**********************************/

      b.echange_espace_virtuel();
    }

  return 1;
}

int Assembleur_P_VEFPreP1B::modifier_secmem_elem(const DoubleTab& Gpoint, DoubleTab& b)
{
  const Domaine_VEF& domaine_VEF =  domaine_Vef();
  const Domaine_Cl_VEF& domaine_Cl = le_dom_Cl_VEF.valeur();

  const int nb_cond_lim = domaine_Cl.nb_cond_lim();

  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const DoubleTab& face_normales = domaine_VEF.face_normales();

  for (int cond_lim=0; cond_lim<nb_cond_lim; cond_lim++)
    {
      const Cond_lim_base& cl_base = domaine_Cl.les_conditions_limites(cond_lim).valeur();

      const Front_VF& front_VF = ref_cast(Front_VF,cl_base.frontiere_dis());
      const Champ_front_base& champ_front = cl_base.champ_front();

      /* Test sur la nature du champ au bord du domaine */
      if (sub_type(Entree_fluide_vitesse_imposee, cl_base)  && champ_front.instationnaire() )
        {
          // Construction de la liste des faces a traiter (reelles + virtuelles)
          const int nb_faces_bord_tot = front_VF.nb_faces_tot();
          ToDo_Kokkos("critical");
          for (int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
            {
              const int num_face =  front_VF.num_face(ind_face);
              const int elem = face_voisins(num_face,0);
              assert(elem!=-1);

              for (int dim=0; dim<Objet_U::dimension; dim++)
                b(elem)-=Gpoint(num_face,dim)*face_normales(num_face,dim);
            }
        }
    }

  return 1;
}

int Assembleur_P_VEFPreP1B::modifier_secmem_som(const DoubleTab& Gpoint, DoubleTab& b)
{
  const Domaine_VEF& domaine_VEF =  domaine_Vef();
  const Domaine_Cl_VEF& domaine_Cl = le_dom_Cl_VEF.valeur();
  const Domaine& domaine = domaine_VEF.domaine();

  const int nb_cond_lim = domaine_Cl.nb_cond_lim();
  const int nb_elem_tot = (domaine_VEF.get_alphaE()?domaine.nb_elem_tot():0);
  const int nb_faces_elem = domaine.nb_faces_elem();

  const double coeff_dim = Objet_U::dimension*(Objet_U::dimension+1);

  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_sommets = domaine_VEF.face_sommets();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const IntTab& elem_sommets = domaine.les_elems();

  const DoubleTab& face_normales = domaine_VEF.face_normales();
  ArrOfDouble sigma(Objet_U::dimension);

  for (int cond_lim=0; cond_lim<nb_cond_lim; cond_lim++)
    {
      const Cond_lim_base& cl_base = domaine_Cl.les_conditions_limites(cond_lim).valeur();

      const Front_VF& front_VF = ref_cast(Front_VF,cl_base.frontiere_dis());
      const Champ_front_base& champ_front = cl_base.champ_front();

      /* Test sur la nature du champ au bord du domaine */
      if (sub_type(Entree_fluide_vitesse_imposee, cl_base)  && champ_front.instationnaire())
        {
          // Construction de la liste des faces a traiter (reelles + virtuelles)
          const int nb_faces_bord_tot = front_VF.nb_faces_tot();
          ToDo_Kokkos("critical");
          for (int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
            {
              const int num_face =  front_VF.num_face(ind_face);
              const int elem = face_voisins(num_face,0);
              assert(elem!=-1);

              //Calcul de la vitesse au centre de l'element
              sigma=0.;
              for (int face_loc=0; face_loc<nb_faces_elem; face_loc++)
                {
                  const int face = elem_faces(elem,face_loc);

                  for(int comp=0; comp<dimension; comp++)
                    sigma[comp]+=Gpoint(face,comp);
                }

              //Calcul de la divergence de la vitesse
              for(int face_loc=0; face_loc<nb_faces_elem; face_loc++)
                {
                  const int som = nb_elem_tot+domaine.get_renum_som_perio(elem_sommets(elem,face_loc));
                  const int face = elem_faces(elem,face_loc);

                  double psc=0;
                  double signe=1.;
                  if(elem!=face_voisins(face,0)) signe=-1.;

                  for(int comp=0; comp<dimension; comp++)
                    psc+=sigma[comp]*face_normales(face,comp);

                  b(som)-=signe*psc/coeff_dim;
                }

              double flux = 0. ;
              for (int comp=0; comp<dimension; comp++)
                flux += Gpoint(num_face,comp) * face_normales(num_face,comp) ;

              flux*=1./dimension;
              for(int som_loc=0; som_loc<nb_faces_elem-1; som_loc++)
                {
                  const int som=domaine.get_renum_som_perio(face_sommets(num_face,som_loc));
                  b(nb_elem_tot+som)-=flux;
                }
              //Fin du calcul de la divergence de la vitesse
            }
        }
    }

  return 1;
}

int Assembleur_P_VEFPreP1B::modifier_secmem_aretes(const DoubleTab& Gpoint, DoubleTab& b)
{
  return 1;
}

int Assembleur_P_VEFPreP1B::modifier_solution(DoubleTab& tab_pression)
{

  //  if (!has_P_ref) exit();
  const Domaine_VEF& le_dom = domaine_Vef();

  // Verification sur les aretes
  if (le_dom.get_alphaA())
    {
      // On impose la pression a 0 sur les aretes superflues:
      const IntVect& ok_arete=le_dom.get_ok_arete();
      const ArrOfInt& renum_arete_perio=le_dom.get_renum_arete_perio();
      // Nombre d'aretes reelles
      int nb_aretes=ok_arete.size();
      int npa=le_dom.numero_premiere_arete();
      for(int i=0; i<nb_aretes; i++)
        {
          if(!ok_arete(i) && tab_pression(npa+i)!=0.)
            {
              Cerr << "Pb pression arete superflue, P(" << npa+i << ")=" << tab_pression(npa+i) << finl;
              tab_pression(npa+i)=0;
              Process::exit();
            }
          else if ( (renum_arete_perio[i]!=i) && tab_pression(npa+i)!=0.)
            {
              Cerr << "Pb pression arete superflue periodique, P(" << npa+i << ")=" << tab_pression(npa+i) << finl;
              tab_pression(npa+i)=0;
              Process::exit();
            }
        }
    }

  // On applique la periodicite sur les sommets pour la pression:
  if (le_dom.get_alphaS())
    {
      const Domaine& dom=le_dom.domaine();
      int nps=le_dom.numero_premier_sommet();
      int ns=le_dom.domaine().nb_som();
      CIntArrView renum_som_perio = dom.get_renum_som_perio().view_ro();
      DoubleArrView pression = static_cast<DoubleVect&>(tab_pression).view_rw();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                           Kokkos::RangePolicy<>(0, ns), KOKKOS_LAMBDA(
                             const int i)
      {
        int k=renum_som_perio(i);
        if (k!=i) pression(nps+i)=pression(nps+k);
      });
      end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
    }
  // pression.echange_espace_virtuel();
  // pour retirer le min de la pression si pas de Pref et si que PO sinon on filtre plus tard
  if (le_dom.get_alphaE() && (le_dom.get_alphaS()==0) && (le_dom.get_alphaA()==0) )
    Assembleur_P_VEF::modifier_solution(tab_pression);
  // Verification possible par variable d'environnement:
  char* theValue = getenv("TRUST_VERIFIE_DIRICHLET");
  if(theValue != nullptr) verifier_dirichlet();

  return 1;
}

void Assembleur_P_VEFPreP1B::verifier_dirichlet()
{
  if (domaine_Vef().get_alphaE()+domaine_Vef().get_alphaS()+domaine_Vef().get_alphaA()!=dimension)
    {
      Cerr << "Assembleur_P_VEFPreP1B::verifier_dirichlet ne fonctionne pas encore avec votre discretisation" << finl;
      Process::exit();
    }
  // Verifications diverses des conditions limites
  // en postraitant le resultat dans le champ Divergence_U
  IntVect Faces_de_Dirichlet(domaine_Vef().nb_elem_tot());
  IntTab faces(domaine_Vef().nb_elem_tot(),2);
  faces=-1;
  const IntTab& face_sommets=domaine_Vef().face_sommets();
  Faces_de_Dirichlet=0;
  const Conds_lim& les_cl = le_dom_Cl_VEF->les_conditions_limites();
  for(int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();
      if ((sub_type(Dirichlet, la_cl.valeur()))
          || (sub_type(Dirichlet_homogene, la_cl.valeur())))
        {
          for (int face=num1; face<num2; face++)
            {
              int elem=domaine_Vef().face_voisins(face,0);
              if (elem!=-1)
                {
                  Faces_de_Dirichlet(elem)++;
                  if (faces(elem,0)==-1) faces(elem,0)=face;
                  else faces(elem,1)=face;

                }
              elem=domaine_Vef().face_voisins(face,1);
              if (elem!=-1) Faces_de_Dirichlet(elem)++;
            }
        }
    }
  DoubleVect& post=ref_cast(Navier_Stokes_std,mon_equation.valeur()).div().valeurs();
  post=0;
  for (int elem=0; elem<Faces_de_Dirichlet.size_array(); elem++)
    {
      //post(elem)=Faces_de_Dirichlet(elem);
      if (Faces_de_Dirichlet(elem)>1)
        {
          Cerr << "L'element " << elem << " a " << Faces_de_Dirichlet(elem) << " faces de Dirichlet separees par une ";
          const IntVect& ok_arete=domaine_Vef().get_ok_arete();
          const IntTab& aretes_som=domaine_Vef().domaine().aretes_som();
          // Parcours des aretes pour verifier les aretes de bord
          for (int k=0; k<6; k++)
            {
              int arete=domaine_Vef().domaine().elem_aretes(elem,k);
              // Les 2 sommets de l'arete
              int S0=aretes_som(arete,0);
              int S1=aretes_som(arete,1);
              // On verifie s'ils sont 2 fois dans les sommets des faces
              // auquels cas c'est l'arete qui partage 2 faces de Dirichlet
              int ok=0;
              for (int l=0; l<2; l++)
                {
                  int face=faces(elem,l);
                  for (int s=0; s<3; s++)
                    {
                      if (face_sommets(face,s)==S0) ok++;
                      if (face_sommets(face,s)==S1) ok++;
                    }
                }
              if (ok==4) Cerr << "arete " << (ok_arete(arete)==0?"superflue":"") << finl;
              // On compte les aretes superflues
              post(elem)+=(ok_arete(arete)==0);
            }
          //Process::exit();
        }
    }
}

void Assembleur_P_VEFPreP1B::projete_L2(DoubleTab& pression)
{
  if (domaine_Vef().get_alphaE()+domaine_Vef().get_alphaS()+domaine_Vef().get_alphaA()!=3)
    {
      Cerr << "Assembleur_P_VEFPreP1B::projete_L2 ne fonctionne qu'en P0+P1+Pa" << finl;
      Process::exit();
    }
  //Cerr << "Projection L2" << finl;
  const Domaine_VEF& domaine_vef = domaine_Vef();
  const Domaine& domaine = domaine_vef.domaine();
  int nb_elem_tot=domaine.nb_elem_tot();
  int ns=domaine.nb_som_tot();
  int nb_elem=domaine.nb_elem();
  int i,j;
  const IntTab& som_elem=domaine_vef.domaine().les_elems();
  const DoubleVect& volumes=domaine_vef.volumes();
  double volume_tot=0;
  double somme=0;
  int nsr=nb_elem_tot+ns;
  int nse=domaine.nb_som_elem();
  for(i=nb_elem_tot; i<nsr; i++)
    somme+=pression(i);
  somme/=ns;
  for(i=nb_elem_tot; i<nsr; i++)
    pression(i)-=somme;
  static double dalpha =(1./dimension*(dimension+1));
  double pmoy=0;
  for(i=0; i<nb_elem; i++)
    {
      double p=pression(i);
      double v=volumes(i);
      for(j=0; j<nse; j++)
        p+=pression(nb_elem_tot+som_elem(i,j))*dalpha;
      pmoy+=p*v;
      volume_tot+=v;
    }
  pmoy/=volume_tot;
  for(i=0; i<nb_elem; i++)
    pression(i)-=pmoy;
}

/*! @brief Modifier eventuellement la matrice pour la rendre definie si elle ne l'est pas Valeurs par defaut:
 *
 *     Contraintes:
 *     Acces: entree
 *
 * @return (int) renvoie 1 si la matrice est modifiee 0 sinon
 */
int Assembleur_P_VEFPreP1B::modifier_matrice(Matrice& la_matrice)
{
  int matrice_modifiee=0;
  has_P_ref=0;
  const Conds_lim& les_cl = le_dom_Cl_VEF->les_conditions_limites();
  const Domaine_VEF& domaine_VEF = domaine_Vef();
  // Recherche s'il y'a une pression de reference, et si oui la matrice n'est pas modifiee
  for(int i=0; i<les_cl.size(); i++)
    if (sub_type(Neumann_sortie_libre,les_cl[i].valeur()))
      {
        has_P_ref=1;
        if (domaine_VEF.get_alphaA())
          {
            // On en profite pour verifier si la pression est bien nulle si support Pa
            const DoubleTab& val=ref_cast(Neumann_sortie_libre,les_cl[i].valeur()).champ_front().valeurs();
            int nbval=val.dimension(0);
            for (int n=0; n<nbval; n++)
              if (val(n,0)!=0)
                {
                  Cerr << "La condition limite pression imposee non nulle n'est pas encore" << finl;
                  Cerr << "supportee en VEF avec support arete Pa." << finl;
                  // Le travail est a faire dans Assembleur_P_VEFPreP1B::modifier_secmem_aretes
                  Process::exit();
                }
          }
      }

  Matrice_Bloc_Sym& matrice=ref_cast(Matrice_Bloc_Sym, la_matrice.valeur());
  int P0 = 0;
  int P1 = P0 + domaine_VEF.get_alphaE();
  int Pa = P1 + domaine_VEF.get_alphaS();



  int CL_neumann=has_P_ref;
  ////////////
  // Partie P0
  ////////////
  if (domaine_VEF.get_alphaE())
    {
      // On impose une pression de reference sur un element si pas de CL de Neumann
      if (has_P_ref)
        {
          Matrice_Bloc& mat_bloc=ref_cast(Matrice_Bloc,matrice.get_bloc(P0,P0).valeur());
          Matrice_Morse_Sym& A00RR = ref_cast(Matrice_Morse_Sym,mat_bloc.get_bloc(0,0).valeur());
          A00RR.set_est_definie(1);
        }
      matrice_modifiee=Assembleur_P_VEF::modifier_matrice(matrice.get_bloc(P0,P0));
    }

  ////////////
  // Partie P1
  ////////////
  if (domaine_VEF.get_alphaS())
    {
      Matrice_Bloc& mat_bloc_p1_p1 = ref_cast(Matrice_Bloc, matrice.get_bloc(P1,P1).valeur());
      Matrice_Morse_Sym& A11RR = ref_cast(Matrice_Morse_Sym,mat_bloc_p1_p1.get_bloc(0,0).valeur());
      // On impose une pression de reference sur un sommet si support P0 ou si pas de CL de Neumann
      const bool is_first_proc_with_real_elems = Process::me() == Process::mp_min(le_dom_VEF->nb_elem() ? Process::me() : 1e8);
      if (((!(Process::mp_max(CL_neumann))) || ((domaine_VEF.get_alphaE()) && (domaine_VEF.get_cl_pression_sommet_faible()==1) ) ) && is_first_proc_with_real_elems)
        {
          int sommet_referent=0;
          double distance=DMAXFLOAT;
          const DoubleTab& coord=domaine_VEF.domaine().coord_sommets();
          int nb_som=domaine_VEF.domaine().nb_som();
          for(int i=0; i<nb_som; i++)
            {
              double tmp=0;
              for (int j=0; j<dimension; j++)
                tmp+=coord(i,j)*coord(i,j);
              if (inf_strict(tmp,distance) && !est_egal(A11RR(i,i),0.))
                {
                  distance=tmp;
                  sommet_referent=i;
                }
            }
          //Cerr << "On modifie la ligne (sommet) " << sommet_referent << finl;
          A11RR(sommet_referent,sommet_referent)*=2;
          // has_P_ref=1;
          matrice_modifiee=1;
        }
      A11RR.set_est_definie(1);
    }

  ////////////
  // Partie Pa
  ////////////
  if (domaine_VEF.get_alphaA())
    {
      Matrice_Bloc& mat_bloc_pa_pa = ref_cast(Matrice_Bloc, matrice.get_bloc(Pa,Pa).valeur());
      Matrice_Morse_Sym& A22RR = ref_cast(Matrice_Morse_Sym,mat_bloc_pa_pa.get_bloc(0,0).valeur());
      // On impose une pression de reference sur une arete en P0+Pa uniquement
      const bool is_first_proc_with_real_elems = Process::me() == Process::mp_min(le_dom_VEF->nb_elem() ? Process::me() : 1e8);
      if ((domaine_VEF.get_alphaE() && !domaine_VEF.get_alphaS()) && is_first_proc_with_real_elems)
        {
          int arete_referente=0;
          double distance=DMAXFLOAT;
          const DoubleTab& coord=domaine_VEF.xa();
          int nb_aretes=domaine_VEF.domaine().nb_aretes();
          for(int i=0; i<nb_aretes; i++)
            {
              double tmp=0;
              for (int j=0; j<dimension; j++)
                tmp+=coord(i,j)*coord(i,j);
              if (tmp<distance && A22RR(i,i)!=0)
                {
                  distance=tmp;
                  arete_referente=i;
                }
            }
          //Cerr << "On modifie la ligne (arete) " << arete_referente << finl;
          A22RR(arete_referente,arete_referente)*=2;
          //has_P_ref=1;
          matrice_modifiee=1;
        }
      A22RR.set_est_definie(1);
    }

  {
    Matrice_Bloc& bloc_P0_P0 = ref_cast( Matrice_Bloc,matrice.get_bloc(P0,P0).valeur( ) );
    Matrice_Morse_Sym& A00RR = ref_cast( Matrice_Morse_Sym, bloc_P0_P0.get_bloc(0,0).valeur() );
    matrice.set_est_definie( A00RR.get_est_definie( ) );
  }

  return matrice_modifiee;
}

inline void range(double& prod, int& i, int& n, int& j, int& m, Matrice_Morse& ARR, Matrice_Morse& ARV, Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  if (i<n)
    if (j<m)
      ARR(i,j)+=prod;
    else
      ARV(i,j-m)+=prod;
  else if (j<m)
    AVR(i-n,j)+=prod;
  else
    AVV(i-n,j-m)+=prod;
}

void operation11(Matrice_Bloc& A00, Matrice_Bloc& A01, Matrice_Bloc& A11, double beta, const Domaine& domaine)
{
  //Cerr << "Operation11" << finl;
  Matrice_Morse_Sym& A11RR=ref_cast(Matrice_Morse_Sym, A11.get_bloc(0,0).valeur());
  Matrice_Morse& A11RV=ref_cast(Matrice_Morse, A11.get_bloc(0,1).valeur());
  Matrice_Morse& A11VR=ref_cast(Matrice_Morse, A11.get_bloc(1,0).valeur());
  Matrice_Morse& A11VV=ref_cast(Matrice_Morse, A11.get_bloc(1,1).valeur());
  const IntTab& les_elems=domaine.les_elems();
  const Domaine& dom=domaine;
  int nb_som=A11RR.nb_lignes();
  int nb_som_elem=les_elems.dimension(1);
  // On parcours les elements de la matrice A00
  //Cerr << "[" << Process::me() << "] Contribution de A00 dans A11~" << finl;
  int ligne=0;
  for (int i_bloc=0; i_bloc<A00.nb_bloc_lignes(); i_bloc++)
    {
      int colonne=0;
      int nb_lignes=0;
      for (int j_bloc=0; j_bloc<A00.nb_bloc_colonnes(); j_bloc++)
        {
          Matrice_Morse& A00ij=ref_cast(Matrice_Morse, A00.get_bloc(i_bloc,j_bloc).valeur());
          const int* tab1=A00ij.get_tab1().addr();
          const int* tab2=A00ij.get_tab2().addr();
          const double* coeff=A00ij.get_coeff().addr();
          nb_lignes=A00ij.nb_lignes();
          int nb_colonnes=A00ij.nb_colonnes();
          for (int i=0; i<nb_lignes; i++)
            {
              int k1=ligne+i; // Element k1
              for (int n=tab1[i]-1; n<tab1[i+1]-1; n++)
                {
                  int k2=colonne+tab2[n]-1; // Element k2
                  if (k2>=k1)
                    {
                      double prod = beta * beta * coeff[n];        // Calcul de beta*beta*Ak1k2
                      for (int som1=0; som1<nb_som_elem; som1++)
                        {
                          int s1 = dom.get_renum_som_perio(les_elems(k1,som1));
                          for (int som2=0; som2<nb_som_elem; som2++)
                            {
                              int s2 = dom.get_renum_som_perio(les_elems(k2,som2));
                              if (s2>=s1) range(prod,s1,nb_som,s2,nb_som,A11RR,A11RV,A11VR,A11VV);
                            }
                        }
                      if (k1!=k2)
                        {
                          for (int som1=0; som1<nb_som_elem; som1++)
                            {
                              int s1 = dom.get_renum_som_perio(les_elems(k2,som1));
                              for (int som2=0; som2<nb_som_elem; som2++)
                                {
                                  int s2 = dom.get_renum_som_perio(les_elems(k1,som2));
                                  if (s2>=s1) range(prod,s1,nb_som,s2,nb_som,A11RR,A11RV,A11VR,A11VV);
                                }
                            }
                        }
                    }
                }
            }
          colonne+=nb_colonnes;
        }
      ligne+=nb_lignes;
    }
  // On parcours les elements de la matrice A01
  //Cerr << "[" << Process::me() << "] Contribution de A01 dans A11~" << finl;
  ligne=0;
  for (int i_bloc=0; i_bloc<A01.nb_bloc_lignes(); i_bloc++)
    {
      int colonne=0;
      int nb_lignes=0;
      for (int j_bloc=0; j_bloc<A01.nb_bloc_colonnes(); j_bloc++)
        {
          Matrice_Morse& A01ij=ref_cast(Matrice_Morse, A01.get_bloc(i_bloc,j_bloc).valeur());
          const int* tab1=A01ij.get_tab1().addr();
          const int* tab2=A01ij.get_tab2().addr();
          const double* coeff=A01ij.get_coeff().addr();
          nb_lignes=A01ij.nb_lignes();
          int nb_colonnes=A01ij.nb_colonnes();
          for (int i=0; i<nb_lignes; i++)
            {
              int k=ligne+i; // Element k
              for (int n=tab1[i]-1; n<tab1[i+1]-1; n++)
                {
                  int s1 = dom.get_renum_som_perio(colonne+tab2[n]-1); // Sommet s1
                  double prod = -beta * coeff[n];        // Calcul de -beta*Aks
                  for (int som=0; som<nb_som_elem; som++)
                    {
                      int s2 = dom.get_renum_som_perio(les_elems(k,som)); // Sommet s2
                      if (s2>=s1) range(prod,s1,nb_som,s2,nb_som,A11RR,A11RV,A11VR,A11VV);
                      if (s1>=s2) range(prod,s2,nb_som,s1,nb_som,A11RR,A11RV,A11VR,A11VV);
                    }
                }
            }
          colonne+=nb_colonnes;
        }
      ligne+=nb_lignes;
    }
}

void operation01(Matrice_Bloc& A00, Matrice_Bloc& A01, double alpha, double beta, const Domaine& domaine)
{
  //Cerr << "[" << Process::me() << "] Operation01" << finl;
  Matrice_Morse& A01RR=ref_cast(Matrice_Morse, A01.get_bloc(0,0).valeur());
  Matrice_Morse& A01RV=ref_cast(Matrice_Morse, A01.get_bloc(0,1).valeur());
  Matrice_Morse& A01VR=ref_cast(Matrice_Morse, A01.get_bloc(1,0).valeur());
  Matrice_Morse& A01VV=ref_cast(Matrice_Morse, A01.get_bloc(1,1).valeur());
  const IntTab& les_elems=domaine.les_elems();
  const Domaine& dom=domaine;
  int nb_elem=A01RR.nb_lignes();
  int nb_som=A01RR.nb_colonnes();
  int nb_som_elem=les_elems.dimension(1);
  // On parcours les coefficients de A00
  int ligne=0;
  for (int i_bloc=0; i_bloc<A00.nb_bloc_lignes(); i_bloc++)
    {
      int colonne=0;
      int nb_lignes=0;
      for (int j_bloc=0; j_bloc<A00.nb_bloc_colonnes(); j_bloc++)
        {
          Matrice_Morse& A00ij=ref_cast(Matrice_Morse, A00.get_bloc(i_bloc,j_bloc).valeur());
          const int* tab1=A00ij.get_tab1().addr();
          const int* tab2=A00ij.get_tab2().addr();
          const double* coeff=A00ij.get_coeff().addr();
          nb_lignes=A00ij.nb_lignes();
          int nb_colonnes=A00ij.nb_colonnes();
          int s;
          for (int i=0; i<nb_lignes; i++)
            {
              int k1=ligne+i; // Element k1
              for (int n=tab1[i]-1; n<tab1[i+1]-1; n++)
                {
                  int k2=colonne+tab2[n]-1; // Element k2
                  if (k2>=k1)
                    {
                      double prod = -alpha * beta * coeff[n];        // Calcul de -alpha*beta*Ak1k2
                      for (int som=0; som<nb_som_elem; som++)
                        {
                          s = dom.get_renum_som_perio(les_elems(k2,som));
                          range(prod,k1,nb_elem,s,nb_som,A01RR,A01RV,A01VR,A01VV);
                          if (k1!=k2)
                            {
                              s = dom.get_renum_som_perio(les_elems(k1,som));
                              range(prod,k2,nb_elem,s,nb_som,A01RR,A01RV,A01VR,A01VV);
                            }
                        }
                    }
                }
            }
          colonne+=nb_colonnes;
        }
      ligne+=nb_lignes;
    }
}

void Assembleur_P_VEFPreP1B::changer_base_matrice(Matrice& la_matrice)
{
  assert(domaine_Vef().get_alphaE() && domaine_Vef().get_alphaS() && !domaine_Vef().get_alphaA()); // P0+P1 uniquement
  Cerr << "Changement de base pour la matrice: P0+P1->P1Bulle" << finl;
  Matrice_Bloc_Sym& matrice=ref_cast(Matrice_Bloc_Sym, la_matrice.valeur());
  Matrice_Bloc& A00=ref_cast(Matrice_Bloc, matrice.get_bloc(0,0).valeur());
  Matrice_Bloc& A01=ref_cast(Matrice_Bloc, matrice.get_bloc(0,1).valeur());
  Matrice_Bloc& A11=ref_cast(Matrice_Bloc, matrice.get_bloc(1,1).valeur());

  // Modification du bloc A11
  // As1s2~=As1s2-beta*[somme(Ak1s2)(s1 appartient a k1)+somme(Ak1s1)(s2 appartenant a k1)]+beta*beta*somme(Ak1k2)(s1 appartenant a k1 et s2 appartenant a k2)
  operation11(A00,A01,A11,beta_,domaine_Vef().domaine());

  // Modification du bloc A01
  // Ak1s~=alpha*Ak1s - alpha*beta*somme(Ak1k2)(s appartenant a k2)
  A01*=alpha_;
  operation01(A00,A01,alpha_,beta_,domaine_Vef().domaine());

  // Modification du bloc A00
  // Ak1k2~ = alpha * alpha * Ak1k2
  A00*=alpha_*alpha_;
}


void Assembleur_P_VEFPreP1B::changer_base_second_membre(DoubleVect& y)
{
  // ys~ = ys - beta * somme(yk)(s appartenant a k)
  // yk~ = alpha * yk
  changer_base<vecteur::second_membre>(y);
  y.echange_espace_virtuel();
}

void Assembleur_P_VEFPreP1B::changer_base_pression_inverse(DoubleVect& x)
{
  // xk = alpha * xk~ - beta * somme(xs~)(s appartenant a k)
  // xs = xs~
  changer_base<vecteur::pression_inverse>(x);
  assert(check_espace_virtuel_vect(x));
}

void Assembleur_P_VEFPreP1B::changer_base_pression(DoubleVect& x)
{
  // xk~ = xk / alpha + beta / alpha * somme(xs)(s appartenant a k)
  // xs~ = xs
  changer_base<vecteur::pression>(x);
  assert(check_espace_virtuel_vect(x));
}

template<vecteur _v_>
void Assembleur_P_VEFPreP1B::changer_base(DoubleVect& tab_v)
{
  assert(domaine_Vef().get_alphaE() && domaine_Vef().get_alphaS() && !domaine_Vef().get_alphaA()); // P0+P1 uniquement
  // xk~ = xk / alpha + beta / alpha * somme(xs)(s appartenant a k)
  // xs~ = xs
  double alpha = alpha_;
  double beta = beta_;
  int nb_elem_tot = domaine_Vef().nb_elem_tot();
  int nb_som_elem = domaine_Vef().domaine().les_elems().dimension(1);
  CIntTabView les_elems = domaine_Vef().domaine().les_elems().view_ro();
  CIntArrView renum_som_perio = domaine_Vef().domaine().get_renum_som_perio().view_ro();
  DoubleArrView v = tab_v.view_rw();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem_tot, KOKKOS_LAMBDA(const int k)
  {
    if (_v_==vecteur::pression) v(k) /= alpha;
    double somme=0;
    for (int som=0; som<nb_som_elem; som++)
      {
        int s = nb_elem_tot + renum_som_perio(les_elems(k, som));
        if (_v_==vecteur::pression)         somme += v(s);
        if (_v_==vecteur::pression_inverse) somme += v(s);
        if (_v_==vecteur::second_membre) Kokkos::atomic_sub(&v(s), beta * v(k));
      }
    if (_v_==vecteur::pression)         v(k) += beta / alpha * somme;
    if (_v_==vecteur::pression_inverse) v(k) = alpha * v(k) - beta * somme;
    if (_v_==vecteur::second_membre)    v(k) *= alpha;
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
}
