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
// File:        Op_Conv_EF_VEF_P1NC.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_EF_VEF_P1NC.h>
#include <Champ_P1NC.h>
#include <Debog.h>
#include <Porosites_champ.h>

Implemente_instanciable(Op_Conv_EF_VEF_P1NC,"Op_Conv_EF_VEF_P1NC",Op_Conv_VEF_base);


//// printOn
//

Sortie& Op_Conv_EF_VEF_P1NC::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Conv_EF_VEF_P1NC::readOn(Entree& s )
{
  transportant_bar=0;
  transporte_bar=1;
  filtrer_resu=1;
  antisym=1;
  Motcle motlu;
  for (int i=0; i<4; i++)
    {
      s >> motlu;
      if (motlu=="defaut_bar")
        break;
      else if (motlu=="transportant_bar")
        s >> transportant_bar;
      else if (motlu=="transporte_bar")
        s >> transporte_bar;
      else if (motlu=="filtrer_resu")
        s >> filtrer_resu;
      else if (motlu=="antisym")
        s >> antisym;
      else
        {
          Cerr << motlu << "n'est pas compris par "
               << que_suis_je() << finl;
          exit();
        }
    }

  if (transportant_bar <0)
    {
      Cerr << "il manque le mot cle transportant_bar" << finl;
      exit();
    }
  if (transporte_bar <0)
    {
      Cerr << "il manque le mot cle transporte_bar" << finl;
      exit();
    }
  if (filtrer_resu <0)
    {
      Cerr << "il manque le mot cle filtrer_resu" << finl;
      exit();
    }
  if (antisym <0)
    {
      Cerr << "il manque le mot cle antisym" << finl;
      exit();
    }
  return s ;
}

//
//   Fonctions de la classe Op_Conv_EF_VEF_P1NC
//

// convbis correspond au calcul de -1*terme_convection


////////////////////////////////////////////////////////////////////
//
//                      Implementation des fonctions
//
//                   de la classe Op_Conv_EF_VEF_P1NC
//
////////////////////////////////////////////////////////////////////


DoubleTab& Op_Conv_EF_VEF_P1NC::ajouter(const DoubleTab& transporte_2,
                                        DoubleTab& resu) const
{
  //Cerr << "Op_Conv_EF_VEF_P1NC::ajouter" << finl;
  DoubleTab sauv(resu);
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Champ_P1NC& la_vitesse=ref_cast( Champ_P1NC, vitesse_.valeur());
  const DoubleTab& vitesse_face_absolue=la_vitesse.valeurs();
  const Champ_P1NC& ch=ref_cast(Champ_P1NC,mon_equation->inconnue().valeur());

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& face_normales=zone_VEF.face_normales();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.dimension(1);

  assert(nb_faces_elem==(dimension+1));
  {
    // calcul de la CFL.
    DoubleVect& fluent_ = ref_cast(DoubleVect, fluent);
    double psc;
    // On remet a zero le tableau qui sert pour
    // le calcul du pas de temps de stabilite
    fluent_ = 0;

    const int nb_faces = zone_VEF.nb_faces();
    for(int num_face=0; num_face<nb_faces; num_face++)
      {
        psc=0.;
        for (int i=0; i<dimension; i++)
          psc+=vitesse_face_absolue(num_face,i)*face_normales(num_face,i);
        fluent_(num_face)=dabs(psc);
      }
  }
  int marq=phi_u_transportant(equation());

  DoubleTab transporte_;
  DoubleTab vitesse_face_;
  // soit on a transporte=phi*transporte_ et vitesse_face=vitesse_
  // soit transporte=transporte_ et vitesse_face=phi*vitesse_
  // cela d~pend si on transporte avec phi u ou avec u.
  const DoubleTab& transporte=modif_par_porosite_si_flag(transporte_2,transporte_,!marq,porosite_face);
  const DoubleTab& tab_vitesse=modif_par_porosite_si_flag(vitesse_face_absolue,vitesse_face_,marq,porosite_face);
  //  DoubleTab ubar(transporte);
  //  la_vitesse.filtrer_L2(ubar);
  //  DoubleTab uprime(transporte);
  //  uprime-=ubar;

  DoubleTab uT(transporte);
  DoubleTab u(tab_vitesse);

  if (transporte_bar)
    ch.filtrer_L2(uT);
  if (transportant_bar)
    la_vitesse.filtrer_L2(u);

  int nb_comp=1;
  if(resu.nb_dim()!=1)
    nb_comp=resu.dimension(1);
  DoubleTab grad_uT(nb_comp, dimension);
  DoubleTab grad_phi(nb_faces_elem,dimension);
  int elem, num_face, j, i;
  double d_inv=(1./nb_faces_elem);
  int face;

  resu=0;
  DoubleTab sigma(dimension, dimension);
  DoubleTab contrib_elem(nb_faces_elem, nb_comp);
  for (elem=0; elem<nb_elem_tot; elem++)
    {
      //for elem
      sigma=0;
      contrib_elem=0;
      grad_uT=0.;
      grad_phi=0.;
      for(num_face=0; num_face<nb_faces_elem; num_face++)
        {
          //for num_face
          face=elem_faces(elem, num_face);
          double cc=1.;
          if(face_voisins(face,0)!=elem)
            cc*=-1;
          for (i=0; i<dimension; i++)
            {
              grad_phi(num_face,i)=face_normales(face,i)*cc;
              if(nb_comp==1)
                {
                  sigma(i,0)+=u(face,i)*uT(face);
                  grad_uT(0,i)+=uT(face)*grad_phi(num_face,i);
                }
              else
                for (j=0; j<nb_comp; j++)
                  {
                    sigma(i,j)+=u(face,i)*uT(face,j);
                    grad_uT(j,i)+=uT(face,j)*grad_phi(num_face,i);
                  }
            }
        }

      if(antisym==1)
        {
          for(num_face=0; num_face<nb_faces_elem; num_face++)
            {
              //for num_face
              face=elem_faces(elem, num_face);
              for (i=0; i<dimension; i++)
                for (j=0; j<nb_comp; j++)
                  contrib_elem(num_face,j)-=0.5*d_inv*(u(face,i)*grad_uT(j,i)-sigma(i,j)*grad_phi(num_face,i));// Ui Uj,i Vj - Ui Vj,i Uj
            }
        }
      else
        {
          for(num_face=0; num_face<nb_faces_elem; num_face++)
            {
              //for num_face
              face=elem_faces(elem, num_face);
              for (i=0; i<dimension; i++)
                for (j=0; j<nb_comp; j++)
                  contrib_elem(num_face,j)-=d_inv*(u(face,i)*grad_uT(j, i));// Ui Uj,i Vj
            }
        }

      for(num_face=0; num_face<nb_faces_elem; num_face++)
        {
          face=elem_faces(elem, num_face);
          if(nb_comp==1)
            resu(face)+=contrib_elem(num_face,0);
          else
            for (i=0; i<nb_comp; i++)
              resu(face,i)+=contrib_elem(num_face,i);
        }
    }

  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot=le_bord.nb_faces_tot();
      int num10 = 0 ;
      int num20 = le_bord.nb_faces_tot();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int face_associee;
          IntVect fait(nb_faces_bord_tot);
          fait = 0;

          for (int ind_face=num10; ind_face<num20; ind_face++)
            {
              face = le_bord.num_face(ind_face);
              if (fait(ind_face) == 0)
                {
                  fait(ind_face) = 1;
                  face_associee=la_cl_perio.face_associee(ind_face);
                  fait(face_associee) = 1;
                  face_associee = le_bord.num_face(face_associee);

                  for (int comp=0; comp<nb_comp; comp++)
                    {
                      double som;
                      if(nb_comp==1)
                        {
                          som=resu(face_associee)+resu(face);
                          resu(face)=resu(face_associee)=som;
                        }
                      else
                        {
                          som=resu(face_associee,comp)+resu(face,comp);
                          resu(face,comp)=resu(face_associee,comp)=som;
                        }
                    }
                }// if fait
            }// for face

        }// sub_type Perio
      //       else if ( (sub_type(Dirichlet,la_cl.valeur())) || (sub_type(Dirichlet_homogene,la_cl.valeur())) )
      //         {
      //           for (face=num1; face<num2; face++)
      //             {
      //               if(nb_comp==1)
      //                 {
      //                   resu(face)=0;
      //                 }
      //               else
      //                 for (int comp=0; comp<nb_comp; comp++)
      //                   {
      //                     resu(face,comp)=0.;
      //                   }
      //             }
      //         }
      else if ((antisym==1) && (!sub_type(Symetrie,la_cl.valeur())) )
        {
          //          Cerr << "Ajout des termes de bords :" << finl;
          int num1 = le_bord.num_premiere_face();
          int nb_faces=le_bord.nb_faces();
          int num2 = num1 + nb_faces;

          for (face=num1; face<num2; face++)
            {
              int comp;
              double psc = 0;
              for (comp=0; comp<nb_comp; comp++)
                psc += face_normales(face,comp)*u(face, comp);
              psc *=0.5;
              if(nb_comp==1)
                resu(face)-=psc*uT(face);
              else
                {
                  for (comp=0; comp<nb_comp; comp++)
                    resu(face, comp)-=psc*uT(face, comp);
                }
            }
        }
    }// for nbord

  //Cerr << "Av filtre resu = " << resu << finl;
  // Filtrage de l'acceleration :
  // PQ : 11/03 : probleme de filtrer_resu constate avec un scalaire
  //  if(filtrer_resu && nb_comp==1)
  //  {
  //  Cerr << " probleme de filtrer_resu constate avec un scalaire dans VEF/Operateurs/OpConvEFP1NC.cpp" << finl;
  //  exit();
  //  }
  if(filtrer_resu)
    ch.filtrer_resu(resu);

  // On desactive le calcul et l'impression des energies u'......
  // on garde le codage au cas ou..
  if (0)
    {
      Cerr << "filtrage petites echelles : " << finl;

      DoubleTab ubar(transporte);
      ch.filtrer_L2(ubar);
      DoubleTab uprime(transporte);
      uprime-=ubar;

      const int nb_faces = zone_VEF.nb_faces();
      const DoubleVect& volumes_entrelaces=zone_VEF.volumes_entrelaces();
      int face2, k;
      double psc=0;
      double pscprim=0;
      double ec=0;
      double ecprime=0;
      int deb=zone_VEF.premiere_face_int();
      for(face2=deb; face2<nb_faces; face2++)
        for(k=0; k< dimension; k++)
          {
            double v=volumes_entrelaces(face2);
            double up, u2, r;
            if(nb_comp>1)
              {
                up=uprime(face2,k);
                uprime(face2,k)*=v;
                u2=ubar(face2,k);
                r=resu(face2,k);
              }
            else
              {
                up=uprime(face2);
                uprime(face2)*=v;
                u2=ubar(face2);
                r=resu(face2);
              }
            pscprim+=r*up;
            psc+=r*u2;
            ec+=u2*u2*v;
            ecprime+=up*up*v;
          }



      for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int nb_faces_b=le_bord.nb_faces();
          int num2 = num1 + nb_faces_b;
          if (sub_type(Periodique,la_cl.valeur()))
            {
              const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
              int face_associee;
              IntVect fait(nb_faces_b);
              fait = 0;

              for (face2=num1; face2<num2; face2++)
                {
                  if (fait(face2-num1) == 0)
                    {
                      fait(face2-num1) = 1;
                      face_associee=la_cl_perio.face_associee(face2-num1);
                      fait(face_associee) = 1;
                      face_associee+=num1;
                      double v=volumes_entrelaces(face2);
                      double up, u2, r;
                      for(k=0; k< dimension; k++)
                        {
                          if(nb_comp>1)
                            {
                              up=(uprime(face2,k));
                              uprime(face2,k)*=v;
                              uprime(face_associee,k)=uprime(face2,k);
                              u2=ubar(face2,k);
                              r=resu(face2,k);
                            }
                          else
                            {
                              up=uprime(face2);
                              uprime(face2)*=v;
                              uprime(face_associee)=uprime(face2);
                              u2=ubar(face2);
                              r=resu(face2);
                            }
                          pscprim+=r*up;
                          psc+=r*u2;
                          ec+=u2*u2*v;
                          ecprime+=up*up*v;
                        }
                    }
                }
            }
          else
            {
              for (face2=num1; face2<num2; face2++)
                {
                  double v=volumes_entrelaces(face2);
                  double up, u2, r;
                  for(k=0; k< dimension; k++)
                    {
                      if(nb_comp>1)
                        {
                          up=(uprime(face2,k));
                          uprime(face2,k)*=v;
                          u2=ubar(face2,k);
                          r=resu(face2,k);
                        }
                      else
                        {
                          up=uprime(face2);
                          uprime(face2)*=v;
                          u2=ubar(face2);
                          r=resu(face2);
                        }
                      pscprim+=r*up;
                      psc+=r*u2;
                      ec+=u2*u2*v;
                      ecprime+=up*up*v;
                    }
                }
            }
        }
      Cerr << "(u,u) "<< ec <<finl;
      Cerr << "(u',u') "<< ecprime <<finl;
      Cerr << "(u grad u ,u) "<< psc <<finl;
      Cerr << "(u grad u ,u') "<< pscprim <<finl;
      double pourcent=0;
      if (!est_egal(ec,0))
        pourcent=100*sqrt(ecprime/ec);
      Cerr << "||u'||/||u|| :  " << pourcent << " %" << endl;
    }
  resu+=sauv;
  modifier_flux(*this);
  return resu;
}


void Op_Conv_EF_VEF_P1NC::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const
{
  Cerr << "Op_Conv_EF_VEF_P1NC::ajouter_contribution non code." << finl;
  exit();
}

void Op_Conv_EF_VEF_P1NC::contribue_au_second_membre(DoubleTab& resu ) const
{
  Cerr << "Op_Conv_EF_VEF_P1NC::contribue_au_second_membre non code." << finl;
  exit();
}
