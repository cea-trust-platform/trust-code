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
// File:        Champ_Q1NC.cpp
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Q1NC.h>
#include <Zone_VEF.h>
#include <Equation.h>
#include <Debog.h>
#include <Periodique.h>

Implemente_instanciable(Champ_Q1NC,"Champ_Q1NC",Champ_Inc_base);


// printOn

Sortie& Champ_Q1NC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

// readOn

Entree& Champ_Q1NC::readOn(Entree& s)
{
  lire_donnees(s) ;
  return s ;
}

//////////////////////////////////////////////////////////////
//
//   Implementation des fonctions de la classe Champ_Q1NC
//
//////////////////////////////////////////////////////////////

// Fonction compo_normale_sortante()
//
// Cette fonction retourne :
//   1 si le fluide est sortant sur la face num_face
//   0 si la face correspond a une reentree de fluide

const Zone_dis_base& Champ_Q1NC::zone_dis_base() const
{
  return la_zone_VEF.valeur();
}

void Champ_Q1NC::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  la_zone_VEF=ref_cast(Zone_VEF, z_dis);
}

//-Cas CL periodique : assure que les valeurs sur des faces periodiques
// en vis a vis sont identiques. Pour cela on prend la demi somme des deux valeurs.
void Champ_Q1NC::verifie_valeurs_cl()
{
  const Zone_Cl_dis_base& zcl = equation().zone_Cl_dis().valeur();
  int nb_cl = zcl.nb_cond_lim();
  DoubleTab& ch_tab = valeurs();
  int nb_compo = nb_comp();
  int nb_dimension = ch_tab.nb_dim();
  int ndeb,nfin,num_face;

  for (int i=0; i<nb_cl; i++)
    {
      const Cond_lim_base& la_cl = zcl.les_conditions_limites(i).valeur();
      if (sub_type(Periodique,la_cl))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          int voisine;
          double moy;

          if (nb_dimension==1)
            {
              for (num_face=ndeb; num_face<nfin; num_face++)
                {
                  voisine = la_cl_perio.face_associee(num_face-ndeb) + ndeb;
                  if (ch_tab[num_face] != ch_tab[voisine])
                    {
                      moy = 0.5*(ch_tab[num_face] + ch_tab[voisine]);
                      ch_tab[num_face] = moy;
                      ch_tab[voisine] = moy;
                    }
                }
            }
          else
            {
              for (num_face=ndeb; num_face<nfin; num_face++)
                {
                  voisine = la_cl_perio.face_associee(num_face-ndeb) + ndeb;
                  for (int comp=0; comp<nb_compo; comp++)
                    {
                      if (ch_tab(num_face,comp) != ch_tab(voisine,comp))
                        {
                          moy = 0.5*(ch_tab(num_face,comp) + ch_tab(voisine,comp));
                          ch_tab(num_face,comp) = moy;
                          ch_tab(voisine,comp) = moy;
                        }
                    }
                }
            }
        }
    }
  ch_tab.echange_espace_virtuel();
}

int Champ_Q1NC::compo_normale_sortante(int num_face) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  double vit_norm =0;
  for (int ncomp=0; ncomp<nb_comp(); ncomp++)
    vit_norm += (*this)(num_face,ncomp)*zone_VEF.face_normales(num_face,ncomp);
  return (vit_norm > 0);
}

DoubleTab& Champ_Q1NC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps) const
{
  return Champ_Q1NC_impl::trace(fr, valeurs(tps), x);
}

void Champ_Q1NC::cal_rot_ordre1(DoubleTab& vorticite)
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_elem = zone_VEF.nb_elem();

  DoubleTab gradient_elem(0, dimension, dimension);
  // le tableau est initialise dans la methode gradient():
  zone_VEF.zone().creer_tableau_elements(gradient_elem, Array_base::NOCOPY_NOINIT);

  gradient(gradient_elem);
  Debog::verifier("apres calcul gradient",gradient_elem);
  int num_elem;

  switch(dimension)
    {
    case 2 :
      {
        for (num_elem=0; num_elem<nb_elem; num_elem++)
          {
            vorticite(num_elem)=gradient_elem(num_elem,1,0)-gradient_elem(num_elem,0,1);
          }
      }
      break;
    case 3 :
      {
        for (num_elem=0; num_elem<nb_elem; num_elem++)
          {
            vorticite(num_elem,0)=gradient_elem(num_elem,2,1)-gradient_elem(num_elem,1,2);
            vorticite(num_elem,1)=gradient_elem(num_elem,0,2)-gradient_elem(num_elem,2,0);
            vorticite(num_elem,2)=gradient_elem(num_elem,1,0)-gradient_elem(num_elem,0,1);
          }
      }
    }

  Debog::verifier("apres calcul vort",vorticite);
  return ;
}

void Champ_Q1NC::gradient(DoubleTab& gradient_elem)
{
  // Calcul du gradient de la vitesse pour le calcul de la vorticite
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const DoubleTab& vitesse = equation().inconnue().valeurs();

  const DoubleTab& face_normales = zone_VEF.face_normales();
  const int nb_faces = zone_VEF.nb_faces();
  const int nb_elem = zone_VEF.nb_elem();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int premiere_face_int = zone_VEF.premiere_face_int();
  const DoubleVect& volumes = zone_VEF.volumes();

  assert(gradient_elem.dimension(0) == nb_elem);
  assert(gradient_elem.dimension(1) == dimension);
  assert(gradient_elem.dimension(2) == dimension);
  operator_egal(gradient_elem, 0.); // Espace reel uniquement
  int icomp,fac,i,elem1,elem2;

  // Boucle sur les faces

  for (fac=0; fac< premiere_face_int; fac++)
    {
      elem1=face_voisins(fac,0);
      for (icomp=0; icomp<dimension; icomp++)
        for (i=0; i<dimension; i++)
          {
            gradient_elem(elem1, icomp, i) +=
              face_normales(fac,i)*vitesse(fac,icomp);
          }
    }
  for (; fac<nb_faces; fac++)
    {
      elem1=face_voisins(fac,0);
      elem2=face_voisins(fac,1);
      for (icomp=0; icomp<dimension; icomp++)
        for (i=0; i<dimension; i++)
          {
            gradient_elem(elem1, icomp, i) +=
              face_normales(fac,i)*vitesse(fac,icomp);
            if(elem2<nb_elem)
              gradient_elem(elem2, icomp, i) -=
                face_normales(fac,i)*vitesse(fac,icomp);
          }
    }

  tab_divide_any_shape(gradient_elem, volumes);
}

int Champ_Q1NC::fixer_nb_valeurs_nodales(int n)
{
  const MD_Vector& md = zone_vef().md_vector_faces();
  creer_tableau_distribue(md);
  return 1;
}
