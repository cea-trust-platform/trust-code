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
// File:        Assembleur_P_EF.cpp
// Directory:   $TRUST_ROOT/src/EF/Operateurs
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Assembleur_P_EF.h>
#include <Zone_Cl_EF.h>
#include <Zone_EF.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet.h>
#include <Champ_front_instationnaire_base.h>
#include <Champ_front_var_instationnaire.h>
#include <Matrice_Bloc.h>

#include <Array_tools.h>
#include <Debog.h>
#include <Connectivite_som_elem.h>
#include <Static_Int_Lists.h>
#include <Champ_Fonc_Q1_EF.h>

Implemente_instanciable(Assembleur_P_EF,"Assembleur_P_EF",Assembleur_base);

Sortie& Assembleur_P_EF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom() ;
}

Entree& Assembleur_P_EF::readOn(Entree& s )
{
  return Assembleur_base::readOn(s);
}


void calculer_inv_volume_special(DoubleTab& inv_volumes_som, const Zone_Cl_EF& zone_Cl_EF,const DoubleTab& volumes_som,const DoubleTab& marqueur)
{
  inv_volumes_som=volumes_som;
  // MODIF GB
  inv_volumes_som=0.;
  int taille=volumes_som.dimension_tot(0);
  for (int i=0; i<taille; i++)
    // MODIF GB
    // if (marqueur(i,0))
    {
      double marq_norm2 = 0.;
      for (int comp=0; comp<Objet_U::dimension; comp++) marq_norm2 += marqueur(i,comp)*marqueur(i,comp);
      if (marq_norm2)
        for (int comp=0; comp<Objet_U::dimension; comp++)
          inv_volumes_som(i,comp)=1./volumes_som(i,comp);
    }

}
void calculer_inv_volume(DoubleTab& inv_volumes_som, const Zone_Cl_EF& zone_Cl_EF,const DoubleVect& volumes_som)
{
  // maintenant l 'inverse du volume est un DoubleTab
  // c'est pour faire fonctionner le Piso

  const DoubleTab* doubleT = dynamic_cast<const DoubleTab*>(&volumes_som);
  if (doubleT)
    {
      DoubleTab marqueur(*doubleT);
      marqueur=1;
      zone_Cl_EF.equation().solv_masse().valeur().appliquer_impl(marqueur);
      calculer_inv_volume_special(inv_volumes_som, zone_Cl_EF,*doubleT,marqueur);
      return;
    }

  DoubleTab marqueur;
  marqueur=(volumes_som);
  marqueur=1;
  zone_Cl_EF.equation().solv_masse().appliquer(marqueur);
  int taille=volumes_som.size_totale();
  inv_volumes_som.resize(taille,Objet_U::dimension);
  // marqueur
  {
    for (int i=0; i<taille; i++)
      if (marqueur(i))
        for (int comp=0; comp<Objet_U::dimension; comp++)
          inv_volumes_som(i,comp)=1./volumes_som(i);
  }
}
/*
DoubleTab inv_volumes_som;
  inv_volumes_som=(volumes_som);
  Debog::verifier("volumes_som",inv_volumes_som);
  inv_volumes_som=1;
  la_zone_Cl_EF->equation().solv_masse().appliquer(inv_volumes_som);
  for (int i=0;i<inv_volumes_som.size_totale()*0;i++)
    inv_volumes_som(i)=1./volumes_som(i);
*/

int Assembleur_P_EF::assembler(Matrice& la_matrice)
{
  const Zone_EF& zone_EF = ref_cast(Zone_EF, la_zone_EF.valeur());
  const DoubleVect& volumes_som=zone_EF.volumes_sommets_thilde();


  return assembler_mat(la_matrice,volumes_som,1,1);
}

int Assembleur_P_EF::assembler_rho_variable(Matrice& la_matrice, const Champ_Don_base& rho)
{
  // On multiplie par la masse volumique aux sommets
  if (!sub_type(Champ_Fonc_Q1_EF, rho))
    {
      Cerr << "La masse volumique n'est pas aux sommets dans Assembleur_P_EF::assembler_rho_variable." << finl;
      Process::exit();
    }
  const DoubleVect& volumes_som=ref_cast(Zone_EF, la_zone_EF.valeur()).volumes_sommets_thilde();
  const DoubleVect& masse_volumique=rho.valeurs();
  DoubleVect quantitee_som(volumes_som);
  int size=quantitee_som.size_array();
  for (int i=0; i<size; i++)
    quantitee_som(i)=(volumes_som(i)*masse_volumique(i));

  // On assemble la matrice
  return assembler_mat(la_matrice,quantitee_som,1,1);
}


int  Assembleur_P_EF::assembler_mat(Matrice& la_matrice,const DoubleVect& volumes_som,int incr_pression,int resoudre_en_u)
{

  DoubleTab inv_volumes_som;
  calculer_inv_volume(inv_volumes_som, la_zone_Cl_EF.valeur(), volumes_som);
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);
  Cerr << "Assemblage de la matrice de pression en cours..." << finl;
  // Matrice de pression :matrice creuse de taille nb_poly x nb_poly
  // Cette fonction range la matrice dans une structure de matrice morse
  // bien adaptee aux matrices creuses.
  // On commence par calculer les tailles des tableaux tab1 et tab2
  // (coeff_ a la meme taille que tab2)
  //   A chaque polyedre on associe :
  //   - une liste d'ints voisins[i] = {j>i t.q Mij est non nul }
  //   - une liste de reels  valeurs[i] = {Mij pour j dans Voisins[i]}
  //   - un reel terme_diag
  // Implementation temporaire:
  // On assemble une matrice de pression pour une equation d'hydraulique
  // On injecte dans cette matrice les conditions aux limites
  // On peut faire cela car a priori la matrice de pression n'est pas
  // partagee par plusieurs equations sur une meme zone.

  const Zone_EF& la_zone = la_zone_EF.valeur();
  const Zone_Cl_EF& la_zone_cl = la_zone_Cl_EF.valeur();
  les_coeff_pression.resize(la_zone_cl.nb_faces_Cl());
  int n1 = la_zone.zone().nb_elem_tot();
  int n2 = la_zone.zone().nb_elem();
  //Journal() << "n1= " << n1 << " n2= " << n2 << finl;
  int elem2;

  has_P_ref=0;


  // on construit la connectivite som_elem
  int nb_sommets_tot=la_zone.zone().nb_som_tot();
  Static_Int_Lists som_elem;
  construire_connectivite_som_elem(nb_sommets_tot,la_zone.zone().les_elems(),som_elem,1);

  //const IntTab& face_voisins = la_zone.face_voisins();
  //  const DoubleTab& face_normales = la_zone.face_normales();
  //  const Conds_lim& les_cl = la_zone_cl.les_conditions_limites();

  // int premiere_face_std=la_zone.premiere_face_std();
  // Rajout des porosites.
  // const DoubleVect& porosite_face = la_zone.porosite_face();
  //const DoubleVect& porosite_face = la_zone.porosite_face();

  la_matrice.typer("Matrice_Bloc");
  Matrice_Bloc& matrice=ref_cast(Matrice_Bloc, la_matrice.valeur());
  matrice.dimensionner(1,2);
  matrice.get_bloc(0,0).typer("Matrice_Morse_Sym");
  matrice.get_bloc(0,1).typer("Matrice_Morse");

  Matrice_Morse_Sym& MBrr =  ref_cast(Matrice_Morse_Sym,matrice.get_bloc(0,0).valeur());
  Matrice_Morse& MBrv = ref_cast (Matrice_Morse,matrice.get_bloc(0,1).valeur());
  MBrr.dimensionner(n2,0);
  MBrv.dimensionner(n2,n1-n2,0);


  // On parcourt les sommets
  const IntTab& sommets_elem=la_zone.zone().les_elems();
  int nb_coeff_rr=0;
  int nb_coeff_rv=0;
  //int nb_elem=n2;
  int nb_som_elem=sommets_elem.dimension(1);
  for (int elem1=0; elem1<n1; elem1++)
    for (int s=0; s<nb_som_elem; s++)
      {
        int num_som=sommets_elem(elem1,s);
        int nb_voisin=som_elem.get_list_size(num_som);
        for (int i2=0; i2<nb_voisin; i2++)
          {
            elem2 = som_elem(num_som,i2);
            if (elem1 >= elem2)
              {
                if(elem1<n2)
                  nb_coeff_rr++;
                else if(elem2<n2)
                  nb_coeff_rv++;
              }
            else // elem2 > elem1
              {
                if(elem2<n2)
                  nb_coeff_rr++;
                else

                  if(elem1<n2)
                    nb_coeff_rv++;
              }
          }
      }
  // on a surevalue  nb_coeff mais bon....
  IntTab indice_rr(nb_coeff_rr,2);
  IntTab indice_rv(nb_coeff_rv,2);
  nb_coeff_rr=0;
  nb_coeff_rv=0;
  for (int elem1=0; elem1<n1; elem1++)
    for (int s=0; s<nb_som_elem; s++)
      {
        int num_som=sommets_elem(elem1,s);
        int nb_voisin=som_elem.get_list_size(num_som);
        for (int i2=0; i2<nb_voisin; i2++)
          {
            elem2 = som_elem(num_som,i2);
            if (elem1 >= elem2)
              {
                if(elem1<n2)
                  {
                    indice_rr(nb_coeff_rr,0)=elem2;
                    indice_rr(nb_coeff_rr,1)=elem1;
                    nb_coeff_rr++;
                  }
                else if(elem2<n2)
                  {
                    indice_rv(nb_coeff_rv,0)=elem2;
                    indice_rv(nb_coeff_rv,1)=elem1;
                    nb_coeff_rv++;
                  }
              }
            else // elem2 > elem1
              {
                if(elem2<n2)
                  {
                    indice_rr(nb_coeff_rr,0)=elem1;
                    indice_rr(nb_coeff_rr,1)=elem2;
                    nb_coeff_rr++;
                  }
                else

                  if(elem1<n2)
                    {
                      indice_rv(nb_coeff_rv,0)=elem1;
                      indice_rv(nb_coeff_rv,1)=elem2;
                      nb_coeff_rv++;
                    }
              }
          }
      }
  tableau_trier_retirer_doublons(indice_rr);

  tableau_trier_retirer_doublons(indice_rv);
  for (int ii=0; ii<indice_rv.dimension(0); ii++) indice_rv(ii,1)-=n2;
  MBrr.dimensionner(indice_rr);
  if (indice_rv.size()>0)
    MBrv.dimensionner(indice_rv);
  MBrr.get_set_coeff() = 0;
  MBrv.get_set_coeff() = 0;
  const DoubleTab& Bthilde=la_zone.Bij_thilde();
  for (int elem1=0; elem1<n1; elem1++)
    for (int s=0; s<nb_som_elem; s++)
      {
        int num_som=sommets_elem(elem1,s);
        int nb_voisin=som_elem.get_list_size(num_som);
        for (int i2=0; i2<nb_voisin; i2++)
          {
            elem2 = som_elem(num_som,i2);
            int s2=0;
            while ((s2<nb_som_elem)&&(num_som!=sommets_elem(elem2,s2))) s2++;
            assert(num_som==sommets_elem(elem2,s2));
            double val=0;
            for (int dir=0; dir<dimension; dir++)
              val+=Bthilde(elem1,s,dir)*Bthilde(elem2,s2,dir)*inv_volumes_som(num_som,dir);
            if (elem1 <= elem2)
              {
//		Cerr<<"ici "<<elem1 <<" "<<elem2 << " "<<s<<finl;
                if(elem2<n2)
                  MBrr(elem1,elem2)+=val;

                else

                  if(elem1<n2)
                    MBrv.coef(elem1,elem2-n2)+=val;
              }
          }
      }
  if (0)
    {
      DoubleTab test(n1),res(n2);
      test=1;
      res=10;
      matrice.multvect(test,res);
      Cerr<<" laplacien 1 "<<mp_max_abs_vect(res)<<finl;
      Cerr<<res<<finl;
      //matrice.imprimer(Cerr);
      MBrr.imprimer_formatte(Cerr);
    }

  //Debog::verifier_Mat_elems("Assemblage EF avt bord ",matrice);

  // correction due aux bors
  //  const IntTab& faces_sommets=la_zone.face_sommets();
  //  int nb_som_face=faces_sommets.dimension(1);
  // const IntTab& face_voisins = la_zone.face_voisins();
  //const DoubleTab& face_normales = la_zone.face_normales();
  const Zone_Cl_EF& zone_Cl_EF  = la_zone_Cl_EF.valeur();
  // prise en compte des " cl " ajout du temr - int P sur le bord
  //int elem_ref=-2;
  for (int n_bord=0; n_bord<la_zone.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_EF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_sortie_libre,la_cl.valeur()) )
        {
          MBrr.set_est_definie(1);
          /*
          if (has_P_ref==0)
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int face=le_bord.num_premiere_face();
              assert(le_bord.nb_faces()>0);
              elem_ref=face_voisins(face,0);
            }
          */
          has_P_ref=1;
        }
    }
  /*
       else {
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        int num1 = 0;//le_bord.num_premiere_face();
        int num2 =  le_bord.nb_faces_tot();
        // test..........
        //num2=0;
        // ppovisoire !!!!!!
        for (int ind_face=num1; ind_face<num2; ind_face++)
  	 {
  	   int face=le_bord.num_face(ind_face);

  	  for (int sf=0;sf<nb_som_face;sf++)
  	    {
  	      int num_som=faces_sommets(face,sf);
    */
  int nb_som=la_zone.zone().nb_som();
  const ArrOfInt& type_sommet=zone_Cl_EF.get_type_sommet();
  for (int num_som=0; num_som<nb_som; num_som++)
    {
      int nb_voisin=som_elem.get_list_size(num_som);
      //      if (type_sommet(num_som)==0) ???

      if (type_sommet(num_som)==1)
        for (int i2=0; i2<nb_voisin; i2++)
          {
            elem2 =  som_elem(num_som,i2);
            // ne marche qu'en 2D !!!!!!!
            //int nb_voisin=som_elem.get_list_size(num_som);
            for (int i1=0; i1<nb_voisin; i1++)
              {
                int elem1 = som_elem(num_som,i1);
                int s1=0;
                while ((s1<nb_som_elem)&&(num_som!=sommets_elem(elem1,s1))) s1++;
                int s2=0;
                while ((s2<nb_som_elem)&&(num_som!=sommets_elem(elem2,s2))) s2++;
                assert(num_som==sommets_elem(elem2,s2));
                assert(num_som==sommets_elem(elem1,s1));
                double val=0;
                ArrOfDouble grad_mod(dimension),grad(dimension);
                //  double grad_n=0;
                for (int comp=0; comp<dimension; comp++)
                  {
                    //		      grad_n+=Bthilde(elem2,s2,comp)*face_normales(face,comp);
                    grad(comp)=Bthilde(elem2,s2,comp);
                  }
                //	  grad_n/=(la_zone.surface(face)*la_zone.surface(face));
                zone_Cl_EF.modifie_gradient(grad_mod,grad,num_som);
                for (int dir=0; dir<dimension; dir++)
                  val-=Bthilde(elem1,s1,dir)*grad_mod(dir)*inv_volumes_som(num_som,dir);
                // val-=Bthilde(elem1,s1,dir)*face_normales(face,dir)*grad_n*inv_volumes_som(num_som,dir);
                // 	  val=0;
// 		  for (int dir=0;dir<dimension;dir++)
// 		    val-=Bthilde(elem1,s1,dir)*Bthilde(elem2,s2,dir);
                //		  val*=-1;

                //	  assert(val==0);
                if (elem1 <= elem2)
                  {
                    if(elem2<n2)
                      MBrr(elem1,elem2)+=val;

                    else

                      if(elem1<n2)
                        MBrv.coef(elem1,elem2-n2)+=val;
                  }
              }
          }

    }
  /*
    }
    }
  */
  has_P_ref=(int)mp_max(has_P_ref);
  //if (je_suis_maitre()) MBrr(elem_ref,elem_ref)*=2;
  if (0)
    if (!(has_P_ref))
      {
        Cerr<<"Pas de pression imposee  --> P(0)=0"<<finl;
        if (je_suis_maitre())
          MBrr(0,0) *= 2;
        MBrr.set_est_definie(1);
        has_P_ref=1;
      }
  // test
  if (0)
    {
      DoubleTab test(n1),res(n2);
      test=1;
      res=10;
      matrice.multvect(test,res);
      Cerr<<" laplacien 1 "<<mp_max_abs_vect(res)<<finl;
      Cerr<<res<<finl;
      //matrice.imprimer(Cerr);
      MBrr.imprimer_formatte(Cerr);
    }
  // Debog::verifier_Mat_elems("Asse",matrice);
// MBrr.coeff_*=-1;
  //  matrice.imprimer(Cerr);exit();
  // exit();
  Cerr << "Fin de l'assemblage de la matrice de pression" << finl;
  return 1;

}

// Description:
//    Assemble la matrice de pression pour un fluide quasi compressible
//    laplacein(P) est remplace par div(grad(P)/rho).
// Precondition:
// Parametre: DoubleTab& tab_rho
//    Signification: mass volumique
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: lecture
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Assembleur_P_EF::assembler_QC(const DoubleTab& tab_rho, Matrice& matrice)
{
  Cerr << "Assemblage de la matrice de pression pour Quasi Compressible en cours..." << finl;
  assembler(matrice);
  set_resoudre_increment_pression(1);
  set_resoudre_en_u(0);
  Matrice_Bloc& matrice_bloc= ref_cast(Matrice_Bloc,matrice.valeur());
  Matrice_Morse_Sym& la_matrice =ref_cast(Matrice_Morse_Sym,matrice_bloc.get_bloc(0,0).valeur());
  if ((la_matrice.get_est_definie()!=1)&&(1))
    {
      Cerr<<"Pas de pression imposee  --> P(0)=0"<<finl;
      if (je_suis_maitre())
        la_matrice(0,0) *= 2;
      la_matrice.set_est_definie(1);
    }

  Cerr << "Fin de l'assemblage de la matrice de pression" << finl;
  return 1;
}

int Assembleur_P_EF::modifier_secmem(DoubleTab& secmem)
{
  Debog::verifier("secmem dans modifier secmem",secmem);

  const Zone_EF& la_zone = la_zone_EF.valeur();
  const Zone_Cl_EF& la_zone_cl = la_zone_Cl_EF.valeur();
  int nb_cond_lim = la_zone_cl.nb_cond_lim();
  const IntTab& face_voisins = la_zone.face_voisins();

  // Modification du second membre :
  int i;
  for (i=0; i<nb_cond_lim; i++)
    {
      const Cond_lim_base& la_cl_base = la_zone_cl.les_conditions_limites(i).valeur();
      const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl_base.frontiere_dis());
      int ndeb = la_front_dis.num_premiere_face();
      int nfin = ndeb + la_front_dis.nb_faces();


      // GF on est passe en increment de pression
      if ((sub_type(Neumann_sortie_libre,la_cl_base)) && (!get_resoudre_increment_pression()))
        {
          double Pimp, coef;
          const Neumann_sortie_libre& la_cl_Neumann = ref_cast(Neumann_sortie_libre, la_cl_base);
          // const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl_base.frontiere_dis());
          //int ndeb = la_front_dis.num_premiere_face();
          //int nfin = ndeb + la_front_dis.nb_faces();
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              Pimp = la_cl_Neumann.flux_impose(num_face-ndeb);
              coef = les_coeff_pression[num_face]*Pimp;
              secmem[face_voisins(num_face,0)] += coef;
            }
        }
      else if (sub_type(Champ_front_instationnaire_base,
                        la_cl_base.champ_front().valeur())&&(get_resoudre_en_u()))
        {
          if (sub_type(Dirichlet,la_cl_base))
            {
              // Cas Dirichlet variable dans le temps
              // N'est utile que pour des champs front. variables dans le temps
              const Champ_front_instationnaire_base& le_ch_front =
                ref_cast( Champ_front_instationnaire_base,
                          la_cl_base.champ_front().valeur());
              const DoubleTab& Gpt = le_ch_front.Gpoint();

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  //for num_face
                  double Stt = 0.;
                  for (int k=0; k<dimension; k++)
                    Stt -= Gpt(k) * la_zone.face_normales(num_face, k);
                  secmem(face_voisins(num_face,0)) += Stt;
                }
            }
        }
      else if (sub_type(Champ_front_var_instationnaire,
                        la_cl_base.champ_front().valeur())&&(get_resoudre_en_u()))
        {
          if (sub_type(Dirichlet,la_cl_base))
            {
              //cas instationaire et variable
              const Champ_front_var_instationnaire& le_ch_front =
                ref_cast( Champ_front_var_instationnaire, la_cl_base.champ_front().valeur());
              const DoubleTab& Gpt = le_ch_front.Gpoint();

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  double Stt = 0.;
                  for (int k=0; k<dimension; k++)
                    Stt -= Gpt(num_face - ndeb, k) *
                           la_zone.face_normales(num_face, k);
                  secmem(face_voisins(num_face,0)) += Stt;
                }
            }
        }
    }
  secmem.echange_espace_virtuel();
  Debog::verifier("secmem dans modifier secmem fin",secmem);
  return 1;
}

int Assembleur_P_EF::modifier_solution(DoubleTab& pression)
{
  Debog::verifier("pression dans modifier solution in",pression);
  // Projection :
  double press_0;
  if(!has_P_ref)
    {
      //abort();
      // On prend la pression minimale comme pression de reference
      // afin d'avoir la meme pression de reference en sequentiel et parallele
      press_0=DMAXFLOAT;
      int n,nb_elem=la_zone_EF.valeur().zone().nb_elem();
      for(n=0; n<nb_elem; n++)
        if (pression[n] < press_0)
          press_0 = pression[n];
      press_0 = Process::mp_min(press_0);

      for(n=0; n<nb_elem; n++)
        pression[n] -=press_0;

      pression.echange_espace_virtuel();
    }
  return 1;
}

const Zone_dis_base& Assembleur_P_EF::zone_dis_base() const
{
  return la_zone_EF.valeur();
}

const Zone_Cl_dis_base& Assembleur_P_EF::zone_Cl_dis_base() const
{
  return la_zone_Cl_EF.valeur();
}

void Assembleur_P_EF::associer_zone_dis_base(const Zone_dis_base& la_zone_dis)
{
  la_zone_EF = ref_cast(Zone_EF,la_zone_dis);
}

void Assembleur_P_EF::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis)
{
  la_zone_Cl_EF = ref_cast(Zone_Cl_EF, la_zone_Cl_dis);
}

void Assembleur_P_EF::completer(const Equation_base& Eqn)
{
  mon_equation=Eqn;
}
