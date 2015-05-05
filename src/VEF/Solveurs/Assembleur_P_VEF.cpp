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
// File:        Assembleur_P_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Solveurs
// Version:     /main/36
//
//////////////////////////////////////////////////////////////////////////////

#include <Assembleur_P_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Zone_VEF.h>
#include <Les_Cl.h>
#include <Champ_front_instationnaire_base.h>
#include <Champ_front_var_instationnaire.h>
#include <Matrice_Bloc.h>
#include <Debog.h>
#include <Scatter.h>

Implemente_instanciable(Assembleur_P_VEF,"Assembleur_P_VEF",Assembleur_base);

Sortie& Assembleur_P_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom() ;
}

Entree& Assembleur_P_VEF::readOn(Entree& s )
{
  return Assembleur_base::readOn(s);
}

int Assembleur_P_VEF::assembler(Matrice& la_matrice)
{
  // Si rho est constant, on resout avec la pression P*=P/rho
  const DoubleVect& volumes_entrelaces_ref=la_zone_VEF->volumes_entrelaces();
  DoubleVect volumes_entrelaces(volumes_entrelaces_ref);
  const DoubleVect& volumes_entrelaces_cl=la_zone_Cl_VEF.valeur().volumes_entrelaces_Cl();
  int size=volumes_entrelaces_cl.size();
  for (int f=0; f<size; f++)
    if (volumes_entrelaces_cl(f)!=0)
      {
        //        if (volumes_entrelaces(f)!=volumes_entrelaces_cl(f))   Cerr<<f <<" vl" << volumes_entrelaces(f)<< " "<<volumes_entrelaces_cl(f)<<finl;
        volumes_entrelaces(f)=volumes_entrelaces_cl(f);
      }

  volumes_entrelaces.echange_espace_virtuel();
  // On assemble la matrice
  return assembler_mat(la_matrice,volumes_entrelaces,1,1);
}



void calculer_inv_volume_special(DoubleTab& inv_volumes_entrelaces, const Zone_Cl_VEF& zone_Cl_VEF,const DoubleTab& volumes_entrelaces)
{
  inv_volumes_entrelaces=volumes_entrelaces;
  int taille=volumes_entrelaces.dimension_tot(0);
  for (int i=0; i<taille; i++)
    for (int comp=0; comp<Objet_U::dimension; comp++)
      inv_volumes_entrelaces(i,comp)=1./volumes_entrelaces(i,comp);

}
void Assembleur_P_VEF::calculer_inv_volume(DoubleTab& inv_volumes_entrelaces, const Zone_Cl_VEF& zone_Cl_VEF,const DoubleVect& volumes_entrelaces)
{
  // maintenant l 'inverse du volume est un DoubleTab
  // c'est pour faire fonctionner le Piso

  if (sub_type(DoubleTab,volumes_entrelaces))
    {
      calculer_inv_volume_special(inv_volumes_entrelaces, zone_Cl_VEF,ref_cast(DoubleTab, volumes_entrelaces));


      return;
    }
  int taille=volumes_entrelaces.size_totale();
  inv_volumes_entrelaces.resize(taille,Objet_U::dimension);
  if (0)
    {
      // cette facon de calculer le volume entrelace pourra etre bonne dans
      // l'avenir a condition de reflechir au pb des porosites
      // et SURTOUT au pb du simpler ou l'on ne veut pas passer par la
      DoubleTab tmp;
      tmp=(inv_volumes_entrelaces);
      tmp=1;
      zone_Cl_VEF.equation().solv_masse().appliquer(tmp);
      int sz=inv_volumes_entrelaces.size_totale();
      for (int i=0; i<sz; i++)
        {
          inv_volumes_entrelaces(i)=tmp(i);
          if (!est_egal(inv_volumes_entrelaces(i),1./volumes_entrelaces(i)))
            Cerr<<i<<" "<<inv_volumes_entrelaces(i)-1./volumes_entrelaces(i)<<" "<<inv_volumes_entrelaces(i)<<finl;;

        }
      Process::exit();

    }
  else
    {
      const DoubleVect& porosite_face=la_zone_VEF->porosite_face();
      for (int i=0; i<taille; i++)
        for (int comp=0; comp<Objet_U::dimension; comp++)
          inv_volumes_entrelaces(i,comp)=1./volumes_entrelaces(i)*porosite_face(i);
    }
}


int Assembleur_P_VEF::assembler_mat(Matrice& la_matrice, const DoubleVect& volumes_entrelaces, int incr_pression, int resoudre_en_u)
{
  // On fixe les drapeaux de Assembleur_base
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);
  DoubleTab inv_volumes_entrelaces;
  const Zone_Cl_VEF& zone_Cl_VEF = la_zone_Cl_VEF.valeur();
  DoubleTab inverse_quantitee_entrelacee;
  calculer_inv_volume(inverse_quantitee_entrelacee, zone_Cl_VEF, volumes_entrelaces);
  remplir(la_matrice, inverse_quantitee_entrelacee);
  modifier_matrice(la_matrice);
  return 1;
}

int Assembleur_P_VEF::remplir(Matrice& la_matrice, const DoubleTab& inverse_quantitee_entrelacee)
{
  has_P_ref=0;
  // Matrice de pression :matrice creuse de taille nb_poly x nb_poly
  // Cette fonction range la matrice dans une structure de matrice morse
  // bien adaptee aux matrices creuses.
  // On commence par calculer les tailles des tableaux tab1 et tab2
  // (coeff_ a la meme taille que tab2)
  //   A chaque polyedre on associe :
  //   - une liste d'entiers voisins[i] = {j>i t.q Mij est non nul }
  //   - une liste de reels  valeurs[i] = {Mij pour j dans Voisins[i]}
  //   - un reel terme_diag
  // Implementation temporaire:
  // On assemble une matrice de pression pour une equation d'hydraulique
  // On injecte dans cette matrice les conditions aux limites
  // On peut faire cela car a priori la matrice de pression n'est pas
  // partagee par plusieurs equations sur une meme zone.

  const Zone_VEF& la_zone = la_zone_VEF.valeur();
  const Zone_Cl_VEF& la_zone_cl = la_zone_Cl_VEF.valeur();
  les_coeff_pression.resize(la_zone_cl.nb_faces_Cl());
  int n1 = la_zone.zone().nb_elem_tot();
  int n2 = la_zone.zone().nb_elem();
  int elem1,elem2;
  double val;
  int i;

  const IntTab& face_voisins = la_zone.face_voisins();
  const DoubleTab& face_normales = la_zone.face_normales();
  const Conds_lim& les_cl = la_zone_cl.les_conditions_limites();

  // int premiere_face_std=la_zone.premiere_face_std();
  // Rajout des porosites.
  //const DoubleVect& porosite_face = la_zone.porosite_face();

  la_matrice.typer("Matrice_Bloc"); // En fait Matrice_Bloc_Sym ?
  Matrice_Bloc& matrice=ref_cast(Matrice_Bloc, la_matrice.valeur());
  matrice.dimensionner(2,2);
  matrice.get_bloc(0,0).typer("Matrice_Morse_Sym");
  matrice.get_bloc(0,1).typer("Matrice_Morse");
  matrice.get_bloc(1,0).typer("Matrice_Morse");
  matrice.get_bloc(1,1).typer("Matrice_Morse"); // En fait Matrice_Morse_Sym ?

  Matrice_Morse_Sym& MBrr = ref_cast(Matrice_Morse_Sym,matrice.get_bloc(0,0).valeur());
  Matrice_Morse& MBrv = ref_cast (Matrice_Morse,matrice.get_bloc(0,1).valeur());
  Matrice_Morse& MBvr = ref_cast (Matrice_Morse, matrice.get_bloc(1,0).valeur());
  Matrice_Morse& MBvv = ref_cast (Matrice_Morse, matrice.get_bloc(1,1).valeur());

  MBrr.dimensionner(n2,0);
  MBrv.dimensionner(n2,0);
  MBvv.dimensionner(n1-n2,0);

  // Le sous blocs vr est dimensionne et nul
  MBvr.dimensionner(n1-n2,n2,0);
  MBvr.tab1_=1;

  IntVect& tab1RR=MBrr.tab1_;
  IntVect& tab2RR=MBrr.tab2_;
  DoubleVect& coeffRR=MBrr.coeff_;
  IntVect& tab1RV=MBrv.tab1_;
  IntVect& tab2RV=MBrv.tab2_;
  DoubleVect& coeffRV=MBrv.coeff_;
  IntVect& tab1VV=MBvv.tab1_;
  IntVect& tab2VV=MBvv.tab2_;
  DoubleVect& coeffVV=MBvv.coeff_;

  // On traite les faces internes:

  int ndeb = la_zone_VEF->premiere_face_int();
  int nfin = la_zone_VEF->nb_faces_tot();
  IntVect rang_voisinRR(n2);
  IntVect rang_voisinRV(n2);
  IntVect rang_voisinVV(n1-n2);
  rang_voisinRR=1; // Diagonale
  rang_voisinRV=0; // Pas de diagonale
  rang_voisinVV=1; // Diagonale

  has_P_ref=0;

  for (int num_face=ndeb; num_face<nfin; num_face++)
    {
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);
      if (!la_zone_VEF->est_une_face_virt_bord(num_face) && elem1 != -1 && elem2 != -1)
        {
          if (elem1 > elem2)
            {
              if(elem1<n2)
                {
                  (rang_voisinRR(elem2))++;
                }
              else
                {
                  if(elem2<n2)
                    {
                      (rang_voisinRV(elem2))++;
                    }
                  else
                    {
                      (rang_voisinVV(elem2-n2))++;
                    }
                }
            }
          else // elem2 >= elem1
            {
              if(elem2<n2)
                {
                  (rang_voisinRR(elem1))++;
                }
              else
                {
                  if(elem1<n2)
                    {
                      (rang_voisinRV(elem1))++;
                    }
                  else
                    {
                      (rang_voisinVV(elem1-n2))++;
                    }
                }
            }
        }
    }

  // Prise en compte des conditions de type periodicite
  for (i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int nb_faces_bord_tot = le_bord.nb_faces_tot();
          IntVect fait(nb_faces_bord_tot);
          fait = 0;
          for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              if (fait[ind_face] == 0)
                {
                  fait[ind_face] = 1;
                  fait[la_cl_perio.face_associee(ind_face)] = 1;
                  elem1 = face_voisins(num_face,0);
                  elem2 = face_voisins(num_face,1);
                  if (elem1 !=-1 && elem2 != -1)
                    {
                      if (elem1 > elem2)
                        {
                          if(elem1<n2)
                            {
                              (rang_voisinRR(elem2))++;
                            }
                          else
                            {
                              if(elem2<n2)
                                {
                                  (rang_voisinRV(elem2))++;
                                }
                              else
                                {
                                  (rang_voisinVV(elem2-n2))++;
                                }
                            }
                        }
                      else // elem2 >= elem1
                        {
                          if(elem2<n2)
                            {
                              (rang_voisinRR(elem1))++;
                            }
                          else
                            {
                              if(elem1<n2)
                                {
                                  (rang_voisinRV(elem1))++;
                                }
                              else
                                {
                                  (rang_voisinVV(elem1-n2))++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

  tab1RR(0)=1;
  tab1RV(0)=1;
  tab1VV(0)=1;

  for(i=0; i<n2; i++)
    {
      tab1RR(i+1)=rang_voisinRR(i)+tab1RR(i);
      tab1RV(i+1)=rang_voisinRV(i)+tab1RV(i);
    }
  for(i=0; i<n1-n2; i++)
    {
      tab1VV(i+1)=rang_voisinVV(i)+tab1VV(i);
    }
  MBrr.dimensionner(n2,tab1RR(n2)-1);
  MBrv.dimensionner(n2,n1-n2,tab1RV(n2)-1);
  MBvv.dimensionner(n1-n2,n1-n2,tab1VV(n1-n2)-1);

  for(i=0; i<n2; i++)
    {
      tab2RR[tab1RR[i]-1]=i+1; // Diagonale
      rang_voisinRR[i]=tab1RR[i];
      rang_voisinRV[i]=tab1RV[i]-1;
    }
  for(i=0; i<n1-n2; i++)
    {
      tab2VV[tab1VV[i]-1]=i+1; // Diagonale
      rang_voisinVV[i]=tab1VV[i];
    }
  int affichage_progression=0;
  int pourcent=0;
  for (int num_face=ndeb; num_face<nfin; num_face++)
    {
      if (affichage_progression)
        {
          int tmp =(num_face*100)/nfin;
          if(tmp>pourcent)
            {
              double dpourcent = tmp;
              Cerr << dpourcent << "% de la matrice de pression P0 a ete assemblee\r " << flush;
            }
        }
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);
      // On ne traite que les faces internes virtuelles ou non
      if (!la_zone_VEF->est_une_face_virt_bord(num_face) && elem1 != -1 && elem2 != -1)
        {
          if(dimension==2)
            {
              val = (face_normales(num_face,0)*face_normales(num_face,0)*inverse_quantitee_entrelacee(num_face,0)
                     + face_normales(num_face,1)*face_normales(num_face,1)*inverse_quantitee_entrelacee(num_face,1));
            }
          else
            {
              val = (face_normales(num_face,0)*face_normales(num_face,0)*inverse_quantitee_entrelacee(num_face,0)
                     +face_normales(num_face,1)*face_normales(num_face,1)*inverse_quantitee_entrelacee(num_face,1)
                     +face_normales(num_face,2)*face_normales(num_face,2)*inverse_quantitee_entrelacee(num_face,2));
            }
          //if(num_face<premiere_face_std)
          // val*=volumes_entrelaces(num_face)/volumes_entrelaces_Cl(num_face);

          // diagonale :
          if(elem1<n2) coeffRR[tab1RR[elem1]-1]+= val;
          else coeffVV[tab1VV[elem1-n2]-1]+= val;
          if(elem2<n2) coeffRR[tab1RR[elem2]-1]+= val;
          else coeffVV[tab1VV[elem2-n2]-1]+= val;
          if (elem1 > elem2)
            {
              if(elem1<n2)
                {
                  tab2RR[rang_voisinRR[elem2]]=elem1+1;
                  coeffRR[rang_voisinRR[elem2]]-=val;
                  rang_voisinRR[elem2]++;
                }
              else
                {
                  if(elem2<n2)
                    {
                      tab2RV[rang_voisinRV[elem2]]=(elem1-n2)+1;
                      coeffRV[rang_voisinRV[elem2]]-=val;
                      rang_voisinRV[elem2]++;
                    }
                  else
                    {
                      tab2VV[rang_voisinVV[elem2-n2]]=(elem1-n2)+1;
                      coeffVV[rang_voisinVV[elem2-n2]]-=val;
                      rang_voisinVV[elem2-n2]++;
                    }
                }
            }
          else
            {
              if(elem2<n2)
                {
                  tab2RR[rang_voisinRR[elem1]]=elem2+1;
                  coeffRR[rang_voisinRR[elem1]]-=val;
                  rang_voisinRR[elem1]++;
                }
              else
                {
                  if(elem1<n2)
                    {
                      tab2RV[rang_voisinRV[elem1]]=(elem2-n2)+1;
                      coeffRV[rang_voisinRV[elem1]]-=val;
                      rang_voisinRV[elem1]++;
                    }
                  else
                    {
                      tab2VV[rang_voisinVV[elem1-n2]]=(elem2-n2)+1;
                      coeffVV[rang_voisinVV[elem1-n2]]-=val;
                      rang_voisinVV[elem1-n2]++;
                    }
                }
            }
        }
    }
  if (affichage_progression) Cerr << finl;
  // On traite les conditions aux limites
  for (i=0; i<les_cl.size(); i++)
    {

      // Le traitement depend du type de la condition aux limites :
      //  - Si condition de Neumann_sortie_libre
      //  il faut calculer le coefficient sur la face et le prendre
      //  en compte dans le terme diagonal associe a l'element voisin.
      // - Si face de Cl avec une autre condition aux limites pas de
      // contribution a la matrice de pression.

      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot();
      if (sub_type(Neumann_sortie_libre,la_cl.valeur()) )
        {
          has_P_ref=1;
          for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              if(dimension==2)
                {
                  val = ( face_normales(num_face,0)*face_normales(num_face,0)*inverse_quantitee_entrelacee(num_face,0)
                          +face_normales(num_face,1)*face_normales(num_face,1)*inverse_quantitee_entrelacee(num_face,1));
                }
              else
                {
                  val = ( face_normales(num_face,0)*face_normales(num_face,0)*inverse_quantitee_entrelacee(num_face,0)
                          +face_normales(num_face,1)*face_normales(num_face,1)*inverse_quantitee_entrelacee(num_face,1)
                          +face_normales(num_face,2)*face_normales(num_face,2)*inverse_quantitee_entrelacee(num_face,2));
                }
              //if(num_face<premiere_face_std)
              //  val*=volumes_entrelaces(num_face)/volumes_entrelaces_Cl(num_face);

              int elem=face_voisins(num_face,0);
              if(elem<n2)
                coeffRR[tab1RR[elem]-1] += val;
              else
                coeffVV[tab1VV[elem-n2]-1] += val;
              // On stocke les coefficients de pression sur les faces reelles
              if (num_face<les_coeff_pression.size_array())
                les_coeff_pression[num_face] = val;
            }
        }

      else if (sub_type(Periodique,la_cl.valeur()) )
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          IntVect fait(nb_faces_bord_tot);
          fait = 0;

          for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              if (fait[ind_face] == 0)
                {
                  fait[ind_face] = 1;
                  fait[la_cl_perio.face_associee(ind_face)] = 1;
                  elem1 = face_voisins(num_face,0);
                  elem2 = face_voisins(num_face,1);
                  if(dimension==2)
                    {
                      val = (face_normales(num_face,0)*face_normales(num_face,0)*inverse_quantitee_entrelacee(num_face,0)
                             +face_normales(num_face,1)*face_normales(num_face,1)*inverse_quantitee_entrelacee(num_face,1));
                    }
                  else
                    {
                      val = (face_normales(num_face,0)*face_normales(num_face,0)*inverse_quantitee_entrelacee(num_face,0)
                             +face_normales(num_face,1)*face_normales(num_face,1)*inverse_quantitee_entrelacee(num_face,1)
                             +face_normales(num_face,2)*face_normales(num_face,2)*inverse_quantitee_entrelacee(num_face,2));
                    }
                  //if(num_face<premiere_face_std)
                  //  val*=volumes_entrelaces(num_face)/volumes_entrelaces_Cl(num_face);
                  // diagonale :
                  if(elem1<n2)
                    coeffRR[tab1RR[elem1]-1]+= val;
                  else
                    coeffVV[tab1VV[elem1-n2]-1]+= val;
                  if(elem2<n2)
                    coeffRR[tab1RR[elem2]-1]+= val;
                  else
                    coeffVV[tab1VV[elem2-n2]-1]+= val;
                  if (elem1 > elem2)
                    {
                      if(elem1<n2)
                        {
                          tab2RR[rang_voisinRR[elem2]]=elem1+1;
                          coeffRR[rang_voisinRR[elem2]]-=val;
                          rang_voisinRR[elem2]++;
                        }
                      else
                        {
                          if(elem2<n2)
                            {
                              tab2RV[rang_voisinRV[elem2]]=elem1-n2+1;
                              coeffRV[rang_voisinRV[elem2]]-=val;
                              rang_voisinRV[elem2]++;
                            }
                          else
                            {
                              tab2VV[rang_voisinVV[elem2-n2]]=elem1-n2+1;
                              coeffVV[rang_voisinVV[elem2-n2]]-=val;
                              rang_voisinVV[elem2-n2]++;
                            }
                        }
                    }
                  else
                    {
                      if(elem2<n2)
                        {
                          tab2RR[rang_voisinRR[elem1]]=elem2+1;
                          coeffRR[rang_voisinRR[elem1]]-=val;
                          rang_voisinRR[elem1]++;
                        }
                      else
                        {
                          if(elem1<n2)
                            {
                              tab2RV[rang_voisinRV[elem1]]=elem2-n2+1;
                              coeffRV[rang_voisinRV[elem1]]-=val;
                              rang_voisinRV[elem1]++;
                            }
                          else
                            {
                              tab2VV[rang_voisinVV[elem1-n2]]=elem2-n2+1;
                              coeffVV[rang_voisinVV[elem1-n2]]-=val;
                              rang_voisinVV[elem1-n2]++;
                            }
                        }
                    }
                }
            }
        }
    }
  has_P_ref = (int)mp_max(has_P_ref);
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
int Assembleur_P_VEF::assembler_QC(const DoubleTab& tab_rho, Matrice& matrice)
{
  Cerr << "Assemblage de la matrice de pression pour Quasi Compressible en cours..." << finl;
  int stat=assembler(matrice);
  set_resoudre_en_u(0);
  return stat;
}

int Assembleur_P_VEF::modifier_secmem(DoubleTab& secmem)
{
  const Zone_VEF& la_zone = la_zone_VEF.valeur();
  const Zone_Cl_VEF& la_zone_cl = la_zone_Cl_VEF.valeur();
  int nb_cond_lim = la_zone_cl.nb_cond_lim();
  const IntTab& face_voisins = la_zone.face_voisins();

  // Modification du second membre :
  for (int i=0; i<nb_cond_lim; i++)
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
          //const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl_base.frontiere_dis());
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
      else if (sub_type(Champ_front_var_instationnaire,
                        la_cl_base.champ_front().valeur())&&(get_resoudre_en_u()))
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
  secmem.echange_espace_virtuel();
  return 1;
}

int Assembleur_P_VEF::modifier_solution(DoubleTab& pression)
{
  // Projection :
  double press_0;
  if(!has_P_ref)
    {
      // On prend la pression minimale comme pression de reference
      // afin d'avoir la meme pression de reference en sequentiel et parallele
      press_0=DMAXFLOAT;
      int n,nb_elem=la_zone_VEF.valeur().zone().nb_elem();
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

// Description:
//    Modifier eventuellement la matrice pour la rendre definie si elle ne l'est pas
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture
// Retour: int
//    Signification: renvoie 1 si la matrice est modifiee 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Assembleur_P_VEF::modifier_matrice(Matrice& matrice)
{
  int matrice_modifiee=0;
  Matrice_Bloc& mat_bloc = ref_cast(Matrice_Bloc, matrice.valeur());
  Matrice_Morse_Sym& A00RR = ref_cast(Matrice_Morse_Sym,mat_bloc.get_bloc(0,0).valeur());
  // Recherche de l'element sur lequel on impose la pression de reference
  if (Process::je_suis_maitre() && !has_P_ref)
    {
      int element_referent=0;
      double distance=DMAXFLOAT;
      const DoubleTab& coord=la_zone_VEF->xp();
      int n = la_zone_VEF.valeur().nb_elem();
      for(int i=0; i<n; i++)
        {
          double tmp=0;
          for (int j=0; j<dimension; j++)
            tmp+=coord(i,j)*coord(i,j);
          if (inf_strict(tmp,distance) && !est_egal(A00RR(i,i),0.))
            {
              distance=tmp;
              element_referent=i;
            }
        }
      //Cerr << "On modifie la ligne (element) " << element_referent << finl;
      A00RR(element_referent,element_referent)*=2;
      matrice_modifiee=1;
    }
  has_P_ref=1;
  A00RR.set_est_definie(1);
  return matrice_modifiee;
}

const Zone_dis_base& Assembleur_P_VEF::zone_dis_base() const
{
  return la_zone_VEF.valeur();
}

const Zone_Cl_dis_base& Assembleur_P_VEF::zone_Cl_dis_base() const
{
  return la_zone_Cl_VEF.valeur();
}

void Assembleur_P_VEF::associer_zone_dis_base(const Zone_dis_base& la_zone_dis)
{
  la_zone_VEF = ref_cast(Zone_VEF,la_zone_dis);
}

void Assembleur_P_VEF::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis)
{
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF, la_zone_Cl_dis);
}

void Assembleur_P_VEF::completer(const Equation_base& Eqn)
{
  mon_equation=Eqn;
}
