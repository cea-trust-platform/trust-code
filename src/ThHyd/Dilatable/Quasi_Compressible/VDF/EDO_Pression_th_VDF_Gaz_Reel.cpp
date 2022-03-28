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
// File:        EDO_Pression_th_VDF_Gaz_Reel.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/VDF
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <EDO_Pression_th_VDF_Gaz_Reel.h>
#include <Fluide_Quasi_Compressible.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Schema_Temps_base.h>
#include <Neumann_sortie_libre.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps.h>
#include <TRUSTTrav.h>

Implemente_instanciable(EDO_Pression_th_VDF_Gaz_Reel,"EDO_Pression_th_VDF_Gaz_Reel",EDO_Pression_th_VDF);


// Description:
//    Imprime la loi sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& EDO_Pression_th_VDF_Gaz_Reel::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

// Description:
//    Lecture d'une EDO sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree pour la lecture des parametres
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& EDO_Pression_th_VDF_Gaz_Reel::readOn(Entree& is)
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


double EDO_Pression_th_VDF_Gaz_Reel::resoudre(double Pth_n)
{
  int n_bord ;
  for (n_bord=0; n_bord<la_zone->nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl->les_conditions_limites(n_bord);
      if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
        return Pth_n;
    }

  double Pth;
  const DoubleTab& tab_vit = ref_cast(Navier_Stokes_std,le_fluide_->vitesse()->equation()).vitesse()->valeurs();
  const DoubleTab& tab_hnp1 = le_fluide_->inco_chaleur().valeurs();       //actuel
  const DoubleTab& tab_hn = le_fluide_->inco_chaleur().passe();        //passe
  const DoubleTab& tab_rho = le_fluide_->masse_volumique().valeurs();    //actuel
  const Loi_Etat& loi_ = le_fluide_->loi_etat();
  //const DoubleVect& tab_rhon = loi_->rho_n();                       //passe

  int elem, nb_elem=la_zone->nb_elem(),i;
  double V = 0; //mesure du domaine
  double Fn = 0; //integrale 1 a l'etape n
  double Fnp1 = 0; //integrale 1 a l'etape n+1
  double S = 0; //second membre

  double dt = le_fluide_->vitesse()->equation().schema_temps().pas_de_temps();
  double v,al,b, bnp1, hn,hnp1, divu;

  const IntTab& elem_faces = la_zone->elem_faces();
  DoubleTrav divU(tab_vit.dimension(0), 1);
  ref_cast(Navier_Stokes_std,le_fluide_->vitesse()->equation()).operateur_divergence().calculer(tab_vit,divU);
  DoubleTrav gradh(tab_vit.dimension(0));
  DoubleTrav Hstar(tab_vit.dimension(0));
  for (elem=0 ; elem<nb_elem ; elem++)
    {
      Hstar(elem) = .5*(tab_hn(elem)+tab_hnp1(elem));
    }
  calculer_grad(Hstar,gradh);
  DoubleTab u_gradh(nb_elem);
  int f1,f2;
  for (elem=0 ; elem<nb_elem ; elem++)
    {
      u_gradh(elem)=0;
      for (i=0 ; i<dimension ; i++)
        {
          f1 = elem_faces(elem,i);
          f2 = elem_faces(elem,i+dimension);
          u_gradh(elem) += .25* (gradh(f1)+gradh(f2)) * (tab_vit(f1)+tab_vit(f2));
        }
    }

  for (elem=0 ; elem<nb_elem ; elem++)
    {
      v = la_zone->volumes(elem);
      V += v;
      hn   = tab_hn(elem);
      hnp1 = tab_hnp1(elem);
      al = loi_->Drho_DT(Pth_n,hn) / loi_->Drho_DP(Pth_n,hn);
      b = tab_rho(elem) / loi_->Drho_DP(Pth_n,hn);
      bnp1 = tab_rho(elem) / loi_->Drho_DP(Pth_n,hnp1);
      //S += al.dh/dt
      S -= v*al * ((hnp1-hn)/dt);
      divu = divU(elem);
      //S -= al.u.gradT*
      S -= v*al * u_gradh(elem);
      //F += b.div(U)
      Fn += v*b * divu;
      Fnp1 += v*bnp1 * divu;
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
  while (std::fabs(tmp-Pth)/Pth>1e-9 && k++<20)
    {
      tmp = Pth;
      Fnp1 = 0;
      for (elem=0 ; elem<nb_elem ; elem++)
        {
          v = la_zone->volumes(elem);
          hnp1 = tab_hnp1(elem);
          r = loi_->calculer_masse_volumique(Pth,hnp1);
          bnp1 = r / loi_->Drho_DP(Pth,hnp1);
          for (i=0 ; i<dimension ; i++)
            {
              Fnp1 += v*bnp1 * (tab_vit(elem_faces(elem,i+dimension))-tab_vit(elem_faces(elem,i)))/la_zone->dim_elem(elem,i);
            }
        }
      Pth = Pth_n + dt/V *(S-.5*(Fn+Fnp1));
      //     Cerr<<"Fnp1="<<Fnp1<<finl;
      Cerr<<"Pression thermo recalculee (impl"<<k<<") = "<<Pth<<finl;
    }

  return Pth;
}


