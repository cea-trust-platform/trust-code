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
// File:        Champ_Q1NC_impl.cpp
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Q1NC_impl.h>
#include <Domaine.h>
#include <Zone_VEF.h>
#include <Champ_Inc_base.h>
#include <verif_cast.h>

DoubleTab& Champ_Q1NC_impl::Derivee_fonction_forme_2D_normalise(double u, double v, DoubleTab& DF)
{
  DF(0,0) = -0.5 + 0.5*u;
  DF(0,1) =      - 0.5*u;
  DF(0,2) =  0.5 + 0.5*u;
  DF(0,3) =      - 0.5*u;

  DF(1,0) =      - 0.5*v;
  DF(1,1) = -0.5 + 0.5*v;
  DF(1,2) =      - 0.5*v;
  DF(1,3) =  0.5 + 0.5*v;

  return DF;

}



DoubleTab& Champ_Q1NC_impl::Derivee_fonction_forme_3D_normalise(double u, double v, double w, DoubleTab& DF)
{
  double tier=1./3.;
  DF(0,0) = -0.5 + 2*tier*u;
  DF(0,1) =      - tier*u;
  DF(0,2) =      - tier*u;
  DF(0,3) =  0.5 + 2*tier*u;
  DF(0,4) =      - tier*u;
  DF(0,5) =      - tier*u;

  DF(1,0) =      - tier*v;
  DF(1,1) = -0.5 + 2*tier*v;
  DF(1,2) =      - tier*v;
  DF(1,3) =      - tier*v;
  DF(1,4) =  0.5 + 2*tier*v;
  DF(1,5) =      - tier*v;

  DF(2,0) =      - tier*w;
  DF(2,1) =      - tier*w;
  DF(2,2) = -0.5 + 2*tier*w;
  DF(2,3) =      - tier*w;
  DF(2,4) =      - tier*w;
  DF(2,5) = 0.5  + 2*tier*w;

  return DF;

}

DoubleVect& Champ_Q1NC_impl::valeur_a_elem(const DoubleVect& position,
                                           DoubleVect& val,
                                           int le_poly) const
{
  const Champ_base& cha=le_champ();
  // Cerr << "Champ_Q1NC_impl::valeur_a_elem  " <<  finl;
  int nb_compo_=cha.nb_comp();
  double xs,ys,zs;
  int face;
  const DoubleTab& ch = cha.valeurs();
  const Zone_VEF& zone_VEF = zone_vef();
  int init=tab_param.size();
  if (init == 0)
    {
      if (Objet_U::dimension == 2)
        {
          (verif_cast(Champ_Q1NC_impl&, *this)).transforme_coord2D();
        }
      if (Objet_U::dimension == 3)
        {
          (verif_cast(Champ_Q1NC_impl&, *this)).transforme_coord3D();
        }
    }
  val=0;
  if (le_poly != -1)
    {
      if (Objet_U::dimension == 2)
        {
          xs = position(0);
          ys = position(1);
          for (int i=0; i< 4; i++)
            {
              face = zone_VEF.elem_faces(le_poly,i);
              for(int ncomp=0; ncomp<nb_compo_; ncomp++)
                {
                  val(ncomp) += ch(face, ncomp)
                                * (verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_2D(xs,ys,le_poly,i);
                }
            }
        }
      else if (Objet_U::dimension == 3)
        {
          xs = position(0);
          ys = position(1);
          zs = position(2);
          for (int i=0; i< 6; i++)
            {
              for(int ncomp=0; ncomp<nb_compo_; ncomp++)
                {
                  face = zone_VEF.elem_faces(le_poly,i);
                  val += ch(face, ncomp)
                         * (verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_3D(xs,ys,zs,le_poly,i);
                }
            }
        }
    }
  //Cerr << "val de Champ_Q1NC_impl::valeur_a_elem " << cha.que_suis_je() << finl;
  return val;
}

double Champ_Q1NC_impl::calcule_valeur_a_elem_compo(double xs, double ys, double zs, int le_poly, int ncomp) const
{
  const Zone_VEF& zone_VEF = zone_vef();
  const DoubleTab& ch = le_champ().valeurs();
  int face,init=tab_param.size();
  if (init == 0)
    {
      if (Objet_U::dimension == 2)
        {
          (verif_cast(Champ_Q1NC_impl&, *this)).transforme_coord2D();
        }
      if (Objet_U::dimension == 3)
        {
          (verif_cast(Champ_Q1NC_impl&, *this)).transforme_coord3D();
        }
    }

  double val;
  if (le_poly == -1)
    {
      val = 0;
    }
  else if(ch.nb_dim()!=1)
    {
      // Calcul d'apres les fonctions de forme sur le quadrangle ou son extension en 3D
      val = 0;
      if (Objet_U::dimension == 2)
        {
          for (int i=0; i< 4; i++)
            {
              face = zone_VEF.elem_faces(le_poly,i);
              val += ch(face, ncomp)*(verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_2D(xs,ys,le_poly,i);
            }
        }
      else if (Objet_U::dimension == 3)
        {
          for (int i=0; i< 6; i++)
            {
              face = zone_VEF.elem_faces(le_poly,i);
              val += ch(face, ncomp)*(verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_3D(xs,ys,zs,le_poly,i);
            }
        }
    }
  else
    {
      // Calcul d'apres les fonctions de forme sur le quadrangle ou son extension en 3D
      assert(ncomp==0);
      val = 0;
      if (Objet_U::dimension == 2)
        {
          for (int i=0; i< 4; i++)
            {
              face = zone_VEF.elem_faces(le_poly,i);
              val += ch(face)*(verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_2D(xs,ys,le_poly,i);
            }
        }
      else if (Objet_U::dimension == 3)
        {
          for (int i=0; i< 6; i++)
            {
              face = zone_VEF.elem_faces(le_poly,i);
              val += ch(face)
                     * (verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_3D(xs,ys,zs,le_poly,i);
            }
        }
    }
  return val;
}

double Champ_Q1NC_impl::valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const
{
  // Cerr << "Champ_Q1NC_impl::valeur_a_elem_compo " << finl;
  double xs,ys,zs=0;
  xs = position(0);
  ys = position(1);
  if (Objet_U::dimension == 3)
    zs = position(2);
  return calcule_valeur_a_elem_compo(xs,ys,zs,le_poly,ncomp);
}

double Champ_Q1NC_impl::valeur_a_sommet_compo(int num_som, int le_poly, int ncomp) const
{
  //Cerr << "Champ_Q1NC_impl::valeur_a_sommet_compo" << finl;
  // Contrairement au Champ_P1NC les fonctions de forme
  // ne sont pas triviales au sommet des hexas... On repasse
  // par calcule_valeur_a_elem_compo
  double xs,ys,zs=0;
  const Domaine& dom=zone().domaine();
  xs = dom.coord(num_som,0);
  ys = dom.coord(num_som,1);
  if (Objet_U::dimension == 3)
    zs = dom.coord(num_som,2);
  return calcule_valeur_a_elem_compo(xs,ys,zs,le_poly,ncomp);
}

DoubleTab& Champ_Q1NC_impl::valeur_aux_elems(const DoubleTab& positions,
                                             const IntVect& les_polys,
                                             DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  // Cerr << "Champ_Q1NC_impl::valeur_aux_elems" << finl;

  int nb_compo_=cha.nb_comp();
  int face;
  double xs,ys,zs;

  const Zone_VEF& zone_VEF = zone_vef();

  int init=tab_param.size();
  if (init == 0)
    {
      if (Objet_U::dimension == 2)
        {
          (verif_cast(Champ_Q1NC_impl&, *this)).transforme_coord2D();
        }
      if (Objet_U::dimension == 3)
        {
          (verif_cast(Champ_Q1NC_impl&, *this)).transforme_coord3D();
        }
    }

  if (val.nb_dim() == 1)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(nb_compo_ == 1);
    }
  else if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(val.dimension(1) == nb_compo_);
    }
  else
    {
      Cerr << "Erreur TRUST dans Champ_Q1NC_impl::valeur_aux_elems()\n";
      Cerr << "Le DoubleTab val a plus de 2 entrees\n";
      Process::exit();
    }

  int le_poly;

  const DoubleTab& ch = cha.valeurs();
  // Cerr << "ch " << cha.valeurs() << finl;
  if (nb_compo_ == 1)
    {
      if ((ch.nb_dim() == 1) && (val.nb_dim() == 1))
        {
          for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
            {
              le_poly=les_polys(rang_poly);
              if (le_poly == -1)
                val(rang_poly) = 0;
              else
                {
                  // Calcul d'apres les fonctions de forme sur le quadrangle
                  val(rang_poly) = 0;
                  if (Objet_U::dimension == 2)
                    {
                      xs = positions(rang_poly,0);
                      ys = positions(rang_poly,1);
                      for (int i=0; i< 4; i++)
                        {
                          face = zone_VEF.elem_faces(le_poly,i);
                          val(rang_poly) += ch(face)*(verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_2D(xs,ys,le_poly,i);
                        }
                    }
                  else if (Objet_U::dimension == 3)
                    {
                      xs = positions(rang_poly,0);
                      ys = positions(rang_poly,1);
                      zs = positions(rang_poly,2);
                      for (int i=0; i< 6; i++)
                        {
                          face = zone_VEF.elem_faces(le_poly,i);
                          val(rang_poly) += ch(face)
                                            * (verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_3D(xs,ys,zs,le_poly,i);
                        }
                    }
                }
            }
        }
      else if ((ch.nb_dim() == 1) && (val.nb_dim() != 1))
        {
          for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
            {
              le_poly=les_polys(rang_poly);
              if (le_poly == -1)
                val(rang_poly,0) = 0;
              else
                {
                  // Calcul d'apres les fonctions de forme sur le quadrangle
                  val(rang_poly,0) = 0;
                  if (Objet_U::dimension == 2)
                    {
                      xs = positions(rang_poly,0);
                      ys = positions(rang_poly,1);
                      for (int i=0; i< 4; i++)
                        {
                          face = zone_VEF.elem_faces(le_poly,i);

                          val(rang_poly,0) += ch(face)* (verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_2D(xs,ys,le_poly,i);
                        }
                    }
                  else if (Objet_U::dimension == 3)
                    {
                      xs = positions(rang_poly,0);
                      ys = positions(rang_poly,1);
                      zs = positions(rang_poly,2);
                      for (int i=0; i< 6; i++)
                        {
                          face = zone_VEF.elem_faces(le_poly,i);
                          val(rang_poly,0) += ch(face)
                                              * (verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_3D(xs,ys,zs,le_poly,i);
                        }
                    }
                }
            }
        }
      else if ((ch.nb_dim() != 1) && (val.nb_dim() == 1))
        {
          for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
            {
              le_poly=les_polys(rang_poly);
              if (le_poly == -1)
                val(rang_poly) = 0;
              else
                {
                  // Calcul d'apres les fonctions de forme sur le quadrangle
                  val(rang_poly) = 0;
                  if (Objet_U::dimension == 2)
                    {
                      xs = positions(rang_poly,0);
                      ys = positions(rang_poly,1);
                      for (int i=0; i< 4; i++)
                        {
                          face = zone_VEF.elem_faces(le_poly,i);

                          val(rang_poly) += ch(face,0)*(verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_2D(xs,ys,le_poly,i);
                        }
                    }
                  else if (Objet_U::dimension == 3)
                    {
                      xs = positions(rang_poly,0);
                      ys = positions(rang_poly,1);
                      zs = positions(rang_poly,2);
                      for (int i=0; i< 6; i++)
                        {
                          face = zone_VEF.elem_faces(le_poly,i);
                          val(rang_poly) += ch(face,0)
                                            * (verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_3D(xs,ys,zs,le_poly,i);
                        }
                    }
                }
            }
        }
    }
  else // nb_compo_ > 1
    {

      for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
        {
          le_poly=les_polys(rang_poly);
          if (le_poly == -1)
            for(int ncomp=0; ncomp<nb_compo_; ncomp++)
              val(rang_poly, ncomp) = 0;
          else
            {
              for(int ncomp=0; ncomp<nb_compo_; ncomp++)
                {
                  val(rang_poly, ncomp) = 0;
                  if (Objet_U::dimension == 2)
                    {
                      xs = positions(rang_poly,0);
                      ys = positions(rang_poly,1);
                      for (int i=0; i< 4; i++)
                        {
                          face = zone_VEF.elem_faces(le_poly,i);

                          val(rang_poly, ncomp) += ch(face, ncomp)*(verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_2D(xs,ys,le_poly,i);
                        }
                    }
                  else if (Objet_U::dimension == 3)
                    {
                      xs = positions(rang_poly,0);
                      ys = positions(rang_poly,1);
                      zs = positions(rang_poly,2);
                      for (int i=0; i< 6; i++)
                        {
                          face = zone_VEF.elem_faces(le_poly,i);
                          val(rang_poly, ncomp) += ch(face, ncomp)
                                                   * (verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_3D(xs,ys,zs,le_poly,i);
                        }
                    }
                }
            }
        }
    }
  // Cerr << "valeur " << val << finl;
  return val;
}


DoubleVect& Champ_Q1NC_impl::valeur_aux_elems_compo(const DoubleTab& positions,
                                                    const IntVect& les_polys,
                                                    DoubleVect& val,int ncomp) const
{
  // Cerr << "Champ_Q1NC_impl::valeur_aux_elems_compo " << finl;
  const Champ_base& cha=le_champ();
  double xs,ys,zs;
  int face;
  const Zone_VEF& zone_VEF = zone_vef();
  assert(val.size() == les_polys.size());
  int le_poly;

  const DoubleTab& ch = cha.valeurs();

  int init=tab_param.size();
  if (init == 0)
    {
      if (Objet_U::dimension == 2)
        {
          (verif_cast(Champ_Q1NC_impl&, *this)).transforme_coord2D();
        }
      if (Objet_U::dimension == 3)
        {
          (verif_cast(Champ_Q1NC_impl&, *this)).transforme_coord3D();
        }
    }

  for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
    {
      le_poly=les_polys(rang_poly);
      if (le_poly == -1)
        val(rang_poly) = 0;
      else
        {
          // Calcul d'apres les fonctions de forme sur le quadrangle ou son extension en 3D
          val = 0;
          if (Objet_U::dimension == 2)
            {
              xs = positions(rang_poly,0);
              ys = positions(rang_poly,1);
              for (int i=0; i< 4; i++)
                {
                  face = zone_VEF.elem_faces(le_poly,i);
                  val(rang_poly) += ch(face, ncomp)*(verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_2D(xs,ys,le_poly,i);
                }
            }
          else if (Objet_U::dimension == 3)
            {
              xs = positions(rang_poly,0);
              ys = positions(rang_poly,1);
              zs = positions(rang_poly,2);
              for (int i=0; i< 6; i++)
                {
                  face = zone_VEF.elem_faces(le_poly,i);
                  val(rang_poly) += ch(face, ncomp)*(verif_cast(Champ_Q1NC_impl&, *this)).fonction_forme_3D(xs,ys,zs,le_poly,i);
                }
            }
        }
    }

  return val;
}

DoubleTab& Champ_Q1NC_impl::valeur_aux_sommets(const Domaine& dom,
                                               DoubleTab& ch_som) const
{
  // Cerr << "Champ_Q1NC_impl::valeur_aux_sommets " << finl;
  const Zone_dis_base& zone_dis = zone_dis_base();
  const Zone& ma_zone = zone_dis.zone();
  int nb_elem_tot = ma_zone.nb_elem_tot();
  int nb_som = ma_zone.nb_som();
  int nb_som_elem = ma_zone.nb_som_elem();
  const Champ_base& cha=le_champ();
  int nb_compo_=cha.nb_comp();
  IntVect compteur(nb_som);
  int ncomp;
  int num_elem,num_som,j;
  ch_som = 0;
  compteur = 0;

  //   if (nb_compo_ == 1)
  //     {
  //     }
  //   else // (nb_compo_ != 1)
  {
    DoubleVect position(Objet_U::dimension);
    for (num_elem=0; num_elem<nb_elem_tot; num_elem++)
      for (j=0; j<nb_som_elem; j++)
        {
          num_som = ma_zone.sommet_elem(num_elem,j);
          //        Cerr << "num_som " << num_som << finl;
          for(int k=0; k<Objet_U::dimension; k++)
            position(k)=dom.coord(num_som,k);
          //        Cerr << "position " << position << finl;
          if(num_som < nb_som)
            {
              compteur[num_som]++;
              for (ncomp=0; ncomp<nb_compo_; ncomp++)
                {
                  ch_som(num_som,ncomp) += valeur_a_elem_compo(position,num_elem,ncomp);
                  //   Cerr << "ch_som " <<  ch_som(num_som,ncomp)  << finl;
                }
            }
        }

    for (num_som=0; num_som<nb_som; num_som++)
      for (ncomp=0; ncomp<nb_compo_; ncomp++)
        ch_som(num_som,ncomp) /= compteur[num_som];
  }
  //Cerr << "ch_som " <<  ch_som << finl;

  return ch_som;
}
DoubleVect& Champ_Q1NC_impl::valeur_aux_sommets_compo(const Domaine& dom,
                                                      DoubleVect& ch_som,
                                                      int ncomp) const
{
  // Cerr << "Champ_Q1NC_impl::valeur_aux_sommets_compo " << finl;
  const Zone_dis_base& zone_dis = zone_dis_base();
  const Zone& ma_zone = zone_dis.zone();

  int nb_elem_tot = ma_zone.nb_elem_tot();
  int nb_som = ma_zone.nb_som();
  int nb_som_elem = ma_zone.nb_som_elem();
  IntVect compteur(nb_som);
  int num_elem,num_som,j;
  ch_som = 0;
  compteur = 0;

  {
    DoubleVect position(Objet_U::dimension);
    for (num_elem=0; num_elem<nb_elem_tot; num_elem++)
      for (j=0; j<nb_som_elem; j++)
        {
          num_som = ma_zone.sommet_elem(num_elem,j);
          for(int k=0; k<Objet_U::dimension; k++)
            position(k)=dom.coord(num_som,k);

          if(num_som < nb_som)
            {
              compteur[num_som]++;
              ch_som(num_som) +=  valeur_a_elem_compo(position,num_elem,ncomp);
            }
        }
    for (num_som=0; num_som<nb_som; num_som++)
      ch_som(num_som) /= compteur[num_som];
  }
  // if (nb_compo_ == 1)
  //     {
  //       DoubleVect position(Objet_U::dimension);
  //       for (num_elem=0; num_elem<nb_elem_tot; num_elem++)
  //         for (j=0; j<nb_som_elem; j++) {
  //           num_som = zone.sommet_elem(num_elem,j);
  //           for(int k=0; k<Objet_U::dimension; k++);
  //           position(k)=dom.coord(num_som,k);
  //           if(num_som < nb_som) {
  //             compteur[num_som]++;
  //             ch_som(num_som) += valeur_a_elem_compo(position,num_elem,ncomp);
  //           }
  //         }

  //       for (num_som=0; num_som<nb_som; num_som++)
  //         ch_som(num_som)/= compteur[num_som];
  //     }
  //   else // (nb_compo_ != 1)
  //     {
  //       for (num_elem=0; num_elem<nb_elem_tot; num_elem++)
  //         for (j=0; j<nb_som_elem; j++) {
  //           num_som = zone.sommet_elem(num_elem,j);
  //           if(num_som < nb_som) {
  //             compteur[num_som]++;
  //             ch_som(num_som) += ch(num_elem,ncomp);
  //           }
  //         }

  //       for (num_som=0; num_som<nb_som; num_som++)
  //         ch_som(num_som) /= compteur[num_som];
  //     }
  return ch_som;
}

DoubleTab& Champ_Q1NC_impl::remplir_coord_noeuds(DoubleTab& noeuds) const
{
  const Zone_VEF& zvef = zone_vef();
  const DoubleTab& xv = zvef.xv();
  int nb_fac = zvef.nb_faces();
  if ( (xv.dimension(0) == nb_fac ) && (xv.dimension(1) == Objet_U::dimension) )
    noeuds.ref(xv);
  else
    {
      Cerr << "Erreur dans Champ_Q1NC_impl::remplir_coord_noeuds()" << finl;
      Cerr << "Les centres de gravite des faces n'ont pas ete calcules" << finl;
      Process::exit();
    }
  return noeuds;
}


int Champ_Q1NC_impl::remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                                   IntVect& polys) const
{
  remplir_coord_noeuds(positions);
  const Zone_VEF& zvef=zone_vef();
  const IntTab& face_voisins=zvef.face_voisins();
  int nb_faces=zvef.nb_faces();
  polys.resize(nb_faces);

  for(int i= 0; i< nb_faces; i++)
    {
      polys(i)=face_voisins(i, 0);
    }
  return 1;
}

//
// cree un tableau des parametres geometrique
// (tab_param) au debut du calcul pour le fonction_form
// (vois les inlines dans Champ_Q1NC_impl.h)
//
void Champ_Q1NC_impl::transforme_coord2D()
{
  //  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Zone_VEF& zone_VEF = zone_vef();
  const Zone& zone_geom = zone();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntTab& sommet_face = zone_VEF.face_sommets();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const Domaine& dom=zone_geom.domaine();
  const DoubleTab& coords=dom.les_sommets();
  double lec0, lec1, alpha, beta;
  int i,poly,som0,som1;
  //
  //
  tab_param.resize(nb_elem_tot,6);
  int Nb_noeud_elem = 4;
  DoubleVect le_coord0(Nb_noeud_elem), le_coord1(Nb_noeud_elem);
  for (poly=0; poly<nb_elem_tot; poly++)
    {
      for ( i = 0; i < Nb_noeud_elem; i++ )
        {
          som0 = sommet_face(elem_faces(poly,i),0);
          som1 = sommet_face(elem_faces(poly,i),1);
          le_coord0(i)=0.5*(coords(som0,0)+coords(som1,0));
          le_coord1(i)=0.5*(coords(som0,1)+coords(som1,1));
        }
      //
      // calcul de ksi
      //
      // ksi = a_ksi x + b_ksi y + c_ksi
      // ksi = [ (x2-x0) x + (y2-y0) y ] alpha + beta
      // a_ksi = alpha (x2-x0)
      // b_ksi = alpha (y2-y0)
      // c_ksi = beta
      //
      // ou ksi(x0,y0) = -1     eqn(1)
      // et ksi(x2,y2) = 1      eqn(2)
      //
      // donc (2)-(1) donne
      // alpha = 2/[ (x2-x0)x2 - (x2-x0)x0 + (y2-y0)y2 - (y2-y0)y0 ]
      // beta  = 1 - [(x2-x0)x2 + (y2-y0)y2] alpha
      //
      lec0=carre(le_coord0(2))+carre(le_coord0(0))-2.*le_coord0(2)*le_coord0(0);
      lec1=carre(le_coord1(2))+carre(le_coord1(0))-2.*le_coord1(2)*le_coord1(0);
      alpha=2./(lec0+lec1);
      lec0=(le_coord0(2)-le_coord0(0))*le_coord0(2);
      lec1=(le_coord1(2)-le_coord1(0))*le_coord1(2);
      beta=1.-(lec0+lec1)*alpha;
      //
      tab_param(poly,0)=alpha*(le_coord0(2)-le_coord0(0));
      tab_param(poly,1)=alpha*(le_coord1(2)-le_coord1(0));
      tab_param(poly,2)=beta;
      //
      // calcul de eta
      //
      lec0=carre(le_coord0(3))+carre(le_coord0(1))-2.*le_coord0(3)*le_coord0(1);
      lec1=carre(le_coord1(3))+carre(le_coord1(1))-2.*le_coord1(3)*le_coord1(1);
      alpha=2./(lec0+lec1);
      lec0=(le_coord0(3)-le_coord0(1))*le_coord0(3);
      lec1=(le_coord1(3)-le_coord1(1))*le_coord1(3);
      beta=1.-(lec0+lec1)*alpha;
      //
      tab_param(poly,3)=alpha*(le_coord0(3)-le_coord0(1));
      tab_param(poly,4)=alpha*(le_coord1(3)-le_coord1(1));
      tab_param(poly,5)=beta;
    }
}

void Champ_Q1NC_impl::transforme_coord3D()
{
  const Zone_VEF& zone_VEF = zone_vef();
  const Zone& zone_geom = zone();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntTab& sommet_face = zone_VEF.face_sommets();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const Domaine& dom=zone_geom.domaine();
  const DoubleTab& coords=dom.les_sommets();
  double lec0, lec1, lec2;
  double alpha, beta;
  int i, poly;
  int som0, som1, som2, som3;
  tab_param.resize(nb_elem_tot,12);
  //
  //
  int Nb_noeud_elem = 6;
  DoubleVect le_coord0(Nb_noeud_elem), le_coord1(Nb_noeud_elem), le_coord2(Nb_noeud_elem);
  for (poly=0; poly<nb_elem_tot; poly++)
    {
      for ( i = 0; i < Nb_noeud_elem; i++ )
        {
          som0 = sommet_face(elem_faces(poly,i),0);
          som1 = sommet_face(elem_faces(poly,i),1);
          som2 = sommet_face(elem_faces(poly,i),2);
          som3 = sommet_face(elem_faces(poly,i),3);
          le_coord0(i)=0.25*(coords(som0,0)+coords(som1,0)+coords(som2,0)+coords(som3,0));
          le_coord1(i)=0.25*(coords(som0,1)+coords(som1,1)+coords(som2,1)+coords(som3,1));
          le_coord2(i)=0.25*(coords(som0,2)+coords(som1,2)+coords(som2,2)+coords(som3,2));
        }
      //
      // calcul de ksi
      //
      lec0=carre(le_coord0(3))+carre(le_coord0(0))-2.*le_coord0(3)*le_coord0(0);
      lec1=carre(le_coord1(3))+carre(le_coord1(0))-2.*le_coord1(3)*le_coord1(0);
      lec2=carre(le_coord2(3))+carre(le_coord2(0))-2.*le_coord2(3)*le_coord2(0);
      alpha=2./(lec0+lec1+lec2);
      lec0=(le_coord0(3)-le_coord0(0))*le_coord0(3);
      lec1=(le_coord1(3)-le_coord1(0))*le_coord1(3);
      lec2=(le_coord2(3)-le_coord2(0))*le_coord2(3);
      beta=1.-(lec0+lec1+lec2)*alpha;
      //
      tab_param(poly,0)=alpha*(le_coord0(3)-le_coord0(0));
      tab_param(poly,1)=alpha*(le_coord1(3)-le_coord1(0));
      tab_param(poly,2)=alpha*(le_coord2(3)-le_coord2(0));
      tab_param(poly,3)=beta;
      //
      // calcul de eta
      //
      lec0=carre(le_coord0(4))+carre(le_coord0(1))-2.*le_coord0(4)*le_coord0(1);
      lec1=carre(le_coord1(4))+carre(le_coord1(1))-2.*le_coord1(4)*le_coord1(1);
      lec2=carre(le_coord2(4))+carre(le_coord2(1))-2.*le_coord2(4)*le_coord2(1);
      alpha=2./(lec0+lec1+lec2);
      lec0=(le_coord0(4)-le_coord0(1))*le_coord0(4);
      lec1=(le_coord1(4)-le_coord1(1))*le_coord1(4);
      lec2=(le_coord2(4)-le_coord2(1))*le_coord2(4);
      beta=1.-(lec0+lec1+lec2)*alpha;
      //
      tab_param(poly,4)=alpha*(le_coord0(4)-le_coord0(1));
      tab_param(poly,5)=alpha*(le_coord1(4)-le_coord1(1));
      tab_param(poly,6)=alpha*(le_coord2(4)-le_coord2(1));
      tab_param(poly,7)=beta;
      //
      // calcul de psi
      //
      lec0=carre(le_coord0(5))+carre(le_coord0(2))-2.*le_coord0(5)*le_coord0(2);
      lec1=carre(le_coord1(5))+carre(le_coord1(2))-2.*le_coord1(5)*le_coord1(2);
      lec2=carre(le_coord2(5))+carre(le_coord2(2))-2.*le_coord2(5)*le_coord2(2);
      alpha=2./(lec0+lec1+lec2);
      lec0=(le_coord0(5)-le_coord0(2))*le_coord0(5);
      lec1=(le_coord1(5)-le_coord1(2))*le_coord1(5);
      lec2=(le_coord2(5)-le_coord2(2))*le_coord2(5);
      beta=1.-(lec0+lec1+lec2)*alpha;
      //
      tab_param(poly,8)=alpha*(le_coord0(5)-le_coord0(2));
      tab_param(poly,9)=alpha*(le_coord1(5)-le_coord1(2));
      tab_param(poly,10)=alpha*(le_coord2(5)-le_coord2(2));
      tab_param(poly,11)=beta;
    }
}

