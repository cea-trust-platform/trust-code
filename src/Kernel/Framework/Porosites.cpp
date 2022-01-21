/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Porosites.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Porosites.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Zone_VF.h>
#include <Param.h>

// Generalisation pour les VEF

Implemente_instanciable(Porosites,"Porosites",Interprete);

Sortie& Porosites::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Porosites::readOn(Entree& is)
{
  return Interprete::readOn(is);
}
Entree& Porosites::interpreter(Entree& is)
{

  int i;
  Nom nom_pb, nom_ss_zone;
  is >> nom_pb;
  if(! sub_type(Probleme_base, objet(nom_pb)))
    {
      Cerr << nom_pb << " is of type " << objet(nom_pb).que_suis_je() << finl;
      Cerr << "We waited an object of type Probleme_base" << finl;
      exit();
    }
  Probleme_base& pb=ref_cast(Probleme_base, objet(nom_pb));

  is >> nom_ss_zone;
  if(! sub_type(Sous_Zone, objet(nom_ss_zone)))
    {
      Cerr << nom_ss_zone << " is of type " << objet(nom_ss_zone).que_suis_je() << finl;
      Cerr << "We waited an object of type Sous_Zone" << finl;
      exit();
    }

  Sous_Zone& ssz=ref_cast(Sous_Zone, objet(nom_ss_zone));

  double porosite_volu=1;
  DoubleVect porosites_surf;

  Param param(que_suis_je());
  param.ajouter("Volumique",&porosite_volu);
  param.ajouter("Surfacique",&porosites_surf);
  param.ajouter_condition("value_of_Volumique_gt_0","Volumic porosity must be greater than 0");
  param.lire_avec_accolades_depuis(is);

  double porsurfmin=porosites_surf.local_min_vect();
  if(porsurfmin<=0)
    {
      Cerr << "You entered a surfacic porosity <=0 " << finl;
      exit();
    }

  const Zone_dis& la_zone_dis = pb.equation(0).zone_dis();
  if (!la_zone_dis.non_nul())
    {
      Cerr << "You must have discretized the problem before defining the porosities" << finl;
      exit();
    }

  // On passe par la zone_VF

  Zone_VF& zvf=ref_cast(Zone_VF, pb.equation(0).zone_dis().valeur());
  if (zvf.que_suis_je().debute_par("Zone_VEF"))
    {
      Cerr << "Porosites should no longer be used in VEF." << finl;
      Cerr << "Porosites_champ should be used instead." << finl;
      exit();
    }
  DoubleVect& porosite_elem = zvf.porosite_elem();
  DoubleVect& porosite_face = zvf.porosite_face();
  const IntTab& elem_faces = zvf.elem_faces();
  int nb_faces_elem = zvf.zone().nb_faces_elem();
  int elem;

  if( (porosite_volu!=1) && (porsurfmin!=1) )
    {
      for (i=0; i<ssz.nb_elem_tot(); i++)
        {
          elem = ssz(i);
          porosite_elem(elem)=porosite_volu;
          for (int j=0; j<nb_faces_elem; j++)
            {
              double norme_normale =0;
              int iii;
              for (iii=0; iii<dimension; iii++)
                norme_normale+=zvf.face_normales(elem_faces(elem, j),iii)*zvf.face_normales(elem_faces(elem, j),iii);

              porosite_face(elem_faces(elem, j))=0.;
              for (iii=0; iii<dimension; iii++)
                {
                  porosite_face(elem_faces(elem, j))+= porosites_surf(iii)*std::fabs(zvf.face_normales(elem_faces(elem, j),iii))
                                                       *std::fabs(zvf.face_normales(elem_faces(elem, j),iii))/norme_normale;
                }
            }
        }
    }
  else if(porosite_volu!=1)
    {
      Cerr << " The volumic porosities are used to calculate the surfacic porosities " << finl ;
      for (i=0; i<ssz.nb_elem_tot(); i++)
        {
          elem = ssz(i);
          porosite_elem(elem)=porosite_volu;
        }
      int nb_elem_tot = zvf.zone().nb_elem_tot();
      for (i=0; i<nb_elem_tot; i++)
        {
          for (int j=0; j<nb_faces_elem; j++)
            {
              int face_glob = elem_faces(i, j) ;
              int num_poly_vois = zvf.face_voisins(face_glob,1);
              int num_poly_vois1 = zvf.face_voisins(face_glob,0);

              if ((num_poly_vois != -1) && (num_poly_vois1 != -1))
                porosite_face(face_glob) = (porosite_elem(num_poly_vois1)+porosite_elem(num_poly_vois))/2. ;
              else porosite_face(face_glob) =  porosite_elem(i) ;

            }
        }
    }
  else if(porsurfmin!=1)
    {
      Cerr << " The surfacic porosities are used to calculate the volumic porosities " << finl ;
      for (i=0; i<ssz.nb_elem_tot(); i++)
        {
          elem = ssz(i);
          for (int j=0; j<nb_faces_elem; j++)
            {
              double norme_normale =0;
              int iii;
              for (iii=0; iii<dimension; iii++)
                norme_normale+=zvf.face_normales(elem_faces(elem, j),iii)*zvf.face_normales(elem_faces(elem, j),iii);

              porosite_face(elem_faces(elem, j))=0.;
              for (iii=0; iii<dimension; iii++)
                {
                  porosite_face(elem_faces(elem, j))+= porosites_surf(iii)*zvf.face_normales(elem_faces(elem, j),iii)
                                                       *zvf.face_normales(elem_faces(elem, j),iii)/norme_normale;
                }
            }
        }
    }
  return is;
}
