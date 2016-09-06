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
// File:        Assembleur_P_VDF_Q4.cpp
// Directory:   $TRUST_ROOT/src/VDF/Solveurs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Assembleur_P_VDF_Q4.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Matrice_Bloc.h>

Implemente_instanciable(Assembleur_P_VDF_Q4,"Assembleur_P_VDF_Q4",Assembleur_base);

Sortie& Assembleur_P_VDF_Q4::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom() ;
}

Entree& Assembleur_P_VDF_Q4::readOn(Entree& s )
{
  return Assembleur_base::readOn(s);
}

int Assembleur_P_VDF_Q4::assembler(Matrice& la_matrice)
{
  Cerr << "Assembleur_P_VDF_Q4::assembler " << finl;
  Cerr << "Assemblage de la matrice Q4 en cours..." << finl;
  const IntTab& FaceSoms = la_zone_VDF->face_sommets();
  const IntTab& FaceVois = la_zone_VDF->face_voisins();
  const IntVect& Orientation = la_zone_VDF->orientation();
  int face, som0,som1;
  int nbfaces = la_zone_VDF->nb_faces(),  nbsom = la_zone_VDF->nb_som();
  const DoubleVect& volumes_entrelaces = la_zone_VDF->volumes_entrelaces();
  const DoubleVect& porosite_face = la_zone_VDF->porosite_face();
  const DoubleTab& CoordSom = la_zone_VDF->zone().domaine().coord_sommets();
  double surf,vol,por,val, r;
  int ori, elem0,elem1;
  //construction matrice
  la_matrice.typer("Matrice_Morse");
  Matrice_Morse& mat = ref_cast(Matrice_Morse,la_matrice.valeur());
  mat.dimensionner(nbsom,0);
  IntVect rang_voisins(nbsom);
  IntVect& tab1 = mat.get_set_tab1();
  IntVect& tab2 = mat.get_set_tab2();
  DoubleVect& coeff = mat.get_set_coeff();

  tab1 = 0;
  tab2 = 0;
  coeff = 0;
  rang_voisins = 1;
  for (face=0 ; face<nbfaces ; face++)
    {
      som0 = FaceSoms(face,0);
      som1 = FaceSoms(face,1);
      rang_voisins(som0)++;
      rang_voisins(som1)++;
    }
  tab1(0) = 1;
  for (som0=0 ; som0<nbsom ; som0++)
    {
      tab1(som0+1) = rang_voisins(som0) + tab1(som0);
    }
  rang_voisins = 1;
  mat.dimensionner(nbsom,tab1(nbsom)-1);
  //calcul coefficient matrice
  for (face=0 ; face<nbfaces ; face++)
    {
      ori = Orientation(face);
      som0 = FaceSoms(face,0);
      som1 = FaceSoms(face,1);
      elem0 = FaceVois(face,0);
      elem1 = FaceVois(face,1);
      if (elem0>-1 && elem1>-1)
        {
          surf = 0.5* (la_zone_VDF->dim_elem(elem0,ori) + la_zone_VDF->dim_elem(elem1,ori));
        }
      else if (elem0>-1)
        {
          surf = la_zone_VDF->dim_elem(elem0,ori);
        }
      else
        {
          surf = 0.;
          surf = la_zone_VDF->dim_elem(elem1,ori);
        }
      if (bidim_axi/* && ori==1*/)
        {
          r = 0.5*(CoordSom(som0,0)+CoordSom(som1,0));
          surf = r * surf * 2 * M_PI;
        }
      vol = volumes_entrelaces(face);
      por = porosite_face(face);
      val = surf*surf /vol * por;
#ifdef _AFFDEBUG
      if (la_zone_VDF->nb_som()<400)
        {
          Cerr<<"face="<<face<<"  ori="<<ori<<"  soms="<<som0<<"/"<<som1<<" coordsom="<<CoordSom(som0,0)<<"/"<<CoordSom(som0,1)<<" "<<CoordSom(som1,0)<<"/"<<CoordSom(som1,1)<<"  elems="<<FaceVois(face,0)<<"/"<<FaceVois(face,1)<<"  dim_elems/2=";
          if (FaceVois(face,0)>-1)
            Cerr<<la_zone_VDF->dim_elem(FaceVois(face,0),ori)/2.<<" / ";
          else
            Cerr<<"--- / ";
          if (FaceVois(face,1)>-1)
            Cerr<<la_zone_VDF->dim_elem(FaceVois(face,1),ori)/2.<<"  ";
          else
            Cerr<<"---  ";
          Cerr<<"  surf="<<surf<<"  vol="<<vol<<"  val="<<val<<finl;
        }
#endif

      tab2(tab1(som0)-1) = som0+1;
      tab2(tab1(som1)-1) = som1+1;
      mat(som0,som0) += val;
      mat(som1,som1) += val;
      //if (coeff(tab1(som0)+rang_voisins(som0)-1)!=-1) {
      tab2(tab1(som0)+rang_voisins(som0)-1) = som1+1;
      mat(som0,som1) -= val;
      rang_voisins(som0)++;
      tab2(tab1(som1)+rang_voisins(som1)-1) = som0+1;
      mat(som1,som0) -= val;
      rang_voisins(som1)++;
      //}
    }
  Cerr << "Fin de l'assemblage de la matrice Q4" << finl;
  return 1;
}

int Assembleur_P_VDF_Q4::modifier_secmem(DoubleTab& secmem)
{
  return 1;
}

int Assembleur_P_VDF_Q4::modifier_secmem(const DoubleTab& tab_secmem_, DoubleVect& secmem)
{
  const DoubleTab& CoordSommets = la_zone_VDF->zone().domaine().coord_sommets();
  const IntTab& FaceSoms = la_zone_VDF->face_sommets();
  const IntTab& FaceVois = la_zone_VDF->face_voisins();
  const IntVect& Orientation = la_zone_VDF->orientation();
  const DoubleTab& CoordSom = la_zone_VDF->zone().domaine().coord_sommets();
  int face, som0,som1, ori, invori, elem0,elem1;
  int nbfaces = la_zone_VDF->nb_faces(),  nbsom = la_zone_VDF->nb_som();
  double surf, r;

  secmem.resize(nbsom);
  secmem = 0.;

  for (face=0 ; face<nbfaces ; face++)
    {
      surf = 0.;
      ori = Orientation(face);
      som0 = FaceSoms(face,0);
      som1 = FaceSoms(face,1);
      elem0 = FaceVois(face,0);
      elem1 = FaceVois(face,1);
      if (elem0>-1 && elem1>-1)
        {
          surf = 0.5* (la_zone_VDF->dim_elem(elem0,ori) + la_zone_VDF->dim_elem(elem1,ori));
        }
      else if (elem0>-1)
        {
          surf = la_zone_VDF->dim_elem(elem0,ori);
        }
      else if (elem1>-1)
        {
          surf = la_zone_VDF->dim_elem(elem1,ori);
        }
      else
        {
          assert(0==1);
        }
      if (bidim_axi/* && ori==1*/)
        {
          r = 0.5*(CoordSom(som0,0)+CoordSom(som1,0));
          surf = r * surf * 2 * M_PI;
        }
      if (ori==0)
        invori = 1;
      else
        invori = 0;
      if (CoordSommets(som1,invori)>CoordSommets(som0,invori))
        {
          secmem(som0) += tab_secmem_(face) * surf;
          secmem(som1) -= tab_secmem_(face) * surf;
        }
      else
        {
          secmem(som0) -= tab_secmem_(face) * surf;
          secmem(som1) += tab_secmem_(face) * surf;
        }
      secmem.echange_espace_virtuel();
    }

  return 1;
}

int Assembleur_P_VDF_Q4::modifier_solution(DoubleTab& pression)
{
  Cerr<<"Assembleur_P_VDF_Q4::modifier_solution"<<finl;
  return 1;
}

const Zone_dis_base& Assembleur_P_VDF_Q4::zone_dis_base() const
{
  return la_zone_VDF.valeur();
}

const Zone_Cl_dis_base& Assembleur_P_VDF_Q4::zone_Cl_dis_base() const
{
  return la_zone_Cl_VDF.valeur();
}

void Assembleur_P_VDF_Q4::associer_zone_dis_base(const Zone_dis_base& la_zone_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, la_zone_dis);
}

void Assembleur_P_VDF_Q4::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis)
{
  la_zone_Cl_VDF =ref_cast(Zone_Cl_VDF, la_zone_Cl_dis);
}

void Assembleur_P_VDF_Q4::completer(const Equation_base& Eqn)
{
  eqn_ = Eqn;
}
