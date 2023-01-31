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

#ifndef distances_VDF_included
#define distances_VDF_included

#include <Zone_VDF.h>

void moy_2D_vit(const DoubleVect&, int, int, const Zone_VDF&, double&);
double norm_2D_vit(const DoubleVect&, int, int, const Zone_VDF&, double&);
double norm_2D_vit(const DoubleVect&, int, int, const Zone_VDF&, double, double, double&);
void moy_3D_vit(const DoubleVect&, int, int, const Zone_VDF&, double&, double&);
double norm_3D_vit(const DoubleVect&, int, int, const Zone_VDF&, double&, double&);
double norm_3D_vit(const DoubleVect&, int, int, const Zone_VDF&, double, double, double, double&, double&);
double norm_vit(const DoubleVect&, int, int, const Zone_VDF&, const ArrOfDouble& vit_paroi, ArrOfDouble& val);

void calcul_interne2D(int num_elem, int elx0, int elx1, int ely0, int ely1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& rot);
void calcul_bord2D(int num_elem, int elx0, int elx1, int ely0, int ely1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& rot);
void calrotord2centelemdim2(DoubleTab& rot, const DoubleTab& val, const Zone_VDF& zone_VDF, int nb_elem, const IntTab& face_voisins, const IntTab& elem_faces);
void calcul_interne3D(int num_elem, int elx0, int elx1, int ely0, int ely1, int elz0, int elz1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& rot);
void calcul_bord3D(int num_elem, int elx0, int elx1, int ely0, int ely1, int elz0, int elz1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& rot);
void calrotord2centelemdim3(DoubleTab& rot, const DoubleTab& val, const Zone_VDF& zone_VDF, int nb_elem, const IntTab& face_voisins, const IntTab& elem_faces);

// Calcul du produit scalaire du tenseur des vitesses de deformation en coordonnees cartesiennes : calcul 2D puis 3D.
void calcul_dscald_interne2D(int num_elem, int elx0, int elx1, int ely0, int ely1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& dscald);
void calcul_dscald_interne3D(int num_elem, int elx0, int elx1, int ely0, int ely1, int elz0, int elz1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& dscald);
void calcul_dscald_bord2D(int num_elem, int elx0, int elx1, int ely0, int ely1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& dscald);
void calcul_dscald_bord3D(int num_elem, int elx0, int elx1, int ely0, int ely1, int elz0, int elz1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& dscald);
void caldscaldcentelemdim2(DoubleTab& dscald, const DoubleTab& val, const Zone_VDF& zone_VDF, int nb_elem, const IntTab& face_voisins, const IntTab& elem_faces);
void caldscaldcentelemdim3(DoubleTab& dscald, const DoubleTab& val, const Zone_VDF& zone_VDF, int nb_elem, const IntTab& face_voisins, const IntTab& elem_faces);

#endif /* distances_VDF_included */
