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

#ifndef IJK_LATA_WRITER_H
#define IJK_LATA_WRITER_H

#include <IJK_Field.h>
#include <IJK_Field_vector.h>
#include <LataTools.h>

void dumplata_header(const char *filename);

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_header(const char *filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f);

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_add_geometry(const char *filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f);

void dumplata_add_geometry(const char *filename, const IJK_Splitting& splitting);

void dumplata_newtime(const char *filename, double time);

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_vector(const char *filename, const char *fieldname,
                     const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz,
                     int step);

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_vector_parallele_plan(const char *filename, const char *fieldname,
                                    const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz,
                                    int step);

template<typename _TYPE_>
void dumplata_cellvector(const char *filename, const char *fieldname,
                         const IJK_Field_vector<_TYPE_, 3>& v,
                         int step);

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_scalar(const char *filename, const char *fieldname,
                     const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f,
                     int step);

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_scalar_parallele_plan(const char *filename, const char *fieldname,
                                    const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f,
                                    int step);

void dumplata_finish(const char *filename);

template<typename _TYPE_, typename _TYPE_ARRAY_>
void lire_dans_lata(const char *filename, int tstep, const char *geometryname, const char *fieldname,
                    IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f);

template<typename _TYPE_, typename _TYPE_ARRAY_>
void lire_dans_lata(const char *filename, int tstep, const char *geometryname, const char *fieldname,
                    IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz);

template<typename _TYPE_, typename _TYPE_ARRAY_>
void read_lata_parallel_template(const char *filename_with_path, int tstep, const char *geometryname, const char *fieldname,
                                 const int i_compo,
                                 IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& field);

void dumplata_ft_field(const char *filename, const char *meshname,
                       const char *field_name, const char *localisation,
                       const ArrOfInt& field, int step);
void dumplata_ft_field(const char *filename, const char *meshname,
                       const char *field_name, const char *localisation,
                       const ArrOfDouble& field, int step);

Nom dirname(const Nom& filename);
Nom basename(const Nom& filename);

#include <IJK_Lata_writer.tpp>

#endif
