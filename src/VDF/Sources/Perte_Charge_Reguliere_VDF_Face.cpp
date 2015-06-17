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
// File:        Perte_Charge_Reguliere_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Perte_Charge_Reguliere_VDF_Face.h>
#include <Zone_VDF.h>
#include <Champ_Face.h>
#include <Fluide_Incompressible.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(Perte_Charge_Reguliere_VDF_Face,"Perte_Charge_Reguliere_VDF_Face",Perte_Charge_VDF_Face);



//// printOn
//

Sortie& Perte_Charge_Reguliere_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Perte_Charge_Reguliere_VDF_Face::readOn(Entree& s )
{
  Perte_Charge_Reguliere::lire_donnees(s);
  Nom nom_sous_zone;
  s >> nom_sous_zone;
  Cerr << " nom de la sous zone " << nom_sous_zone << finl ;
  remplir_num_faces(nom_sous_zone);
  return s ;
}


/////////////////////////////////////////////////////////////////////
//
//                    Implementation des fonctions
//
//               de la classe Perte_Charge_Reguliere_VDF_Face
//
////////////////////////////////////////////////////////////////////

void Perte_Charge_Reguliere_VDF_Face::remplir_num_faces(Nom& nom_sous_zone)
{
  const Domaine& le_domaine = equation().probleme().domaine();
  const Zone_VDF& zone_VDF = ref_cast(Zone_VDF,equation().zone_dis().valeur());
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleTab& xv = zone_VDF.xv() ;
  const Sous_Zone& la_sous_zone = le_domaine.ss_zone(nom_sous_zone);
  int nb_poly_ss_zone = la_sous_zone.nb_elem_tot();
  int k = direction_perte_charge();
  DoubleVect xv_(dimension) ;
  int dir_a_faire ;
  int face_associee,nfac;
  int num_poly;
  num_faces.resize(nb_poly_ss_zone*6);
  corr_front_ss.resize(nb_poly_ss_zone*6);

  corr_front_ss = 1. ;

  nfac = 0 ;


  IntVect num_loc(zone_VDF.nb_elem_tot());
  num_loc = -1;
  int num_elem,num_face;
  for (num_elem=0; num_elem<nb_poly_ss_zone; num_elem++)
    {
      num_loc[la_sous_zone(num_elem)] = num_elem;

    }
  for (int direction = 0; direction<dimension; direction++)
    {
      Cerr << " Perte_Charge_Reguliere_VDF_Face::remplir num_face en direction " << direction << finl ;

      if ((direction ==  k) || (dir[direction] == 1 ))
        {
          dir_a_faire = k ;
          if (dir[direction] == 1 ) dir_a_faire = direction ;
          for (num_elem=0; num_elem<nb_poly_ss_zone; num_elem++)
            {
              num_poly = la_sous_zone(num_elem);
              num_face =  elem_faces(num_poly,dir_a_faire);
              num_faces[nfac++] = num_face;
              for (int ifa=0; ifa<dimension; ifa++) xv_(ifa)= xv(num_face,ifa) ;
              corr_front_ss[nfac-1] *= correction_direction( xv_ , dir_a_faire );

              int num_poly_vois0 = face_voisins(num_face,0);
              if (num_poly_vois0 != -1)
                if (num_loc[num_poly_vois0] == -1)  // le poly voisin n'est pas dans la sous_zone
                  {
                    corr_front_ss[nfac-1]*= volumes(num_poly)/(volumes(num_poly)+volumes(num_poly_vois0)) ;
                  }
              face_associee = elem_faces(num_poly,dir_a_faire+dimension);
              int num_poly_vois1 = face_voisins(face_associee,1);
              if (num_poly_vois1 != -1)
                if (num_loc[num_poly_vois1] == -1)  // le poly voisin n'est pas dans la sous_zone
                  {
                    num_faces[nfac++] = face_associee;
                    for (int ifa=0; ifa<dimension; ifa++) xv_(ifa) = xv(face_associee,ifa) ;
                    corr_front_ss[nfac-1] *= correction_direction( xv_, dir_a_faire);
                    corr_front_ss[nfac-1] *= volumes(num_poly)/(volumes(num_poly)+volumes(num_poly_vois1)) ;
                  }
            }
        }
    }
  num_faces.resize(nfac);
  corr_front_ss.resize(nfac);

}

DoubleTab& Perte_Charge_Reguliere_VDF_Face::ajouter_(const DoubleTab& inco,DoubleTab& resu) const
{

  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleVect& porosite_surf = zone_VDF.porosite_face();
  const DoubleTab& vit = la_vitesse->valeurs();
  int ndeb_faces_int = zone_VDF.premiere_face_int();
  // on prend nu et non mu
  const Champ_Don& nu = le_fluide->viscosite_cinematique();


  double d_visco=-1;
  int l_visco_unif=0;

  if (sub_type(Champ_Uniforme,nu.valeur()))
    {
      const Champ_Uniforme& ch_nu = ref_cast(Champ_Uniforme,nu.valeur());
      d_visco = ch_nu(0,0);
      l_visco_unif = 1;
    }

  {
    int nb_faces,numfa;

    double Cf,CK,U,U_abs,Reynolds;
    const DoubleTab& visco = nu.valeurs();
    {
      nb_faces = num_faces.size();
      for (int i=0; i<nb_faces; i++)
        {
          numfa = num_faces[i];
          U = inco[numfa];
          U_abs = dabs(vit[numfa]);

          if (!l_visco_unif)
            {
              if (numfa < ndeb_faces_int)
                {
                  int n0 = face_voisins(numfa,0);
                  if (n0 != -1)
                    d_visco =  visco[n0];
                  else
                    d_visco = visco[face_voisins(numfa,1)];
                }
              else
                d_visco = 0.5*(visco[face_voisins(numfa,0)]+visco[face_voisins(numfa,1)]);

            }
          if (couronne_tube == 1)
            {
              if (vit!=inco)
                {
                  Cerr<<"not implemented " <<finl;
                  exit();
                }
              double U_res=0.,vit1,vit2,div,corr_sign=1.0;
              int ori,el,fa1,fa2,fa3,fa4 ;
              // modifications ajoutees par fabio :
              ori = zone_VDF.orientation(numfa);
              vit1 = 0;
              vit2 = 0;
              div = 4.;
              for (int ind=0; ind <2; ind++)
                {
                  el = face_voisins(numfa,ind);
                  if (el!=-1)
                    {
                      fa1 = elem_faces(el,(ori+1));
                      fa2 = elem_faces(el,(ori+1+dimension)%(2*dimension));
                      vit1 += vit[fa1] + vit[fa2] ;
                      if (dimension == 3)
                        {
                          fa3 = elem_faces(el,(ori+2));
                          fa4 = elem_faces(el,(ori+2+dimension)%(2*dimension));
                          vit2 += vit[fa3] + vit[fa4];
                        }
                    }
                  else div = 2.;
                }
              vit1 = vit1/div;
              U_res = U*U+vit1*vit1;
              if (dimension == 3)
                {
                  vit2 = vit2/div;
                  U_res += vit2*vit2;
                }
              // U_res = sqrt(U_res)/rasu();
              U_res = sqrt(U_res);
              Reynolds = U_res*d()/d_visco;
              if (!Cf_utilisateur)
                Cf = calculer_Cf_blasius(Reynolds);
              else
                Cf = CF();
              CK = -0.5*Cf/D();
              if (U != 0)
                {
                  corr_sign = U/dabs(U);
                }
              resu[numfa] += CK*U_res*U_res*volumes_entrelaces[numfa]*porosite_surf[numfa]*corr_front_ss[i]*corr_sign;
            }
          else
            {
              Reynolds = U_abs*d()/d_visco;
              if (!Cf_utilisateur)
                Cf = calculer_Cf_blasius(Reynolds);
              else
                Cf = CF();
              CK = -0.5*Cf/D();
              resu[numfa] += CK*U*U_abs*volumes_entrelaces[numfa]*porosite_surf[numfa]*corr_front_ss[i];
            }
        }
    }

  }
  return resu;
}




