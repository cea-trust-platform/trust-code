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
    if (!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override { return os; }

  Entree& readOn(Entree& is) override { return is; }

public:
  IJK_Field_template() :
    IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>(),
    localisation_(IJK_Splitting::Localisation::ELEM)
  { }
  void allocate(const IJK_Splitting&, IJK_Splitting::Localisation, int ghost_size, int additional_k_layers = 0, int nb_compo = 1, bool external_storage = false, int monofluide=0, double rov=0., double rol=0., int use_inv_rho_in_pressure_solver=0);

  const IJK_Splitting& get_splitting() const { return splitting_ref_.valeur(); }

  IJK_Splitting::Localisation get_localisation() const { return localisation_; }
  void echange_espace_virtuel(int ghost);
  void prepare_interpolation_for_shear_periodicity(const int send_i, const double istmp, const int real_size_i);
  //_TYPE_ interpolation_for_shear_periodicity(const int phase, const int send_j, const int send_k);
  _TYPE_ interpolation_for_shear_periodicity_IJK_Field(const int send_j, const int send_k);
  void interpolation_for_shear_periodicity_I_sig_kappa(const int send_j, const int send_k_zmin, const int send_k_zmax, _TYPE_ Isigkappazmin, _TYPE_ Isigkappazmax);
  void redistribute_with_shear_domain_ft(const IJK_Field_double& input, double DU_perio, const int ft_extension);
  void ajouter_second_membre_shear_perio(IJK_Field_double& resu);


  int monofluide_variable_ = -123, order_interpolation_ = -123;
  int compo_ ;
  int real_size_i_ ;
  ArrOfInt x_;
  ArrOfInt send_i_real_;
  ArrOfDouble denum_;
  ArrOfDouble res_;
  int use_inv_rho_in_pressure_solver_ ;

  double rho_v_ = -123., rho_l_ = -123.;
  IJK_Field_local_template<double,ArrOfDouble> indicatrice_ghost_zmin_ ;
  IJK_Field_local_template<double,ArrOfDouble> indicatrice_ghost_zmax_ ;
  IJK_Field_local_template<double,ArrOfDouble> I_sig_kappa_zmin_;
  IJK_Field_local_template<double,ArrOfDouble> I_sig_kappa_zmax_;
  //IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_> I_sigma_kappa_ghost_zmin_ ;
  //IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_> I_sigma_kappa_ghost_zmax_ ;
  double DU_ ;
  const IJK_Field_local_template<double,ArrOfDouble>& get_indicatrice_ghost_zmin_() const
  {
    return indicatrice_ghost_zmin_;
  }
  const IJK_Field_local_template<double,ArrOfDouble>& get_indicatrice_ghost_zmax_() const
  {
    return indicatrice_ghost_zmax_;
  }
  const IJK_Field_local_template<double,ArrOfDouble>& get_I_kappa_ghost_zmin_() const
  {
    return I_sig_kappa_zmin_;
  }
  const IJK_Field_local_template<double,ArrOfDouble>& get_I_kappa_ghost_zmax_() const
  {
    return I_sig_kappa_zmax_;
  }

  void show_indicatrice_ghost_() const
  {
    std::cout << "get   indicatrice_ghost_zmin_" <<  std::endl ;
    for (int i = 0; i < indicatrice_ghost_zmin_.ni(); i++)
      {
        std::cout << std::endl ;
        for (int k = 0; k < indicatrice_ghost_zmin_.nk() ; k++)
          std::cout << indicatrice_ghost_zmin_(i, 5, k) << " " ;
      }

    std::cout << "get   indicatrice_ghost_zmax_" <<  std::endl ;
    for (int i = 0; i < indicatrice_ghost_zmin_.ni(); i++)
      {
        std::cout << std::endl ;
        for (int k = 0; k < indicatrice_ghost_zmin_.nk() ; k++)
          std::cout << indicatrice_ghost_zmin_(i, 5, k) << " " ;
      }

    return;
  }

  void set_indicatrice_ghost_zmin_(const IJK_Field_local_template<double,TRUSTArray<double>>& indic_z_min, int decallage = 0)
  {
    for (int k = 0; k < indicatrice_ghost_zmin_.nk() ; k++)
      for (int j = 0; j < indicatrice_ghost_zmin_.nj() ; j++)
        for (int i = 0; i < indicatrice_ghost_zmin_.ni(); i++)
          indicatrice_ghost_zmin_(i,j,k)=indic_z_min(i,j,k+decallage);
  }
  void set_indicatrice_ghost_zmax_(const IJK_Field_local_template<double,TRUSTArray<double>>& indic_z_max, int decallage = 0)
  {
    for (int k = 0; k < indicatrice_ghost_zmax_.nk() ; k++)
      for (int j = 0; j < indicatrice_ghost_zmax_.nj() ; j++)
        for (int i = 0; i < indicatrice_ghost_zmax_.ni(); i++)
          indicatrice_ghost_zmax_(i,j,k)=indic_z_max(i,j,k+decallage);

  }

  void set_I_sig_kappa_zmin_(const IJK_Field_local_template<double,TRUSTArray<double>>& indic_z_min, const IJK_Field_local_template<double,TRUSTArray<double>>& kappa_z_min, const double sigma, const int decallage = 0)
  {
    if (monofluide_variable_==1)
      {
        for (int k = 0; k < I_sig_kappa_zmin_.nk() ; k++)
          for (int j = 0; j < I_sig_kappa_zmin_.nj() ; j++)
            for (int i = 0; i < I_sig_kappa_zmin_.ni(); i++)
              I_sig_kappa_zmin_(i,j,k)=indic_z_min(i,j,k+decallage)*kappa_z_min(i,j,k+decallage)*sigma;
      }
  }
  void set_I_sig_kappa_zmax_(const IJK_Field_local_template<double,TRUSTArray<double>>& indic_z_max, const IJK_Field_local_template<double,TRUSTArray<double>>& kappa_z_max, const double sigma, const int decallage = 0)
  {
    if (monofluide_variable_==1)
      {
        for (int k = 0; k < I_sig_kappa_zmax_.nk() ; k++)
          for (int j = 0; j < I_sig_kappa_zmax_.nj() ; j++)
            for (int i = 0; i < I_sig_kappa_zmax_.ni(); i++)
              I_sig_kappa_zmax_(i,j,k)=indic_z_max(i,j,k+decallage)*kappa_z_max(i,j,k+decallage)*sigma;
      }

  }
  void set_dU_(double DU)
  {
    DU_ = DU;
  }

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
