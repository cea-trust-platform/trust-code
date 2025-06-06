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

#ifndef IJK_Navier_Stokes_tools_included
#define IJK_Navier_Stokes_tools_included

#include <IJK_tools.h>

class Multigrille_Adrien;
class Boundary_Conditions_Thermique;

double compute_fractionnal_timestep_rk3(const double dt_tot, int step);

void compute_divergence_times_constant(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                                       const double constant, IJK_Field_double& resu);

void compute_divergence(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                        IJK_Field_double& resu);

void add_gradient_times_constant(const IJK_Field_double& pressure, const double constant,
                                 IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz);

void add_gradient_times_constant_over_rho(const IJK_Field_double& pressure, const IJK_Field_double& rho, const double constant,
                                          IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz);
void add_gradient_times_constant_times_inv_rho(const IJK_Field_double& pressure, const IJK_Field_double& inv_rho, const double constant,
                                               IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz);

void pressure_projection(IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz,
                         IJK_Field_double& pressure, double dt,
                         IJK_Field_double& pressure_rhs,
                         Multigrille_Adrien& poisson_solver);

void pressure_projection_with_rho(const IJK_Field_double& rho,
                                  IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz,
                                  IJK_Field_double& pressure, double dt,
                                  IJK_Field_double& pressure_rhs,
                                  Multigrille_Adrien& poisson_solver);

void pressure_projection_with_inv_rho(const IJK_Field_double& rho,
                                      IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz,
                                      IJK_Field_double& pressure, double dt,
                                      IJK_Field_double& pressure_rhs,
                                      Multigrille_Adrien& poisson_solver);

void forward_euler_update(const IJK_Field_double& dv, IJK_Field_double& v,
                          const int k_layer, double dt_tot);

void runge_kutta3_update(const IJK_Field_double& dv, IJK_Field_double& F, IJK_Field_double& v,
                         const int step, const int k_layer, double dt_tot);

void runge_kutta3_update_surfacic_fluxes(IJK_Field_double& dv, IJK_Field_double& F,
                                         const int step, const int k_layer, double dt_tot);

void force_zero_on_walls(IJK_Field_double& vz);

template<class T, int N>
void allocate_velocity(IJK_Field_vector<T, N>& v, const Domaine_IJK& s, int ghost, const Nom& nam=Nom())
{
  assert(static_cast<int>(N) == 3);

  v.get_ptr(0) = std::make_shared<IJK_Field_template<T,TRUSTArray<T>>>();
  v.get_ptr(1) = std::make_shared<IJK_Field_template<T,TRUSTArray<T>>>();
  v.get_ptr(2) = std::make_shared<IJK_Field_template<T,TRUSTArray<T>>>();

  v[0].allocate(s, Domaine_IJK::FACES_I, ghost);
  v[1].allocate(s, Domaine_IJK::FACES_J, ghost);
  v[2].allocate(s, Domaine_IJK::FACES_K, ghost);

  v.nommer(nam);
  v.localisation() = Entity::FACE;
}

template<class T, int N>
void allocate_cell_vector(IJK_Field_vector<T, N>& v, const Domaine_IJK& s, int ghost, const Nom& nam=Nom())
{
  for (int i=0; i<N ; i++)
    {
      v.get_ptr(i) = std::make_shared<IJK_Field_template<T,TRUSTArray<T>>>();
      v[i].allocate(s, Domaine_IJK::ELEM, ghost);
      IJK_Field_template<T,TRUSTArray<T>>::increase_alloc_counter();
    }
  v.nommer(nam);
  v.localisation() = Entity::ELEMENT;
}

void calculer_rho_v(const IJK_Field_double& rho,
                    const IJK_Field_vector3_double& v,
                    IJK_Field_vector3_double& rho_v);

void calculer_rho_harmonic_v(const IJK_Field_double& rho,
                             const IJK_Field_vector3_double& v,
                             IJK_Field_vector3_double& rho_v);

double get_channel_control_volume(IJK_Field_double& field, int local_k_layer, const ArrOfDouble_with_ghost& delta_z_local);

void mass_solver_with_rho(IJK_Field_double& velocity, const IJK_Field_double& rho, const ArrOfDouble_with_ghost& delta_z_local, const int k);
void mass_solver_with_inv_rho(IJK_Field_double& velocity, const IJK_Field_double& inv_rho, const ArrOfDouble_with_ghost& delta_z_local, const int k);

void mass_solver_scalar(IJK_Field_double& dv, const ArrOfDouble_with_ghost& delta_z_local, int k_index);

void density_solver_with_rho(IJK_Field_double& velocity, const IJK_Field_double& rho, const ArrOfDouble_with_ghost& delta_z_local, const int k);

// fonction moyenne en temps du champs de vitesse utilise dans le cas de bulles fixes
void update_integral_velocity(const IJK_Field_vector3_double& v_instant,  IJK_Field_vector3_double& v_tmp,
                              const IJK_Field_double& indic, const IJK_Field_double& indic_tmp);
void compute_and_store_gradU_cell(const IJK_Field_double& vitesse_i,
                                  const IJK_Field_double& vitesse_j,
                                  const IJK_Field_double& vitesse_k,
                                  /* Et les champs en sortie */
                                  IJK_Field_double& dudx, IJK_Field_double& dvdy, IJK_Field_double& dwdx,
                                  IJK_Field_double& dudz, IJK_Field_double& dvdz, IJK_Field_double& dwdz,
                                  const int compute_all,
                                  /* Following will be untouched if compute_all is 0 */
                                  IJK_Field_double& dudy, IJK_Field_double& dvdx, IJK_Field_double& dwdy,
                                  IJK_Field_double& lambda2);

void supprimer_chevauchement(IJK_Field_double& ind);

void update_integral_pressure(const IJK_Field_double& p_instant,  IJK_Field_double& p_tmp, const IJK_Field_double& indic, const IJK_Field_double& indic_tmp);
void update_integral_indicatrice(const IJK_Field_double& indic, const double deltat, IJK_Field_double& out);

double calculer_v_moyen(const IJK_Field_double& vx);
double calculer_vl_moyen(const IJK_Field_double& vx, const IJK_Field_double& indic);
double calculer_rho_cp_u_moyen(const IJK_Field_double& vx, const IJK_Field_double& cp_rhocp, const IJK_Field_double& rho_field, const double& rho_cp, const int rho_cp_case);



//double compute_spatial_mean(const IJK_Field_double& vx, const IJK_Field_double& variable, const IJK_Field_double& cp, const IJK_Field_double& rho_field, const int kmin, const int nktot, const int k);
double calculer_temperature_adimensionnelle_theta_moy(const IJK_Field_double& vx,
                                                      const IJK_Field_double& temperature_adimensionnelle_theta,
                                                      const IJK_Field_double& cp_rhocp_rhocpinv,
                                                      const IJK_Field_double& rho_field,
                                                      const double& rho_cp,
                                                      const int rho_cp_case);

double calculer_temperature_theta_moy(const IJK_Field_double& vx, const IJK_Field_double& temperature,
                                      const IJK_Field_double& cp, const IJK_Field_double& rho_field);

double calculer_variable_wall(const IJK_Field_double& variable, const IJK_Field_double& cp_rhocp_rhocpinv, const IJK_Field_double& rho_field, const double& rho_cp, const int kmin, const int kmax, const int rho_cp_case);
void calculer_rho_cp_var(const IJK_Field_double& variable, const IJK_Field_double& cp_rhocp_rhocpinv, const IJK_Field_double& rho, const double& rho_cp, double& rho_cp_moy, double& variable_moy, int k, const int rho_cp_case);

double calculer_tauw(const IJK_Field_double& vx, const double mu_liquide);

void calculer_gradient_temperature(const IJK_Field_double& temperature, IJK_Field_vector3_double& grad_T);
void add_gradient_temperature(const IJK_Field_double& temperature, const double constant, IJK_Field_double& vx,
                              IJK_Field_double& vy, IJK_Field_double& vz, const Boundary_Conditions_Thermique& boundary, const IJK_Field_double& lambda);

void force_entry_velocity(IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz, double v_imposed, const int& dir, const int& compo, const int& stencil);

double calculer_v_moyen(const IJK_Field_double& vx);
double calculer_vl_moyen(const IJK_Field_double& vx, const IJK_Field_double& indic);
double calculer_rho_cp_u_moyen(const IJK_Field_double& vx, const IJK_Field_double& cp_rhocp, const IJK_Field_double& rho_field, const double& rho_cp, const int rho_cp_case);

#endif /* IJK_Navier_Stokes_tools_included */
