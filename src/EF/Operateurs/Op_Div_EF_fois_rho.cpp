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
// File:        Op_Div_EF_fois_rho.cpp
// Directory:   $TRUST_ROOT/src/EF/Operateurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Div_EF_fois_rho.h>
#include <Zone_Cl_EF.h>
#include <Les_Cl.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Check_espace_virtuel.h>
#include <EcrFicPartage.h>
#include <SFichier.h>

Implemente_instanciable(Op_Div_EF_fois_rho,"Op_Div_EF_fois_rho",Op_Div_EF);


//// printOn
//

Sortie& Op_Div_EF_fois_rho::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Div_EF_fois_rho::readOn(Entree& s)
{
  return s ;
}



DoubleTab& Op_Div_EF_fois_rho::ajouter(const DoubleTab& vit, DoubleTab& div) const
{

  const Zone_EF& zone_ef=ref_cast(Zone_EF,equation().zone_dis().valeur());


  const DoubleTab& Bij_thilde=zone_ef.Bij_thilde();
  int nb_elem=zone_ef.zone().nb_elem();
  int nb_som_elem=zone_ef.zone().nb_som_elem();
  const IntTab& elems=zone_ef.zone().les_elems() ;

  const DoubleTab& rho=equation().get_champ("rho_comme_u").valeurs();

  for (int elem=0; elem<nb_elem; elem++)
    {
      for (int s=0; s<nb_som_elem; s++)
        {
          int som=elems(elem,s);
          for (int i=0; i<dimension; i++)
            {
              div(elem)+=Bij_thilde(elem,s,i)*rho(som)*vit(som,i);
            }
          // Cerr<<finl;
        }
    }
  // L'espace virtuel du tableau div n'est pas mis a jour par l'operateur,
  //  assert(invalide_espace_virtuel(div));
  declare_espace_virtuel_invalide(div);

  // calcul de flux bord

  const IntTab& face_sommets=zone_ef.face_sommets();
  int nb_som_face=zone_ef.nb_som_face();
  const DoubleTab& face_normales = zone_ef.face_normales();
  DoubleTab& tab_flux_bords = ref_cast(DoubleTab,flux_bords_);
  const DoubleVect& porosite_sommet=zone_ef.porosite_sommet();
  tab_flux_bords.resize(zone_ef.nb_faces_bord(),1);
  tab_flux_bords=0;
  int premiere_face_int=zone_ef.premiere_face_int();
  for (int face=0; face<premiere_face_int; face++)
    {
      for (int s=0; s<nb_som_face; s++)
        {
          int som=face_sommets(face,s);
          for (int dir=0; dir<Objet_U::dimension; dir++)

            tab_flux_bords(face,0)+=porosite_sommet(som)*rho(som)*vit(som,dir)*face_normales(face,dir)/nb_som_face;
        }
    }


  return div;
}


