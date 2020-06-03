/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champ_front_calc_interne.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_front_calc_interne.h>
#include <Interprete.h>
#include <Equation_base.h>
#include <Frontiere_dis_base.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <Zone_VF.h>
#include <DoubleTrav.h>
#include <MEDCouplingMemArray.hxx>

Implemente_instanciable_sans_constructeur(Champ_front_calc_interne,"Champ_front_calc_interne",Champ_front_calc);


Champ_front_calc_interne::Champ_front_calc_interne()
{
  set_distant(0);
}

// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Champ_front_calc_interne::printOn(Sortie& os) const
{
  return os;
}

// Description:
//    Lit le nom d'un champ inconnue a partir d'un flot d'entree.
//    Cree ensuite le champ de frontiere correspondant qui mettra en relation les deux
//    cotes de la frontiere interne.
//    Format:
//      Champ_front_calc_interne nom_pb nom_bord nom_champ
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entre
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Champ_front_calc_interne::readOn(Entree& is)
{
  Champ_front_calc::readOn(is);
  return is;
}

void Champ_front_calc_interne::completer()
{
  using namespace MEDCoupling;

  Champ_front_calc::completer();

  // Get barycenters of the concerned faces:
  const Front_VF& fvf = ref_cast(Front_VF, front_dis());
  const Zone_VF& zvf = ref_cast(Zone_VF, fvf.zone_dis());
  const DoubleTab& xv = zvf.xv();
  int nbfaces = fvf.nb_faces();

  if (nbfaces % 2)
    {
      Cerr << "ERROR : Champ_front_calc_interne::completer() - number of faces of the internal boundary is not even!"<< finl;
      Process::exit(1);
    }
  face_map_.resize(nbfaces,Array_base::NOCOPY_NOINIT);

  if (nbfaces>0)
    {
      const int ndim = xv.dimension(1);
      MCAuto<DataArrayDouble> coo(DataArrayDouble::New());
      coo->alloc(nbfaces, ndim);

      for (int i = 0; i < nbfaces; i++)
        {
          int num_f = fvf.num_face(i);

          for (int d = 0; d < ndim; d++)
            coo->setIJSilent(i, d, xv(num_f, d));
        }

      DataArrayInt *cP, *cIP;
      coo->findCommonTuples(Objet_U::precision_geom, 0, cP, cIP);
      int nb_faces_dup = cP->getNumberOfTuples();
      if (nb_faces_dup == nbfaces)
        {
          // Faces dupliquees -> singularite au niveau du gap
          MCAuto<DataArrayInt> c(cP), cI(cIP);
          MCAuto<DataArrayInt> dsi(cI->deltaShiftIndex());

          if (!dsi->isUniform(2))
            {
              Cerr << "ERROR : Problem on internal boundary. Some face(s) not duplicated." << finl;
              Process::exit(1);
            }

          const int *p(c->begin());
          for (int i = 0; i < nbfaces / 2; i++, p += 2)
            {
              face_map_(*p) = *(p + 1);
              face_map_(*(p + 1)) = *p;
            }
        }
      else
        {
          // Gap de dimension non-nulle -> on cherche le point
          // d'en face le plus proche
          const DoubleVect& face_surfaces = zvf.face_surfaces();
          MCAuto<DataArrayDouble> fnorm(DataArrayDouble::New());
          fnorm->alloc(nbfaces, 1);
          fnorm->fillWithZero();

          double* pfnorm = fnorm->getPointer();

          for (int i = 0; i < nbfaces; i++)
            {
              int num_f = fvf.num_face(i);

              for (int d = 0; d < ndim; d++)
                {
                  pfnorm[i]+=zvf.face_normales(num_f,d);
                  coo->setIJSilent(i, d, xv(num_f, d));
                }
              pfnorm[i]/=face_surfaces(num_f);
            }

          double eps = 1e-10;

          MCAuto<DataArrayInt> Idpos = fnorm->findIdsInRange(1-eps,1+eps);
          MCAuto<DataArrayInt> Idneg = fnorm->findIdsInRange(-1-eps,-1+eps);

          int n1 = Idpos->getNumberOfTuples();
          int n2 = Idneg->getNumberOfTuples();

          if ((n1!=n2) || (n1+n2)!=nbfaces)
            {
              Cerr << "ERROR : Problem on internal boundary. Cannot associate pairs of faces." << finl;
              Process::exit(1);
            }
          MCAuto<DataArrayDouble> coo_pos = coo->selectByTupleId(*Idpos);
          MCAuto<DataArrayDouble> coo_neg = coo->selectByTupleId(*Idneg);

          const int* pIdpos = Idpos->getConstPointer();
          const int* pIdneg = Idneg->getConstPointer();

          MCAuto<DataArrayInt> idx = coo_pos->findClosestTupleId(coo_neg);

          const int* pidx = idx->getConstPointer();

          for (int i=0; i<nbfaces/2; i++)
            {
              int j2 = pIdneg[i];
              int k = pidx[i];
              int j1 = pIdpos[k];

              face_map_(j1) = j2;
              face_map_(j2) = j1;
            }
        }
    }
}

void Champ_front_calc_interne::mettre_a_jour(double temps)
{
  Champ_front_calc::mettre_a_jour(temps);
  // Swap values (implies a copy)
  DoubleTab& tab=valeurs_au_temps(temps);
  assert(tab.nb_dim() == 2 && tab.dimension(1) == 1);
  DoubleTab tmp(tab);
  for (int i = 0; i < tab.dimension(0); i++)
    tab(i,0) = tmp(face_map_(i),0);
}
