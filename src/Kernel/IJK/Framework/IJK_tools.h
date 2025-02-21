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

#ifndef IJK_tools_included
#define IJK_tools_included

#include <IJK_Field.h>
#include <IJK_Field_vector.h>
#include <TRUSTTab.h>
#include <Probleme_base.h>

#define select_dir(a,x,y,z) ((a==0)?(x):((a==1)?(y):(z)))

void build_extended_splitting(const Domaine_IJK& split, Domaine_IJK& split_ext, int n_cells);

Probleme_base& creer_domaine_vdf(const Domaine_IJK& splitting, const Nom& nom_domaine);

void ijk_interpolate(const IJK_Field_double& field, const DoubleTab& coordinates, ArrOfDouble& result);
void ijk_interpolate_skip_unknown_points(const IJK_Field_double& field, const DoubleTab& coordinates, ArrOfDouble& result, const double value_for_bad_points);

double ijk_interpolate(const IJK_Field_double& field, const Vecteur3& coordinates);
double ijk_interpolate_skip_unknown_points(const IJK_Field_double& field, const Vecteur3& coordinates, const double value_for_bad_points);



void compose_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z);

void set_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z);
void set_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z_and_t, const double current_time);
void set_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z_and_t,
                    const IJK_Field_double& input_f, const double current_time);
void set_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z_and_t,
                    const IJK_Field_double& input_f1,const IJK_Field_double& input_f2, const double current_time);

void set_field_data(IJK_Field_double& f, double func(double, double, double));
void set_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z_and_t,
                    const IJK_Field_double& input_f1,const IJK_Field_double& input_f2, const double current_time);


void build_local_coords(const IJK_Field_double& f, ArrOfDouble& coord_i, ArrOfDouble& coord_j, ArrOfDouble& coord_k);


void complex_to_trig(const double re, const double im, double& modul, double& arg);
void squared_3x3(double& a11, double& a12, double& a13,
                 double& a21, double& a22, double& a23,
                 double& a31, double& a32, double& a33);


double maxValue(IJK_Field_double& indic);

void interpolate_to_center(IJK_Field_vector3_double& cell_center_field, const IJK_Field_vector3_double& face_field);


#endif /* IJK_tools_included */
