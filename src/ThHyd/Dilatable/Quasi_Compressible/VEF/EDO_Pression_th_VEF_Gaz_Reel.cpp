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
// File:        EDO_Pression_th_VEF_Gaz_Reel.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/VEF
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <EDO_Pression_th_VEF_Gaz_Reel.h>
#include <Fluide_Quasi_Compressible.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Schema_Temps_base.h>
#include <Neumann_sortie_libre.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps.h>
#include <DoubleTrav.h>

Implemente_instanciable(EDO_Pression_th_VEF_Gaz_Reel,"EDO_Pression_th_VEF_Gaz_Reel_non",EDO_Pression_th_VEF);

Sortie& EDO_Pression_th_VEF_Gaz_Reel::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& EDO_Pression_th_VEF_Gaz_Reel::readOn(Entree& is)
{
  return is;
}

// Description:
//    Resoud l'EDO
// Precondition:
// Parametre: double Pth_n
//    Signification: La pression a l'etape precedente
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: double
//    Signification: La nouvelle valeur de la pression
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double EDO_Pression_th_VEF_Gaz_Reel::resoudre(double Pth_n)
{
  int n_bord ;
  for (n_bord=0; n_bord<la_zone->nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl->les_conditions_limites(n_bord);
      if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
        return Pth_n;
    }
  double Pth;
  const DoubleTab& tab_vit = ref_cast(Navier_Stokes_std,le_fluide_->vitesse()->equation()).vitesse();
  const DoubleTab& tab_hnp1 = le_fluide_->inco_chaleur().valeurs();       //actuel
  const DoubleTab& tab_hn = le_fluide_->inco_chaleur().passe();        //passe
  const DoubleTab& tab_rho = le_fluide_->masse_volumique().valeurs();    //actuel
  const Loi_Etat& loi_ = le_fluide_->loi_etat();
  //  const DoubleVect& tab_rhon = loi_->rho_n();                       //passe

  int elem, nb_elem=la_zone->nb_elem();
  double V = 0; //mesure du domaine
  double Fn = 0; //integrale 1 a l'etape n
  double Fnp1 = 0; //integrale 1 a l'etape n+1
  double S = 0; //second membre

  double dt = le_fluide_->vitesse()->equation().schema_temps().pas_de_temps();
  double v,al,bn, bnp1, hn,hnp1, divu;

  int i, face,nb_faces=la_zone->nb_faces();
  //calcul T* aux elements
  DoubleTab HstarP0(nb_elem);
  DoubleTab gradh(nb_faces,dimension);
  DoubleTab u_gradh(nb_faces);
  int nfe = la_zone->zone().nb_faces_elem();
  const IntTab& elem_faces = la_zone->elem_faces();
  for (elem=0 ; elem<nb_elem ; elem++)
    {
      HstarP0(elem) = 0;
      for (face=0 ; face<nfe ; face++)
        {
          hn = tab_hn(elem_faces(elem,face));
          hnp1 = tab_hnp1(elem_faces(elem,face));
          HstarP0(elem) += .5*(hn+hnp1);
        }
      HstarP0(elem) /= nfe;
    }
  //calcul gradT*
  calculer_grad(HstarP0,gradh);
  //calcul u.gradT*
  for (face=0 ; face<nb_faces ; face++)
    {
      u_gradh(face)=0;
      for (i=0 ; i<dimension ; i++)
        {
          u_gradh(face) += gradh(face,i) * tab_vit(face,i);
        }
    }
  //calcul divU en P0 et P1
  DoubleTrav divUP0(nb_elem);
  ref_cast(Navier_Stokes_std,le_fluide_->vitesse()->equation()).operateur_divergence().calculer(tab_vit,divUP0);
  DoubleTrav divU(nb_faces);
  int e0,e1;
  for (face=0 ; face<nb_faces ; face++)
    {
      e0 = la_zone->face_voisins(face,0);
      e1 = la_zone->face_voisins(face,1);
      if (e0!=-1 && e1!=-1)
        {
          divU(face) = .5*(divUP0(e0)+divUP0(e1));
        }
      else if (e0!=-1)
        {
          divU(face) = divUP0(e0);
        }
      else
        {
          divU(face) = divUP0(e1);
        }
    }

  for (face=0 ; face<nb_faces ; face++)
    {
      v = la_zone->volumes_entrelaces(face);
      V += v;
      hn   = tab_hn(face);
      hnp1 = tab_hnp1(face);
      al = loi_->Drho_DT(Pth_n,hn) / loi_->Drho_DP(Pth_n,hn);
      bn = tab_rho(face)/ loi_->Drho_DP(Pth_n,hn);
      bnp1 = loi_->calculer_masse_volumique(Pth_n,hnp1) / loi_->Drho_DP(Pth_n,hnp1);
      divu = divU(face);

      S -= v *al *((hnp1-hn)/dt + u_gradh(face));
      Fn += v * bn * divu;
      Fnp1 += v * bnp1 * divu;
    }


  Pth = Pth_n + dt/V *(S-Fn);
  //   Cerr<<"Volume="<<V<<finl;
  //   Cerr<<"Fn="<<Fn<<finl;
  //   Cerr<<"S="<<S<<finl;
  //   Cerr<<"Pression thermo recalculee (expl) = "<<Pth<<finl;
  Pth = Pth_n + dt/V *(S-.5*(Fn+Fnp1));
  //   Cerr<<"Fnp1="<<Fnp1<<finl;
  //   Cerr<<"Pression thermo recalculee (impl) = "<<Pth<<finl;
  double tmp=0,r;
  int k=0;
  while (fabs(tmp-Pth)/Pth>1e-9 && k++<20)
    {
      tmp = Pth;
      Fnp1 = 0;
      for (face=0 ; face<nb_faces ; face++)
        {
          v = la_zone->volumes_entrelaces(face);
          hnp1 = tab_hnp1(face);
          r = loi_->calculer_masse_volumique(Pth,hnp1);
          bnp1 = r / loi_->Drho_DP(Pth,hnp1);
          Fnp1 += v * bnp1 * divU(face);
        }
      Pth = Pth_n + dt/V *(S-.5*(Fn+Fnp1));
      //     Cerr<<"Fnp1="<<Fnp1<<finl;
      Cerr<<"Pression thermo recalculee (impl"<<k<<") = "<<Pth<<finl;
    }
  return Pth;
}

