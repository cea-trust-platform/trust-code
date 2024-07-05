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

#ifndef SSE_Kernels_H_TPP
#define SSE_Kernels_H_TPP
#include <iostream>
#include <sys/time.h>
#include <IJK_Field.h>

// Factors to apply to the number of iterations (0=>no performance check, 1=>check for 1 second or so)
#ifndef NDEBUG
static const double check_performance = 0.002;
#else
static const double check_performance = 0.2;
#endif

static inline double get_clock()
{
  struct timeval time;
  gettimeofday(& time, 0);
  return (double) time.tv_sec + 0.000001 * (double)time.tv_usec;
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void fill_dummy(IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& tab, int seed)
{
  const int ghost = tab.ghost();
  const int imin = -ghost;
  const int imax = tab.ni() + ghost;
  const int jmin = -ghost;
  const int jmax = tab.nj() + ghost;
  const int kmin = -ghost;
  const int kmax = tab.nk() + ghost;
  if (tab.nb_compo() == 1)
    {
      for (int k = kmin; k < kmax; k++)
        for (int j = jmin; j < jmax; j++)
          for (int i = imin; i < imax; i++)
            {
              double x = i * 0.2 + j * 0.9893 + k * 0.87987 + seed;
              tab(i,j,k) = (_TYPE_)(1. + 0.5 * sin(x));
            }
    }
  else
    {
      const int n = tab.nb_compo();
      for (int k = kmin; k < kmax; k++)
        for (int compo = 0; compo < n; compo++)
          for (int j = jmin; j < jmax; j++)
            for (int i = imin; i < imax; i++)
              {
                double x = i * 0.2 + j * 0.9893 + k * 0.87987 + compo * 0.238768 + seed;
                tab(i,j,k,compo) = (_TYPE_)(1. + 0.5 * sin(x));
              }
    }
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void reference_kernel_template(const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& tab,
                               const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& coeffs,
                               const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& secmem, IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& resu,
                               const _TYPE_ relax, const bool residue)
{
  const _TYPE_ one_minus_relax = (_TYPE_)(1. - relax);

  const int kmin = - tab.ghost() + 1;
  const int kmax = tab.nk() + tab.ghost() - 1;
  const int jmin = - tab.ghost() + 1;
  const int jmax = tab.nj() + tab.ghost() - 1;
  const int imin = - tab.ghost() + 1;
  const int imax = tab.ni() + tab.ghost() - 1;
  int i, j, k;
  for (k = kmin; k < kmax; k++)
    {
      for (j = jmin; j < jmax; j++)
        {
          for (i = imin; i < imax; i++)
            {
              _TYPE_ x =
                coeffs(i,j,k,0) * tab(i-1,j,k)
                + coeffs(i+1,j,k,0) * tab(i+1,j,k)
                + coeffs(i,j,k,1) * tab(i,j-1,k)
                + coeffs(i,j+1,k,1) * tab(i,j+1,k)
                + coeffs(i,j,k,2) * tab(i,j,k-1)
                + coeffs(i,j,k+1,2) * tab(i,j,k+1);
              _TYPE_ r;
              if (residue)
                r = tab(i,j,k) * coeffs(i,j,k,3) - x - secmem(i,j,k);
              else
                r = tab(i,j,k) * one_minus_relax + (x + secmem(i,j,k)) * relax / coeffs(i,j,k,3);
              resu(i,j,k) = r;
            }
        }
    }
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ compute_difference(const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& a, const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& b)
{
  _TYPE_ diff = 0;
  const int ni = a.ni();
  const int nj = a.nj();
  const int nk = a.nk();
  int i, j, k;
  for (i = 0; i < ni; i++)
    {
      for (j = 0; j < nj; j++)
        {
          for (k = 0; k < nk; k++)
            {
              diff = std::max(diff, std::fabs(a(i,j,k) - b(i,j,k)));
              assert(std::fabs(a(i,j,k) - b(i,j,k)) < 1e-4);
            }
        }
    }
  return diff;
}

template <typename _TYPE_, int _KSTRIDE_, int _JSTRIDE_>
void Jacobi_Residue_template(const _TYPE_ *tab, const _TYPE_ *coeffs_ptr, const _TYPE_ *secmem_ptr,
                             _TYPE_ *result_ptr,
                             const int kstride_input, const int jstride_input, const int nvalues,
                             const _TYPE_ relax_coefficient,
                             bool is_jacobi
                            )
{
  const int vsize = Simd_template<_TYPE_>::size();
  const int kstride = _KSTRIDE_ != SSE_Kernels::GENERIC_STRIDE? _KSTRIDE_ : kstride_input;
  const int jstride = _JSTRIDE_ != SSE_Kernels::GENERIC_STRIDE? _JSTRIDE_ : jstride_input;
  assert(kstride == kstride_input);
  assert(jstride == jstride_input);
  const int nvectors = (nvalues + vsize-1) / vsize;
  // Code version 1: perform all operations at once:
  // coeffs_ptr points to the x direction coefficient for the left face
  // coeffs_ptr + kstride = y direction coefficient
  // coeffs_ptr + 2 * kstride = z direction coefficient (lower face)
  // coeffs_ptr + 3 * kstride = sum of coefficients
  // coeffs_ptr + 6 * kstride = z direction coefficient (upper face)
  const int ylow_coeff_offset = kstride;
  const int yup_coeff_offset = kstride + jstride;
  const int zlow_coeff_offset = 2 * kstride;
  const int zup_coeff_offset = 6 * kstride;
  const int sum_coeff_offset = 3 * kstride;
  const int zlow_tab_offset = -kstride;
  const int zup_tab_offset = kstride;
  const int ylow_tab_offset = -jstride;
  const int yup_tab_offset = jstride;

  const Simd_template<_TYPE_> relax = relax_coefficient;
  const Simd_template<_TYPE_> one_minus_relax = 1.f - relax_coefficient;

  for (int i = 0; i < nvectors; i++)
    {
      Simd_template<_TYPE_> coeff, x1, x2;
      coeff = SimdGet(coeffs_ptr + zlow_coeff_offset);
      x1 = SimdGet(tab + zlow_tab_offset) * coeff;
      coeff = SimdGet(coeffs_ptr + zup_coeff_offset);
      x2 = SimdGet(tab + zup_tab_offset) * coeff;
      coeff = SimdGet(coeffs_ptr + ylow_coeff_offset);
      x1 += SimdGet(tab + ylow_tab_offset) * coeff;
      coeff = SimdGet(coeffs_ptr + yup_coeff_offset);
      x2 += SimdGet(tab + yup_tab_offset) * coeff;
      Simd_template<_TYPE_> c_left, c_right, tab_left, tab_center, tab_right;
      SimdGetCenterRight(coeffs_ptr, c_left, c_right);
      SimdGetLeftCenterRight(tab, tab_left, tab_center, tab_right);
      x1 += tab_left * c_left;
      x2 += tab_right * c_right;

      Simd_template<_TYPE_> coeff_sum = SimdGet(coeffs_ptr + sum_coeff_offset);
      Simd_template<_TYPE_> secmem = SimdGet(secmem_ptr);

      Simd_template<_TYPE_> tmp;
      if(is_jacobi)
        tmp = tab_center * one_minus_relax + SimdDivideMed( (x1 + x2 + secmem) * relax, coeff_sum);
      else
        tmp = tab_center * coeff_sum - x1 - x2 - secmem;
      SimdPut(result_ptr, tmp);

      coeffs_ptr += vsize;
      secmem_ptr += vsize;
      tab += vsize;
      result_ptr += vsize;
    }
}

template <typename _TYPE_, int _KSTRIDE_, int _JSTRIDE_>
void Jacobi_Residue_template_proto(const _TYPE_ *tab, const _TYPE_ *coeffs_ptr, const _TYPE_ *secmem_ptr,
                                   _TYPE_ *result_ptr,
                                   const int kstride_input, const int jstride_input, const int nvalues,
                                   const _TYPE_ relax_coefficient,
                                   bool is_jacobi
                                  )
{
  const int vsize = Simd_template<_TYPE_>::size();
  const int kstride = _KSTRIDE_ != SSE_Kernels::GENERIC_STRIDE? _KSTRIDE_ : kstride_input;
  const int jstride = _JSTRIDE_ != SSE_Kernels::GENERIC_STRIDE? _JSTRIDE_ : jstride_input;
  assert(kstride == kstride_input);
  assert(jstride == jstride_input);
  const int nvectors = (nvalues + vsize-1) / vsize;
  // Code version 1: perform all operations at once:
  // coeffs_ptr points to the x direction coefficient for the left face
  // coeffs_ptr + kstride = y direction coefficient
  // coeffs_ptr + 2 * kstride = z direction coefficient (lower face)
  // coeffs_ptr + 3 * kstride = sum of coefficients
  // coeffs_ptr + 6 * kstride = z direction coefficient (upper face)
  const int ylow_coeff_offset = kstride;
  const int yup_coeff_offset = kstride + jstride;
  const int zlow_coeff_offset = 2 * kstride;
  const int zup_coeff_offset = 6 * kstride;
  const int sum_coeff_offset = 3 * kstride;
  const int zlow_tab_offset = -kstride;
  const int zup_tab_offset = kstride;
  const int ylow_tab_offset = -jstride;
  const int yup_tab_offset = jstride;
  const Simd_template<_TYPE_> relax = relax_coefficient;
  const Simd_template<_TYPE_> one_minus_relax = 1.f - relax_coefficient;

  int i = 0;
  const int chunksize = 1024/vsize;
  while (i < nvectors)
    {
      int jmax = nvectors - i;
      if (jmax > chunksize)
        jmax = chunksize;

      // Part 1
      int j;
      for (j = 0; j < jmax; j++)
        {
          Simd_template<_TYPE_> coeff, x;
          coeff = SimdGet(coeffs_ptr + zlow_coeff_offset);
          x = SimdGet(tab + zlow_tab_offset) * coeff;
          coeff = SimdGet(coeffs_ptr + zup_coeff_offset);
          x += SimdGet(tab + zup_tab_offset) * coeff;
          coeff = SimdGet(coeffs_ptr + ylow_coeff_offset);
          x += SimdGet(tab + ylow_tab_offset) * coeff;
          SimdPut(result_ptr, x);
          coeffs_ptr += vsize;
          tab += vsize;
          result_ptr += vsize;
        }
      coeffs_ptr -= vsize * jmax;
      tab -= vsize * jmax;
      result_ptr -= vsize * jmax;
      // Part 2
      for (j = 0; j < jmax; j++)
        {
          Simd_template<_TYPE_> coeff, x;
          x = SimdGet(result_ptr);
          coeff = SimdGet(coeffs_ptr + yup_coeff_offset);
          x += SimdGet(tab + yup_tab_offset) * coeff;
          Simd_template<_TYPE_> c_left, c_right, tab_left, tab_center, tab_right;
          SimdGetCenterRight(coeffs_ptr, c_left, c_right);
          SimdGetLeftCenterRight(tab, tab_left, tab_center, tab_right);
          x += tab_left * c_left;
          x += tab_right * c_right;
          Simd_template<_TYPE_> coeff_sum = SimdGet(coeffs_ptr + sum_coeff_offset);
          Simd_template<_TYPE_> secmem = SimdGet(secmem_ptr);

          Simd_template<_TYPE_> tmp;
          if(is_jacobi)
            tmp = tab_center * one_minus_relax + SimdDivideMed((x + secmem) * relax, coeff_sum);
          else
            tmp = tab_center * coeff_sum - x - secmem;
          SimdPut(result_ptr, tmp);
          coeffs_ptr += vsize;
          secmem_ptr += vsize;
          tab += vsize;
          result_ptr += vsize;
        }
      i += jmax;
    }
  //flop_count += nmax * 14;
}

// Realize npass simultaneous passes of jacobi smoother on the x field,
// if last_pass_is_residue, compute npass-1 iterations of jacobi smoothers and compute
// the residue into "residue".
// The algorithm relies on the fact that all data necessary to compute
// the npass passes on npass layers can reside in cache memory, otherwise
// will have same performance than a trivial algorithm that performs independant
// passes.
// x, coeffs and secmem must have npass uptodate layers of ghost cells.
template <typename _TYPE_, typename _TYPE_ARRAY_, int _KSTRIDE_, int _JSTRIDE_>
void Multipass_Jacobi_template(IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& x,
                               IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& residue,
                               const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& coeffs,
                               const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& secmem,
                               const int npass,
                               const bool last_pass_is_residue,
                               const _TYPE_ relax_coefficient)
{
  // The result of jacobi iterations will be stored here:
  int final_k_shift;
  if (!last_pass_is_residue)
    final_k_shift = npass;
  else
    final_k_shift = npass - 1;
  assert(x.k_shift() >= final_k_shift);
  const int sweep_k_begin = - npass + 1;
  const int sweep_k_end   = x.nk() + npass - 1;
  const int jstart = - npass + 1;
  int istart = - npass + 1;
  // Align istart on SIMD vector size (might compute a few useless values at beginning,
  // must also take care that padding is ok)
  istart = istart & (~(Simd_template<_TYPE_>::size()-1));

  const int kstride = x.k_stride();
  const int jstride = x.j_stride();

  const int residue_pass = (last_pass_is_residue ? npass - 1 : npass);

  const int MAXPASS = 8; // 8 simultaneous passes is a reasonable max...
  if (npass > MAXPASS)
    {
      Cerr << "Error: MAXPASS too low, increase value and recompile..." << finl;
      Process::exit();
    }
  int nvalues[MAXPASS];
  for (int i = 0; i < npass; i++)
    {
      const int jstart_this_pass = jstart + i;
      const int iend = x.ni() + npass-i - 2; // i index of the last computed cell
      const int jend = x.nj() + npass-i - 2; // j index of the last computed cell
      const int end_index = jend * jstride + iend;
      const int start_index = jstart_this_pass * jstride + istart;
      nvalues[i] = end_index - start_index + 1;
    }

  // sweep_k_pos is the k layer of the first pass
  for (int sweep_k_pos = sweep_k_begin; sweep_k_pos < sweep_k_end; sweep_k_pos++)
    {
      const int k_layer = sweep_k_pos;

      _TYPE_ *src_ptr = &x.get_in_allocated_area(istart, jstart, k_layer);
      const _TYPE_ *coeffs_ptr = &coeffs.get_in_allocated_area(istart, jstart, k_layer,0);
      const _TYPE_ *secmem_ptr = &secmem.get_in_allocated_area(istart, jstart, k_layer);

      for (int current_pass = 0; current_pass < npass; current_pass++)
        {
          // layer number (where to take rhs and coefficients for this pass
          const int k_layer_mpass = sweep_k_pos - current_pass;
          // The data is not ready yet for this pass (happens for the first layers)
          if (current_pass > k_layer_mpass - sweep_k_begin)
            continue;
          assert(src_ptr == &x.get_in_allocated_area(istart, jstart + current_pass, k_layer_mpass - current_pass));
          assert(coeffs_ptr == &coeffs.get_in_allocated_area(istart, jstart + current_pass, k_layer_mpass,0));
          assert(secmem_ptr == &secmem.get_in_allocated_area(istart, jstart + current_pass, k_layer_mpass));

          if (current_pass == residue_pass)
            {
              _TYPE_ *residue_ptr = &residue.get_in_allocated_area(istart, jstart + current_pass, k_layer_mpass);
              Jacobi_Residue_template<_TYPE_,_KSTRIDE_, _JSTRIDE_>(src_ptr, coeffs_ptr, secmem_ptr, residue_ptr /* where to store result */,
                                                                   kstride, jstride, nvalues[current_pass], relax_coefficient, false /* residu*/);
            }
          else
            {
              Jacobi_Residue_template<_TYPE_,_KSTRIDE_, _JSTRIDE_>(src_ptr, coeffs_ptr, secmem_ptr, src_ptr - kstride /* where to store result */,
                                                                   kstride, jstride, nvalues[current_pass], relax_coefficient, true /*jacobi*/);
            }


          // Shift source, result and coefficient data to layer for next pass.
          // Next pass requires one less row so add also "jstride"
          src_ptr = src_ptr - 2 * kstride + jstride;
          assert(coeffs.nb_compo() == 4);
          coeffs_ptr = coeffs_ptr - kstride * 4 + jstride;
          secmem_ptr = secmem_ptr - kstride + jstride;
        }

    }

  // The result is stored with shift in k direction:
  x.shift_k_origin(-final_k_shift);
}

// Tests the kernel (compares with reference implementation) and gives performance result.
// Can be run in parallel to test concurrency efficiency (memory and cache bandwidth).
// if npass==0, runs only single plane algorithm, otherwise runs multipass algorithm
template <typename _TYPE_, typename _TYPE_ARRAY_, int _KSTRIDE_, int _JSTRIDE_>
void test_template(const int kstride_input, const int jstride_input, int nk, int npass, bool residue)
{
  IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_> coeffs, tab, secmem, resu, resu_reference;
  const int ghost = (npass == 0) ? 1 : npass;
  const int additional_layers = npass;
  const int ni = jstride_input - 2 * ghost;
  const int nj = kstride_input / jstride_input - 2 * ghost;
  if (npass == 0)
    {
      nk = 1;
    }
  else
    {
      if (npass < 0 || nk < 1)
        {
          Cerr << "Error ! in test_template" << finl;
          Process::exit();
        }
    }
  coeffs.allocate(ni, nj, nk, ghost, additional_layers, 4 /*nb compo*/);
  tab.allocate(ni, nj, nk, ghost, additional_layers);
  secmem.allocate(ni, nj, nk, ghost);
  resu.allocate(ni, nj, nk, ghost);
  resu_reference.allocate(ni, nj, nk, ghost);

  tab.shift_k_origin(additional_layers);
  // Fill with dummy data:
  fill_dummy(tab, 0);
  fill_dummy(coeffs, 1);
  fill_dummy(secmem, 2);
  fill_dummy(resu, 3);

  for (int i = -ghost + 1; i < ni + ghost - 1; i++)
    {
      for (int j = -ghost + 1; j < nj + ghost - 1; j++)
        {
          for (int k = -ghost + 1; k < nk + ghost - 1; k++)
            {
              coeffs(i,j,k,3) = coeffs(i,j,k,0) + coeffs(i,j,k,1) + coeffs(i,j,k,2)
                                + coeffs(i+1,j,k,0) + coeffs(i,j+1,k,1) + coeffs(i,j,k+1,2);
            }
        }
    }

  const _TYPE_ relax_coefficient = (_TYPE_)0.65;
  const int nvalues = tab.j_stride() * (nj-1) + ni;

  Nom kernname;
  double diff;
  if (npass == 0)
    {
      // Unit test of the kernel for one plane:
      // Compute result:
      if (residue)
        {
          kernname = "residue";
        }
      else
        {
          kernname = "jacobi";
        }
      Jacobi_Residue_template<_TYPE_,_KSTRIDE_, _JSTRIDE_>(&tab(0,0,0), &coeffs(0,0,0,0), &secmem(0,0,0), &resu(0,0,0),
                                                           kstride_input, jstride_input, nvalues,
                                                           relax_coefficient, !residue);
      reference_kernel_template<_TYPE_>(tab, coeffs, secmem, resu_reference, relax_coefficient, residue);
      diff = compute_difference(resu_reference, resu);
    }
  else
    {
      kernname = "multipass_jacobi";
      IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_> copy_tab(tab);
      Multipass_Jacobi_template<_TYPE_, _TYPE_ARRAY_, _KSTRIDE_, _JSTRIDE_>(tab, resu, coeffs, secmem, npass, residue, relax_coefficient);

      for (int i = 0; i < npass - 1; i++)
        {
          reference_kernel_template<_TYPE_>(copy_tab, coeffs, secmem, resu_reference, relax_coefficient, false);
          copy_tab = resu_reference;
        }
      reference_kernel_template<_TYPE_>(copy_tab, coeffs, secmem, resu_reference, relax_coefficient, residue);
      if (residue)
        diff = compute_difference(resu_reference, resu);
      else
        diff = compute_difference(resu_reference, tab);
    }

  if (Process::je_suis_maitre())
    Cout << "Checking " << kernname << " : " << kstride_input << "," << jstride_input
         << ") Maximum difference= " << diff << finl;
  if (diff > 1e-5)
    {
      Cerr << "Error: difference found in testing kernel." << finl;
      Process::exit();
    }
  if (check_performance)
    {
      int niter;
      double nflop = 0.;
      const double flops_per_cell_residue = 14.;
      const double flops_per_cell_jacobi = 17.;
      double usefull_flops = 0.;
      double dt = 0.;
      if (npass == 0)
        {
          niter = (int)floor(check_performance * 1e9/nvalues);
          Process::barrier();
          double t1 = get_clock();
          for (int i = 0; i < niter; i++)
            {
              Jacobi_Residue_template<_TYPE_,_KSTRIDE_, _JSTRIDE_>(&tab(0,0,0), &coeffs(0,0,0,0), &secmem(0,0,0), &resu(0,0,0),
                                                                   kstride_input, jstride_input, nvalues,
                                                                   relax_coefficient, !residue);
            }
          Process::barrier();
          dt = get_clock()-t1;
          if (residue)
            nflop = flops_per_cell_residue * nvalues;
          else
            nflop = flops_per_cell_jacobi * nvalues;
        }
      else
        {
          for (int i = 0; i < npass; i++)
            {
              int ncells = (nk + 2 * i) * (nj + 2 * i) * jstride_input;
              if (i == 0 && residue)
                {
                  nflop += flops_per_cell_residue * ncells;
                  usefull_flops += flops_per_cell_residue * ni * nj * nk;
                }
              else
                {
                  nflop += flops_per_cell_jacobi * ncells;
                  usefull_flops += flops_per_cell_jacobi * ni * nj * nk;
                }
            }
          niter = (int) floor(check_performance * 1e10 / nflop) + 1;
          Process::barrier();
          double t1 = get_clock();
          for (int i = 0; i < niter; i++)
            {
              // GF je remets a zero sinon cela diverge ???
              fill_dummy(tab, 0);

              tab.shift_k_origin(residue ? npass-1 : npass);

              Multipass_Jacobi_template<_TYPE_, _TYPE_ARRAY_, _KSTRIDE_, _JSTRIDE_>(tab, resu, coeffs, secmem, npass, residue, relax_coefficient);
            }
          Process::barrier();
          dt = get_clock()-t1;
        }
      double gflops = nflop * niter / dt * 1e-9;
      if (Process::je_suis_maitre())
        {
          Cout << "Performance " << kernname << "(kstride=" << kstride_input
               << ",jstride=" << jstride_input << ",nk=" << nk << ",npass=" << npass << ",residue=" << (int)(residue?1:0);
          Cout   << ") niter=" << niter << " time=" << dt << " flops per iteration=" << nflop
                 << " gflops=" << gflops << finl;
          if (npass > 0)
            {
              double data_factor = 2 /* read/write tab */ + 1 /* secmem */ + 2 /* read/write resu */ + 4 /* coeffs */;
              double data_size = kstride_input * (nk + 2 * npass);
              double bandwidth = (double) data_size * data_factor * sizeof(_TYPE_) * niter / dt;

              Cout << " Amount of data read/write to RAM per iteration= " << data_size << " * " << data_factor << " * " << (int)sizeof(_TYPE_)
                   << " = " << data_size * data_factor * sizeof(_TYPE_) << " MB" << finl;
              Cout    << " Bandwidth per process if optimal caching(GB/s)=" << bandwidth << finl
                      << " Usefull GFlops=" << usefull_flops * niter / dt * 1e-9 << finl;
            }
        }
    }
}

#endif
