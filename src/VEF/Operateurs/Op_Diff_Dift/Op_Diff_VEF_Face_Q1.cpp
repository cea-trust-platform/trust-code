/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Op_Diff_VEF_Face_Q1.h>
#include <Champ_P1NC.h>
#include <Champ_Q1NC.h>
#include <Champ_Uniforme.h>
#include <Periodique.h>
#include <Neumann_paroi.h>
#include <Echange_externe_impose.h>

Implemente_instanciable(Op_Diff_VEF_Face_Q1,"Op_Diff_VEF_Q1NC",Op_Diff_VEF_base);

Sortie& Op_Diff_VEF_Face_Q1::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Op_Diff_VEF_Face_Q1::readOn(Entree& s )
{
  return s ;
}

/*! @brief associe le champ de diffusivite
 *
 */
void Op_Diff_VEF_Face_Q1::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = diffu;
}

void Op_Diff_VEF_Face_Q1::completer()
{
  Operateur_base::completer();
}


const Champ_base& Op_Diff_VEF_Face_Q1::diffusivite() const
{
  return diffusivite_.valeur();
}

double Op_Diff_VEF_Face_Q1::calculer_dt_stab() const
{
  // La diffusivite est constante dans le domaine donc
  //
  //          dt_diff = h*h/diffusivite
  remplir_nu(nu_);
  double dt_stab;
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  double alpha = local_max_vect((diffusivite_pour_pas_de_temps().valeurs()));
  if (alpha==0)
    dt_stab = DMAXFLOAT;
  else
    dt_stab = 0.5*domaine_VEF.carre_pas_du_maillage()/(2.*dimension*alpha);
  return dt_stab;
}

// Fonction utile visc_Q1

double visc_Q1(const Domaine_VEF& le_dom,const Domaine_Cl_VEF& zcl, int num_face,int num2,
               int num_elem,int dimension,DoubleTab& nu)
{
  double constante = nu(num_elem)/le_dom.volumes(num_elem)*zcl.equation().milieu().porosite_elem(num_elem);
  double Valeur=0;

  if(dimension==2)
    {
      Valeur=le_dom.face_normales(num_face,0)*
             le_dom.face_normales(num2,1) -
             le_dom.face_normales(num_face,1)*le_dom.face_normales(num2,0);
      Valeur=constante*std::fabs(Valeur)*(zcl.equation().milieu().porosite_face(num_face)* zcl.equation().milieu().porosite_face(num2));
    }
  else if (dimension == 3)
    {
      Valeur=(le_dom.face_normales(num_face,1)*le_dom.face_normales(num2,2) -
              le_dom.face_normales(num_face,2)*le_dom.face_normales(num2,1)) +
             (le_dom.face_normales(num_face,2)*le_dom.face_normales(num2,0) -
              le_dom.face_normales(num_face,0)*le_dom.face_normales(num2,2)) +
             (le_dom.face_normales(num_face,0)*le_dom.face_normales(num2,1) -
              le_dom.face_normales(num_face,1)*le_dom.face_normales(num2,0));
      Valeur=constante*std::fabs(Valeur)*(zcl.equation().milieu().porosite_face(num_face)* zcl.equation().milieu().porosite_face(num2));
    }
  // Cerr << "Valeur " << Valeur << finl;
  return Valeur;
}

DoubleTab& Op_Diff_VEF_Face_Q1::ajouter(const DoubleTab& inconnue, DoubleTab& resu) const
{
  remplir_nu(nu_);
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  int n1 = domaine_VEF.nb_faces();
  int nb_comp = 1;
  int nb_dim = resu.nb_dim();
  if(nb_dim==2)
    nb_comp=resu.dimension(1);

  int i,j,num_face;
  int elem1,elem2;
  int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  double val;

  // On traite les faces bord
  for (int n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          for (num_face=num1; num_face<num2; num_face++)
            {
              elem1 = face_voisins(num_face,0);
              fac_asso = la_cl_perio.face_associee(num_face-num1)+num1;
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( ( (j= elem_faces(elem1,i)) > num_face ) && (j != fac_asso ) )
                    {
                      val = visc_Q1(domaine_VEF,domaine_Cl_VEF,num_face,j,elem1,dimension,nu_);

                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          resu(num_face,nc)+=val*inconnue(j,nc);
                          resu(num_face,nc)-=val*inconnue(num_face,nc);
                          resu(j,nc)+=0.5*val*inconnue(num_face,nc);
                          resu(j,nc)-=0.5*val*inconnue(j,nc);
                        }
                    }
                }
              elem2 = face_voisins(num_face,1);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( ( (j= elem_faces(elem2,i)) > num_face ) && (j != fac_asso ) )
                    {
                      val = visc_Q1(domaine_VEF,domaine_Cl_VEF,num_face,j,elem2,dimension,nu_);
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          resu(num_face,nc)+=val*inconnue(j,nc);
                          resu(num_face,nc)-=val*inconnue(num_face,nc);
                          resu(j,nc)+=0.5*val*inconnue(num_face,nc);
                          resu(j,nc)-=0.5*val*inconnue(j,nc);
                        }
                    }
                }
            }
        }
      else
        {
          for (num_face=num1; num_face<num2; num_face++)
            {
              elem1 = face_voisins(num_face,0);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem1,i)) > num_face )
                    {
                      val = visc_Q1(domaine_VEF,domaine_Cl_VEF,num_face,j,elem1,dimension,nu_);
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          resu(num_face,nc)+=val*inconnue(j,nc);
                          resu(num_face,nc)-=val*inconnue(num_face,nc);
                          resu(j,nc)+=val*inconnue(num_face,nc);
                          resu(j,nc)-=val*inconnue(j,nc);
                        }
                    }
                }
            }
        }
    }

  // On traite les faces internes
  for (num_face=domaine_VEF.premiere_face_int(); num_face<n1; num_face++)
    {
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);

      for (i=0; i<nb_faces_elem; i++)
        {
          if ( (j=elem_faces(elem1,i)) > num_face )
            {
              val = visc_Q1(domaine_VEF,domaine_Cl_VEF,num_face,j,elem1,dimension,nu_);
              for (int nc=0; nc<nb_comp; nc++)
                {
                  resu(num_face,nc)+=val*inconnue(j,nc);
                  resu(num_face,nc)-=val*inconnue(num_face,nc);
                  resu(j,nc)+=val*inconnue(num_face,nc);
                  resu(j,nc)-=val*inconnue(j,nc);
                }

            }
          if ( (j=elem_faces(elem2,i)) > num_face )
            {
              val = visc_Q1(domaine_VEF,domaine_Cl_VEF,num_face,j,elem2,dimension,nu_);
              for (int nc=0; nc<nb_comp; nc++)
                {
                  resu(num_face,nc)+=val*inconnue(j,nc);
                  resu(num_face,nc)-=val*inconnue(num_face,nc);
                  resu(j,nc)+=val*inconnue(num_face,nc);
                  resu(j,nc)-=val*inconnue(j,nc);
                }
            }
        }
    }

  // Partie imposee :
  int nbcomp = resu.dimension(1);
  for (int n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            for (int comp=0; comp<nbcomp; comp++)
              resu(face,comp) += la_cl_paroi.flux_impose(face-ndeb,comp)*domaine_VEF.surface(face);
        }
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            for (int comp=0; comp<nbcomp; comp++)
              resu(face,comp) += la_cl_paroi.h_imp(face-ndeb,comp)*(la_cl_paroi.T_ext(face-ndeb)-inconnue(face,comp))*domaine_VEF.surface(face);
        }
    }

  modifier_flux(*this);
  return resu;
}

DoubleTab& Op_Diff_VEF_Face_Q1::calculer(const DoubleTab& inconnue, DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inconnue,resu);
}



/////////////////////////////////////////
// Methode pour l'implicite
/////////////////////////////////////////

void Op_Diff_VEF_Face_Q1::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const
{
  remplir_nu(nu_);
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  int n1 = domaine_VEF.nb_faces();
  const int nb_comp = transporte.line_size();

  int i,j,k,num_face,elem1,elem2;
  int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  // DoubleTab val= 0;
  double val;
  IntVect& tab1 = matrice.get_set_tab1();
  IntVect& tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();

  // On traite les faces bord
  for (int n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          for (num_face=num1; num_face<num2; num_face++)
            {
              elem1 = face_voisins(num_face,0);
              fac_asso = la_cl_perio.face_associee(num_face-num1)+num1;
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( ( (j= elem_faces(elem1,i)) > num_face ) && (j != fac_asso ) )
                    {
                      val = visc_Q1(domaine_VEF,domaine_Cl_VEF,num_face,j,elem1,dimension,nu_);
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          for (k=tab1[num_face*nb_comp+nc]-1; k<tab1[num_face*nb_comp+nc+1]-1; k++)
                            {
                              if (tab2[k]-1==num_face*nb_comp+nc)
                                coeff(k)+=val;
                              if (tab2[k]-1==j*nb_comp+nc)
                                coeff(k)-=0.5;
                            }
                          for (k=tab1[j*nb_comp+nc]-1; k<tab1[j*nb_comp+nc+1]-1; k++)
                            {
                              if (tab2[k]-1==num_face*nb_comp+nc)
                                coeff(k)-=val;
                              if (tab2[k]-1==j*nb_comp+nc)
                                coeff(k)+=0.5*val;
                            }
                        }
                    }
                }
              elem2 = face_voisins(num_face,1);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( ( (j= elem_faces(elem2,i)) > num_face ) && (j != fac_asso ) )
                    {
                      val = visc_Q1(domaine_VEF,domaine_Cl_VEF,num_face,j,elem2,dimension,nu_);
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          for (k=tab1[num_face*nb_comp+nc]-1; k<tab1[num_face*nb_comp+nc+1]-1; k++)
                            {
                              if (tab2[k]-1==num_face*nb_comp+nc)
                                coeff(k)+=val;
                              if (tab2[k]-1==j*nb_comp+nc)
                                coeff(k)-=0.5*val;
                            }
                          for (k=tab1[j*nb_comp+nc]-1; k<tab1[j*nb_comp+nc+1]-1; k++)
                            {
                              if (tab2[k]-1==num_face*nb_comp+nc)
                                coeff(k)-=val;
                              if (tab2[k]-1==j*nb_comp+nc)
                                coeff(k)+=0.5*val;
                            }
                        }
                    }
                }
            }
        }
      else
        {
          for (num_face=num1; num_face<num2; num_face++)
            {
              elem1 = face_voisins(num_face,0);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem1,i)) > num_face )
                    {
                      val = visc_Q1(domaine_VEF,domaine_Cl_VEF,num_face,j,elem1,dimension,nu_);
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          for (k=tab1[num_face*nb_comp+nc]-1; k<tab1[num_face*nb_comp+nc+1]-1; k++)
                            {
                              if (tab2[k]-1==num_face*nb_comp+nc)
                                coeff(k)+=val;
                              if (tab2[k]-1==j*nb_comp+nc)
                                coeff(k)-=val;
                            }
                          for (k=tab1[j*nb_comp+nc]-1; k<tab1[j*nb_comp+nc+1]-1; k++)
                            {
                              if (tab2[k]-1==num_face*nb_comp+nc)
                                coeff(k)-=val;
                              if (tab2[k]-1==j*nb_comp+nc)
                                coeff(k)+=val;
                            }

                        }
                    }
                }
            }
        }
    }

  // On traite les faces internes
  for (num_face=domaine_VEF.premiere_face_int(); num_face<n1; num_face++)
    {
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);

      for (i=0; i<nb_faces_elem; i++)
        {
          if ( (j=elem_faces(elem1,i)) > num_face )
            {
              val = visc_Q1(domaine_VEF,domaine_Cl_VEF,num_face,j,elem1,dimension,nu_);
              for (int nc=0; nc<nb_comp; nc++)
                {
                  for (k=tab1[num_face*nb_comp+nc]-1; k<tab1[num_face*nb_comp+nc+1]-1; k++)
                    {
                      if (tab2[k]-1==num_face*nb_comp+nc)
                        coeff(k) += val;
                      if (tab2[k]-1==j*nb_comp+nc)
                        coeff(k) -= val;
                    }
                  for (k=tab1[j*nb_comp+nc]-1; k<tab1[j*nb_comp+nc+1]-1; k++)
                    {
                      if (tab2[k]-1==num_face*nb_comp+nc)
                        coeff(k) -= val;
                      if (tab2[k]-1==j*nb_comp+nc)
                        coeff(k) += val;
                    }
                }
            }
          if ( (j=elem_faces(elem2,i)) > num_face )
            {
              // E Saikali : can not factorize more
              if (nb_comp == 1)
                {
                  for (int nc=0; nc<nb_comp; nc++)
                    {
                      val = visc_Q1(domaine_VEF,domaine_Cl_VEF,num_face,j,elem2,dimension,nu_);
                      for (k=tab1[num_face*nb_comp+nc]-1; k<tab1[num_face*nb_comp+nc+1]-1; k++)
                        {
                          if (tab2[k]-1==num_face*nb_comp+nc)
                            coeff(k)+=val;
                          if (tab2[k]-1==j*nb_comp+nc)
                            coeff(k)-=val;
                        }
                      for (k=tab1[j]-1; k<tab1[j+1]-1; k++)
                        {
                          if (tab2[k]-1==num_face*nb_comp+nc)
                            coeff(k)-=val;
                          if (tab2[k]-1==j*nb_comp+nc)
                            coeff(k)+=val;
                        }
                    }
                }
              else
                {
                  val= visc_Q1(domaine_VEF,domaine_Cl_VEF,num_face,j,elem1,dimension,nu_);
                  for (int nc=0; nc<nb_comp; nc++)
                    {
                      for (k=tab1[num_face*nb_comp+nc]-1; k<tab1[num_face*nb_comp+nc+1]-1; k++)
                        {
                          if (tab2[k]-1==num_face*nb_comp+nc)
                            coeff(k)+=val;
                          if (tab2[k]-1==j*nb_comp+nc)
                            coeff(k)-=val;
                        }
                      for (k=tab1[j*nb_comp+nc]-1; k<tab1[j*nb_comp+nc+1]-1; k++)
                        {
                          if (tab2[k]-1==num_face*nb_comp+nc)
                            coeff(k)-=val;
                          if (tab2[k]-1==j*nb_comp+nc)
                            coeff(k)+=val;
                        }
                    }
                }

            }
        }
    }

}

void Op_Diff_VEF_Face_Q1::contribue_au_second_membre(DoubleTab& resu ) const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const int nb_comp=resu.line_size();

  // Partie imposee :
  for (int n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            for (int comp=0; comp<nb_comp; comp++)
              resu(face,comp) += la_cl_paroi.flux_impose(face-ndeb,comp)*domaine_VEF.surface(face);
        }
    }
}
