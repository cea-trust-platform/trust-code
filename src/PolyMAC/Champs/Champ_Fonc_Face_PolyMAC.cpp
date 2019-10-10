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
// File:        Champ_Fonc_Face_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_Face_PolyMAC.h>
#include <Domaine.h>
#include <Zone_VF.h>
#include <Champ_Uniforme.h>
#include <Zone_PolyMAC.h>
#include <DoubleTab.h>

Implemente_instanciable(Champ_Fonc_Face_PolyMAC,"Champ_Fonc_Face_PolyMAC",Champ_Fonc_base) ;

Sortie& Champ_Fonc_Face_PolyMAC::printOn(Sortie& os) const
{
  os << que_suis_je() << " " << le_nom();
  return os;
}

Entree& Champ_Fonc_Face_PolyMAC::readOn(Entree& is)
{
  return is;
}

Champ_base& Champ_Fonc_Face_PolyMAC::le_champ(void)
{
  return *this;
}

const Champ_base& Champ_Fonc_Face_PolyMAC::le_champ(void) const
{
  return *this;
}

void Champ_Fonc_Face_PolyMAC::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  ref_zone_vf_=ref_cast(Zone_VF, z_dis);
}


int Champ_Fonc_Face_PolyMAC::fixer_nb_valeurs_nodales(int n)
{

  // j'utilise le meme genre de code que dans Champ_Fonc_P0_base
  // sauf que je recupere le nombre de faces au lieu du nombre d'elements
  //
  // je suis tout de meme etonne du code utilise dans
  // Champ_Fonc_P0_base::fixer_nb_valeurs_nodales()
  // pour recuperer la zone discrete...

  const Champ_Fonc_base& self = ref_cast(Champ_Fonc_base, *this);
  const Zone_VF& la_zone_vf = ref_cast(Zone_VF,self.zone_dis_base());

  assert(n == la_zone_vf.nb_faces());

  const MD_Vector& md = la_zone_vf.md_vector_faces();
  // Probleme: nb_comp vaut 2 mais on ne veut qu'une dimension !!!
  // HACK :
  int old_nb_compo = nb_compo_;
  nb_compo_ = 1;
  creer_tableau_distribue(md);
  nb_compo_ = old_nb_compo;
  return n;

}
Champ_base& Champ_Fonc_Face_PolyMAC::affecter_(const Champ_base& ch)
{
  const DoubleTab& v = ch.valeurs();
  DoubleTab& val = valeurs();
  const Zone_PolyMAC& zone_PolyMAC = ref_cast( Zone_PolyMAC,ref_zone_vf_.valeur());
  int nb_faces = zone_PolyMAC.nb_faces();
  const DoubleVect& surface = zone_PolyMAC.face_surfaces();
  const DoubleTab& normales = zone_PolyMAC.face_normales();


  if (sub_type(Champ_Uniforme,ch))
    {
      for (int num_face=0; num_face<nb_faces; num_face++)
        {
          double vn=0;
          for (int dir=0; dir<dimension; dir++)
            vn+=v(0,dir)*normales(num_face,dir);

          vn/=surface(num_face);
          val(num_face) = vn;
        }
    }
  else if (nb_compo_ == dimension)
    {
      //      int ndeb_int = zone_PolyMAC.premiere_face_int();
      //      const IntTab& face_voisins = zone_PolyMAC.face_voisins();
      const DoubleTab& xv=zone_PolyMAC.xv();
      DoubleTab eval(val.dimension_tot(0),dimension);
      ch.valeur_aux(xv,eval);
      for (int num_face=0; num_face<nb_faces; num_face++)
        {
          double vn=0;
          for (int dir=0; dir<dimension; dir++)
            vn+=eval(num_face,dir)*normales(num_face,dir);


          vn/=surface(num_face);
          val(num_face) = vn;
        }
    }
  else if (nb_compo_ == 1)
    {
      //      int ndeb_int = zone_PolyMAC.premiere_face_int();
      //      const IntTab& face_voisins = zone_PolyMAC.face_voisins();
      const DoubleTab& xv=zone_PolyMAC.xv();
      ch.valeur_aux(xv,val);
    }
  else abort();
  return *this;
}


DoubleVect& Champ_Fonc_Face_PolyMAC::valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const
{
  throw;
  // return Champ_implementation_RT0::valeur_a_elem(position,result,poly);
}

double Champ_Fonc_Face_PolyMAC::valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const
{
  throw;
  //return Champ_implementation_RT0::valeur_a_elem_compo(position,poly,ncomp);
}

DoubleTab& Champ_Fonc_Face_PolyMAC::valeur_aux_elems(const DoubleTab& positions,
                                                     const IntVect& les_polys,
                                                     DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo=cha.nb_comp();
  if (val.nb_dim() == 1)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(nb_compo == 1);
    }
  else if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(val.dimension(1) == nb_compo);
    }
  else
    {
      Cerr << "Erreur TRUST dans Champ_Face_implementation::valeur_aux_elems()" << finl;
      Cerr << "Le DoubleTab val a plus de 2 entrees" << finl;
      Process::exit();
    }

  const Zone_PolyMAC& zone_VF = ref_cast(Zone_PolyMAC,zone_vf());
  //  const Zone& zone_geom = zone_VDF.zone();
  const DoubleTab& normales = zone_VF.face_normales();
  //  const DoubleVect& surfaces = zone_VF.face_surfaces();
  const IntTab& elem_faces = zone_VF.elem_faces();

  const DoubleTab& ch = cha.valeurs();

  if (nb_compo == 1)
    {
      Cerr<<"Champ_Face_implementation::valeur_aux_elems"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }
  else // (nb_compo != 1)
    {
      ArrOfDouble vale(nb_compo),s(nb_compo);
      for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
        {
          int le_poly=les_polys(rang_poly);
          if (le_poly == -1)
            for(int ncomp=0; ncomp<nb_compo; ncomp++)
              val(rang_poly,ncomp) = 0;
          else
            {
              vale=0;
              s=0;
              int nb_faces_elem_max=elem_faces.dimension(1);
              int nf;

              for (nf=0; nf<nb_faces_elem_max; nf++)
                {
                  int face=elem_faces(le_poly,nf);
                  if(face<0)
                    break;
                  for(int ncomp=0; ncomp<nb_compo; ncomp++)
                    {
                      vale(ncomp)+=ch(face)*normales(face,ncomp);
                      s(ncomp)+=dabs(normales(face,ncomp));
                    }
                }


              for(int ncomp=0; ncomp<nb_compo; ncomp++)
                val(rang_poly,ncomp)=vale(ncomp)/s(ncomp);
            }
        }
    }
  return val;
}

DoubleVect& Champ_Fonc_Face_PolyMAC::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const
{

  throw;
  // return Champ_implementation_RT0::valeur_aux_elems_compo(positions,polys,result,ncomp);
}

DoubleTab& Champ_Fonc_Face_PolyMAC::remplir_coord_noeuds(DoubleTab& positions) const
{

  throw;
  // return Champ_implementation_RT0::remplir_coord_noeuds(positions);
}

int Champ_Fonc_Face_PolyMAC::remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const
{

  throw;
  //  return Champ_implementation_RT0::remplir_coord_noeuds_et_polys(positions,polys);
}

DoubleTab& Champ_Fonc_Face_PolyMAC::valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const
{

  throw;
  //  return Champ_implementation_RT0::valeur_aux_sommets(domain,result);
}

DoubleVect& Champ_Fonc_Face_PolyMAC::valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const
{

  throw;
  //  return Champ_implementation_RT0::valeur_aux_sommets_compo(domain,result,ncomp);
}

void Champ_Fonc_Face_PolyMAC::mettre_a_jour(double un_temps)
{
  return Champ_Fonc_base::mettre_a_jour( un_temps);
}

