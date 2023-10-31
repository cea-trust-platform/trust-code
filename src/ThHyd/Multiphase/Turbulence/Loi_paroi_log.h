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

#ifndef Loi_paroi_log_included
#define Loi_paroi_log_included
#include <TRUSTTab.h>
#include <TRUSTTab.h>
#include <Correlation_base.h>
#include <Loi_paroi_base.h>
#include <vector>
#include <map>
#include <string>

/*! @brief classe Loi_paroi_adaptative correlation pour une loi de paroi adaptative qui calcule u_tau et du y_plus
 *
 *     Methodes implementees :
 *
 *
 */
class Loi_paroi_log : public Loi_paroi_base
{
  Declare_instanciable(Loi_paroi_log);
public:
  void   calc_y_plus(const DoubleTab& vit, const DoubleTab& nu_visc) override;
  virtual double u_plus_de_y_plus(double y_p) ; // Blended Reichardt model
  virtual double deriv_u_plus_de_y_plus(double y_p) ;

protected:
  double calc_y_plus_loc( double y_p, double nu, double y, double y_p_0) ;

  double von_karman_ = 0.41;
  double limiteur_y_p = 0.01; // To prevent numerical issues ; no consequence on the calculation, as it falls in the region where the blending function is zero

};

#endif
