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

#include <Fluide_Dilatable_base.h>
#include <MD_Vector_composite.h>
#include <Discretisation_base.h>
#include <Navier_Stokes_IBM.h>
#include <Schema_Temps_base.h>
#include <MD_Vector_tools.h>
#include <Source_PDF_base.h>
#include <Assembleur_base.h>
#include <TRUSTTab_parts.h>
#include <Probleme_base.h>
#include <MD_Vector_std.h>
#include <Matrice_Bloc.h>
#include <TRUSTTrav.h>
#include <EChaine.h>
#include <Debog.h>
#include <Piso.h>

Implemente_instanciable(Piso,"Piso",Simpler);
// XD piso simpler piso -1 Piso (Pressure Implicit with Split Operator) - method to solve N_S.
// XD attr seuil_convergence_implicite floattant seuil_convergence_implicite 1 Convergence criteria.
// XD attr nb_corrections_max entier nb_corrections_max 1 Maximum number of corrections performed by the PISO algorithm to achieve the projection of the velocity field. The algorithm may perform less corrections then nb_corrections_max if the accuracy of the projection is sufficient. (By default nb_corrections_max is set to 21).

Implemente_instanciable_sans_constructeur(Implicite,"Implicite",Piso);
// XD implicite piso implicite -1 similar to PISO, but as it looks like a simplified solver, it will use fewer timesteps. But it may run faster because the pressure matrix is not re-assembled and thus provides CPU gains.

Implicite::Implicite()
{
  avancement_crank_ = 1;
}

Sortie& Piso::printOn(Sortie& os ) const { return Simpler::printOn(os); }

Entree& Piso::readOn(Entree& is ) { return Simpler::readOn(is); }

Sortie& Implicite::printOn(Sortie& os ) const { return Piso::printOn(os); }

Entree& Implicite::readOn(Entree& is ) { return Piso::readOn(is); }


Entree& Piso::lire(const Motcle& motlu,Entree& is)
{
  Motcles les_mots(2);
  {
    les_mots[0] = "nb_corrections_max";
    les_mots[1] = "with_sources";
  }

  int rang = les_mots.search(motlu);
  switch(rang)
    {
    case 0:
      {
        is >> nb_corrections_max_;
        if (nb_corrections_max_ < 2)
          {
            Cerr<<"There should be at least two corrections steps for the PISO algorithm."<<finl;
          }
        break;
      }
    case 1:
      {
        is >> with_sources_;
        break;
      }
    default :
      {
        Cerr << "Keyword : " << motlu << " is not understood in " << que_suis_je() << finl;
        exit();
      }
    }
  return is;
}



void test_imposer_cond_lim(Equation_base& eqn,DoubleTab& current2,const char * msg,int flag)
{
  return;
  /*
  DoubleTab& present = eqn.inconnue().futur();
  DoubleTab sauv(present);
  const Schema_Temps_base& sch = eqn.probleme().schema_temps();
  eqn.domaine_Cl_dis().imposer_cond_lim(eqn.inconnue(),sch.temps_courant()+sch.pas_de_temps());
  present -= sauv;
  // BM, je remplace max_abs par mp_pax_abs: du coup la methode doit etre appelee simultanement par tous les procs.
  double ecart_max=mp_max_abs_vect(present);
  Cout<<msg <<" "<<ecart_max<<finl;

  if ((ecart_max>1e-10))
    abort();
  present = sauv;
   */
}

//Entree Un ; Pn
//Sortie Un+1 = U***_k ; Pn+1 = P**_k
//n designe une etape temporelle

void Piso::iterer_NS(Equation_base& eqn, DoubleTab& current, DoubleTab& pressure,
                     double dt, Matrice_Morse& A, double /*seuil_resol*/, DoubleTrav& secmem, int nb_ite, int& converge, int& ok)
{
  converge = 1;
  if (nb_ite > 1) return;
  Navier_Stokes_std& eqnNS = ref_cast(Navier_Stokes_std, eqn);
  if (eqnNS.discretisation().que_suis_je() == "PolyMAC")
    return iterer_NS_PolyMAC(eqnNS, current, pressure, dt, A, ok);

  DoubleTrav gradP(current), resu(current), deltaP(pressure), correction_en_vitesse(current);

  assembleMomentumSystem_(eqnNS, eqn, current, pressure, gradP, resu, A, dt);
  solvePredictor_(eqn, A, resu, current);
  buildContinuityRhs_(eqn, current, secmem);
  if (applyFirstCorrection_(eqnNS, eqn, current, pressure, gradP, deltaP, secmem, dt, A, correction_en_vitesse))
    return;
  projectionLoop_(eqnNS, eqn, current, pressure, A, gradP, resu, deltaP, correction_en_vitesse, secmem);
}

//version PolyMAC de la fonction ci-dessus
void Piso::iterer_NS_PolyMAC(Navier_Stokes_std& eqn, DoubleTab& current, DoubleTab& pression, double dt, Matrice_Morse& matrice, int& ok)
{
  if (avancement_crank_ == 0)
    Process::exit("sorry, the PISO solver is not implemented with PolyMAC! Please use Implicite instead");
  Parametre_implicite& param_eqn = get_and_set_parametre_implicite(eqn);
  SolveurSys& le_solveur_ = param_eqn.solveur();

  Navier_Stokes_std& eqnNS = ref_cast(Navier_Stokes_std, eqn);
  Operateur_Grad& op_grad = eqnNS.operateur_gradient();
  Operateur_Div& op_div = eqnNS.operateur_divergence();

  DoubleTrav dv(current); //, dP(pression); //corrections en vitesse / pression

  /* etape de prediction : current <- v(0) ne verifiant pas div = 0 */
  DoubleTrav secmem_NS(current), v_new(current);
  op_grad.ajouter(pression, secmem_NS);
  secmem_NS *= -1;
  eqnNS.assembler_avec_inertie(matrice, current, secmem_NS);
  le_solveur_->reinit();
  le_solveur_.resoudre_systeme(matrice, secmem_NS, v_new);
  v_new.echange_espace_virtuel();

  Matrice& mat_press_orig = eqn.matrice_pression(), mat_press;

  /* etapes de correction : current <- v(i) verifiant div = 0 mais approche pour NS, pression <- p(i) correspondant a v(i) */
  for (int i = 0; i < 1; i++)
    {
      DoubleTrav sol_M(pression), secmem_M(pression);
      /* resolution en (dt * dp(i), dv(i)) */
      //second membre : divergence, NS
      DoubleTab_parts p_sec(secmem_M), p_sol(sol_M); //p_sec/sol[0] -> elements, p_sec/sol[1] -> faces
      //bloc superieur : div v(i-1)
      op_div.ajouter(v_new, p_sec[0]);
      //bloc inferieur : residu de l'etape de prediction
      p_sec[1] = 0;      //p_res[0];

      //matrice (sauf si avancement_crank_ == 1) : prise en compte des contributions des sources (et pas des operateurs!)
      if (avancement_crank_==0 || with_sources_)
        {
          matrice.get_set_coeff() = 0, eqn.sources().contribuer_a_avec(current, matrice);
          DoubleTrav diag(p_sec[1]);
          for (int j = 0; j < p_sec[1].dimension(0); j++)
            diag(j) = dt * matrice(j, j);
          diag.echange_espace_virtuel();
          eqn.assembleur_pression()->assembler_mat(mat_press, diag, 1, 1);
          eqn.solveur_pression()->reinit();
        }

      //resolution
      Cerr << "PISO : |sec dp| < " << mp_max_abs_vect(p_sec[0]) << " |sec dv| < " << mp_max_abs_vect(p_sec[1]) << finl;
      eqn.solveur_pression().resoudre_systeme(avancement_crank_ == 1 ? mat_press_orig.valeur() : mat_press.valeur(), secmem_M, sol_M);
      //mises a jour : v^(i) = v^(i-1)+dv^(i), p^(i) = p^(i-1) + dp^(i)
      eqn.assembleur_pression()->corriger_vitesses(sol_M, dv);
      Cerr << "PISO : |dp| < " << mp_max_abs_vect(p_sol[0]) / dt << " |dv| < " << mp_max_abs_vect(dv);
      v_new += dv, sol_M /= dt, pression -= sol_M;
      //
      p_sec[0] = 0, op_div.ajouter(v_new, p_sec[0]);
      Cerr << " |div v| < " << mp_max_abs_vect(p_sec[0]) << finl;
      pression.echange_espace_virtuel();
    }
  current = v_new;
}

void Piso::first_special_treatment(Equation_base& eqn, Navier_Stokes_std& eqnNS, DoubleTab& current, double dt, DoubleTrav& resu)
{
  //nothing to do
}

void Piso::second_special_treatment(Equation_base& eqn,DoubleTab& current, DoubleTrav& resu, Matrice_Morse& matrice)
{
  //nothing to do
}

void Piso::add_penality_term(Navier_Stokes_std& eqnNS, DoubleTrav& resu , DoubleTrav& gradP)
{
  // <IBM> Taking into account penality term for Immersed Boundary Method
  Navier_Stokes_IBM& eqnNS_IBM = ref_cast(Navier_Stokes_IBM, eqnNS);
  const int i_source_PDF = eqnNS_IBM.get_i_source_pdf();
  if (i_source_PDF != -1)
    {
      Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eqnNS.sources())[i_source_PDF].valeur());
      DoubleTab secmem_pdf(resu);
      src.calculer_pdf(secmem_pdf);

      // Terme en temps : -rho/delta_t ksi_gamma Un
      int pdf_bilan = src.get_modele().pdf_bilan();
      if (pdf_bilan == 1)
        {
          int i_traitement_special = 101;
          if (eqnNS.nombre_d_operateurs() > 1)
            {
              if (eqnNS.vitesse_pour_transport().le_nom() == "rho_u")
                i_traitement_special = 1;
            }
          DoubleTrav secmem_pdf_time(resu);
          src.calculer(secmem_pdf_time, i_traitement_special);
          secmem_pdf += secmem_pdf_time;
        }

      // Sauvegarde de secmem_pdf
      secmem_pdf.echange_espace_virtuel();
      src.set_sec_mem_pdf(secmem_pdf);

      const DoubleTab& coeff = eqnNS_IBM.get_champ_coeff_pdf_som();
      if (eqnNS_IBM.get_gradient_pression_qdm_modifie() == 1)
        {
          Cerr << "(IBM) Immersed Interface: modified pressure gradient in momentum equation." << finl;
          gradP /= coeff;
        }
      gradP.echange_espace_virtuel();
    }
}

void Piso::correct_gradP(Navier_Stokes_std& eqnNS, DoubleTrav& gradP)
{
  Navier_Stokes_IBM& eqnNS_IBM = ref_cast(Navier_Stokes_IBM, eqnNS);
  const int i_source_PDF = eqnNS_IBM.get_i_source_pdf();
  if ((i_source_PDF != -1) && (eqnNS_IBM.get_correction_vitesse_modifie() == 1))
    {
      Cerr << "(IBM) Immersed Interface: modified velocity correction." << finl;
      const DoubleTab& coeff = eqnNS_IBM.get_champ_coeff_pdf_som();
      gradP /= coeff;
      gradP.echange_espace_virtuel();
    }
}

void Piso::correct_incr_pressure(Navier_Stokes_std& eqnNS, DoubleTrav& secmem)
{
  // <IBM> Taking into account zero velocity divergence for Immersed Boundary Method
  Navier_Stokes_IBM& eqnNS_IBM = ref_cast(Navier_Stokes_IBM, eqnNS);
  const int i_source_PDF = eqnNS_IBM.get_i_source_pdf();
  if ((i_source_PDF != -1) && (eqnNS_IBM.get_correction_matrice_pression() == 1))
    {
      Cerr << "(IBM) Immersed Interface: velocity divergence is zero for crossed elements." << finl;
      const DoubleTab& coeff = eqnNS_IBM.get_champ_coeff_pdf_som();
      Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eqnNS.sources())[i_source_PDF].valeur());
      src.correct_incr_pressure(coeff, secmem);
    }
}

void Piso::correct_pressure(Navier_Stokes_std& eqnNS, DoubleTab& pression, DoubleTab& correction_en_pression)
{
  // <IBM> Immersed Interface: modified pressure correction.
  Navier_Stokes_IBM& eqnNS_IBM = ref_cast(Navier_Stokes_IBM, eqnNS);
  const int i_source_PDF = eqnNS_IBM.get_i_source_pdf();
  if ((i_source_PDF != -1) && (eqnNS_IBM.get_correction_pression_modifie()==1))
    {
      Cerr<<"Immersed Interface: modified pressure correction."<<finl;
      const DoubleTab& coeff = eqnNS_IBM.get_champ_coeff_pdf_som();
      const Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eqnNS.sources())[i_source_PDF].valeur());
      src.correct_pressure(coeff,pression,correction_en_pression);
    }
  else
    pression += correction_en_pression;
}

void Implicite::first_special_treatment(Equation_base& eqn, Navier_Stokes_std& eqnNS, DoubleTab& current, double dt, DoubleTrav& resu)
{
  //nothing to do
}

void Implicite::second_special_treatment(Equation_base& eqn,DoubleTab& current, DoubleTrav& resu, Matrice_Morse& matrice)
{
  //nothing to do
}

void Piso::assembleMomentumSystem_(Navier_Stokes_std& eqnNS, Equation_base& eqn,
                                   DoubleTab& current, DoubleTab& pressure,
                                   DoubleTrav& gradP, DoubleTrav& resu,
                                   Matrice_Morse& matrice, double dt)
{
  //Construction de matrice et resu
  //matrice = A[Un] = M/delta_t + CONV +DIFF
  //resu =  A[Un]Un -(A[Un]Un-Ss) + Sv -BtPn
  const bool is_NS_IBM = sub_type(Navier_Stokes_IBM, eqnNS);
  Operateur_Grad& gradient = eqnNS.operateur_gradient();
  if (is_NS_IBM)
    add_penality_term(eqnNS, resu, gradP);
  gradient->calculer(pressure, gradP);
  if (eqnNS.has_interface_blocs())
    {
      first_special_treatment(eqn, eqnNS, current, dt, resu);
      eqnNS.assembler_blocs_avec_inertie({{ "vitesse", &matrice }}, resu);
      if (eqnNS.discretisation().is_polymac_family())
        matrice.ajouter_multvect(current, resu);  //pour ne pas etre en increment
    }
  else
    {
      resu -= gradP;
      first_special_treatment(eqn, eqnNS, current, dt, resu);
      eqnNS.assembler_avec_inertie(matrice, current, resu);
    }
  second_special_treatment(eqn, current, resu, matrice);
}

void Piso::solvePredictor_(Equation_base& eqn, Matrice_Morse& matrice,
                           DoubleTrav& resu, DoubleTab& current)
{
  Parametre_implicite& p = get_and_set_parametre_implicite(eqn);
  SolveurSys& solver = p.solveur();
  solver->reinit();
  solver.resoudre_systeme(matrice, resu, current);
  test_imposer_cond_lim(eqn, current, "apres resolution ", 0);
  current.echange_espace_virtuel();
  Debog::verifier("Piso::iterer_NS current apres solveur", current);
}

void Piso::buildContinuityRhs_(Equation_base& eqn, DoubleTab& current,
                               DoubleTrav& secmem)
{
  const bool is_NS_IBM = sub_type(Navier_Stokes_IBM, eqn);
  Operateur_Div& divergence = ref_cast(Navier_Stokes_std, eqn).operateur_divergence();
  const bool is_dilat = eqn.probleme().is_dilatable();
  if (is_dilat)
    {
      if (with_d_rho_dt_)
        {
          Fluide_Dilatable_base& fluide_dil = ref_cast(Fluide_Dilatable_base, eqn.milieu());
          fluide_dil.secmembre_divU_Z(secmem);
          secmem *= -1;
        }
      else secmem = 0;
      divergence.ajouter(current, secmem);
    }
  else
    divergence.calculer(current, secmem);
  secmem *= -1;
  if (is_NS_IBM)
    correct_incr_pressure(ref_cast(Navier_Stokes_std, eqn), secmem);
  secmem.echange_espace_virtuel();
  Debog::verifier("Piso::iterer_NS secmem", secmem);
}

bool Piso::applyFirstCorrection_(Navier_Stokes_std& eqnNS, Equation_base& eqn,
                                 DoubleTab& current, DoubleTab& pressure,
                                 DoubleTrav& gradP, DoubleTrav& deltaP,
                                 DoubleTrav& secmem, double dt,
                                 Matrice_Morse& matrice, DoubleTrav& correction_en_vitesse)
{
  const bool is_NS_IBM = sub_type(Navier_Stokes_IBM, eqnNS);
  const bool is_dilat = eqn.probleme().is_dilatable();
  //Etape de correction 1
  Cout << "Solving mass equation :" << finl;
  Matrice& matrice_en_pression_2 = eqnNS.matrice_pression();
  SolveurSys& solveur_pression_ = eqnNS.solveur_pression();
  if (avancement_crank_ == 0 && !with_sources_)
    {
      assembler_matrice_pression_implicite(eqnNS, matrice, matrice_en_pression_2);
      solveur_pression_->reinit();
    }
  if (with_sources_)
    {
      matrice.get_set_coeff() = 0;
      eqnNS.sources().contribuer_a_avec(current, matrice);
      eqnNS.solv_masse().ajouter_masse(dt, matrice, 1);
      assembler_matrice_pression_implicite(eqnNS, matrice, matrice_en_pression_2);
      solveur_pression_->reinit();
    }
  eqnNS.assembleur_pression()->modifier_secmem_pour_incr_p(pressure, 1. / dt, secmem);
  if (with_sources_)
    matrice_en_pression_2->ajouter_multvect(pressure, secmem);
  solveur_pression_.resoudre_systeme(matrice_en_pression_2.valeur(), secmem, deltaP);
  if (with_sources_)
    deltaP -= pressure;
  deltaP.echange_espace_virtuel();
  Debog::verifier("Piso::iterer_NS apres correction_pression", deltaP);
  if (avancement_crank_ == 1)
    {
      Operateur_Grad& gradient = eqnNS.operateur_gradient();
      gradient->multvect(deltaP, gradP);
      if (with_sources_)
        {
          for (int i = 0, N = gradP.nb_dim() == 2 ? gradP.dimension(1) : 1; i < gradP.dimension_tot(0); i++)
            for (int n = 0; n < N; n++)
              gradP(i, n) = matrice.get_tab1()(N * i + n + 1) > matrice.get_tab1()(N * i + n) && matrice(N * i + n, N * i + n) ? gradP(i, n) / matrice(N * i + n, N * i + n) : 0;
        }
      else
        eqn.solv_masse().appliquer(gradP);
      if (is_NS_IBM)
        correct_gradP(eqnNS, gradP);
      current -= gradP;
      eqn.solv_masse().corriger_solution(current, current);
      current.echange_espace_virtuel();
      Operateur_Div& divergence = eqnNS.operateur_divergence();
      divergence.calculer(current, secmem);
      Debog::verifier("Piso::iterer_NS correction avant dt", deltaP);
      if (!with_sources_)
        deltaP /= dt;
      if (is_NS_IBM)
        correct_pressure(eqnNS, pressure, deltaP);
      else
        pressure += deltaP;
      eqnNS.assembleur_pression()->modifier_solution(pressure);
      pressure.echange_espace_virtuel();
      Debog::verifier("Piso::iterer_NS pression finale", pressure);
      Debog::verifier("Piso::iterer_NS current final", current);
      if (is_dilat)
        diviser_par_rho_np1_face(eqn, current);
      return true;
    }
  pressure += deltaP;
  eqnNS.assembleur_pression()->modifier_solution(pressure);
  Operateur_Grad& gradient = eqnNS.operateur_gradient();
  calculer_correction_en_vitesse(deltaP, gradP, correction_en_vitesse, matrice, gradient);
  current += correction_en_vitesse;
  test_imposer_cond_lim(eqn, current, "apres premiere correction ", 0);
  Debog::verifier("Piso::iterer_NS arpes cor pression", pressure);
  Debog::verifier("Piso::iterer_NS arpes cor vitesse", current);
  return false;
}

void Piso::projectionLoop_(Navier_Stokes_std& eqnNS, Equation_base& eqn,
                           DoubleTab& current, DoubleTab& pressure,
                           Matrice_Morse& matrice, DoubleTrav& gradP,
                           DoubleTrav& resu, DoubleTrav& deltaP, DoubleTrav& correction_en_vitesse, DoubleTrav& secmem)
{
  const bool is_dilat = eqn.probleme().is_dilatable();
  Operateur_Div& divergence = eqnNS.operateur_divergence();
  Operateur_Grad& gradient = eqnNS.operateur_gradient();
  double vitesse_norme, vitesse_norme_old = 1.e10;
  double pression_norme, pression_norme_old = 1.e10;
  Matrice& matrice_en_pression_2 = eqnNS.matrice_pression();
  SolveurSys& solveur_pression_ = eqnNS.solveur_pression();
  for (int compt=0; compt<nb_corrections_max_-1; compt++)
    {
      correction_en_vitesse.echange_espace_virtuel();
      //Resolution du systeme D resu = EU' + (resu2=0) pour stocker resu = D-1EU'
      DoubleTrav resu2(resu);
      int status = inverser_par_diagonale(matrice,resu2,correction_en_vitesse,resu);

      if (status!=0) exit();
      // calcul de P''  BD-1Bt P''= -div(D-1EU')

      resu.echange_espace_virtuel();
      //Calcul de B(D-1EU')
      divergence.calculer(resu,secmem);
      secmem *= -1;
      secmem.echange_espace_virtuel();

      //Resolution du systeme (BD-1Bt)P'' = (BD-1E)U'
      //correction_en_pression = P''
      deltaP = 0;
      solveur_pression_.resoudre_systeme(matrice_en_pression_2.valeur(),
                                         secmem,deltaP);

#ifdef DEBUG
      // Pour verifier que le calcul du gradient ne modifie pas la pression
      DoubleTrav correction_en_pression_mod(pression);
      correction_en_pression_mod = deltaP;
#endif
      //Resolution du systeme D[Un]U'' = -BtP''
      //correction_en_vitesse = U''
      calculer_correction_en_vitesse(deltaP,gradP,correction_en_vitesse,matrice,gradient);

#ifdef DEBUG
      correction_en_pression_mod -= correction_en_pression;
      assert(max_abs(correction_en_pression_mod)==0);
#endif

      //Calcul de U'' = U'' + D-1EU'
      correction_en_vitesse += resu;
      // ajout des increments

      vitesse_norme = mp_norme_vect(correction_en_vitesse);
      pression_norme = mp_norme_vect(deltaP);

      if ( (vitesse_norme>vitesse_norme_old) || (pression_norme>pression_norme_old) )
        {
          Cout <<"PISO : "<< compt+1 <<" corrections to perform the projection."<< finl;
          if (is_dilat)
            {
              // on redivise par rho_np_1 avant de sortir
              diviser_par_rho_np1_face(eqn,current);
            }
          return ;
        }

      vitesse_norme_old = vitesse_norme;
      pression_norme_old = pression_norme;
      //Calcul de P** = P* + P''
      pressure += deltaP;
      eqnNS.assembleur_pression()->modifier_solution(pressure);

      //Calcul de U*** = U** + U''
      current += correction_en_vitesse;
      test_imposer_cond_lim(eqn,current,"apres correction (int)__LINE__",0);

      Debog::verifier("Piso::iterer_NS apres correction pression",pressure);
      Debog::verifier("Piso::iterer_NS apres correction vitesse",current);
    }
  if (is_dilat)
    {
      diviser_par_rho_np1_face(eqn,current);
      //ref_cast(Navier_Stokes_QC,eqn).impr_impl(eqnNS, Cout);
    }
  current.echange_espace_virtuel();
  // divergence.calculer(current, secmem); Cerr<<" ici DIVU  "<<mp_max_abs_vect(secmem)<<finl;;
  Cout <<"PISO : "<<nb_corrections_max_<<" corrections to perform the projection."<< finl;
}


