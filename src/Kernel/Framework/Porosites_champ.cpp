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

#include <Porosites_champ.h>
#include <Champ_Don_base.h>
#include <Sous_Domaine.h>
#include <Domaine_VF.h>

/*! @brief renvoit le tableau res, res=org si flag=0, res= porosite*org sinon attention ne pas modifier res car sinon on ne sait pas ce que l'on fait sur org (d'ou le renvoi const)
 *
 */
const DoubleTab& modif_par_porosite_si_flag(const DoubleTab& org, DoubleTab& res, int flag, const DoubleVect& porosite)
{
  // Switch mp_min_vect to local_min_abs_vect to avoid mp_min call during the calculation of operators (convection, diffusion,...)
  if ((flag == 0) || (local_min_abs_vect(porosite) == 1))
    res.ref(org);
  else
    {
      assert(org.dimension_tot(0) == porosite.size_totale());
      res = org;
      tab_multiply_any_shape(res, porosite, VECT_ALL_ITEMS);
    }
  return res;
}

// TODO : REMOVE for V1.9.3 ...
Implemente_instanciable(Porosites_champ,"Porosites_champ",Interprete);
Sortie& Porosites_champ::printOn(Sortie& os) const { return Interprete::printOn(os); }
Entree& Porosites_champ::readOn(Entree& is) { return Interprete::readOn(is); }
Entree& Porosites_champ::interpreter(Entree& is)
{
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "TRUST-V1.9.1 : Porosites_champ interpret is no more available !" << finl;
  Cerr << "You should define the porosity in the medium bloc, for example like this : " << finl;
  Cerr << "porosites_champ champ_fonc_xyz dom 1 1-0.5*(y>1)*(y<2) " << finl;
  Process::exit();
  return is;
}


Implemente_instanciable(Porosites,"Porosites",Objet_U);
// XD porosites objet_u porosites 0 To define the volume porosity and surface porosity that are uniform in every direction in space on a sub-area. NL2 Porosity was only usable in VDF discretization, and now available for VEF P1NC/P0. NL2 Observations : NL2 - Surface porosity values must be given in every direction in space (set this value to 1 if there is no porosity), NL2 - Prior to defining porosity, the problem must have been discretized.NL2 Can \'t be used in VEF discretization, use Porosites_champ instead.
// XD attr aco chaine(into=["{"]) aco 0 Opening curly bracket.
// XD attr sous_zone|sous_zone1 chaine sous_zone 0 Name of the sub-area to which porosity are allocated.
// XD attr bloc bloc_lecture_poro bloc 0 Surface and volume porosity values.
// XD attr sous_zone2 chaine sous_zone2 1 Name of the 2nd sub-area to which porosity are allocated.
// XD attr bloc2 bloc_lecture_poro bloc2 1 Surface and volume porosity values.
// XD attr acof chaine(into=["}"]) acof 0 Closing curly bracket.

// XD bloc_lecture_poro objet_lecture nul 1 Surface and volume porosity values.
// XD  attr volumique floattant volumique 0 Volume porosity value.
// XD  attr surfacique list surfacique 0 Surface porosity values (in X, Y, Z directions).

Sortie& Porosites::printOn(Sortie& os) const { return Objet_U::printOn(os); }

Entree& Porosites::readOn(Entree& is)
{
  is_read_ = true;
  Nom mot;
  Motcle motcle;
  is >> mot;
  double vol = 1.;
  DoubleVect surf;

  if (mot != "{")
    {
      Cerr << "Porosites : { expected after porosites instead of " << mot << finl;
      Process::exit();
    }
  for (is >> mot; mot != "}"; is >> mot)
    {
      // 1er truc a lire : nom sous-domaine
      les_sous_domaines.push_back(mot);
      // 2eme truc a lire : accolade
      is >> mot;
      if (mot != "{")
        {
          Cerr << "Porosites : { expected after the sous domaine instead of " << mot << finl;
          Process::exit();
        }
      for (is >> motcle; motcle != "}"; is >> motcle)
        {
          if (motcle == "VOLUMIQUE")
            {
              is >> vol;
              porosites_volu.push_back(vol);
            }
          else if (motcle == "SURFACIQUE")
            {
              is >> surf;
              porosites_surf.push_back(surf);
            }
          else
            {
              Cerr << "Porosites : " << motcle << " is not known !! Use either VOLUMIQUE or SURFACIQUE" << finl;
              Process::exit();
            }
        }
    }
  assert(les_sous_domaines.size() == porosites_volu.size() && les_sous_domaines.size() == porosites_surf.size());

  // quelques tests !
  for (const auto &sz : les_sous_domaines)
    if (!sub_type(Sous_Domaine, Interprete::objet(sz)))
      {
        Cerr << sz << " is of type " << Interprete::objet(sz).que_suis_je() << ". We waited an object of type Sous_Domaine" << finl;
        Process::exit();
      }

  for (const auto &val : porosites_surf)
    if (val.local_min_vect() <= 0)
      {
        Cerr << "You entered a negative surfacic porosity ! " << finl;
        Process::exit();
      }

  return is;
}

void Porosites::remplir_champ(const Domaine_VF& zvf, DoubleVect& porosite_elem, DoubleVect& porosite_face)
{
  if (zvf.que_suis_je().debute_par("Domaine_VEF"))
    {
      Cerr << "Porosites should no longer be used in VEF. Porosites_champ should be used instead." << finl;
      Process::exit();
    }

  const IntTab& elem_faces = zvf.elem_faces();
  const int sz = (int)les_sous_domaines.size();
  int nb_faces_elem = zvf.domaine().nb_faces_elem();

  for (int ind = 0; ind < sz; ind++)
    {
      Sous_Domaine& ssz = ref_cast(Sous_Domaine, Interprete::objet(les_sous_domaines[ind]));
      const double porsurfmin = porosites_surf[ind].local_min_vect();

      if ((porosites_volu[ind] != 1) && (porsurfmin != 1))
        {
          for (int i = 0; i < ssz.nb_elem_tot(); i++)
            {
              const int elem = ssz(i);
              porosite_elem(elem) = porosites_volu[ind];
              for (int j = 0; j < nb_faces_elem; j++)
                {
                  double norme_normale = 0;
                  for (int iii = 0; iii < dimension; iii++)
                    norme_normale += zvf.face_normales(elem_faces(elem, j), iii) * zvf.face_normales(elem_faces(elem, j), iii);

                  porosite_face(elem_faces(elem, j)) = 0.;
                  for (int iii = 0; iii < dimension; iii++)
                    porosite_face(elem_faces(elem, j)) += porosites_surf[ind](iii) * std::fabs(zvf.face_normales(elem_faces(elem, j), iii))
                                                          * std::fabs(zvf.face_normales(elem_faces(elem, j), iii)) / norme_normale;
                }
            }
        }
      else if (porosites_volu[ind] != 1)
        {
          Cerr << " The volumic porosities are used to calculate the surfacic porosities " << finl;
          for (int i = 0; i < ssz.nb_elem_tot(); i++)
            {
              const int elem = ssz(i);
              porosite_elem(elem) = porosites_volu[ind];
            }

          const int nb_elem_tot = zvf.domaine().nb_elem_tot();
          for (int i = 0; i < nb_elem_tot; i++)
            {
              for (int j = 0; j < nb_faces_elem; j++)
                {
                  int face_glob = elem_faces(i, j), num_poly_vois = zvf.face_voisins(face_glob, 1), num_poly_vois1 = zvf.face_voisins(face_glob, 0);

                  if ((num_poly_vois != -1) && (num_poly_vois1 != -1))
                    porosite_face(face_glob) = (porosite_elem(num_poly_vois1) + porosite_elem(num_poly_vois)) / 2.;
                  else
                    porosite_face(face_glob) = porosite_elem(i);

                }
            }
        }
      else if (porsurfmin != 1)
        {
          Cerr << " The surfacic porosities are used to calculate the volumic porosities " << finl;
          for (int i = 0; i < ssz.nb_elem_tot(); i++)
            {
              const int elem = ssz(i);
              for (int j = 0; j < nb_faces_elem; j++)
                {
                  double norme_normale = 0.;
                  for (int iii = 0; iii < dimension; iii++)
                    norme_normale += zvf.face_normales(elem_faces(elem, j), iii) * zvf.face_normales(elem_faces(elem, j), iii);

                  porosite_face(elem_faces(elem, j)) = 0.;
                  for (int iii = 0; iii < dimension; iii++)
                    porosite_face(elem_faces(elem, j)) += porosites_surf[ind](iii) * zvf.face_normales(elem_faces(elem, j), iii) * zvf.face_normales(elem_faces(elem, j), iii) / norme_normale;
                }
            }
        }
    }

  porosites_surf.clear();
  les_sous_domaines.clear();
  porosites_volu.clear();
}
