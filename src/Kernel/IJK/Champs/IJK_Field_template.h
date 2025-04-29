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

#ifndef IJK_Field_template_included
#define IJK_Field_template_included

#include <IJK_Shear_Periodic_helpler.h>
#include <communications.h>
#include <Domaine_IJK.h>
#include <stat_counters.h>
#include <Statistiques.h>
#include <TRUST_Ref.h>
#include <TRUSTVect.h>

// pour IJK_Lata_writer.cpp. TODO : FIXME : to do enum class !!!!!
#define DIRECTION_I 0
#define DIRECTION_J 1
#define DIRECTION_K 2

/*! @brief : This class is an IJK_Field_local with parallel informations.
 *
 * Each processor has a sub_box of the global box, and echange_espace_virtuel(n) exchanges n layers of ghost cells,
 *  echange_espace_virtuel handles periodicity by copying the first layer into the ghost layer on the opposite side.
 *
 */
template<typename _TYPE_, typename _TYPE_ARRAY_>
class IJK_Field_template : public IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>
{
protected:
  unsigned taille_memoire() const override { throw; }

  int duplique() const override
  {
    IJK_Field_template *xxx = new IJK_Field_template(*this);
    if (!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override { return os; }

  Entree& readOn(Entree& is) override { return is; }

public:
  IJK_Field_template() :
    IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>(),
    localisation_(Domaine_IJK::Localisation::ELEM)
  { }
  void allocate(const Domaine_IJK& d, Domaine_IJK::Localisation l, int ghost_size, int additional_k_layers=0, int nb_compo=1, const Nom& name=Nom());
  void allocate(const Domaine_IJK& d, Domaine_IJK::Localisation l, int ghost_size, const Nom& name)
  {
    allocate(d,l,ghost_size,0,1,name);
  }
  void allocate_shear_BC(int monofluide, double rov, double rol, int use_inv_rho_in_pressure_solver=0);

  virtual void dumplata_scalar(const char *filename, int step);

  const Domaine_IJK& get_domaine() const { return domaine_ref_.valeur(); }
  Domaine_IJK::Localisation get_localisation() const { return localisation_; }
  void echange_espace_virtuel(int ghost);
  //_TYPE_ interpolation_for_shear_periodicity(const int phase, const int send_j, const int send_k);
  _TYPE_ interpolation_for_shear_periodicity_IJK_Field(const int send_j, const int send_k);
  void interpolation_for_shear_periodicity_I_sig_kappa(const int send_j, const int send_k_zmin, const int send_k_zmax, _TYPE_ Isigkappazmin, _TYPE_ Isigkappazmax);
  void redistribute_with_shear_domain_ft(const IJK_Field_double& input, double DU_perio, const int ft_extension);
  void ajouter_second_membre_shear_perio(IJK_Field_double& resu);

  inline IJK_Shear_Periodic_helpler& get_shear_BC_helpler() { return shear_BC_helpler_; }
  inline const IJK_Shear_Periodic_helpler& get_shear_BC_helpler() const { return shear_BC_helpler_; }

  // TODO : we should overload '=' operator and copy ctor to increase counter in those cases too...
  static inline void increase_alloc_counter(int i=1) { alloc_counter_ += i; }
  static inline int alloc_counter() { return alloc_counter_; }

protected:
  OBS_PTR(Domaine_IJK) domaine_ref_;
  Domaine_IJK::Localisation localisation_;
  IJK_Shear_Periodic_helpler shear_BC_helpler_;

  static int alloc_counter_;  ///< To monitor how many field allocations we perform.

  void exchange_data(int pe_imin_, /* processor to send to */
                     int is, int js, int ks, /* ijk coordinates of first data to send */
                     int pe_imax_, /* processor to recv from */
                     int ir, int jr, int kr, /* ijk coordinates of first data to recv */
                     int isz, int jsz, int ksz, /* size of block data to send/recv */
                     double offset_i = 0., double jump_i=0., int nb_ghost=0);
};

#include <IJK_Field_template.tpp>

#endif /* IJK_Field_template_included */
