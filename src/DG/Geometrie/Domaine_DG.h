
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

//class Quadrature;

class Domaine_DG : public Domaine_Poly_base
{
  Declare_instanciable(Domaine_DG);
public :
  void discretiser() override;
  void modifier_pour_Cl(const Conds_lim& ) override;

  inline const std::map<Nom,IntTab>& dof_elem() const;
  inline const IntTab& dof_elem(const Nom& variable) const;

  inline const IntTab& elem_voisins() const;
  inline const IntTab& indices_glob_elem() const;

  void initialize_dof(const Nom& nom);

//  void set_quadrature(int order, const Quadrature* quad);
//  const Quadrature& get_quadrature(int order) const;

private:

  IntTab elem_voisins_;
  std::map<Nom, IntTab> dof_elem_; // table of the degree of freedom for each unknown (velocity, pressure, temperature) within each cell
  IntTab indices_glob_elem_; //indirection pour obtenir l'indice de l'element dans l'inconnu

  // A voir si on rajoute un tableau elem_voisins (mix de elem_faces+face_voisins)

//  std::map<int, const Quadrature*> quad_map_;   // Key: quadrature order, value: DoubleTab representing the quadrature barycenters for that order
  void remplir_elem_voisins();
};

inline const std::map<Nom,IntTab>& Domaine_DG::dof_elem() const
{
  return dof_elem_;
}

inline const IntTab& Domaine_DG::dof_elem(const Nom& variable) const
{
  return dof_elem_.at(variable);
}

inline const IntTab& Domaine_DG::elem_voisins() const
{
  return elem_voisins_;
}

inline const IntTab& Domaine_DG::indices_glob_elem() const
{
  return indices_glob_elem_;
}




#endif /* Domaine_DG_included */
