/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Solv_Externe.h>
#include <Matrice_Base.h>
#include <Matrice_Morse_Sym.h>
#include <Matrice_Bloc_Sym.h>
#include <Device.h>

Implemente_base_sans_constructeur_ni_destructeur(Solv_Externe,"Solv_Externe",SolveurSys_base);

Sortie& Solv_Externe::printOn(Sortie& s) const
{
  //s << chaine_lue_;
  return s;
}

// readOn
Entree& Solv_Externe::readOn(Entree& is)
{
  //create_solver(is);
  return is;
}

void Solv_Externe::construit_renum(const DoubleVect& b)
{
  // Initialisation du tableau items_to_keep_ si ce n'est pas deja fait
  nb_items_to_keep_ = MD_Vector_tools::get_sequential_items_flags(b.get_md_vector(), items_to_keep_, b.line_size());

  // Compute important value:
  secmem_sz_ = b.size_totale();
  nb_rows_ = nb_items_to_keep_;
  nb_rows_tot_ = mp_sum(nb_rows_);
  decalage_local_global_ = mppartial_sum(nb_rows_);
  //Journal()<<"nb_rows_=" << nb_rows_ << " nb_rows_tot_=" << nb_rows_tot_ << " decalage_local_global_=" << decalage_local_global_ << finl;

  /**********************/
  /* Build renum_ array */
  /**********************/
  //if (MatricePetsc_==nullptr)
  {
    const MD_Vector& md = b.get_md_vector();
    renum_.reset();
    renum_.resize(0, b.line_size());
    MD_Vector_tools::creer_tableau_distribue(md, renum_, RESIZE_OPTIONS::NOCOPY_NOINIT);
  }
  int cpt=0;
  int size=items_to_keep_.size_array();
  renum_ = INT_MAX; //pour crasher si le MD_Vector est incoherent
  ArrOfInt& renum_array = renum_;  // tableau vu comme lineaire
  for(int i=0; i<size; i++)
    {
      if(items_to_keep_[i])
        {
          renum_array[i]=cpt+decalage_local_global_;
          cpt++;
        }
    }
  renum_.echange_espace_virtuel();
  // Construction de index_
  index_.resize(size);
  int index = 0;
  // ToDo OpenMP factoriser avec ix car index_=ix-decalage_local_global_
  for (int i=0; i<size; i++)
    {
      if (items_to_keep_[i])
        {
          index_[i] = index;
          index++;
        }
      else
        index_[i] = -1;
    }
  // Construction de ix
  size=b.size_array();
  int colonne_globale=decalage_local_global_;
  ix.resize(size);
  for (int i=0; i<size; i++)
    if (items_to_keep_[i])
      {
        ix[i] = colonne_globale;
        colonne_globale++;
      }
    else
      ix[i] = -1;
}

void Solv_Externe::MorseSymToMorse(const Matrice_Morse_Sym& MS, Matrice_Morse& M)
{
  M = MS;
  Matrice_Morse mattmp(MS);
  M.transpose(mattmp);
  int ordre = M.ordre();
  for (int i=0; i<ordre; i++)
    if (M.nb_vois(i))
      M(i, i) = 0.;
  M = mattmp + M;
}

void Solv_Externe::construit_matrice_morse_intermediaire(const Matrice_Base& la_matrice, Matrice_Morse& matrice_morse_intermediaire)
{
  if (sub_type(Matrice_Morse_Sym, la_matrice))
    {
      // Exemple: matrice de pression en VEFPreP1B
      const Matrice_Morse_Sym& matrice_morse_sym = ref_cast(Matrice_Morse_Sym, la_matrice);
      assert(matrice_morse_sym.get_est_definie());
      MorseSymToMorse(matrice_morse_sym, matrice_morse_intermediaire);
    }
  else if (sub_type(Matrice_Bloc_Sym, la_matrice))
    {
      // Exemple : matrice de pression en VEF P0+P1+Pa
      const Matrice_Bloc_Sym& matrice = ref_cast(Matrice_Bloc_Sym, la_matrice);
      // Conversion de la matrice Matrice_Bloc_Sym au format Matrice_Morse_Sym
      Matrice_Morse_Sym matrice_morse_sym;
      matrice.BlocSymToMatMorseSym(matrice_morse_sym);
      MorseSymToMorse(matrice_morse_sym, matrice_morse_intermediaire);
      matrice_morse_sym.dimensionner(0, 0); // Destruction de la matrice morse sym desormais inutile
    }
  else if (sub_type(Matrice_Morse, la_matrice))
    {
      // Exemple : matrice implicite
      matrice_symetrique_ = 0;
    }
  else if (sub_type(Matrice_Bloc, la_matrice))
    {
      // Exemple : matrice de pression en VDF
      const Matrice_Bloc& matrice_bloc = ref_cast(Matrice_Bloc, la_matrice);
      if (!sub_type(Matrice_Morse_Sym, matrice_bloc.get_bloc(0, 0).valeur()))
        matrice_symetrique_ = 0;
      else
        {
          // Pour un solveur direct, operation si la matrice n'est pas definie (en incompressible VDF, rien n'etait fait...)
          Matrice_Morse_Sym& mat00 = ref_cast_non_const(Matrice_Morse_Sym, matrice_bloc.get_bloc(0, 0).valeur());
          if (solveur_direct() && mat00.get_est_definie() == 0 && Process::je_suis_maitre())
            mat00(0, 0) *= 2;
        }
      matrice_bloc.BlocToMatMorse(matrice_morse_intermediaire);
    }
  else
    {
      Cerr << "Error, we do not know yet treat a matrix of type " << la_matrice.que_suis_je() << finl;
      exit();
    }
}

void Solv_Externe::Create_lhs_rhs_onDevice()
{
  lhs_.resize(nb_rows_);
  rhs_.resize(nb_rows_);
}

template<typename ExecSpace>
void Solv_Externe::Update_lhs_rhs(const DoubleVect& tab_b, DoubleVect& tab_x)
{
  int size = tab_b.size_array();
  auto x = tab_x.template view_ro<ExecSpace>();
  auto b = tab_b.template view_ro<ExecSpace>();
  auto index = static_cast<const ArrOfInt&>(index_).template view_ro<ExecSpace>();
  auto lhs = lhs_.template view_wo<ExecSpace>();
  auto rhs = rhs_.template view_wo<ExecSpace>();
  Kokkos::RangePolicy<ExecSpace> policy({0}, {size});
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(
                         const int i)
  {
    int ind = index[i];
    if (ind != -1)
      {
        lhs[ind] = x[i];
        rhs[ind] = b[i];
      }
  });
  static constexpr bool kernelOnDevice = !std::is_same<ExecSpace, Kokkos::DefaultHostExecutionSpace>::value;
  end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
}

template<typename ExecSpace>
void Solv_Externe::Update_solution(DoubleVect& tab_x)
{
  int size = tab_x.size_array();
  auto index = static_cast<const ArrOfInt&>(index_).template view_ro<ExecSpace>();
  auto lhs = lhs_.template view_ro<ExecSpace>();
  auto x = tab_x.template view_wo<ExecSpace>();
  Kokkos::RangePolicy<ExecSpace> policy({0}, {size});
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(
                         const int i)
  {
    int ind = index[i];
    if (ind != -1)
      x[i] = lhs[ind];
  });
  static constexpr bool kernelOnDevice = !std::is_same<ExecSpace, Kokkos::DefaultHostExecutionSpace>::value;
  end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
}

template void Solv_Externe::Update_lhs_rhs<Kokkos::DefaultExecutionSpace>(const DoubleVect&, DoubleVect&);
template void Solv_Externe::Update_lhs_rhs<Kokkos::DefaultHostExecutionSpace>(const DoubleVect&, DoubleVect&);
template void Solv_Externe::Update_solution<Kokkos::DefaultExecutionSpace>(DoubleVect&);
template void Solv_Externe::Update_solution<Kokkos::DefaultHostExecutionSpace>(DoubleVect&);

