/****************************************************************************
* Copyright (c) 2023, CEA
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
  if (Process::nproc()==1) return 1; // ToDo Provisoire on ne verifie pas en sequentiel
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

template <typename _TYPE_>
void remplir_items_non_calcules_(TRUSTVect<_TYPE_>& v, _TYPE_ valeur)
{
  if (v.get_md_vector().non_nul())
    {
      const ArrOfInt& blocs = v.get_md_vector().valeur().get_items_to_compute();
      const int sz = blocs.size_array() / 2, line_size = v.line_size();
      int j = 0;
      // Ne pas passer par operator[], sinon plantage si la valeur actuelle est invalide
      bool kernelOnDevice = v.checkDataOnDevice();
      _TYPE_ *ptr = computeOnTheDevice(v, "", kernelOnDevice);
      start_timer();
      for (int i = 0; i < sz; i++)
        {
          // remplir les elements jusqu'au debut du bloc:
          const int j_fin = blocs[i*2] * line_size;
          assert(j >= 0 && j_fin <= v.size_array());
          #pragma omp target teams distribute parallel for if (kernelOnDevice)
          for (int k=j; k < j_fin; k++)
            ptr[k] = valeur;
          // Sauter a la fin du bloc
          j = blocs[i*2+1] * line_size;
        }
      // Remplir les elements entre la fin du dernier bloc et la fin du vecteur
      const int j_fin = v.size_array();
      #pragma omp target teams distribute parallel for if (kernelOnDevice)
      for (int k=j; k < j_fin; k++)
        ptr[k] = valeur;
      end_timer(kernelOnDevice, "remplir_items_non_calcules_(x)");
    }
}
// Explicit instanciation
template void remplir_items_non_calcules_<double>(TRUSTVect<double>& v, double valeur);
template void remplir_items_non_calcules_<int>(TRUSTVect<int>& v, int valeur);
template void remplir_items_non_calcules_<float>(TRUSTVect<float>& v, float valeur);
