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

#include <Champ_front_MED.h>
#include <Domaine.h>
#include <Frontiere_dis_base.h>
#include <Param.h>

Implemente_instanciable(Champ_front_MED,"Champ_front_MED",Champ_front_txyz);
// XD champ_front_MED front_field_base champ_front_MED 0 Field allowing the loading of a boundary condition from a MED file using Champ_fonc_med

Sortie& Champ_front_MED::printOn(Sortie& os) const
{
  assert(0);
  return os;
}

Entree& Champ_front_MED::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("champ_volumique",&champ_volumique_,Param::REQUIRED);  // XD attr champ_fonc_med field_base champ_fonc_med 0 a champ_fonc_med loading the values of the unknown on a domain boundary
  param.lire_sans_accolade(is);

  fixer_nb_comp(champ_volumique_.valeur().nb_comp());
  pos_.resize(1,dimension);
  val_.resize(1,nb_comp());
  return is;
}

void Champ_front_MED::remplir_tab(DoubleTab& tab,double temps) const
{
  int dim=tab.dimension(1);
  const Frontiere_dis_base& fr_dis=frontiere_dis();
  const Frontiere& frontiere=fr_dis.frontiere();
  const Zone& zone=frontiere.zone();
  const Domaine& domaine=zone.domaine();
  const Faces& faces=frontiere.faces();
  int nb_faces=frontiere.nb_faces();
  double x,y,z;
  int nbsf=faces.nb_som_faces();
  int i,k;

  for( i=0; i<nb_faces; i++)
    {
      x=y=z=0;
      for( k=0; k<nbsf; k++)
        {
          x+=domaine.coord(faces.sommet(i,k),0);
          if(dimension>1)
            y+=domaine.coord(faces.sommet(i,k),1);
          if(dimension>2)
            z+=domaine.coord(faces.sommet(i,k),2);
        }
      x/=nbsf;
      y/=nbsf;
      z/=nbsf;
      for( k=0; k<dim; k++)
        {
          tab(i,k)=valeur_au_temps_et_au_point(temps,-1,x,y,z,k);
        }
    }
  tab.echange_espace_virtuel();
}
Champ_front_base& Champ_front_MED::affecter_(const Champ_front_base& ch)
{
  double temps=0;
  DoubleTab& tab=valeurs_au_temps(temps);
  remplir_tab(tab,temps);
  return *this;
}

void Champ_front_MED::mettre_a_jour(double temps)
{
  champ_volumique_->mettre_a_jour(temps);
  DoubleTab& tab=valeurs_au_temps(temps);
  remplir_tab(tab,temps);
//  Cerr<<" VIN MAX" <<mp_max_abs_vect(valeurs_au_temps(temps))<<finl;
  return;
}
double Champ_front_MED::valeur_au_temps_et_au_point(double temps,int som,double x,double y, double z,int comp) const
{
  pos_(0,0)=x;
  pos_(0,1)=y;
  if (dimension>2)
    pos_(0,2)=z;
  champ_volumique_.valeur().valeur_aux(pos_,val_);
  return val_(0,comp);
}
