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
// File:        Solv_GCP.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/54
//
//////////////////////////////////////////////////////////////////////////////

#include <Solv_GCP.h>
#include <SSOR.h>
#include <Param.h>
#include <Matrice_Bloc_Sym.h>
#include <Sparskit.h>
#include <Check_espace_virtuel.h>
#include <MD_Vector_base.h>
#include <MD_Vector_tools.h>
#include <communications.h>
#include <ConstDoubleTab_parts.h>

Implemente_instanciable_sans_constructeur(Solv_GCP,"Solv_GCP",solv_iteratif);
//
// printOn et readOn

Solv_GCP::Solv_GCP()
{
  seuil_=1.e-12;
  reinit_ = 0;
  precond_diag_ = 0;
  optimized_ = 0;
  nb_it_max_=-1;
}

Sortie& Solv_GCP::printOn(Sortie& s ) const
{
  s<<" { seuil " << seuil_ ;
  if (le_precond_.non_nul())
    s<<" precond " <<le_precond_;
  else
    s<<" precond_nul ";
  if (limpr()==1) s<<" impr ";
  if (limpr()==-1) s<<" quiet ";
  if (save_matrice_) s<<" save_matrice ";
  if (nb_it_max_!=-1) s<<" nb_it_max "<<nb_it_max_;

  s<<" } ";
  return s ;
}

Entree& Solv_GCP::readOn(Entree& is )
{
  int precond_nul;
  int impr,quiet;
  Param param((*this).que_suis_je());
  param.ajouter("seuil",&seuil_,Param::REQUIRED);
  param.ajouter("nb_it_max",&nb_it_max_);
  param.ajouter_flag("impr",&impr);
  param.ajouter_flag("quiet",&quiet);
  param.ajouter_flag("save_matrice|save_matrix",&save_matrice_);
  param.ajouter("precond",&le_precond_);
  param.ajouter_flag("precond_nul",&precond_nul);
  param.ajouter_flag("precond_diagonal", &precond_diag_);
  param.ajouter_flag("optimized", &optimized_);
  param.lire_avec_accolades_depuis(is);
  // Obligation de definir un precond
  if (!le_precond_.non_nul() && precond_nul==0 && precond_diag_==0)
    {
      Cerr << "You forgot to define a preconditionner with the keyword precond." << finl;
      Cerr << "If you don't want a preconditionner, add for the solver definition:" << finl;
      Cerr << "precond_nul" << finl;
      Process::exit();
    }
  if (precond_nul)
    {
      le_precond_.detach();
    }
  assert(seuil_>0);
  fixer_limpr(impr);
  if (quiet)
    fixer_limpr(-1);
  return is;
}

int Solv_GCP::resoudre_systeme(const Matrice_Base& matrice, const DoubleVect& secmem, DoubleVect& solution)
{


  int n = resoudre_(matrice, secmem, solution, 100);
  return n;
}

int Solv_GCP::resoudre_systeme(const Matrice_Base& matrice, const DoubleVect& secmem, DoubleVect& solution,
                               int nmax)
{


  int n = resoudre_(matrice, secmem, solution, nmax);
  return n;
}


void Solv_GCP::reinit()
{
  if (reinit_ > 1) // Si reinit_ = 0, ne pas toucher.
    reinit_ = 1;
  SolveurSys_base::reinit();
  if (le_precond_.non_nul())
    le_precond_.valeur().reinit();
}

void Solv_GCP::prepare_data(const Matrice_Base& matrice, const DoubleVect& secmem, DoubleVect& solution)
{
  if (reinit_ == 0)
    {
      // Reconstruction de toute la structure (tableaux d'index et coefficients)

      if (secmem.line_size() != 1)
        {
          Cerr << "Error line_size>1 not coded (GCP)" << finl;
          exit();
        }

      const Matrice_Bloc& mat_bloc = ref_cast(Matrice_Bloc, matrice);
      const Matrice_Morse_Sym& mat = ref_cast(Matrice_Morse_Sym, mat_bloc.get_bloc(0,0).valeur());
      const Matrice_Morse& mat_virt = ref_cast(Matrice_Morse, mat_bloc.get_bloc(0,1).valeur());

      // Determination du nombre d'items reellement utilises:
      {
        const int sztot_source = secmem.size_array();
        const int sz = secmem.size();
        renum_.reset();
        renum_.resize(sztot_source, Array_base::NOCOPY_NOINIT);
        renum_ = 0;
        // Retirer les items virtuels
        int i;
        for (i = sz; i < sztot_source; i++)
          renum_[i] = -1;
        renum_.set_md_vector(secmem.get_md_vector());
        const ArrOfInt& tab2 = mat_virt.get_tab2();
        const int n = tab2.size_array();
        for (i = 0; i < n; i++)
          {
            // Attention: tab2 de la partie reele-virtuelle contient des indices
            //  relatifs au debut de la partie virtuelle (d'ou "+ sz")
            const int j = tab2[i]-1 + sz; // fortran -> c
            renum_[j] = 0;
          }
      }
      // Determination du nombre de lignes non vides de mat_virt
      int nb_lignes_mat_virt = 0;
      {
        const int n = mat_virt.get_tab1().size_array() - 1;
        for (int i = 0; i < n; i++)
          if (mat_virt.get_tab1()(i+1) - mat_virt.get_tab1()(i) > 0)
            nb_lignes_mat_virt++;
      }

      // Descripteur contenant uniquement les items utiles:
      MD_Vector md;
      MD_Vector_tools::creer_md_vect_renum_auto(renum_, md);
      const int sz_tot = md.valeur().get_nb_items_tot();
      const int sz = md.valeur().get_nb_items_reels();

      // Calcul de la taille memoire requise:
      int mem_size = 0;
      mem_size += sz_tot * sizeof(double); //  vecteurs avec espace virtuel (tmp_p_)
      mem_size += sz * sizeof(double) * 3; // vecteurs sans espace virtuel
      const int nb_lignes_mat = sz;
      // matrice reel/reel
      mem_size += (sz + 1) * sizeof(int); // pour tab1_
      int nnz_reel_reel = 0;
      assert(mat.get_tab1().size_array() == sz + 1);
      assert(mat.get_tab2().size_array() == mat.get_coeff().size_array());
      if (! precond_diag_)
        {
          nnz_reel_reel = mat.get_coeff().size_array();
        }
      else
        {
          // On ne stocke pas les coefficients diagonaux:
          nnz_reel_reel = mat.get_coeff().size_array() - sz;
        }
      mem_size += nnz_reel_reel * sizeof(double); // pour les coefficients
      mem_size += nnz_reel_reel * sizeof(int); // pour les indices
      // matrice reel/virtuel
      mem_size += (nb_lignes_mat_virt+1) * sizeof(int); // pour tab1_
      const int nnz_reel_virtuel = mat_virt.get_coeff().size_array();
      assert(mat_virt.get_tab2().size_array() == nnz_reel_virtuel);
      mem_size += nnz_reel_virtuel * sizeof(double); // pour les coefficients
      mem_size += nnz_reel_virtuel * sizeof(int); // pour les indices
      // taille de tmp_mat_virt_.lignes_non_vides_
      mem_size += nb_lignes_mat_virt * sizeof(int);
      // aligner la taille sur un multiple de 8
      if (mem_size % 8 != 0)
        mem_size = (mem_size/8+1)*8;

      // Allocation des tableaux:
      // (on met d'abord tous les tableaux de double, puis a la fin les tableaux d'entiers
      //  sinon il faut ajouter du padding pour aligner si on remet des double apres de int)
      //
      Journal() << "Solv_GCP::prepare allocating data chunk : " << mem_size << " bytes" << finl;
      tmp_data_block_.resize_array(mem_size/8, Array_base::NOCOPY_NOINIT);

      double *ptr = tmp_data_block_.addr();
      resu_.ref_data(ptr, sz);
      ptr += sz;
      residu_.ref_data(ptr, sz);
      ptr += sz;
      tmp_p_avec_items_virt_.ref_data(ptr, sz_tot); // avec espace virtuel
      tmp_p_avec_items_virt_.set_md_vector(md);
      // tmp_p_ pointe sur la meme zone:
      tmp_p_.ref_data(ptr, sz); // sans l'espace virtuel
      ptr += sz_tot;
      tmp_solution_.ref_data(ptr, sz);
      ptr += sz;
      // Allocation des tableaux pour les matrices:
      tmp_mat_.get_set_coeff().ref_data(ptr, nnz_reel_reel);
      ptr += nnz_reel_reel;
      tmp_mat_virt_.get_set_coeff().ref_data(ptr, nnz_reel_virtuel);
      ptr += nnz_reel_virtuel;
      // On a fini les double, on passe aux tableaux d'entiers:
      int * iptr = (int*)ptr;
      tmp_mat_.get_set_tab1().ref_data(iptr, nb_lignes_mat + 1);
      iptr += nb_lignes_mat + 1;
      tmp_mat_.get_set_tab2().ref_data(iptr, nnz_reel_reel);
      iptr += nnz_reel_reel;
      tmp_mat_virt_.lignes_non_vides_.ref_data(iptr, nb_lignes_mat_virt);
      iptr += nb_lignes_mat_virt;
      tmp_mat_virt_.get_set_tab1().ref_data(iptr, nb_lignes_mat_virt + 1);
      iptr += nb_lignes_mat_virt + 1;
      tmp_mat_virt_.get_set_tab2().ref_data(iptr, nnz_reel_virtuel);
      iptr += nnz_reel_virtuel;
      // Allocation terminee.
      assert(((char*)iptr) <= ((char*)tmp_data_block_.addr() + mem_size));

      // Remplissages des tableaux d'index (tab1_, tab2_ et lignes_non_vides_)
      if (! precond_diag_)
        {
          tmp_mat_.get_set_tab1().inject_array(mat.get_tab1());
          {
            // remplissage de tab2 (renumerotation eventuelle)
            for (int i = 0; i < nnz_reel_reel; i++)
              {
                int j = mat.get_tab2()(i)-1; // fortran->c
                int rj = renum_[j];
                assert(rj < sz_tot);
                tmp_mat_.get_set_tab2()(i) = rj+1; // c->fortran
              }
          }
          tmp_mat_.set_nb_columns( sz_tot );
        }
      else
        {
          // Construction de la matrice D^(-1/2) * A * D^(-1/2)
          // on ne stocke pas les coeffs diagonaux
          // Le remplissage de tab1_ n'est pas trivial, du coup:
          {
            int src_index = 0; // index dans mat.tab2_ et coeff_
            int dest_index = 0; // index dans tmp_mat_.tab2_ et coeff_
            int i_ligne;
            for (i_ligne = 0; i_ligne < nb_lignes_mat; i_ligne++)
              {
                // A chaque ligne on a un coefficient de moins que dans la matrice d'origine
                // (on ne met pas le coeff diagonal)
                tmp_mat_.get_set_tab1()(i_ligne) = dest_index + 1; // indice fortran du debut de ligne
                const int ncoeff = mat.get_tab1()(i_ligne+1) - mat.get_tab1()(i_ligne) - 1;
                // Ne pas inserer le coeff diagonal
                assert(mat.get_tab2()(src_index) == i_ligne + 1); // index fortran
                src_index++;
                // Inserer les autres coeffs:
                for (int i = 0; i < ncoeff; i++, src_index++, dest_index++)
                  tmp_mat_.get_set_tab2()(dest_index) = mat.get_tab2()(src_index);

              }
            // Fin de la derniere ligne:
            tmp_mat_.get_set_tab1()(i_ligne) = dest_index + 1; // indice fortran du debut de ligne
          }
          tmp_mat_.set_nb_columns( sz_tot );
        }
      // remplissage de tmp_mat_virt_
      {
        tmp_mat_virt_.get_set_tab1()(0) = 1;
        int i_ligne_dest = 0;
        int dest_index = 0;
        for (int i_ligne = 0; i_ligne < nb_lignes_mat; i_ligne++)
          {
            const int count = mat_virt.get_tab1()(i_ligne+1) - mat_virt.get_tab1()(i_ligne);
            if (count > 0)
              {
                tmp_mat_virt_.lignes_non_vides_[i_ligne_dest] = i_ligne + 1; // indice fortran
                tmp_mat_virt_.get_set_tab1()(i_ligne_dest) = dest_index + 1; // index fortran
                i_ligne_dest++;
                int src_index = mat_virt.get_tab1()(i_ligne) - 1; // fortran->c
                for (int i = 0; i < count; i++, src_index++, dest_index++)
                  {
                    // mat_virt contient des indices fortran relatifs au debut de la partie virtuelle,
                    // on transform en indice C, relatif au vecteur complet
                    int j = mat_virt.get_tab2()(src_index) + sz - 1;
                    int rj = renum_[j];
                    // on stocke dans tmp_mat_virt des indices de colonnes relatifs au vecteur complet
                    // (pas seulement la partie virtuelle)
                    tmp_mat_virt_.get_set_tab2()(dest_index) = rj + 1; // indice fortran
                  }
              }
          }
        // Fin de la derniere ligne:
        tmp_mat_virt_.get_set_tab1()(i_ligne_dest) = dest_index + 1; // index fortran
      }
      reinit_ = 1;
    }

  if (reinit_ < 2)
    {
      const Matrice_Bloc& mat_bloc = ref_cast(Matrice_Bloc, matrice);
      const Matrice_Morse_Sym& mat = ref_cast(Matrice_Morse_Sym, mat_bloc.get_bloc(0,0).valeur());
      const Matrice_Morse& mat_virt = ref_cast(Matrice_Morse, mat_bloc.get_bloc(0,1).valeur());
      if (!precond_diag_)
        {
          tmp_mat_.get_set_coeff().inject_array(mat.get_coeff());
          tmp_mat_virt_.get_set_coeff().inject_array(mat_virt.get_coeff());
        }
      else
        {
          // calcul de D^(-1/2)
          exit();
          // calcul du produit D^(-1/2) * A * D^(-1/2)

        }
      reinit_ = 2;
    }

  if (!precond_diag_)
    {
      tmp_solution_.inject_array(solution, tmp_solution_.size());
      resu_.inject_array(secmem, resu_.size_array());
    }
  else
    {
      exit();
    }
}

// Calcul de vx = vx * alpha - vy
static void multiply_sub(DoubleVect& vx, DoubleVect& vy, double alpha)
{
  int n = vx.size_reelle_ok() ? vx.size() : vx.size_totale();
  assert(vy.size_totale() >= n);
  double *x_ptr = vx.addr();
  double *y_ptr = vy.addr();
  for (n = n - 1; n > 0; n -= 2, x_ptr += 2, y_ptr += 2)
    {
      double a = x_ptr[0] * alpha - y_ptr[0];
      double b = x_ptr[1] * alpha - y_ptr[1];
      x_ptr[0] = a;
      x_ptr[1] = b;
    }
  // n etait-il impair au depart ?
  if (n == 0)
    x_ptr[0] = x_ptr[0] * alpha - y_ptr[0];
}

// Calcul de vx += alpha * vy
// Valeur de retour: somme locale sur ce processeur des vx[i]*vx[i] (apres ajout)
// Attention: pas code pour les items communs
static double ajoute_alpha_v_norme(DoubleVect& vx, double alpha, DoubleVect& vy)
{
  int n = vx.size();
  assert(vy.size() == n);
  double *x_ptr = vx.addr();
  double *y_ptr = vy.addr();
  double norme1 = 0., norme2 = 0.;
  for (n = n - 1; n > 0; n -= 2, x_ptr += 2, y_ptr += 2)
    {
      double a = x_ptr[0] + alpha * y_ptr[0];
      double b = x_ptr[1] + alpha * y_ptr[1];
      x_ptr[0] = a;
      x_ptr[1] = b;
      norme1 += a * a;
      norme2 += b * b;
    }
  // n etait-il impair au depart ?
  if (n == 0)
    {
      double a = x_ptr[0] + alpha * y_ptr[0];
      x_ptr[0] = a;
      norme1 += a * a;
    }
  return norme1 + norme2;
}

int Solv_GCP::resoudre_(const Matrice_Base& matrice,
                        const DoubleVect& secmem,
                        DoubleVect& solution,
                        int nmax)
{
  const int n_items_reels = solution.size_reelle_ok() ? solution.size_reelle() : solution.size_totale();
  {
    const int nb_items_seq = solution.get_md_vector().valeur().nb_items_seq_tot();
    const int ls = secmem.line_size();
    const int nb_inco_tot = nb_items_seq * ls;
    nmax = max(nb_inco_tot, nmax);
  }

  const int avec_precond = le_precond_.non_nul();
  const int precond_requires_echange_espace_virtuel =
    avec_precond && (le_precond_.valeur().get_flag_updated_input());

  const int optimized = optimized_;

  if (optimized)
    {
      prepare_data(matrice, secmem, solution);
    }
  else
    {
      resu_.reset();
      residu_.reset();
      tmp_p_avec_items_virt_.reset();
      resu_.copy(solution, Array_base::NOCOPY_NOINIT);
      residu_.copy(solution, Array_base::NOCOPY_NOINIT);
      tmp_p_avec_items_virt_.copy(solution, Array_base::NOCOPY_NOINIT);
      tmp_p_.ref(tmp_p_avec_items_virt_);
      tmp_solution_.ref(solution);
      resu_.inject_array(secmem);
    }

  tmp_p_avec_items_virt_.inject_array(tmp_solution_, n_items_reels);
  tmp_p_avec_items_virt_.echange_espace_virtuel();
  // On n'a pas besoin que residu ait son espace virtuel a jour
  // En revanche, on a besoin de l'espace virtuel de tmp_p_...
  if (optimized)
    {
      tmp_mat_.multvect_(tmp_p_avec_items_virt_, residu_);
      // calcul du produit, le produit scalaire n'est pas utilise:
      tmp_mat_virt_.ajouter_mult_vect_et_prodscal(tmp_p_avec_items_virt_, residu_);
    }
  else
    {
      matrice.multvect(tmp_p_avec_items_virt_, residu_);
    }
  // ATTENTION: on suppose que secmem a ete copie dans resu_
  operator_sub(residu_, resu_, VECT_REAL_ITEMS); // ne pas toucher a l'espace virtuel

  if (avec_precond)
    {
      if (precond_requires_echange_espace_virtuel)
        residu_.echange_espace_virtuel();

      if (optimized)
        le_precond_.valeur().preconditionner(tmp_mat_, residu_, tmp_p_);
      else
        le_precond_.valeur().preconditionner(matrice, residu_, tmp_p_);

    }
  else
    {
      tmp_p_.inject_array(residu_, n_items_reels);
    }
  double dold = mp_prodscal(residu_, tmp_p_);

  operator_negate(tmp_p_, VECT_REAL_ITEMS);

  double norme = mp_norme_vect(residu_);
  double norme_b = mp_norme_vect(resu_);

  if (limpr()==1)
    {
      double norme_relative=(norme_b>DMINFLOAT?norme/(norme_b+DMINFLOAT):norme);
      Cout << "Norm of the residue: " << norme << " (" << norme_relative << ")" << finl;
    }
  int niter = 0;
  int nb_it_max=nmax;
  if (nb_it_max_>-1)
    nb_it_max=nb_it_max_;
  while ( ( norme > seuil_ ) && (niter++ < nmax) &&( niter<nb_it_max))
    {
      // Precondition pour multvect
      // (le seul echange espace virtuel de l'algo sauf si le precond en a besoin)
      tmp_p_avec_items_virt_.echange_espace_virtuel();
      // En revanche, on n'a pas besoin de l'espace virtuel a jour de resu:
      double resu_scalaire_p_local;
      if (optimized)
        {
          resu_scalaire_p_local = tmp_mat_.multvect_et_prodscal(tmp_p_avec_items_virt_, resu_);
          resu_scalaire_p_local += tmp_mat_virt_.ajouter_mult_vect_et_prodscal(tmp_p_avec_items_virt_, resu_);
        }
      else
        {
          matrice.multvect(tmp_p_avec_items_virt_, resu_);
          resu_scalaire_p_local = local_prodscal(resu_, tmp_p_avec_items_virt_);
        }
      const double resu_scalaire_p = mp_sum(resu_scalaire_p_local);
      const double alfa = dold / resu_scalaire_p;
      ajoute_alpha_v(tmp_solution_, alfa, tmp_p_, VECT_REAL_ITEMS);

      double norme_residu_locale;
      if (optimized)
        {
          norme_residu_locale = ajoute_alpha_v_norme(residu_, alfa, resu_);
        }
      else
        {
          ajoute_alpha_v(residu_, alfa, resu_);
          norme_residu_locale = local_carre_norme_vect(residu_);
        }

      if(avec_precond)
        {
          if (precond_requires_echange_espace_virtuel)
            residu_.echange_espace_virtuel();
          if (optimized)
            le_precond_.valeur().preconditionner(tmp_mat_, residu_, resu_);
          else
            le_precond_.valeur().preconditionner(matrice, residu_, resu_);

          double residu_scalaire_resu = local_prodscal(residu_, resu_);
          norme = norme_residu_locale;
          // optimisation: on calcule en une seule fois les deux sommes
          mpsum_multiple(residu_scalaire_resu, norme);
          assert(residu_scalaire_resu >= 0);
          multiply_sub(tmp_p_, resu_, residu_scalaire_resu / dold);
          dold = residu_scalaire_resu;
        }
      else
        {
          const double dnew = mp_carre_norme_vect(residu_);
          norme = mp_sum(norme_residu_locale);
          assert(dnew >= 0);
          multiply_sub(tmp_p_, residu_, dnew / dold);
          dold = dnew;
        }
      norme = sqrt(norme);

      if (limpr()==1)
        {
          Cout << norme << " ";
          if ((niter % 15) == 0) Cout << finl ;
        }
    }
  if ((nb_it_max_<0)&& (norme > seuil_))
    {
      Cerr << "No convergence after : " << niter << " iterations\n";
      Cerr << " Residue : "<< norme << "\n";
      Cerr << " threshold : "<< seuil_ << "\n";
      Cerr << "Change your data set." << finl;
      exit();
    }

  if (optimized)
    solution.inject_array(tmp_solution_, n_items_reels);

  // The user wants a result with updated virtual space:
  if (get_flag_updated_result())
    solution.echange_espace_virtuel();

  // On affiche quand meme le nombre d'iterations
  if (limpr()>-1)
    {
      double norme_relative=(norme_b>0?norme/(norme_b+DMINFLOAT):norme);
      Cout << finl;
      Cout << "Final residue: " << norme << " ( " << norme_relative << " )"<<finl;
    }
  return(niter);

}



