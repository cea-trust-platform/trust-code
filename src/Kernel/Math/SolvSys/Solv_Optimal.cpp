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
// File:        Solv_Optimal.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/33
//
//////////////////////////////////////////////////////////////////////////////
#include <Solv_Optimal.h>
#include <Param.h>
#include <LecFicDistribueBin.h>
#include <stat_counters.h>
#include <LecFicDiffuse.h>
#include <Matrice_Bloc.h>
#include <Matrice_Morse_Sym.h>
#include <SFichier.h>
#include <petsc_for_kernel.h>
#undef setbit // Car sinon conflit avec Petsc
#include <MD_Vector_tools.h>

Implemente_instanciable(Test_solveur,"Test_solveur",Interprete);

Implemente_instanciable_sans_constructeur_ni_destructeur(Solv_Optimal,"Solv_Optimal",solv_iteratif);
//
// printOn et readOn

Sortie& Test_solveur::printOn(Sortie& s ) const
{
  return s;
}

Entree& Test_solveur::readOn(Entree& is )
{
  return is;
}

void test_un_solveur(SolveurSys& solveur, const Matrice_Base& matrice, const DoubleVect& secmem, DoubleVect& solution, int nmax, ArrOfDouble& temps, double seuil_verification=DMAXFLOAT)
{
  DoubleVect solution_ref(solution);
  int n=temps.size_array();
  Stat_Counter_Id  solv_sys_counter_l= statistiques().new_counter(1, "SolveurSys::resoudre_systeme", 0);

  for (int i=0; i<n; i++)
    {
      solution=solution_ref;

      // etape de resolution
      Stat_Results stat_resol_0;

      statistiques().get_stats(solv_sys_counter_l, stat_resol_0);
      Cout<<"------------------------------------"<<finl;
      Cout<<"Try " << i << " of solver " << solveur <<finl;
      //solveur.valeur().fixer_limpr(0);
      statistiques().begin_count(solv_sys_counter_l);
      solveur.nommer("test_solver");

      solveur.resoudre_systeme(matrice,secmem,solution);
      statistiques().end_count(solv_sys_counter_l);

      Stat_Results stat_resol;
      statistiques().get_stats(solv_sys_counter_l, stat_resol);
      // on recupere un delta time et non un time absolu !!
      double time_resol=stat_resol.time-stat_resol_0.time;
      DoubleVect test(secmem);
      test*=-1;
      matrice.ajouter_multvect(solution,test);
      //test-=secmem;
      double norme=mp_norme_vect(test);
      //statistiques().dump("Statistiques de resolution du probleme");
      //print_statistics_analyse("Statistiques de resolution du probleme", 1);

      Cout<<"CPU= " <<time_resol<<" s , ||Ax -b||= " << norme << finl;
      Process::imprimer_ram_totale();
      temps[i]=Process::mp_max(time_resol);

      if (norme>seuil_verification)
        {
          Cerr<<"residue calculated greater than the threshold value indicated ("<<seuil_verification<<")"<<finl;
          Cerr<<" one will not use the solver "<<solveur<<finl;
          temps=1.e37;
        }
    }
}
int test_solveur(SolveurSys& solveur,  const Matrice_Base& matrice , const DoubleVect& secmem , DoubleVect& solution , int nmax, ArrOfDouble& temps, Entree& list_solveur, double seuil_verification=DMAXFLOAT)
{
  DoubleVect solution_ref(solution);
  int numero=0,numero_best=-1;
  double best_time=1e36;
  Motcle motsolveur("solveur"),motlu;
  list_solveur >> motlu;
  int dernier=temps.size_array()-1;
  while (list_solveur.good())
    {
      if (motlu!=motsolveur)
        {
          Cerr<<" One expected "<<motsolveur <<" and not "<<motlu<<finl;
          Process::exit();
        }
      numero++;
      SolveurSys newsolveur;
      list_solveur>>newsolveur;
      solution=solution_ref;
      test_un_solveur(newsolveur, matrice, secmem ,solution, nmax, temps, seuil_verification);
      if (temps[dernier]<best_time)
        {
          solveur=newsolveur;
          best_time=temps[dernier];
          numero_best=numero;
        }
      list_solveur>>motlu;
    }
  if (numero_best==-1)
    {
      Cerr<<" None of the tested solvers give the expected residue" <<finl;
      Cerr<< " does the same solver must be kept ? "<<finl;
      // exit();
    }
  temps=best_time;
  return numero_best;
}
// Description genere un fichier de solveur a tester different selon si la matrice peut etre resolue avec ou sans GCP
void generate_defaut(const Matrice_Base& matrice, const double& seuil, Sortie& sortie, int limpr=0)
{
  Nom impr(" impr " );
  if (limpr==0) impr=" ";
  if (limpr==-1) impr=" quiet ";
  if (Process::je_suis_maitre())
    {
      if((!sub_type(Matrice_Morse_Sym,matrice))&&(!sub_type(Matrice_Bloc,matrice)))
        {
          sortie <<" solveur gen { seuil "<<seuil <<" "<<impr<< " solv_elem bicgstab precond ilu { type 2 filling 10 } }" <<finl;
          sortie <<" solveur gen { seuil "<<seuil <<" "<<impr<< " solv_elem bicgstab precond ilu { type 2 filling 20 } }" <<finl;
          sortie <<" solveur gmres { diag seuil "<<seuil <<" "<<impr<<"}"<<finl;
#ifdef __PETSCKSP_H
          sortie <<" solveur petsc bicgstab { precond diag { }                  seuil "<<seuil <<" "<<impr<<"}"<<finl;
#endif
        }
      else
        {
          // on a une matrice de type pression (?)
          // Mise a jour des solveurs testes 24/05/2012
          sortie <<" solveur gcp       { precond ssor       { omega 1.6 }       seuil "<<seuil <<" "<<impr<<"}"<<finl;
#ifdef __PETSCKSP_H
          sortie <<" solveur petsc gcp { precond ssor       { omega 1.6 }       seuil "<<seuil <<" "<<impr<<"}"<<finl;
          if (Process::nproc()<512)
            sortie <<" solveur petsc cholesky { impr }"<< finl;
          else
            {
              // Pour les tres grands calculs on bascule de Cholesky a BICGSTAB ILU(1) par bloc
              sortie <<" solveur petsc bicgstab { precond block_jacobi_icc { level 1 } seuil "<<seuil <<" "<<impr<<"}"<<finl;
              // Voire CG ILU(1) par bloc car BICGSTAB peut avoir du mal a converger lors de la projection initiale...
              sortie <<" solveur petsc gcp { precond block_jacobi_icc { level 1 } seuil "<<seuil <<" "<<impr<<"}"<<finl;
            }
          // SPAI n'a jamais fait ses preuves (comme tous les preconditionnements de Hypre), on l'enleve
          //sortie <<" solveur petsc gcp { precond spai       { level 2 epsilon 0.2 } seuil "<<seuil <<" "<<impr<<"}"<<finl;
#endif
        }
    }
}
Entree& Test_solveur::interpreter(Entree& is)
{
  Matrice matrice;
  DoubleVect secmem,solution;
  Nom fichier_secmem("Secmem.sa");
  Nom fichier_solution("Solution.sa");
  Nom fichier_matrice("Matrice.sa");
  Nom fichier_solveur;
  int pas_de_solution_init=0,ascii;
  double seuil_verification=DMAXFLOAT;
  SolveurSys solveur;
  double seuil_list=0;
  int limpr_;
  int nb_test=2; // On teste 2 fois un solveur car la premiere fois, le cout du preconditionnement peut penaliser
  Param  param((*this).que_suis_je());
  param.ajouter("fichier_secmem",&fichier_secmem);  // nom du fichier contenant le second membre (Secmem.sa par defaut)
  param.ajouter("fichier_matrice",&fichier_matrice);  // nom du fichier contenant la matrice (Matrice.sa par defaut)
  param.ajouter("fichier_solution",&fichier_solution);  // nom du fichier contenant la solution (Solution.sa par defaut)
  param.ajouter("nb_test",&nb_test);  // nb de resolution pour mesurer le temps (un seul preconditionnemt)
  param.ajouter_flag("impr",&limpr_); // impr des solveurs
  param.ajouter("solveur",&solveur); // pour specifier un solveur
  param.ajouter("fichier_solveur",&fichier_solveur); // pour specifier un fichier contenant des solveurs
  param.ajouter("genere_fichier_solveur",&seuil_list); // genere le fichier de solveur avec un seuil donne
  param.ajouter("seuil_verification",&seuil_verification); // verifie si la soulution trouvee par la resolution est telle que Ax -b < seuil_verification
  param.ajouter_flag("pas_de_solution_initiale",&pas_de_solution_init); // pas_de_solution_initiale : on n'initialise pas la resolution avec la solution
  param.ajouter_flag("ascii",&ascii); // dans le cas ou les fichiers sont ascii
  param.lire_avec_accolades_depuis(is);
  int binaire=1;
  if (ascii)
    binaire=0;
  // On relit la matrice et le secmem
  {
    LecFicDistribue entree;
    entree.set_bin(binaire);
    entree.ouvrir(fichier_matrice);
    entree>>matrice;
    Cout<<" size of system "<<matrice.valeur( ).nb_colonnes()<<finl;
    //matrice.valeur().imprimer_formatte(Cout);
  }
  {
    LecFicDistribue entree;
    entree.set_bin(binaire);
    entree.ouvrir(fichier_secmem);
    MD_Vector_tools::restore_vector_with_md(secmem,entree);
    Cout<<" size of system "<<secmem.size_totale()<<finl;
  }


  if (pas_de_solution_init)
    {
      solution=secmem;
      solution=0.;
    }
  else
    {
      LecFicDistribue entree;
      entree.set_bin(binaire);
      entree.ouvrir(fichier_solution);

      MD_Vector_tools::restore_vector_with_md(solution,entree);
      Cout<<" size of system "<<solution.size_totale()<<finl;
      solution.set_md_vector(secmem.get_md_vector());
    }

  if (seuil_list!=0)
    {
      if (fichier_solveur==Nom())
        fichier_solveur="list_solveur";
      if (je_suis_maitre())
        {
          SFichier list_solveur(fichier_solveur);
          generate_defaut(matrice,seuil_list,list_solveur,limpr_);
        }
    }

  secmem.echange_espace_virtuel();
  solution.echange_espace_virtuel();
  //Champ_Inc bidon;
  //bidon.typer("Champ_P0_VDF");
  ArrOfDouble temps(nb_test);
  if (fichier_solveur==Nom())
    test_un_solveur(solveur,  matrice , secmem , solution , -10, temps,seuil_verification);
  else
    {
      LecFicDiffuse list_solveur(fichier_solveur);
      int numero_best=test_solveur(solveur,  matrice , secmem , solution  , -10, temps,list_solveur,seuil_verification);
      Cout <<"------------------------------------------------"<<finl;
      Cout <<"Best solver : number "<<numero_best<<" "<<solveur<<finl;
      Cout <<"Best CPU time = "<<temps[0]<<finl;
      Cout <<"------------------------------------------------"<<finl;
    }
  return is;
}

static int numero_solv_optimal=0; // numero du solveur pour avoir des noms de fichiers solveurs par defaut differents pour chaque solveur
Solv_Optimal::Solv_Optimal():n_resol_(0),n_reinit_(0)
{
  freq_recalc_ = (int)(pow(2.0,(double)((sizeof(True_int)*8)-1))-1);
  freq_recalc_ = 100;
  fichier_solveur_="solveurs_";
  fichier_solveur_+=Nom(numero_solv_optimal);
  fichier_solveur_non_recree_=0;
  numero_solv_optimal++;

}
Solv_Optimal::~Solv_Optimal()
{
  if (le_solveur_.non_nul()) Cerr<<" The solver used by Solv_Optimal was "<<le_solveur_<<finl;
}
Sortie& Solv_Optimal::printOn(Sortie& s ) const
{
  return s;
}

Entree& Solv_Optimal::readOn(Entree& is )
{
  int impr,quiet;
  Param param((*this).que_suis_je());
  param.ajouter("seuil",&seuil_,Param::REQUIRED); // seuil de resolution
  param.ajouter_flag("impr",&impr); // active impression des solveurs
  param.ajouter_flag("quiet",&quiet);
  param.ajouter_flag("save_matrice|save_matrix",&save_matrice_); // pour sauvegarder A,x,b
  param.ajouter("frequence_recalc",&freq_recalc_); // frequence pour reevaluer le solveur optimal
  param.ajouter("nom_fichier_solveur",&fichier_solveur_); //nom du fichier contenant les solveurs testes
  param.ajouter_flag("fichier_solveur_non_recree",&fichier_solveur_non_recree_); //  si flag mis alors le fichier n'est pas cree au debut du calcul
  param.lire_avec_accolades_depuis(is);
  fixer_limpr(impr);
  if (quiet)
    fixer_limpr(-1);

  return is;
}

// Description : methode clef de Solv_Optimal
// a la premiere iteration
//    genere le fichier fichier_solveur_ contenant la liste des solveurs a tester
//    prend le premier solveur du fichier
//    A la 3 ite et avec la frequence freq_recalc_ cherche le solveur le + rapide, en prenant en compte le fait que la matrice a change ou non
//    appel test_solveur
void Solv_Optimal::prepare_resol(const Matrice_Base& matrice, const DoubleVect& secmem, DoubleVect& solution, int nmax)
{
  if (n_resol_==0)
    {
      // premiere chose on genere le fichier par defaut, puis on cree le solveur avec le premier du fichier

      if ((!fichier_solveur_non_recree_)&&(je_suis_maitre()))
        {
          SFichier list_solveur(fichier_solveur_);
          generate_defaut(matrice,seuil_,list_solveur,limpr());
        }
      LecFicDiffuse list_solveur2(fichier_solveur_);
      Nom mot;
      list_solveur2>>mot;
      list_solveur2>>le_solveur_;
    }
  // OK
  n_resol_++;
  if ((n_resol_>=3)&&((n_resol_-3)%freq_recalc_==0))
    {
      LecFicDiffuse list_solveur(fichier_solveur_);
      int nb_ite;
      if (n_reinit_<2)
        nb_ite=2; // Matrice constante (on resout 2 fois pour ne retenir que le 2eme temps exempt d'eventuel preconditionnement)
      else
        nb_ite=1; // Matrice non constante (on resout 1 seul fois)
      ArrOfDouble temps(nb_ite);
      int numero_best=test_solveur(le_solveur_,  matrice , secmem , solution  , nmax, temps,list_solveur,seuil_);
      Cout <<"------------------------------------------------"<<finl;
      Cout <<"Best solver : number "<<numero_best<<" "<<le_solveur_<<finl;
      Cout <<"Best CPU time = "<<temps[0]<<finl;
      Cout <<"------------------------------------------------"<<finl;
      if (je_suis_maitre())
        {
          Nom best("best_");
          best+=fichier_solveur_;
          SFichier sf(best);
          sf << le_solveur_ <<finl;
        }
    }
}
int Solv_Optimal::resoudre_systeme(const Matrice_Base& matrice, const DoubleVect& secmem, DoubleVect& solution)
{
  prepare_resol(matrice,secmem,solution,-20);
  return le_solveur_->resoudre_systeme( matrice, secmem,  solution);
}
int Solv_Optimal::resoudre_systeme(const Matrice_Base& matrice, const DoubleVect& secmem, DoubleVect& solution, int nmax)
{
  prepare_resol(matrice,secmem,solution, nmax);
  return le_solveur_->resoudre_systeme( matrice, secmem,  solution, nmax );
}


void Solv_Optimal::reinit()
{
  n_reinit_++;
  if (le_solveur_.non_nul())
    le_solveur_->reinit();
}




