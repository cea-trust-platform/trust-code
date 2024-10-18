
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

#ifndef Domaine_DG_included
#define Domaine_DG_included

#include <Domaine_Poly_base.h>
#include <Option_DG.h>

class Quadrature_base;

class Domaine_DG : public Domaine_Poly_base
{
  Declare_instanciable(Domaine_DG);
public :
  void discretiser() override;

  inline const DoubleTab& get_DiaTri() const { return DiaTri; }
  inline const DoubleTab& get_invDiaTri() const { return invDiaTri; }
  inline const DoubleTab& get_PerTri() const { return PerTri; }
  inline const DoubleTab& get_rhoTri() const { return rhoTri; }
  inline const DoubleTab& get_sigTri() const { return sigTri; }


  inline void set_quadrature(int order, const Quadrature_base* quad);
  inline const Quadrature_base& get_quadrature(int order) const;
  DoubleTab DiaTri;
  DoubleTab invDiaTri;
  DoubleTab PerTri;
  DoubleTab rhoTri;
  DoubleTab sigTri;

//  void calculer_h_carre() override;


private:

  std::map<int, const Quadrature_base*> quad_map_;   // Key: quadrature order, value: DoubleTab representing the quadrature barycenters for that order
// DoubleVect h_, sigma;

  void compute_mesh_param(); // Compute the stabilization parameters
};

void Domaine_DG::set_quadrature(int order, const Quadrature_base* quad)
{
  assert(quad_map_.count(order) == 0); // fail if a quadrature is already registered for this order
  quad_map_[order] = quad;
}

const Quadrature_base& Domaine_DG::get_quadrature(int order) const
{
  return *quad_map_.at(order);
}



#endif /* Domaine_DG_included */
