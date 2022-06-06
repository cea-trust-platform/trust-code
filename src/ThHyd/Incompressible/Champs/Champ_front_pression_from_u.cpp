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


#include <Champ_front_pression_from_u.h>
#include <TRUSTTrav.h>
#include <Navier_Stokes_std.h>
#include <Zone_VF.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(Champ_front_pression_from_u,"Champ_front_pression_from_u",Ch_front_var_instationnaire_dep);


Sortie& Champ_front_pression_from_u::printOn(Sortie& os) const
{
  return (os);
}


Entree& Champ_front_pression_from_u::readOn(Entree& is)
{
  fixer_nb_comp(1);
  Nom expr;
  is >> expr;
  fonction_.setNbVar(1);
  fonction_.addVar("u_moy");
  fonction_.setString(expr);
  fonction_.parseString();
  return (is);
}



// Description:
// Mise a jour du temps

int Champ_front_pression_from_u::initialiser(double tps, const Champ_Inc_base& inco)
{
  if (!Ch_front_var_instationnaire_dep::initialiser(tps,inco))
    return 0;

  ref_inco_=inco;



  // le champ fornt recupere u moyen par div u .... on verfifie que rho est constant
  int ok=0;
  const Milieu_base& mil=inco.equation().milieu();
  if (sub_type(Fluide_Incompressible,mil))
    {
      if ((sub_type(Champ_Uniforme,mil.masse_volumique().valeur())))
        {



          ok=1;
        }
    }
  if (ok==0)
    {
      Cerr<<que_suis_je()<<" is available only for incompressible problem"<<finl;
      exit();
    }




  mettre_a_jour(tps);


  return 1;
}
void  Champ_front_pression_from_u::mettre_a_jour(double tps)
{
  const Champ_Inc_base& inco=ref_inco_.valeur();
  // on recupere le flux_bord de l'op div c'est donc u
  const Navier_Stokes_std& eqns=ref_cast(Navier_Stokes_std,inco.equation());
  const Operateur_Div& opdiv=eqns.operateur_divergence();
  const Zone_VF& zone_VF = ref_cast(Zone_VF,zone_dis());

  const DoubleTab& Flux0=opdiv.valeur().flux_bords();
  if (Flux0.size()==0)
    {
      Cerr<<" Div pas encore pres...."<<finl;
      return;
      //   DoubleTrav resu(eqns.div());
      //opdiv.calculer(inco.valeurs(),resu);
    }
  const DoubleTab& Flux=opdiv.valeur().flux_bords();
  const Front_VF& le_bord= ref_cast(Front_VF,frontiere_dis());
  int premiere=le_bord.num_premiere_face();
  double surf=0;
  double fluxb=0;
  for(int i=0; i<le_bord.nb_faces(); i++)
    {
      int face=premiere+i;
      surf+=zone_VF.face_surfaces(face);
      fluxb+=Flux(face,0);
    }
  surf=mp_sum(surf);
  fluxb=mp_sum(fluxb);
  double u_moy=fluxb/surf;


  fonction_.setVar(0,u_moy);
  double Pout=fonction_.eval();


  Cout << frontiere_dis().le_nom()<<" time "<<  tps <<" u_moy "<< u_moy<<" Pout "<<Pout <<finl;
  int nb_cases=les_valeurs->nb_cases();


  for (int t=0; t<nb_cases; t++)
    {
      DoubleTab& tab=les_valeurs[t].valeurs();
      tab=Pout;
    }

  // exit();
}


