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

#ifndef Parallel_io_parameters_included
#define Parallel_io_parameters_included
#include <Interprete.h>

class Parallel_io_parameters : public Interprete
{
  Declare_instanciable(Parallel_io_parameters);
public:
  Entree& interpreter(Entree&) override;
  static long long get_max_block_size();
  static int       get_nb_writing_processes();
protected:
  static void run_bench_read(const Nom& ijk_splitting);
  static void run_bench_write(const Nom& ijk_splitting);

  // File writes will be performed by chunks of this size (in bytes)
  // The size should be a multiple of the GPFS block size or lustre stripping size
  // (typically several megabytes).
  static long long max_block_size_;
  // This is the number of processes that will write concurrently to the file system
  // (this must be set according to the capacity of the filesystem, set to 1 on
  //  small computers, can be up to 64 or 128 on very large systems).
  static int       nb_writing_processes_;
};
#endif
