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

#include <Stat_per_proc_perf_log.h>

Implemente_instanciable(Stat_per_proc_perf_log,"Stat_per_proc_perf_log",Interprete);
// XD stat_per_proc_perf_log interprete stat_per_proc_perf_log -1 Keyword allowing to activate the detailed statistics per processor (by default this is false, and only the master proc will produce stats).
// XD attr flg int flg 0 A flag that can be either 0 or 1 to turn off (default) or on the detailed stats.

/*! @brief Simple appel a: Interprete::printOn(Sortie&)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Stat_per_proc_perf_log::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


/*! @brief Simple appel a: Interprete::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Stat_per_proc_perf_log::readOn(Entree& is)
{
  return Interprete::readOn(is);
}


/*! @brief Fonction principale de l'interprete Dimension Lit la dimension d'espace du probleme.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Stat_per_proc_perf_log::interpreter(Entree& is)
{
  int tmp;
  is >> tmp; // If is ==0 no statistics detailed per processor in the _csv.TU file, if is == 1 then the detailed statistics are printed
  stat_per_proc_perf_log = (tmp != 0);
  Cerr << "Stat_per_proc_perf_log::interpreter : stat_per_proc_perf_log = " << (int)stat_per_proc_perf_log << finl;
  return is;
}
