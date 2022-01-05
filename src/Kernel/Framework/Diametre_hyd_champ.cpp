/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Diametre_hyd_champ.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Diametre_hyd_champ.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Zone_VF.h>
#include <Champ.h>
#include <Champ_Don.h>
#include <Champ_Uniforme.h>
#include <Discretisation_base.h>

Implemente_instanciable(Diametre_hyd_champ,"Diametre_hyd_champ",Interprete);
// XD Diametre_hyd_champ interprete Diametre_hyd_champ -2 The hydraulic diameter is given at each element and the hydraulic diameter at each face is calculated by the average of the hydraulic diameters of the two neighbour elements
// XD attr pb ref_Pb_base pb 0 Name of the problem to which the sub-area is attached
// XD attr ch field_base ch 0 field used to define the hydraulic diameter field

Sortie& Diametre_hyd_champ::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Diametre_hyd_champ::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description: Diametre_hyd_champ nom_pb champ affecte le champ champ a la porosite volumique du domaine du probleme de nom_pb, puis calcule les porosites surfacqiues comme moyenne harmonique
Entree& Diametre_hyd_champ::interpreter(Entree& is)
{
  Nom nom_pb;
  is >> nom_pb;
  if(! sub_type(Probleme_base, objet(nom_pb)))
    {
      Cerr << nom_pb << " is of type " << objet(nom_pb).que_suis_je() << finl;
      Cerr << "We waited an object of type Probleme_base" << finl;
      Process::exit();
    }
  Probleme_base& pb=ref_cast(Probleme_base, objet(nom_pb));
  Champ le_champ;
  is>>le_champ;

  // On passe par la zone_VF
  Zone_VF& zvf=ref_cast(Zone_VF, pb.equation(0).zone_dis().valeur());
  DoubleTab& dh_elem = zvf.diametre_hydraulique_elem();

  // on discretise des champs equivalents pour pouvoir faire affecter
  // puis on recopie les valeurs
  Champ_Don dh_elem_p;
  pb.discretisation().discretiser_champ("CHAMP_ELEM", pb.equation(0).zone_dis(), "pp", "1",1,0., dh_elem_p);

  dh_elem_p.valeur().valeurs()=0;
  dh_elem_p.valeur().affecter(le_champ);
  dh_elem = dh_elem_p.valeurs();

  // calcul des diametres_hydrauliques aux faces
  DoubleVect& dh_face = zvf.diametre_hydraulique_face();
  dh_face = 0.;
  const int nb_face_tot = zvf.nb_faces_tot();
  const IntTab& f_e = zvf.face_voisins();
  int e;

  for (int f = 0; f < nb_face_tot; f++)
    {
      double nv = 0.0;
      for (int i = 0; i < 2; i++) if ((e = f_e(f, i)) > -1)
          {
            dh_face(f) += dh_elem(e);
            nv += 1.0;
          }
      dh_face(f) /= nv;
    }


  return is;
}
