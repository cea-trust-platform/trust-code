/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Champ_Fonc_P0_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_P0_PolyMAC.h>
#include <Zone_PolyMAC.h>

Implemente_instanciable(Champ_Fonc_P0_PolyMAC,"Champ_Fonc_P0_PolyMAC",Champ_Fonc_P0_base);


//     printOn()
/////

Sortie& Champ_Fonc_P0_PolyMAC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Champ_Fonc_P0_PolyMAC::readOn(Entree& s)
{
  return s ;
}

void Champ_Fonc_P0_PolyMAC::mettre_a_jour(double t)
{
  Champ_Fonc_base::mettre_a_jour(t);
}

double Champ_Fonc_P0_PolyMAC::valeur_au_bord(int face) const
{
  double val_bord;
  const DoubleTab& val = valeurs();
  const Zone_PolyMAC& zone_PolyMAC=la_zone_PolyMAC.valeur();

  int n0 = zone_PolyMAC.face_voisins(face,0);
  if (n0 != -1)
    val_bord = val[n0];
  else
    val_bord = val[zone_PolyMAC.face_voisins(face,1)];
  return val_bord;
}

const Zone_dis_base& Champ_Fonc_P0_PolyMAC::zone_dis_base() const
{
  return la_zone_PolyMAC.valeur();
}

int Champ_Fonc_P0_PolyMAC::
remplir_coord_noeuds_et_polys(DoubleTab& positions,
                              IntVect& polys) const
{
  const Zone_dis_base& zone_dis = zone_dis_base();
  const Zone& zone = zone_dis.zone();
  int nb_elem=zone.nb_elem();
  polys.resize(nb_elem);

  remplir_coord_noeuds(positions);
  for(int i=0; i<nb_elem; i++)
    polys(i)=i;
  return 0;
}


// Description:
// renvoie la moyenne du champ au sens P0 i.e la somme, divisee par
// le volume total du domaine, des valeurs constantes par element
// multipliees par les volumes des elements

DoubleVect Champ_Fonc_P0_PolyMAC::moyenne() const
{
  const Zone_PolyMAC& zPolyMAC=la_zone_PolyMAC.valeur();
  const DoubleVect& porosite = zPolyMAC.porosite_elem();
  const DoubleVect& volumes = zPolyMAC.volumes();
  const DoubleTab& val = valeurs();

  int k,nb_compo = nb_comp();
  DoubleVect moy(nb_compo);
  moy =0;
  double coef,sum_vol=0;

  if (nb_compo == 1)
    for (int num_elem=0; num_elem<zPolyMAC.nb_elem(); num_elem++)
      {
        coef = porosite(num_elem)*volumes(num_elem);
        moy += val(num_elem)*coef;
        sum_vol += coef;
      }
  else
    for (int num_elem=0; num_elem<zPolyMAC.nb_elem(); num_elem++)
      {
        coef = porosite(num_elem)*volumes(num_elem);
        for (k=0; k<nb_compo; k++)
          moy[k] += val(num_elem,k)*coef;
        sum_vol += coef;
      }

  moy /= sum_vol;
  return moy;
}

// Description:
// Cette fonction effectue le calcul de la moyenne au sens P0
// de la kieme composante du champ
double Champ_Fonc_P0_PolyMAC::moyenne(int ncomp) const
{
  const Zone_PolyMAC& zPolyMAC=la_zone_PolyMAC.valeur();
  const DoubleVect& porosite = zPolyMAC.porosite_elem();
  const DoubleVect& volumes = zPolyMAC.volumes();
  const DoubleTab& val = valeurs();

  double moy=0;
  double coef,sum_vol=0;

  if (nb_comp() == 1)
    for (int num_elem=0; num_elem<zPolyMAC.nb_elem(); num_elem++)
      {
        coef = porosite(num_elem)*volumes(num_elem);
        moy += val(num_elem)*coef;
        sum_vol += coef;
      }
  else
    for (int num_elem=0; num_elem<zPolyMAC.nb_elem(); num_elem++)
      {
        coef = porosite(num_elem)*volumes(num_elem);
        moy += val(num_elem,ncomp)*coef;
        sum_vol += coef;
      }
  moy /= sum_vol;
  return moy;
}


int Champ_Fonc_P0_PolyMAC::imprime(Sortie& os, int ncomp) const
{
  imprime_P0(os,ncomp);
  return 1;
}

