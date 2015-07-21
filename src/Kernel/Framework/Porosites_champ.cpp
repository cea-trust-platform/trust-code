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
// File:        Porosites_champ.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Porosites_champ.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Zone_VF.h>
#include <Champ.h>
#include <Champ_Don.h>
#include <Champ_Uniforme.h>
#include <Discretisation_base.h>

// Generalisation pour les VEF
// Description: renvoit le tableau res, res=org si flag=0, res= porosite*org sinon
// attention ne pas modifier res car sinon on ne sait pas ce que l'on fait sur org (d'ou le renvoi const)
const DoubleTab& modif_par_porosite_si_flag(const DoubleTab& org, DoubleTab& res,int flag,const DoubleVect& porosite)
{
  // Switch mp_min_vect to local_min_abs_vect to avoid mp_min
  // call during the calculation of operators (convection, diffusion,...)
  if ((flag==0)||(local_min_abs_vect(porosite)==1))
    res.ref(org);
  else
    {
      assert(org.dimension_tot(0)==porosite.size_totale());
      res = org;
      tab_multiply_any_shape(res, porosite, VECT_ALL_ITEMS);
    }
  return res;
}

Implemente_instanciable(Porosites_champ,"Porosites_champ",Interprete);

Sortie& Porosites_champ::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Porosites_champ::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description: Porosites_champ nom_pb champ affecte le champ champ a la porosite volumique du domaine du probleme de nom_pb, puis calcule les porosites surfacqiues comme moyenne harmonique
Entree& Porosites_champ::interpreter(Entree& is)
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
  DoubleVect& porosite_elem = zvf.porosite_elem();
  DoubleVect& porosite_face_r = zvf.porosite_face();
  const IntTab& face_voisins = zvf.face_voisins();
  int nb_elem_tot = zvf.zone().nb_elem_tot();

  // on discretise des champs equivalents pour pouvoir faire affecter
  // puis on recopie les valeurs
  Champ_Don porosite_elem_p;
  pb.discretisation().discretiser_champ("CHAMP_ELEM", pb.equation(0).zone_dis(), "pp", "1",1,0., porosite_elem_p);

  porosite_elem_p.valeur().valeurs()=1;
  porosite_elem_p.valeur().affecter(le_champ);
  for (int elem=0; elem<nb_elem_tot; elem++)
    porosite_elem(elem)=porosite_elem_p.valeurs()(elem);

  // calcul de la porosite surfacique comme la moyenne harmonique
  // cf D.J pour justification
  Champ_Don porosite_face_p;
  pb.discretisation().discretiser_champ("VITESSE", pb.equation(0).zone_dis(), "pp", "1",1,0., porosite_face_p);
  DoubleTab& porosite_face=porosite_face_p.valeurs();
  int nb_face_tot = zvf.nb_faces_tot();
  if (porosite_face_r.size()==porosite_face.size())
    {
      for (int face=0; face<nb_face_tot; face++)
        {
          int elem1 = face_voisins(face,1);
          int elem2 = face_voisins(face,0);
          if ((elem1 != -1) && (elem2 != -1))
            {
              porosite_face(face)=2./(1./porosite_elem(elem1)+1./porosite_elem(elem2));
            }
          else if (elem1!=-1)
            porosite_face(face) =  porosite_elem(elem1) ;
          else
            porosite_face(face) =  porosite_elem(elem2) ;
        }
      porosite_face.echange_espace_virtuel();
      for (int face=0; face<nb_face_tot; face++)
        porosite_face_r(face) = porosite_face(face);
    }
  return is;
}
