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

#include <Coarsen_Operator_K.h>
#include <IJK_Grid_Geometry.h>
#include <EFichier.h>
#include <communications.h>
#include <TRUSTTab.h>
#include <stat_counters.h>

Implemente_instanciable_sans_constructeur(Coarsen_Operator_K, "Coarsen_Operator_K", Coarsen_Operator_base);

Coarsen_Operator_K::Coarsen_Operator_K()
{
}

// Reads coarsening parameters in the .data file. See ajouter_param()
Entree& Coarsen_Operator_K::readOn(Entree& is)
{
  Coarsen_Operator_base::readOn(is);
  // Read the coordinates from file:
  if (Process::je_suis_maitre())
    {
      EFichier fic_coor(file_z_coord_);

      while(1)
        {
          double x;
          fic_coor >> x;
          if (fic_coor.eof()) break;
          z_coord_all_.append_array(x);
        }
    }
  envoyer_broadcast(z_coord_all_, 0);

  return is;
}

Sortie& Coarsen_Operator_K::printOn(Sortie& os) const
{
  return Coarsen_Operator_base::printOn(os);
}

void Coarsen_Operator_K::ajouter_param(Param& param)
{
  param.ajouter("file_z_coord", &file_z_coord_, Param::REQUIRED);
  Coarsen_Operator_base::ajouter_param(param);
}
