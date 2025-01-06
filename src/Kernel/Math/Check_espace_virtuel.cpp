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

#include <Check_espace_virtuel.h>
#include <MD_Vector_base.h>

/*! @brief Verifie si le vecteur v a son espace virtuel a jour.
 *
 * Cette fonction doit etre appelee simultanement par tous les processeurs qui partagent le vecteur. (pour cela, on cree une copie qu'on echange, puis on compare a l'original).
 *
 */
int check_espace_virtuel_vect(const DoubleVect& v)
{
  if (Process::is_sequential()) return 1; // ToDo Provisoire on ne verifie pas en sequentiel
  // On fait une copie:
  DoubleVect w(v);
  // On echange
  w.echange_espace_virtuel();
  // Norme de w :
  const double norme_w = norme_array(w);
  // On compare : difference entre les deux vecteurs au sens
  // ArrOfDouble (pas d'echange espace virtuel)
  w.ArrOfDouble::operator-=(v);
  const double ecart = norme_array(w);
  if (ecart > (norme_w + 1e-30) * 1e-12)
    return 0;
  else
    return 1;
}

/*! @brief Idem que  check_espace_virtuel_vect(const DoubleVect & v)
 *
 */
int check_espace_virtuel_vect(const IntVect& v)
{
  // On fait une copie:
  IntVect w(v);
  // On echange
  w.echange_espace_virtuel();
  // On compare:
  const int n = v.size_array();
  for (int i = 0; i < n; i++)
    if (v[i] != w[i])
      return 0;
  return 1;
}

/*! @brief Appelle remplir_items_non_calcules() si on est en mode comm_check_enabled() ou en mode debug (NDEBUG non defini)
 *
 *   La "valeur" par defaut est censee provoquer une erreur si on
 *   essaye de l'utiliser.
 *
 */
void assert_invalide_items_non_calcules(DoubleVect& v, double valeur)
{
#ifdef NDEBUG
  if (Comm_Group::check_enabled())
    remplir_items_non_calcules(v, valeur);
#else
  remplir_items_non_calcules(v, valeur);
#endif
}

#ifndef LATATOOLS
namespace
{
template <typename ExecSpace, typename _TYPE_>
void remplir_items_non_calcules_kernel_(TRUSTVect<_TYPE_>& v, _TYPE_ valeur, const ArrOfInt& blocs, const int sz, const int line_size)
{
  auto v_view= v.template view_wo<1, ExecSpace>();

  int j = 0;

  for (int i = 0; i <= sz; i++)
    {

      // Fill the elemns until the beginning of the bloc
      // If i ==sz, fill the elems between the end of the last block and the end of the vect
      // (<=sz instead of <sz + extra loop: small) Hack from Remi B to avoid re-writing the loop two times

      const int j_fin = i==sz ? v.size_array() : blocs[i*2] * line_size;
      assert(j >= 0 && j_fin <= v.size_array());

      Kokkos::RangePolicy<ExecSpace> policy(j, j_fin);
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),policy,
      KOKKOS_LAMBDA(const int k) {v_view[k]=valeur;});

      bool kernelOnDevice = is_default_exec_space<ExecSpace>;
      end_gpu_timer(__KERNEL_NAME__, kernelOnDevice);

      // Sauter a la fin du bloc
      if (i<sz) j = blocs[i*2+1] * line_size;
    }
}
}
#endif

template <typename _TYPE_>
void remplir_items_non_calcules_(TRUSTVect<_TYPE_>& v, _TYPE_ valeur)
{
#ifndef LATATOOLS
  if (v.get_md_vector().non_nul() && Process::is_parallel()) // Checking virtual items in sequential is meaningless
    {
      const ArrOfInt& blocs = v.get_md_vector()->get_items_to_compute();
      const int sz = blocs.size_array() / 2, line_size = v.line_size();

      bool kernelOnDevice = v.checkDataOnDevice();

      if (kernelOnDevice)
        remplir_items_non_calcules_kernel_<Kokkos::DefaultExecutionSpace, _TYPE_>(v, valeur, blocs, sz, line_size);
      else
        remplir_items_non_calcules_kernel_<Kokkos::DefaultHostExecutionSpace, _TYPE_>(v, valeur, blocs, sz, line_size);
    }
#else
  return
#endif
}

// Explicit instanciation
template void remplir_items_non_calcules_<double>(TRUSTVect<double>& v, double valeur);
template void remplir_items_non_calcules_<int>(TRUSTVect<int>& v, int valeur);
template void remplir_items_non_calcules_<float>(TRUSTVect<float>& v, float valeur);
