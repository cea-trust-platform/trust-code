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

#ifndef IJK_Field_template_included
#define IJK_Field_template_included


#include <IJK_Field_local_template.h>
#include <communications.h>
#include <IJK_Splitting.h>
#include <stat_counters.h>
#include <Statistiques.h>
#include <simd_tools.h>
#include <TRUST_Ref.h>
#include <TRUSTVect.h>


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
    if (!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override { return os; }

  Entree& readOn(Entree& is) override { return is; }

public:
  IJK_Field_template() :
    IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>(),
    localisation_(IJK_Splitting::Localisation::ELEM)
  { }
  void allocate(const IJK_Splitting&, IJK_Splitting::Localisation, int ghost_size, int additional_k_layers = 0, int nb_compo = 1, bool external_storage = false, bool monofluide=false);

  const IJK_Splitting& get_splitting() const { return splitting_ref_.valeur(); }

  const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& get_projection_liquide() const {return projection_liquide_ ;}
  const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& get_projection_vapeur() const {return projection_vapeur_ ;}
  IJK_Splitting::Localisation get_localisation() const { return localisation_; }
  void echange_espace_virtuel(int ghost, double Shear_DU=0.);
  void change_to_sheared_reference_frame(double sens, int loc, int time=1);
  void change_to_u_prime_to_u(double sens, int loc, double jump_i);
  void redistribute_with_shear_domain_ft(const IJK_Field_double& input_field, double DU_perio, int dir=-1);
  void update_monofluide_to_phase_value();
  void update_indicatrice(const IJK_Field_double & indic, const int ft_extension);
  double calculer_distance(Vecteur3 & pos_ijk, const Vecteur3 xyz_cible);
  _TYPE_ fourth_order_interpolation_for_shear_periodicity(const int phase, const int send_i, const int send_j, const int send_k, const _TYPE_ istmp, const int real_size_i);
  _TYPE_ second_order_interpolation_for_shear_periodicity(const int phase, const int send_i, const int send_j, const int send_k, const _TYPE_ istmp, const int real_size_i);


  bool monofluide_variable_ ;
  int order_interpolation_ ;
  IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_> projection_liquide_ ;
  IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_> projection_vapeur_ ;
  IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_> indicatrice_ ;

protected:
  REF(IJK_Splitting) splitting_ref_;
  IJK_Splitting::Localisation localisation_;

  void exchange_data(int pe_imin_, /* processor to send to */
                     int is, int js, int ks, /* ijk coordinates of first data to send */
                     int pe_imax_, /* processor to recv from */
                     int ir, int jr, int kr, /* ijk coordinates of first data to recv */
                     int isz, int jsz, int ksz, /* size of block data to send/recv */
                     double offset_i = 0., double jump_i=0., int nb_ghost=0);
};

#include <IJK_Field_template.tpp>

#endif /* IJK_Field_template_included */
