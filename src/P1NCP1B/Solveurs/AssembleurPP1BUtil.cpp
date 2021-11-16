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
//////////////////////////////////////////////////////////////////////////////
//
// File:        AssembleurPP1BUtil.cpp
// Directory:   $TRUST_ROOT/src/P1NCP1B/Solveurs
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Assembleur_P_VEFPreP1B.h>
#include <Matrice_Bloc_Sym.h>
#include <Domaine.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Periodique.h>
#include <Neumann_sortie_libre.h>
#include <Symetrie.h>
#include <LecFicDistribueBin.h>
#include <EcrFicCollecteBin.h>
#include <DoubleLists.h>
#include <Navier_Stokes_std.h>
#include <Op_Div_VEFP1B_Elem.h>
#include <Champ_front_instationnaire_base.h>
#include <Champ_front_var.h>
#include <Op_Grad_VEF_P1B_Face.h>
#include <Milieu_base.h>
#include <IntLists.h>
#include <Debog.h>
#include <Check_espace_virtuel.h>
#include <Scatter.h>
#include <communications.h>

static int face_associee=-1;

//static ArrOfDouble coef_som;

static inline void
projette(ArrOfDouble& gradi, int face, const DoubleTab& normales)
{
  double psc=0, norm=0;
  int dimension=Objet_U::dimension, comp;
  for(comp=0; comp<dimension; comp++)
    {
      psc+=gradi(comp)*normales(face,comp);
      norm+=normales(face,comp)*normales(face,comp);
    }
  // psc/=norm; // Fixed bug: Arithmetic exception
  if (fabs(norm)>=DMINFLOAT) psc/=norm;
  for(comp=0; comp<dimension; comp++)
    {
      gradi(comp)-=psc*normales(face,comp);
    }
  psc=0;
  //   for(comp=0; comp<dimension; comp++)
  //     {
  //       psc+=gradi(comp)*normales(face,comp);
  //     }
  //   assert(psc < 1.e-10);
}
//

// renvoie la premiere face non Dirichlet
//                            2 si Perio
//                            3 si Neumann
//                            4 si Symetrie
//                            1 sinon
// et face_associee=-1 sauf si perio (face_associee=face associee)
static inline int okface(int& ind_face, int& face, const Cond_lim& la_cl)
{
  face_associee=-1;
  int ok=1;
  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
  int nb_faces_bord_tot = le_bord.nb_faces_tot();
  do
    {
      face=le_bord.num_face(ind_face);
      if ((sub_type(Dirichlet, la_cl.valeur()))
          || (sub_type(Dirichlet_homogene, la_cl.valeur())))
        {
          ok=0;
        }
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          //periodicite
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          face_associee=le_bord.num_face(la_cl_perio.face_associee(ind_face));
          ok=2;
        }
      else if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
        {
          //sortie_libre
          ok=3;
        }
      else if (sub_type(Symetrie, la_cl.valeur()))
        {
          //symetrie
          ok=4;
        }
    }
  while ( ( (ok==0) || ((ok==2)&&(face_associee<face)) ) && (++ind_face<nb_faces_bord_tot) );
  if (ind_face==nb_faces_bord_tot) ok=-1;
  return ok;
}

inline int verifier_complet(const Assembleur_P_VEFPreP1B& ass,
                            const Matrice_Bloc_Sym& matrice,
                            const Zone_VEF_PreP1b& zone_VEF)
{
  const Navier_Stokes_std& eqn=ref_cast(Navier_Stokes_std, ass.equation());
  const Operateur_Div& opdiv=eqn.operateur_divergence();
  const Op_Div_VEFP1B_Elem& div=ref_cast(Op_Div_VEFP1B_Elem,
                                         opdiv.valeur());
  //div.verifier();
  const Operateur_Grad& opgrad=eqn.operateur_gradient();
  const Op_Grad_VEF_P1B_Face& grad=ref_cast(Op_Grad_VEF_P1B_Face,
                                            opgrad.valeur());
  //grad.verifier();
  const Solveur_Masse& solvm=eqn.solv_masse();
  const DoubleTab& pression=eqn.pression().valeurs();
  DoubleTab tab(pression);
  DoubleTab resu(tab), resu2(tab), erreur(tab);

  // On calcule un champ de pression quelconque
  exemple_champ_non_homogene(zone_VEF, tab);

  DoubleTab gradP(eqn.inconnue().valeurs());
  grad.calculer(tab, gradP);
  solvm.appliquer(gradP);
  div.calculer(gradP, resu);
  matrice.multvect(tab, resu2);
  return 0;
}
int verifier( const Assembleur_P_VEFPreP1B& ass,
              const Matrice_Bloc_Sym& matrice,
              const Zone_VEF_PreP1b& zone_VEF,
              const DoubleTab& inverse_quantitee_entrelacee)
{
  if (zone_VEF.get_alphaE()+zone_VEF.get_alphaS()+zone_VEF.get_alphaA()!=Objet_U::dimension)
    {
      Cerr << "Assembleur_P_VEFPreP1B::verifier n'est pas prevu pour verifier votre discretisation." << finl;
      Process::exit();
    }
  const Navier_Stokes_std& eqn=ref_cast(Navier_Stokes_std, ass.equation());
  const Operateur_Div& opdiv=eqn.operateur_divergence();
  const Op_Div_VEFP1B_Elem& div=ref_cast(Op_Div_VEFP1B_Elem,
                                         opdiv.valeur());
  //div.verifier();
  const Operateur_Grad& opgrad=eqn.operateur_gradient();
  const Op_Grad_VEF_P1B_Face& grad=ref_cast(Op_Grad_VEF_P1B_Face,
                                            opgrad.valeur());
  //grad.verifier();
  //  const Solveur_Masse& solvm=eqn.solv_masse();
  const DoubleTab& pression=eqn.pression().valeurs();
  int ko=0;
  ko=verifier_complet(ass, matrice, zone_VEF);
  DoubleTab pre(pression);
  DoubleTab resu(pre), resu2(pre), erreur(pre);
  DoubleTab gradP(eqn.inconnue().valeurs());
  const Zone& zone=zone_VEF.zone();
  int nb_elem=zone.nb_elem();
  int nb_elem_tot=zone.nb_elem_tot();
  int nb_som=zone.domaine().nb_som();
  int nb_som_tot=zone.domaine().nb_som_tot();
  int nb_aretes=zone.nb_aretes();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  for (int proc=0; proc<Process::nproc(); proc++)
    {
      int n = pre.dimension_tot(0);
      // Le processeur proc impose sa valeur de n a tout le monde
      envoyer_broadcast(n, proc);

      for(int i=0; i<n; i++)
        {
          //Cerr << "[" << Process::me() << "] On verifie la ligne " << i << " de la matrice." << finl;
          pre=0;
          if (Process::me()==proc)
            {
              if (0<=i && i<nb_elem)
                {
                  Cerr << "On verifie l'element reel " << i << " du processeur " << proc;
                  pre(i)=1;
                }
              else if (nb_elem_tot<=i && i<nb_elem_tot+nb_som)
                {
                  int sommet=i-nb_elem_tot;
                  Cerr << "On verifie le sommet reel ";
                  if (zone.domaine().get_renum_som_perio(sommet)!=sommet) Cerr << "periodique ";
                  Cerr << i-nb_elem_tot << " du processeur " << proc;
                  pre(i)=1;
                }
              else if (nb_elem_tot+nb_som_tot<=i && i<nb_elem_tot+nb_som_tot+nb_aretes && ok_arete(i-nb_elem_tot-nb_som_tot))
                {
                  int arete=i-nb_elem_tot-nb_som_tot;
                  const ArrOfInt& renum_arete_perio=zone_VEF.get_renum_arete_perio();
                  if (renum_arete_perio(arete)==arete)
                    {
                      Cerr << "On verifie l'arete reelle non superflue et non periodique " << arete << " du processeur " << proc;
                      pre(i)=1;
                    }
                }
              Cerr << " ";
              //Cerr << finl;
            }
          double verifie=mp_max_vect(pre);
          //verifie=1;pre(i)=1; // On teste tout
          if (verifie)
            {
              pre.echange_espace_virtuel();
              /* Inutile le debog ce n'est pas comparable le sequentiel et le parallele
                 Nom ch;
                 ch="pre pour la ligne ";
                 ch+=(Nom)i+" du processeur ";
                 ch+=(Nom)proc+" =";
                 Debog::verifier(ch,pre);
              */
              // Calcul par Div(Grad(P))
              grad.calculer(pre, gradP);
              {
                int nbf=inverse_quantitee_entrelacee.size();
                int d = Objet_U::dimension;
                for (int face=0; face<nbf; face++)
                  for(int k=0; k<d; k++)
                    gradP(face,k)*=inverse_quantitee_entrelacee(face,k);
              }
              //solvm.appliquer(gradP);
              div.calculer(gradP, resu);
              // Calcul par -Lap(P)
              matrice.multvect(pre, resu2);
              // On doit trouver erreur nul
              erreur=resu2;
              erreur+=resu;
              resu*=-1;
              // Cas ou la diagonale est *2, on corrige:
              if (est_egal(2*resu(i),resu2(i))) erreur(i)=0;
              double erreur_absolue=mp_norme_vect(erreur);
              double erreur_relative=erreur_absolue/(mp_norme_vect(resu2)+mp_norme_vect(resu)+DMINFLOAT);
              double app=mp_prodscal(resu,pre);
              if(erreur_absolue>1.e-12 && erreur_relative>1.e-6)
                {
                  Cerr << "[" << Process::me() << "] KO a la ligne " << i << " pour le proc " << proc << " (AP,P)= " << app << " erreur= " << erreur_absolue << finl;
                  ko=1;
                  Cerr << "[" << Process::me() << "] pre= ";
                  pre.ecrit(Cerr);
                  Cerr << "[" << Process::me() << "] Div(gradP) = ";
                  resu.ecrit(Cerr);
                  Cerr << "[" << Process::me() << "] Lap(P) = ";
                  resu2.ecrit(Cerr);
                  Cerr << "[" << Process::me() << "] erreur = ";
                  erreur.ecrit(Cerr);
                  Cerr << "[" << Process::me() << "] invqtentrelacee = ";
                  inverse_quantitee_entrelacee.ecrit(Cerr);
                }
              else
                {
                  Cerr << "[" << Process::me() << "] OK a la ligne " << i << " pour le proc " << proc << " (AP,P)= " << app << " erreur= " << erreur_absolue << finl;
                }
            }
        }
    }
  if (ko)
    {
      Cerr << "[" << Process::me() << "] Matrice en pression:" << finl;
      matrice.imprimer_formatte(Cerr);
      Cerr << "Echec de la methode verifier de l'assembleur. Voir les KO." << finl;
      Process::exit();
    }
  return 1;
}
//
// trie le tableau sommets dans l'ordre croissant et
// faces_op1 et faces_op2 consequemment.
//
static inline void sort( ArrOfInt& sommets, ArrOfInt& faces_op1, ArrOfInt& faces_op2)
{
  int sz=sommets.size_array();
  if(sommets(sz-1)==-1) sz--;
  int i,j;
  for(i=0; i<sz; i++)
    for(j=i; j<sz; j++)
      if(sommets(i)>sommets(j))
        {
          int tmp=sommets(i);
          sommets(i)=sommets(j);
          sommets(j)=tmp;
          tmp=faces_op1(i);
          faces_op1(i)=faces_op1(j);
          faces_op1(j)=tmp;
          tmp=faces_op2(i);
          faces_op2(i)=faces_op2(j);
          faces_op2(j)=tmp;
        }
}
static inline int chercher_arete(const Zone_VEF_PreP1b& zone_VEF,
                                 int elem, int somi, int somj,
                                 const IntTab& elem_aretes,
                                 const IntTab& aretes_som)
{
  const ArrOfInt& renum_arete_perio=zone_VEF.get_renum_arete_perio();
  const Domaine& dom=zone_VEF.zone().domaine();
  if(somi>somj)
    {
      int k=somi;
      somi=somj;
      somj=k;
    }
  for(int i_arete=0; i_arete<6; i_arete++)
    {
      int arete=elem_aretes(elem, i_arete);
      int som1=dom.get_renum_som_perio(aretes_som(arete,0));
      int som2=dom.get_renum_som_perio(aretes_som(arete,1));
      somi=dom.get_renum_som_perio(somi);
      somj=dom.get_renum_som_perio(somj);
      if( (somi==som1)
          && (somj==som2) )
        return renum_arete_perio(arete);
      if( (somi==som2)
          && (somj==som1) )
        return renum_arete_perio(arete);
    }
  return -1;
}
static inline void swap (int& i, int& j)
{
  int k=i;
  i=j;
  j=k;
}

//
// rempli sommets, faces_op1 et faces_op2
// ou sommets contient les sommets de face et les sommets de elem1 et elem2
// qui ne sont pas dans face. face_op1(i) est le numero de la face opposee a sommets(i)
// dans elem1. face_op2(i) est ... dans elem2. (si elem2=-1, alors face_op2=-1)
// les dimension premiers sommets sont ceux de face
// le dernier est dans elem2
static inline void remplir_sommets(const Zone_VEF& zone_VEF,
                                   int face, int elem1, int elem2,
                                   ArrOfInt& sommets,
                                   ArrOfInt& faces_op1,
                                   ArrOfInt& faces_op2)
{
  int dplusun=Objet_U::dimension+1;
  const IntTab& elem_som = zone_VEF.zone().les_elems();
  const IntTab& face_som = zone_VEF.face_sommets();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const Domaine& dom=zone_VEF.zone().domaine();
  int i,j,k;
  for(i=0; i<Objet_U::dimension; i++)
    sommets(i)=dom.get_renum_som_perio(face_som(face,i));
  if(elem1!=-1)
    {
      int ok=0;
      for(i=0; i<dplusun; i++)
        if( (elem_faces(elem1,i)==face) ||
            (elem_faces(elem1,i)==face_associee) )
          {
            sommets(Objet_U::dimension)=
              dom.get_renum_som_perio(elem_som(elem1, i));
            faces_op1(Objet_U::dimension)=face;
            faces_op2(Objet_U::dimension)=-1;
            ok=1;
          }
        else
          {
            j=dom.get_renum_som_perio(elem_som(elem1, i));
            for(k=0; k<Objet_U::dimension; k++)
              if(j==sommets(k))
                faces_op1(k)=elem_faces(elem1, i);
          }
      if (ok!=1)
        {
          Cerr << "The discretization used has a P1 component" << finl;
          Cerr << "which is not available to deal your mesh." << finl;
          Cerr << "The mesh with this discretization must contain only ";
          Cerr << (Objet_U::dimension==2?"triangles":"tetraedras") << "." << finl;
          Process::exit();
        }
    }
  else
    {
      Cerr << "pas prevu ... " << finl;
      Process::exit();
    }
  if(elem2!=-1)
    {
      //int ok=0;
      for(i=0; i<dplusun; i++)
        if( (elem_faces(elem2,i)==face)||
            (elem_faces(elem2,i)==face_associee) )
          {
            sommets(dplusun)=dom.get_renum_som_perio(elem_som(elem2, i));
            faces_op2(dplusun)=face;
            faces_op1(dplusun)=-1;
            //ok=1;
          }
        else
          {
            j=dom.get_renum_som_perio(elem_som(elem2, i));
            for(k=0; k<Objet_U::dimension; k++)
              if(j==sommets(k))
                faces_op2(k)=elem_faces(elem2, i);
          }
      // A cause de mise en commentaire de ok=1 assert(ok==1);
    }
  else
    {
      sommets(dplusun)=-1;
      faces_op2(dplusun)=-1;
      faces_op1(dplusun)=-1;
    }
}

// calcule le gradient a la face separant elem1 et elem2
// de la fonction de forme associee au sommet s
//
static void calculer_grad(const IntTab& face_voisins,
                          int elem1, int elem2,
                          const ArrOfDouble& coef_som,
                          int s, int fop1, int fop2,
                          const DoubleTab& normales,
                          ArrOfDouble& grad)
{
  int dimension=Objet_U::dimension;
  double signe=1;
  if(fop1!=-1)
    {
      if(elem1!=face_voisins(fop1,0))
        signe=-1;
      signe*=coef_som[elem1];
      for(int comp=0; comp<dimension; comp++)
        grad[comp]=signe*normales(fop1,comp);
    }
  else
    grad=0;
  if((elem2!=-1)&&(fop2!=-1))
    {
      signe=1;
      if(elem2!=face_voisins(fop2,0))
        signe=-1;
      signe*=coef_som[elem2];
      for(int comp=0; comp<dimension; comp++)
        grad[comp]+=signe*normales(fop2,comp);
    }
}

// calcule le gradient a la face separant elem1 et elem2
// de la fonction de forme associee au sommet s
//
static void calculer_grad_arete(int face,
                                const IntTab& face_voisins,
                                int i, int j,
                                int elem1, int elem2,
                                int fop1, int fop2,
                                int fop3, int fop4,
                                const DoubleTab& normales,
                                ArrOfDouble& grad)
{
  assert(face_voisins(face,0)==elem1);
  int signe1=1,signe2=1,signe3=1,signe4=1;
  if((!(fop1==-1) && !(face_voisins(fop1,0)==elem1)))
    signe1=-1;
  if(!(fop3==-1) && !(face_voisins(fop3,0)==elem1))
    signe3=-1;
  if(elem2!=-1)
    {
      if((!(fop2==-1) && !(face_voisins(fop2,0)==elem2)))
        signe2=-1;
      if(!(fop4==-1) && !( face_voisins(fop4,0)==elem2))
        signe4=-1;
    }
  if(j<3) // une arete de la face
    {
      if(elem2!=-1)
        {
          for(int comp=0; comp<3; comp++)
            grad(comp)=-2./15.*(signe1*normales(fop1,comp)
                                +signe2*normales(fop2,comp)
                                +signe3*normales(fop3,comp)
                                +signe4*normales(fop4,comp));
        }
      else
        {
          for(int comp=0; comp<3; comp++)
            grad(comp)=-2./15.*(signe1*normales(fop1,comp)
                                +signe3*normales(fop3,comp)
                                +normales(face,comp));
        }

    }
  else if (j==3) // une arete de elem1 mais pas de face
    {
      for(int comp=0; comp<3; comp++)
        grad(comp)=1./15.*(signe1*normales(fop1,comp)
                           +signe3*normales(fop3,comp));
    }
  else // une arete de elem2 mais pas de face
    {
      assert(j==4);
      for(int comp=0; comp<3; comp++)
        grad(comp)=1./15.*(signe2*normales(fop2,comp)
                           +signe4*normales(fop4,comp));
    }
}

static double dotproduct_array_fois_inverse_quantitee_entrelacee(const ArrOfDouble& gradi,const ArrOfDouble& gradE,const DoubleTab& inverse_quantitee_entrelacee, int face )
{
  double dot=0;
  int size=gradi.size_array();
  assert(size == gradE.size_array());
  for (int i=0; i<size; i++) dot+=gradi(i)*gradE(i)*inverse_quantitee_entrelacee(face,i);
  return dot;
  return  dotproduct_array(gradi,gradE)*inverse_quantitee_entrelacee(face,0);
}

static void contribuer_matriceP0P1(const Zone_VEF& zone_VEF,
                                   const DoubleTab& inverse_quantitee_entrelacee,
                                   int face, int elem1, int elem2,
                                   ArrOfInt& sommets, ArrOfInt& faces_op1,
                                   ArrOfInt& faces_op2,
                                   const ArrOfDouble& coef_som,
                                   IntLists& voisins, DoubleLists& coeffs,
                                   int& nnz)
{
  const DoubleTab& normales = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  assert(elem1==face_voisins(face, 0));
  assert(elem2==face_voisins(face, 1));

  int dimension=Objet_U::dimension,
      dplusdeux=dimension+2;
  double psc;
  //double coeff_som=1./(dimension)/(dimension+1);

  ArrOfDouble gradi(dimension);
  ArrOfDouble gradE(dimension);

  for(int i=0; i<dplusdeux; i++)
    {
      int si=sommets(i);
      if (si<0) break;
      calculer_grad(face_voisins, elem1, elem2,coef_som, si, faces_op1(i),
                    faces_op2(i), normales, gradi);
      for(int k=0; k<dimension; k++)
        gradE(k)=normales(face,k);
      psc=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradE,
                                                             inverse_quantitee_entrelacee,face);
      int rang1=voisins[elem1].rang(si);
      if(rang1==-1)
        {
          voisins[elem1].add(si);
          coeffs[elem1].add(psc);
          nnz++;
        }
      else
        {
          coeffs[elem1][rang1]+=psc;
        }
      if (elem2!=-1)
        {
          int rang2=voisins[elem2].rang(si);
          psc*=-1;
          if(rang2==-1)
            {
              voisins[elem2].add(si);
              coeffs[elem2].add(psc);
              nnz++;
            }
          else
            {
              coeffs[elem2][rang2]+=psc;
            }
        }
    }
}
inline void range(int& i, int& n, int& j, int& m, Matrice_Morse& ARR, Matrice_Morse& ARV, Matrice_Morse& AVR, Matrice_Morse& AVV, double coeff)
{
  if(i<n)
    if(j<m)
      ARR(i,j)+=coeff;
    else
      ARV(i,j-m)+=coeff;
  else if(j<m)
    AVR(i-n,j)+=coeff;
  else
    AVV(i-n,j-m)+=coeff;
}
static void update_matriceP0P1(const Zone_VEF& zone_VEF,
                               const DoubleTab& inverse_quantitee_entrelacee,
                               int face, int elem1, int elem2,
                               ArrOfInt& sommets, ArrOfInt& faces_op1,
                               ArrOfInt& faces_op2,           const ArrOfDouble& coef_som,
                               Matrice_Morse& ARR, Matrice_Morse& ARV,
                               Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  const DoubleTab& normales = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  assert(elem1==face_voisins(face, 0));
  assert(elem2==face_voisins(face, 1));

  int dimension=Objet_U::dimension,
      dplusdeux=dimension+2;
  double psc;
  //double coeff_som=1./(dimension)/(dimension+1);
  ArrOfDouble gradi(dimension);
  ArrOfDouble gradE(dimension);

  int nb_elem=zone_VEF.nb_elem();
  int nb_som=zone_VEF.nb_som();
  for(int i=0; i<dplusdeux; i++)
    {
      int si=sommets(i);
      if (si<0) break;
      calculer_grad(face_voisins, elem1, elem2, coef_som,si, faces_op1(i),
                    faces_op2(i), normales, gradi);
      for(int k=0; k<dimension; k++)
        gradE(k)=normales(face,k);
      psc=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradE,
                                                             inverse_quantitee_entrelacee,face);
      range(elem1,nb_elem,si,nb_som,ARR,ARV,AVR,AVV,psc);
      if (elem2!=-1)
        range(elem2,nb_elem,si,nb_som,ARR,ARV,AVR,AVV,-psc);
    }
}

static void contribuer_matriceP1P1(const Zone_VEF& zone_VEF,
                                   const DoubleTab& inverse_quantitee_entrelacee,
                                   int face, int elem1, int elem2,
                                   ArrOfInt& sommets, ArrOfInt& faces_op1,
                                   ArrOfInt& faces_op2,           const ArrOfDouble& coef_som,
                                   IntLists& voisins, DoubleLists& coeffs,
                                   DoubleVect& diag, int& nnz)
{
  const DoubleTab& normales = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int dimension=Objet_U::dimension,
      dplusdeux=dimension+2;
  double psc;
  //double coeff_som=1./(dimension)/(dimension+1);
  //coeff_som*=coeff_som;
  ArrOfDouble gradi(dimension);
  ArrOfDouble gradj(dimension);
  int i,j;

  // On ne traite pas les sommets -1 qui
  // sont en fin de tableau sommets:
  while (sommets(dplusdeux-1)==-1)
    dplusdeux--;

  for(i=0; i<dplusdeux; i++)
    {
      int si=sommets(i);
      calculer_grad(face_voisins, elem1, elem2,  coef_som,si, faces_op1(i),
                    faces_op2(i), normales, gradi);
      diag[si]+=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradi,
                                                                   inverse_quantitee_entrelacee,face);
      for(j=i+1; j<dplusdeux; j++)
        {
          int sj=sommets(j);
          calculer_grad(face_voisins, elem1, elem2,  coef_som,sj, faces_op1(j),
                        faces_op2(j), normales, gradj);
          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradj,inverse_quantitee_entrelacee,face);
          int rang=voisins[si].rang(sj);
          if (sj>si)
            {
              if(rang==-1)
                {
                  voisins[si].add(sj);
                  coeffs[si].add(psc);
                  nnz++;
                }
              else
                {
                  coeffs[si][rang]+=psc;
                }
            }
        }
    }
}

static void update_matriceP1P1(const Zone_VEF& zone_VEF,
                               const DoubleTab& inverse_quantitee_entrelacee,
                               int face, int elem1, int elem2,
                               ArrOfInt& sommets, ArrOfInt& faces_op1,
                               ArrOfInt& faces_op2,           const ArrOfDouble& coef_som,
                               Matrice_Morse& ARR, Matrice_Morse& ARV,
                               Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  const DoubleTab& normales = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int dimension=Objet_U::dimension,
      dplusdeux=dimension+2;
  double psc;
  //double coeff_som=1./(dimension)/(dimension+1);
  //coeff_som*=coeff_som;
  ArrOfDouble gradi(dimension);
  ArrOfDouble gradj(dimension);
  int nb_som_tot=zone_VEF.nb_som();
  int i,j;

  // On ne traite pas les sommets -1 qui
  // sont en fin de tableau sommets:
  while (sommets(dplusdeux-1)==-1)
    dplusdeux--;

  for(i=0; i<dplusdeux; i++)
    {
      int si=sommets(i);
      calculer_grad(face_voisins, elem1, elem2, coef_som,si, faces_op1(i),
                    faces_op2(i), normales, gradi);
      if(si<nb_som_tot)
        ARR(si,si)+=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradi,
                                                                       inverse_quantitee_entrelacee,face);
      for(j=i+1; j<dplusdeux; j++)
        {
          int sj=sommets(j);
          calculer_grad(face_voisins, elem1, elem2,coef_som, sj, faces_op1(j),
                        faces_op2(j), normales, gradj);
          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradj,inverse_quantitee_entrelacee,face);
          //assert(sj>si);
          if(si<nb_som_tot)
            if(sj<nb_som_tot)
              ARR(si,sj)+=psc;
            else
              ARV(si,sj-nb_som_tot)+=psc;
          else if(sj<nb_som_tot)
            AVR(si-nb_som_tot,sj)+=psc;
          else
            AVV(si-nb_som_tot,sj-nb_som_tot)+=psc;
        }
    }
}

static void contribuer_matricePaPa(const Zone_VEF_PreP1b& zone_VEF,
                                   const DoubleTab& inverse_quantitee_entrelacee,
                                   int face, int elem1, int elem2,
                                   ArrOfInt& sommets, ArrOfInt& faces_op1,
                                   ArrOfInt& faces_op2,
                                   IntLists& voisins, DoubleLists& coeffs,
                                   DoubleVect& diag, int& nnz)
{
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int i, j, k, l;
  double psc;
  // On ne traite pas les sommets -1 qui
  // sont en fin de tableau sommets:
  //while (sommets(dplusdeux-1)==-1)
  //   dplusdeux--;

  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  int jmax=5;
  if(elem2==-1) jmax=4;
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<jmax; j++)
        {
          int sj=sommets(j);
          int arete1;
          if(j<4)
            arete1 = chercher_arete(zone_VEF,elem1, si, sj,
                                    elem_aretes, aretes_som);
          else
            arete1 = chercher_arete(zone_VEF,elem2, si, sj,
                                    elem_aretes, aretes_som);
          assert(arete1!=-1);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem1, elem2,
                                  faces_op1(i), faces_op2(i),
                                  faces_op1(j), faces_op2(j),
                                  normales, grad1);
              diag[arete1]+=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad1,
                                                                               inverse_quantitee_entrelacee,face);
              int jj=j;
              for(k=i; k<3; k++)
                {
                  int sk=sommets(k);
                  for(l=jj+1; l<jmax; l++)
                    {
                      int sl=sommets(l);
                      int arete2;
                      if(l<4)
                        arete2 = chercher_arete(zone_VEF,elem1, sl, sk,
                                                elem_aretes,
                                                aretes_som);
                      else
                        arete2 = chercher_arete(zone_VEF,elem2, sl, sk,
                                                elem_aretes,
                                                aretes_som);
                      assert(arete2!=-1);
                      if(ok_arete(arete2))
                        {
                          calculer_grad_arete(face, face_voisins, k, l,
                                              elem1, elem2,
                                              faces_op1(k), faces_op2(k),
                                              faces_op1(l), faces_op2(l),
                                              normales, grad2);
                          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                                 inverse_quantitee_entrelacee,face);
                          int tmp=arete1;
                          if(arete1>arete2) swap(arete1, arete2);
                          int rang=voisins[arete1].rang(arete2);
                          if(rang==-1)
                            {
                              voisins[arete1].add(arete2);
                              coeffs[arete1].add(psc);
                              nnz++;
                            }
                          else
                            {
                              coeffs[arete1][rang]+=psc;
                            }
                          arete1=tmp;
                        }
                    }
                  jj=k+1;
                }
            }
        }
    }
}
static void update_matricePaPa(const Zone_VEF_PreP1b& zone_VEF,
                               const DoubleTab& inverse_quantitee_entrelacee,
                               int face, int elem1, int elem2,
                               ArrOfInt& sommets, ArrOfInt& faces_op1,
                               ArrOfInt& faces_op2,
                               Matrice_Morse& ARR, Matrice_Morse& ARV,
                               Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int nb_aretes_tot=zone_VEF.zone().nb_aretes();
  int i, j, k, l;
  double psc;
  // On ne traite pas les sommets -1 qui
  // sont en fin de tableau sommets:
  //while (sommets(dplusdeux-1)==-1)
  //   dplusdeux--;

  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  int jmax=5;
  if(elem2==-1) jmax=4;
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<jmax; j++)
        {
          int sj=sommets(j);
          int arete1;
          if(j<4)
            arete1 = chercher_arete(zone_VEF,elem1, si, sj,
                                    elem_aretes, aretes_som);
          else
            arete1 = chercher_arete(zone_VEF,elem2, si, sj,
                                    elem_aretes, aretes_som);
          assert(arete1!=-1);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem1, elem2,
                                  faces_op1(i), faces_op2(i),
                                  faces_op1(j), faces_op2(j),
                                  normales, grad1);
              if(arete1<nb_aretes_tot)
                ARR(arete1,arete1)+=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad1,
                                                                                       inverse_quantitee_entrelacee,face);
              int jj=j;
              for(k=i; k<3; k++)
                {
                  int sk=sommets(k);
                  for(l=jj+1; l<jmax; l++)
                    {
                      int sl=sommets(l);
                      int arete2;
                      if(l<4)
                        arete2 = chercher_arete(zone_VEF,elem1, sl, sk,
                                                elem_aretes,
                                                aretes_som);
                      else
                        arete2 = chercher_arete(zone_VEF,elem2, sl, sk,
                                                elem_aretes,
                                                aretes_som);
                      assert(arete2!=-1);
                      if(ok_arete(arete2))
                        {
                          calculer_grad_arete(face, face_voisins, k, l,
                                              elem1, elem2,
                                              faces_op1(k), faces_op2(k),
                                              faces_op1(l), faces_op2(l),
                                              normales, grad2);
                          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                                 inverse_quantitee_entrelacee,face);
                          int tmp=arete1;
                          if(arete1>arete2) swap(arete1, arete2);
                          if(arete1<nb_aretes_tot)
                            if(arete2<nb_aretes_tot)
                              ARR(arete1,arete2)+=psc;
                            else
                              ARV(arete1,arete2-nb_aretes_tot)+=psc;
                          else if(arete2<nb_aretes_tot)
                            AVR(arete1-nb_aretes_tot,arete2)+=psc;
                          else
                            AVV(arete1-nb_aretes_tot,arete2-nb_aretes_tot)+=psc;
                          arete1=tmp;
                        }
                    }
                  jj=k+1;
                }
            }
        }
    }
}
static void
contribuer_matrice_NeumannP0P1(const Zone_VEF& zone_VEF,
                               const DoubleTab& inverse_quantitee_entrelacee,
                               int face, int elem,
                               ArrOfInt& sommets, ArrOfInt& faces_op1,   const ArrOfDouble& coef_som,
                               IntLists& voisins, DoubleLists& coeffs,
                               int& nnz)
{
  const DoubleTab& normales = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int dimension=Objet_U::dimension,
      dplusun=dimension+1;
  double unsurdim=1./Objet_U::dimension;
  double psc;
  //  double coeff_som=1./(dimension)/(dplusun);
  ArrOfDouble gradi(dimension);
  ArrOfDouble gradE(dimension);
  for(int i=0; i<dplusun; i++)
    {
      int si=sommets(i);
      calculer_grad(face_voisins, elem, -1, coef_som,si, faces_op1(i),
                    -1, normales, gradi);
      if(faces_op1(i)!=face)
        for (int comp=0; comp<dimension; comp++)
          gradi(comp)+=normales(face,comp)*unsurdim;
      for(int k=0; k<dimension; k++)
        gradE(k)=normales(face,k);
      psc=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradE,
                                                             inverse_quantitee_entrelacee,face);
      int rang1=voisins[elem].rang(si);
      if(rang1==-1)
        {
          voisins[elem].add(si);
          coeffs[elem].add(psc);
          nnz++;
        }
      else
        {
          coeffs[elem][rang1]+=psc;
        }
    }
}

static void
update_matrice_NeumannP0P1(const Zone_VEF& zone_VEF,
                           const DoubleTab& inverse_quantitee_entrelacee,
                           int face, int elem,
                           ArrOfInt& sommets, ArrOfInt& faces_op1,   const ArrOfDouble& coef_som,
                           Matrice_Morse& ARR, Matrice_Morse& ARV,
                           Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  const DoubleTab& normales = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int nb_elem_tot=zone_VEF.nb_elem();
  int nb_som_tot=zone_VEF.nb_som();

  int dimension=Objet_U::dimension,
      dplusun=dimension+1;
  double unsurdim=1./Objet_U::dimension;
  double psc;
  //  double coeff_som=1./(dimension)/(dplusun);
  ArrOfDouble gradi(dimension);
  ArrOfDouble gradE(dimension);
  for(int i=0; i<dplusun; i++)
    {
      int si=sommets(i);
      calculer_grad(face_voisins, elem, -1,  coef_som,si, faces_op1(i),
                    -1, normales, gradi);
      if(faces_op1(i)!=face)
        for (int comp=0; comp<dimension; comp++)
          gradi(comp)+= normales(face,comp)*unsurdim;
      for(int k=0; k<dimension; k++)
        gradE(k)=normales(face,k);
      psc=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradE,
                                                             inverse_quantitee_entrelacee,face);
      if(elem<nb_elem_tot)
        if(si<nb_som_tot)
          ARR(elem,si)+=psc;
        else
          ARV(elem,si-nb_som_tot)+=psc;
      else if(si<nb_som_tot)
        AVR(elem-nb_elem_tot,si)+=psc;
      else
        AVV(elem-nb_elem_tot,si-nb_som_tot)+=psc;
    }
}

static void
contribuer_matrice_NeumannP1P1(const Zone_VEF& zone_VEF,
                               const DoubleTab& inverse_quantitee_entrelacee,
                               int face, int elem,
                               ArrOfInt& sommets, ArrOfInt& faces_op1,   const ArrOfDouble& coef_som,
                               IntLists& voisins, DoubleLists& coeffs,
                               DoubleVect& diag, int& nnz)
{
  const DoubleTab& normales = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int dimension=Objet_U::dimension,
      dplusun=dimension+1;
  double unsurdim=1./Objet_U::dimension;
  double psc;
  //  double coeff_som=1./(dimension)/(dplusun);
  //coeff_som*=coeff_som;
  ArrOfDouble gradi(dimension);
  ArrOfDouble gradj(dimension);
  int i,j;
  for(i=0; i<dplusun; i++)
    {
      int si=sommets(i);
      calculer_grad(face_voisins, elem, -1, coef_som, si, faces_op1(i),
                    -1, normales, gradi);
      if(faces_op1(i)!=face)
        for (int comp=0; comp<dimension; comp++)
          gradi(comp)+= normales(face,comp)*unsurdim;
      diag[si]+=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradi,
                                                                   inverse_quantitee_entrelacee,face);
      for(j=i+1; j<dplusun; j++)
        {
          int sj=sommets(j);
          calculer_grad(face_voisins, elem, -1,  coef_som,sj, faces_op1(j),
                        -1, normales, gradj);
          if(faces_op1(j)!=face)
            for (int comp=0; comp<dimension; comp++)
              gradj(comp)+= normales(face,comp)*unsurdim;
          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradj,inverse_quantitee_entrelacee,face);
          int rang=voisins[si].rang(sj);
          if(rang==-1)
            {
              voisins[si].add(sj);
              coeffs[si].add(psc);
              nnz++;
            }
          else
            {

              coeffs[si][rang]+=psc;
            }
        }
    }
}


static void
update_matrice_NeumannP1P1(const Zone_VEF& zone_VEF,
                           const DoubleTab& inverse_quantitee_entrelacee,
                           int face, int elem,
                           ArrOfInt& sommets, ArrOfInt& faces_op1,   const ArrOfDouble& coef_som,
                           Matrice_Morse& ARR, Matrice_Morse& ARV,
                           Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  const DoubleTab& normales = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int dimension=Objet_U::dimension,
      dplusun=dimension+1;
  double unsurdim=1./Objet_U::dimension;
  double psc;
  //  double coeff_som=1./(dimension)/(dplusun);
  //coeff_som*=coeff_som;
  ArrOfDouble gradi(dimension);
  ArrOfDouble gradj(dimension);
  int nb_som_tot=zone_VEF.nb_som();
  int i,j;
  for(i=0; i<dplusun; i++)
    {
      int si=sommets(i);
      calculer_grad(face_voisins, elem, -1,  coef_som,si, faces_op1(i),
                    -1, normales, gradi);
      if(faces_op1(i)!=face)
        for (int comp=0; comp<dimension; comp++)
          gradi(comp)+= normales(face,comp)*unsurdim;
      if(si<nb_som_tot)
        ARR(si,si)+=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradi,
                                                                       inverse_quantitee_entrelacee,face);
      for(j=i+1; j<dplusun; j++)
        {
          int sj=sommets(j);
          calculer_grad(face_voisins, elem, -1,  coef_som,sj, faces_op1(j),
                        -1, normales, gradj);
          if(faces_op1(j)!=face)
            for (int comp=0; comp<dimension; comp++)
              gradj(comp)+= normales(face,comp)*unsurdim;
          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradj,inverse_quantitee_entrelacee,face);
          if(si<nb_som_tot)
            if(sj<nb_som_tot)
              ARR(si,sj)+=psc;
            else
              ARV(si,sj-nb_som_tot)+=psc;
          else if(sj<nb_som_tot)
            AVR(si-nb_som_tot,sj)+=psc;
          else
            AVV(si-nb_som_tot,sj-nb_som_tot)+=psc;
        }
    }
}

static void
contribuer_matrice_SymetrieP1P1(const Zone_VEF& zone_VEF,
                                const DoubleTab& inverse_quantitee_entrelacee,
                                int face, int elem,
                                ArrOfInt& sommets, ArrOfInt& faces_op1,const ArrOfDouble& coef_som,
                                IntLists& voisins, DoubleLists& coeffs,
                                DoubleVect& diag, int& nnz)
{
  const DoubleTab& normales = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int dimension=Objet_U::dimension,
      dplusun=dimension+1;

  double psc;
  //  double coeff_som=1./(dimension)/(dplusun);
  //  coeff_som*=coeff_som;
  ArrOfDouble gradi(dimension);
  ArrOfDouble gradj(dimension);
  int i,j;
  for(i=0; i<dplusun; i++)
    {
      int si=sommets(i);
      calculer_grad(face_voisins, elem, -1, coef_som, si, faces_op1(i),
                    -1, normales, gradi);
      projette(gradi, face, normales);
      diag[si]+=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradi,
                                                                   inverse_quantitee_entrelacee,face);
      for(j=i+1; j<dplusun; j++)
        {
          int sj=sommets(j);
          calculer_grad(face_voisins, elem,  -1, coef_som, sj, faces_op1(j),
                        -1, normales, gradj);
          projette(gradj, face, normales);
          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradj,
                                                                 inverse_quantitee_entrelacee,face);
          int rang=voisins[si].rang(sj);
          if(rang==-1)
            {
              voisins[si].add(sj);
              coeffs[si].add(psc);
              nnz++;
            }
          else
            {

              coeffs[si][rang]+=psc;
            }
        }
    }
}


static void
update_matrice_SymetrieP1P1(const Zone_VEF& zone_VEF,
                            const DoubleTab& inverse_quantitee_entrelacee,
                            int face, int elem,
                            ArrOfInt& sommets, ArrOfInt& faces_op1,   const ArrOfDouble& coef_som,
                            Matrice_Morse& ARR, Matrice_Morse& ARV,
                            Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  const DoubleTab& normales = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int dimension=Objet_U::dimension,
      dplusun=dimension+1;
  double psc;
  //  double coeff_som=1./(dimension)/(dplusun);
  //coeff_som*=coeff_som;
  ArrOfDouble gradi(dimension);
  ArrOfDouble gradj(dimension);
  int nb_som_tot=zone_VEF.nb_som();
  int i,j;
  for(i=0; i<dplusun; i++)
    {
      int si=sommets(i);
      calculer_grad(face_voisins, elem, -1, coef_som, si, faces_op1(i),
                    -1, normales, gradi);
      projette(gradi, face, normales);
      if(si<nb_som_tot)
        ARR(si,si)+=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradi,
                                                                       inverse_quantitee_entrelacee,face);
      for(j=i+1; j<dplusun; j++)
        {
          int sj=sommets(j);
          calculer_grad(face_voisins, elem, -1,  coef_som,sj, faces_op1(j),
                        -1, normales, gradj);
          projette(gradj, face, normales);
          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(gradi,gradj,
                                                                 inverse_quantitee_entrelacee,face);
          if(si<nb_som_tot)
            if(sj<nb_som_tot)
              ARR(si,sj)+=psc;
            else
              ARV(si,sj-nb_som_tot)+=psc;
          else if(sj<nb_som_tot)
            AVR(si-nb_som_tot,sj)+=psc;
          else
            AVV(si-nb_som_tot,sj-nb_som_tot)+=psc;
        }
    }
}

static void
contribuer_matrice_NeumannPaPa(const Zone_VEF_PreP1b& zone_VEF,
                               const DoubleTab& inverse_quantitee_entrelacee,
                               int face, int elem,
                               ArrOfInt& sommets, ArrOfInt& faces_op1,
                               IntLists& voisins, DoubleLists& coeffs,
                               DoubleVect& diag, int& nnz)
{
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int i, j, k, l;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<4; j++)
        {
          int sj=sommets(j);
          int arete1;
          arete1 = chercher_arete(zone_VEF,elem, si, sj,
                                  elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem, -1,
                                  faces_op1(i), -1,
                                  faces_op1(j), -1,
                                  normales, grad1);
              diag[arete1]+=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad1,
                                                                               inverse_quantitee_entrelacee,face);
              int jj=j;
              for(k=i; k<3; k++)
                {
                  int sk=sommets(k);
                  for(l=jj+1; l<4; l++)
                    {
                      int sl=sommets(l);
                      int arete2= chercher_arete(zone_VEF,elem, sl, sk,
                                                 elem_aretes,
                                                 aretes_som);
                      assert(arete2!=-1);
                      if(ok_arete(arete2))
                        {
                          calculer_grad_arete(face, face_voisins, k, l,
                                              elem, -1,
                                              faces_op1(k), -1,
                                              faces_op1(l), -1,
                                              normales, grad2);
                          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                                 inverse_quantitee_entrelacee,face);
                          int tmp=arete1;
                          if(arete1>arete2) swap(arete1, arete2);
                          int rang=voisins[arete1].rang(arete2);
                          if(rang==-1)
                            {
                              voisins[arete1].add(arete2);
                              coeffs[arete1].add(psc);
                              nnz++;
                            }
                          else
                            {
                              coeffs[arete1][rang]+=psc;
                            }
                          arete1=tmp;
                        }
                    }
                  jj=k+1;
                }
            }
        }
    }
}

static void
update_matrice_NeumannPaPa(const Zone_VEF_PreP1b& zone_VEF,
                           const DoubleTab& inverse_quantitee_entrelacee,
                           int face, int elem,
                           ArrOfInt& sommets, ArrOfInt& faces_op1,
                           Matrice_Morse& ARR, Matrice_Morse& ARV,
                           Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int nb_aretes_tot=zone_VEF.zone().nb_aretes();

  int i, j, k, l;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<4; j++)
        {
          int sj=sommets(j);
          int arete1;
          arete1 = chercher_arete(zone_VEF,elem, si, sj,
                                  elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem, -1,
                                  faces_op1(i), -1,
                                  faces_op1(j), -1,
                                  normales, grad1);
              if(arete1<nb_aretes_tot)
                ARR(arete1,arete1)+=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad1,
                                                                                       inverse_quantitee_entrelacee,face);
              int jj=j;
              for(k=i; k<3; k++)
                {
                  int sk=sommets(k);
                  for(l=jj+1; l<4; l++)
                    {
                      int sl=sommets(l);
                      int arete2= chercher_arete(zone_VEF,elem, sl, sk,
                                                 elem_aretes,
                                                 aretes_som);
                      assert(arete2!=-1);
                      if(ok_arete(arete2))
                        {
                          calculer_grad_arete(face, face_voisins, k, l,
                                              elem, -1,
                                              faces_op1(k), -1,
                                              faces_op1(l), -1,
                                              normales, grad2);
                          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                                 inverse_quantitee_entrelacee,face);
                          int tmp=arete1;
                          if(arete1>arete2) swap(arete1, arete2);
                          if(arete1<nb_aretes_tot)
                            if(arete2<nb_aretes_tot)
                              ARR(arete1,arete2)+=psc;
                            else
                              ARV(arete1,arete2-nb_aretes_tot)+=psc;
                          else if(arete2<nb_aretes_tot)
                            AVR(arete1-nb_aretes_tot,arete2)+=psc;
                          else
                            AVV(arete1-nb_aretes_tot,arete2-nb_aretes_tot)+=psc;
                          arete1=tmp;
                        }
                    }
                  jj=k+1;
                }
            }
        }
    }
}

static void
contribuer_matrice_SymetriePaPa(const Zone_VEF_PreP1b& zone_VEF,
                                const DoubleTab& inverse_quantitee_entrelacee,
                                int face, int elem,
                                ArrOfInt& sommets, ArrOfInt& faces_op1,
                                IntLists& voisins, DoubleLists& coeffs,
                                DoubleVect& diag, int& nnz)
{
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int i, j, k, l;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<4; j++)
        {
          int sj=sommets(j);
          int arete1;
          arete1 = chercher_arete(zone_VEF,elem, si, sj,
                                  elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem, -1,
                                  faces_op1(i), -1,
                                  faces_op1(j), -1,
                                  normales, grad1);
              projette(grad1, face, normales);
              diag[arete1]+=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad1,
                                                                               inverse_quantitee_entrelacee,face);
              int jj=j;
              for(k=i; k<3; k++)
                {
                  int sk=sommets(k);
                  for(l=jj+1; l<4; l++)
                    {
                      int sl=sommets(l);
                      int arete2= chercher_arete(zone_VEF,elem, sl, sk,
                                                 elem_aretes,
                                                 aretes_som);
                      assert(arete2!=-1);
                      if(ok_arete(arete2))
                        {
                          calculer_grad_arete(face, face_voisins, k, l,
                                              elem, -1,
                                              faces_op1(k), -1,
                                              faces_op1(l), -1,
                                              normales, grad2);
                          projette(grad2, face, normales);
                          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                                 inverse_quantitee_entrelacee,face);
                          int tmp=arete1;
                          if(arete1>arete2) swap(arete1, arete2);
                          int rang=voisins[arete1].rang(arete2);
                          if(rang==-1)
                            {
                              voisins[arete1].add(arete2);
                              coeffs[arete1].add(psc);
                              nnz++;
                            }
                          else
                            {
                              coeffs[arete1][rang]+=psc;
                            }
                          arete1=tmp;
                        }
                    }
                  jj=k+1;
                }
            }
        }
    }
}

static void
update_matrice_SymetriePaPa(const Zone_VEF_PreP1b& zone_VEF,
                            const DoubleTab& inverse_quantitee_entrelacee,
                            int face, int elem,
                            ArrOfInt& sommets, ArrOfInt& faces_op1,
                            Matrice_Morse& ARR, Matrice_Morse& ARV,
                            Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int i, j, k, l;
  int nb_aretes_tot=zone_VEF.zone().nb_aretes();

  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<4; j++)
        {
          int sj=sommets(j);
          int arete1;
          arete1 = chercher_arete(zone_VEF,elem, si, sj,
                                  elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem, -1,
                                  faces_op1(i), -1,
                                  faces_op1(j), -1,
                                  normales, grad1);
              projette(grad1, face, normales);
              if(arete1<nb_aretes_tot)
                ARR(arete1,arete1)+=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad1,
                                                                                       inverse_quantitee_entrelacee,face);
              int jj=j;
              for(k=i; k<3; k++)
                {
                  int sk=sommets(k);
                  for(l=jj+1; l<4; l++)
                    {
                      int sl=sommets(l);
                      int arete2= chercher_arete(zone_VEF,elem, sl, sk,
                                                 elem_aretes,
                                                 aretes_som);
                      assert(arete2!=-1);
                      if(ok_arete(arete2))
                        {
                          calculer_grad_arete(face, face_voisins, k, l,
                                              elem, -1,
                                              faces_op1(k), -1,
                                              faces_op1(l), -1,
                                              normales, grad2);
                          projette(grad2, face, normales);
                          psc=dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                                 inverse_quantitee_entrelacee,face);
                          int tmp=arete1;
                          if(arete1>arete2) swap(arete1, arete2);
                          if(arete1<nb_aretes_tot)
                            if(arete2<nb_aretes_tot)
                              ARR(arete1,arete2)+=psc;
                            else
                              ARV(arete1,arete2-nb_aretes_tot)+=psc;
                          else if(arete2<nb_aretes_tot)
                            AVR(arete1-nb_aretes_tot,arete2)+=psc;
                          else
                            AVV(arete1-nb_aretes_tot,arete2-nb_aretes_tot)+=psc;
                          arete1=tmp;
                        }
                    }
                  jj=k+1;
                }
            }
        }
    }
}

static void contribuer_matriceP0Pa(const Zone_VEF_PreP1b& zone_VEF,
                                   const DoubleTab& inverse_quantitee_entrelacee,
                                   int face, int elem1, int elem2,
                                   ArrOfInt& sommets,
                                   ArrOfInt& faces_op1,
                                   ArrOfInt& faces_op2,
                                   IntLists& voisins,
                                   DoubleLists& coeffs,
                                   int& nnz)
{
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int i, j;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  int jmax=5;
  if(elem2==-1) jmax=4;
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<jmax; j++)
        {
          int sj=sommets(j);
          int arete1;
          if(j<4)
            arete1 = chercher_arete(zone_VEF,elem1, si, sj,
                                    elem_aretes, aretes_som);
          else
            arete1 = chercher_arete(zone_VEF,elem2, si, sj,
                                    elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem1, elem2,
                                  faces_op1(i), faces_op2(i),
                                  faces_op1(j), faces_op2(j),
                                  normales, grad1);
              psc=0;
              for(int comp=0; comp<3; comp++)
                psc+=grad1(comp)*normales(face, comp)*
                     (-inverse_quantitee_entrelacee(face,comp));
              int rang=voisins[elem1].rang(arete1);
              if(rang==-1)
                {
                  voisins[elem1].add(arete1);
                  coeffs[elem1].add(psc);
                  nnz++;
                }
              else
                {
                  coeffs[elem1][rang]+=psc;
                }
              if(elem2!=-1)
                {
                  psc*=-1.0;
                  int rangbis=voisins[elem2].rang(arete1);
                  if(rangbis==-1)
                    {
                      voisins[elem2].add(arete1);
                      coeffs[elem2].add(psc);
                      nnz++;
                    }
                  else
                    {
                      coeffs[elem2][rangbis]+=psc;
                    }
                }
            }
        }
    }
}

static void update_matriceP0Pa(const Zone_VEF_PreP1b& zone_VEF,
                               const DoubleTab& inverse_quantitee_entrelacee,
                               int face, int elem1, int elem2,
                               ArrOfInt& sommets,
                               ArrOfInt& faces_op1,
                               ArrOfInt& faces_op2,
                               Matrice_Morse& ARR, Matrice_Morse& ARV,
                               Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int nb_aretes_tot=zone_VEF.zone().nb_aretes();
  int nb_elem_tot=zone_VEF.zone().nb_elem();

  int i, j;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  int jmax=5;
  if(elem2==-1) jmax=4;
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<jmax; j++)
        {
          int sj=sommets(j);
          int arete1;
          if(j<4)
            arete1 = chercher_arete(zone_VEF,elem1, si, sj,
                                    elem_aretes, aretes_som);
          else
            arete1 = chercher_arete(zone_VEF,elem2, si, sj,
                                    elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem1, elem2,
                                  faces_op1(i), faces_op2(i),
                                  faces_op1(j), faces_op2(j),
                                  normales, grad1);
              psc=0;
              for(int comp=0; comp<3; comp++)
                psc+=grad1(comp)*normales(face, comp)
                     *(-inverse_quantitee_entrelacee(face,comp));
              if(elem1<nb_elem_tot)
                if(arete1<nb_aretes_tot)
                  ARR(elem1,arete1)+=psc;
                else
                  ARV(elem1,arete1-nb_aretes_tot)+=psc;
              else if(arete1<nb_aretes_tot)
                AVR(elem1-nb_elem_tot,arete1)+=psc;
              else
                AVV(elem1-nb_elem_tot,arete1-nb_aretes_tot)+=psc;

              if(elem2!=-1)
                {
                  psc*=-1.0;
                  if(elem2<nb_elem_tot)
                    if(arete1<nb_aretes_tot)
                      ARR(elem2,arete1)+=psc;
                    else
                      ARV(elem2,arete1-nb_aretes_tot)+=psc;
                  else if(arete1<nb_aretes_tot)
                    AVR(elem2-nb_elem_tot,arete1)+=psc;
                  else
                    AVV(elem2-nb_elem_tot,arete1-nb_aretes_tot)+=psc;
                }
            }
        }
    }
}

static void
contribuer_matrice_NeumannP0Pa(const Zone_VEF_PreP1b& zone_VEF,
                               const DoubleTab& inverse_quantitee_entrelacee,
                               int face, int elem,
                               ArrOfInt& sommets,
                               ArrOfInt& faces_op1,
                               IntLists& voisins,
                               DoubleLists& coeffs,
                               int& nnz)
{
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int i, j;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<4; j++)
        {
          int sj=sommets(j);
          int arete1;
          arete1 = chercher_arete(zone_VEF,elem, si, sj,
                                  elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem, -1,
                                  faces_op1(i), -1,
                                  faces_op1(j), -1,
                                  normales, grad1);
              psc=0;
              for(int comp=0; comp<3; comp++)
                psc+=grad1(comp)*normales(face, comp)
                     *(-inverse_quantitee_entrelacee(face,comp));
              int rang=voisins[elem].rang(arete1);
              if(rang==-1)
                {
                  voisins[elem].add(arete1);
                  coeffs[elem].add(psc);
                  nnz++;
                }
              else
                {
                  coeffs[elem][rang]+=psc;
                }
            }
        }
    }
}

static void
update_matrice_NeumannP0Pa(const Zone_VEF_PreP1b& zone_VEF,
                           const DoubleTab& inverse_quantitee_entrelacee,
                           int face, int elem,
                           ArrOfInt& sommets,
                           ArrOfInt& faces_op1,
                           Matrice_Morse& ARR, Matrice_Morse& ARV,
                           Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int nb_aretes_tot=zone_VEF.zone().nb_aretes();
  int nb_elem_tot=zone_VEF.zone().nb_elem();

  int i, j;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<4; j++)
        {
          int sj=sommets(j);
          int arete1;
          arete1 = chercher_arete(zone_VEF,elem, si, sj,
                                  elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem, -1,
                                  faces_op1(i), -1,
                                  faces_op1(j), -1,
                                  normales, grad1);
              psc=0;
              for(int comp=0; comp<3; comp++)
                psc+=grad1(comp)*normales(face, comp)
                     *(-inverse_quantitee_entrelacee(face,comp));
              if(elem<nb_elem_tot)
                if(arete1<nb_aretes_tot)
                  ARR(elem,arete1)+=psc;
                else
                  ARV(elem,arete1-nb_aretes_tot)+=psc;
              else if(arete1<nb_aretes_tot)
                AVR(elem-nb_elem_tot,arete1)+=psc;
              else
                AVV(elem-nb_elem_tot,arete1-nb_aretes_tot)+=psc;
            }
        }
    }
}

static void contribuer_matriceP1Pa(const Zone_VEF_PreP1b& zone_VEF,
                                   const DoubleTab& inverse_quantitee_entrelacee,
                                   int face, int elem1, int elem2,
                                   ArrOfInt& sommets,
                                   ArrOfInt& faces_op1,
                                   ArrOfInt& faces_op2,           const ArrOfDouble& coef_som,
                                   IntLists& voisins,
                                   DoubleLists& coeffs,
                                   int& nnz)
{
  // int dimension=Objet_U::dimension,
  //    dplusun=dimension+1;
  //  double coeff_som=1./(dimension)/(dplusun);
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int i, j, k;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  int jmax=5;
  if(elem2==-1) jmax=4;
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<jmax; j++)
        {
          int sj=sommets(j);
          int arete1;
          if(j<4)
            arete1 = chercher_arete(zone_VEF,elem1, si, sj,
                                    elem_aretes, aretes_som);
          else
            arete1 = chercher_arete(zone_VEF,elem2, si, sj,
                                    elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem1, elem2,
                                  faces_op1(i), faces_op2(i),
                                  faces_op1(j), faces_op2(j),
                                  normales, grad1);
              for(k=0; k<jmax; k++)
                {
                  int sk=sommets(k);
                  calculer_grad(face_voisins, elem1, elem2, coef_som, sk,
                                faces_op1(k), faces_op2(k),
                                normales, grad2);
                  psc=-dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                          inverse_quantitee_entrelacee,face);
                  int rang1=voisins[sk].rang(arete1);
                  if(rang1==-1)
                    {
                      voisins[sk].add(arete1);
                      coeffs[sk].add(psc);
                      nnz++;
                    }
                  else
                    {
                      coeffs[sk][rang1]+=psc;
                    }
                }
            }
        }
    }
}

static void update_matriceP1Pa(const Zone_VEF_PreP1b& zone_VEF,
                               const DoubleTab& inverse_quantitee_entrelacee,
                               int face, int elem1, int elem2,
                               ArrOfInt& sommets,
                               ArrOfInt& faces_op1,
                               ArrOfInt& faces_op2,           const ArrOfDouble& coef_som,
                               Matrice_Morse& ARR, Matrice_Morse& ARV,
                               Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  //  int dimension=Objet_U::dimension,
  //    dplusun=dimension+1;
  //  double coeff_som=1./(dimension)/(dplusun);
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int nb_aretes_tot=zone_VEF.zone().nb_aretes();
  int nb_som_tot=zone_VEF.zone().nb_som();

  int i, j, k;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  int jmax=5;
  if(elem2==-1) jmax=4;
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<jmax; j++)
        {
          int sj=sommets(j);
          int arete1;
          if(j<4)
            arete1 = chercher_arete(zone_VEF,elem1, si, sj,
                                    elem_aretes, aretes_som);
          else
            arete1 = chercher_arete(zone_VEF,elem2, si, sj,
                                    elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem1, elem2,
                                  faces_op1(i), faces_op2(i),
                                  faces_op1(j), faces_op2(j),
                                  normales, grad1);
              for(k=0; k<jmax; k++)
                {
                  int sk=sommets(k);
                  calculer_grad(face_voisins, elem1, elem2, coef_som, sk,
                                faces_op1(k), faces_op2(k),
                                normales, grad2);
                  psc=-dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                          inverse_quantitee_entrelacee,face);
                  if(sk<nb_som_tot)
                    if(arete1<nb_aretes_tot)
                      ARR(sk,arete1)+=psc;
                    else
                      ARV(sk,arete1-nb_aretes_tot)+=psc;
                  else if(arete1<nb_aretes_tot)
                    AVR(sk-nb_som_tot,arete1)+=psc;
                  else
                    AVV(sk-nb_som_tot,arete1-nb_aretes_tot)+=psc;
                }
            }
        }
    }
}

static void
contribuer_matrice_NeumannP1Pa(const Zone_VEF_PreP1b& zone_VEF,
                               const DoubleTab& inverse_quantitee_entrelacee,
                               int face, int elem,
                               ArrOfInt& sommets,
                               ArrOfInt& faces_op1,    const ArrOfDouble& coef_som,
                               IntLists& voisins,
                               DoubleLists& coeffs,
                               int& nnz)
{
  int dimension=Objet_U::dimension;
  //    dplusun=dimension+1;
  //  double coeff_som=1./(dimension)/(dplusun);
  double unsurdim=1./Objet_U::dimension;
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int i, j, k;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<4; j++)
        {
          int sj=sommets(j);
          int arete1;
          arete1 = chercher_arete(zone_VEF,elem, si, sj,
                                  elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem, -1,
                                  faces_op1(i), -1,
                                  faces_op1(j), -1,
                                  normales, grad1);
              for(k=0; k<4; k++)
                {
                  int sk=sommets(k);
                  calculer_grad(face_voisins, elem, -1, coef_som, sk,
                                faces_op1(k), -1,
                                normales, grad2);
                  if(faces_op1(k)!=face)
                    for (int comp=0; comp<dimension; comp++)
                      grad2(comp)+= normales(face,comp)*unsurdim;
                  psc=-dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                          inverse_quantitee_entrelacee,face);
                  int rang1=voisins[sk].rang(arete1);
                  if(rang1==-1)
                    {
                      voisins[sk].add(arete1);
                      coeffs[sk].add(psc);
                      nnz++;
                    }
                  else
                    {
                      coeffs[sk][rang1]+=psc;
                    }
                }
            }
        }
    }
}

static void
update_matrice_NeumannP1Pa(const Zone_VEF_PreP1b& zone_VEF,
                           const DoubleTab& inverse_quantitee_entrelacee,
                           int face, int elem,
                           ArrOfInt& sommets,
                           ArrOfInt& faces_op1,   const ArrOfDouble& coef_som,
                           Matrice_Morse& ARR, Matrice_Morse& ARV,
                           Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  int dimension=Objet_U::dimension;
  //  dplusun=dimension+1;
  //  double coeff_som=1./(dimension)/(dplusun);
  double unsurdim=1./Objet_U::dimension;
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int nb_aretes_tot=zone_VEF.zone().nb_aretes();
  int nb_som_tot=zone_VEF.zone().nb_som();

  int i, j, k;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<4; j++)
        {
          int sj=sommets(j);
          int arete1;
          arete1 = chercher_arete(zone_VEF,elem, si, sj,
                                  elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem, -1,
                                  faces_op1(i), -1,
                                  faces_op1(j), -1,
                                  normales, grad1);
              for(k=0; k<4; k++)
                {
                  int sk=sommets(k);
                  calculer_grad(face_voisins, elem, -1,  coef_som,sk,
                                faces_op1(k), -1,
                                normales, grad2);
                  if(faces_op1(k)!=face)
                    for (int comp=0; comp<dimension; comp++)
                      grad2(comp)+= normales(face,comp)*unsurdim;
                  psc=-dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                          inverse_quantitee_entrelacee,face);
                  if(sk<nb_som_tot)
                    if(arete1<nb_aretes_tot)
                      ARR(sk,arete1)+=psc;
                    else
                      ARV(sk,arete1-nb_aretes_tot)+=psc;
                  else if(arete1<nb_aretes_tot)
                    AVR(sk-nb_som_tot,arete1)+=psc;
                  else
                    AVV(sk-nb_som_tot,arete1-nb_aretes_tot)+=psc;
                }
            }
        }
    }
}

static void
contribuer_matrice_SymetrieP1Pa(const Zone_VEF_PreP1b& zone_VEF,
                                const DoubleTab& inverse_quantitee_entrelacee,
                                int face, int elem,
                                ArrOfInt& sommets,
                                ArrOfInt& faces_op1,   const ArrOfDouble& coef_som,
                                IntLists& voisins,
                                DoubleLists& coeffs,
                                int& nnz)
{
  //int dimension=Objet_U::dimension,
  //    dplusun=dimension+1;
  //  double coeff_som=1./(dimension)/(dplusun);
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int i, j, k;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<4; j++)
        {
          int sj=sommets(j);
          int arete1;
          arete1 = chercher_arete(zone_VEF,elem, si, sj,
                                  elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem, -1,
                                  faces_op1(i), -1,
                                  faces_op1(j), -1,
                                  normales, grad1);
              projette(grad1, face, normales);
              for(k=0; k<4; k++)
                {
                  int sk=sommets(k);
                  calculer_grad(face_voisins, elem, -1, coef_som, sk,
                                faces_op1(k), -1,
                                normales, grad2);
                  projette(grad2, face, normales);
                  psc=-dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                          inverse_quantitee_entrelacee,face);
                  int rang1=voisins[sk].rang(arete1);
                  if(rang1==-1)
                    {
                      voisins[sk].add(arete1);
                      coeffs[sk].add(psc);
                      nnz++;
                    }
                  else
                    {
                      coeffs[sk][rang1]+=psc;
                    }
                }
            }
        }
    }
}

static void
update_matrice_SymetrieP1Pa(const Zone_VEF_PreP1b& zone_VEF,
                            const DoubleTab& inverse_quantitee_entrelacee,
                            int face, int elem,
                            ArrOfInt& sommets,
                            ArrOfInt& faces_op1,   const ArrOfDouble& coef_som,
                            Matrice_Morse& ARR, Matrice_Morse& ARV,
                            Matrice_Morse& AVR, Matrice_Morse& AVV)
{
  //  int dimension=Objet_U::dimension,
  //    dplusun=dimension+1;
  //  double coeff_som=1./(dimension)/(dplusun);
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const DoubleTab& normales = zone_VEF.face_normales();
  int nb_aretes_tot=zone_VEF.zone().nb_aretes();
  int nb_som_tot=zone_VEF.zone().nb_som();

  int i, j, k;
  double psc;
  ArrOfDouble grad1(3);
  ArrOfDouble grad2(3);
  for(i=0; i<3; i++)
    {
      int si=sommets(i);
      for(j=i+1; j<4; j++)
        {
          int sj=sommets(j);
          int arete1;
          arete1 = chercher_arete(zone_VEF,elem, si, sj,
                                  elem_aretes, aretes_som);
          if(ok_arete(arete1))
            {
              calculer_grad_arete(face, face_voisins, i, j,
                                  elem, -1,
                                  faces_op1(i), -1,
                                  faces_op1(j), -1,
                                  normales, grad1);
              projette(grad1, face, normales);
              for(k=0; k<4; k++)
                {
                  int sk=sommets(k);
                  calculer_grad(face_voisins, elem, -1,  coef_som,sk,
                                faces_op1(k), -1,
                                normales, grad2);
                  projette(grad2, face, normales);
                  psc=-dotproduct_array_fois_inverse_quantitee_entrelacee(grad1,grad2,
                                                                          inverse_quantitee_entrelacee,face);
                  if(sk<nb_som_tot)
                    if(arete1<nb_aretes_tot)
                      ARR(sk,arete1)+=psc;
                    else
                      ARV(sk,arete1-nb_aretes_tot)+=psc;
                  else if(arete1<nb_aretes_tot)
                    AVR(sk-nb_som_tot,arete1)+=psc;
                  else
                    AVV(sk-nb_som_tot,arete1-nb_aretes_tot)+=psc;
                }
            }
        }
    }
}

void
assemblerP0P0(const Zone_dis_base& z,
              const Zone_Cl_dis_base& zcl,
              Matrice& matrice,
              const DoubleTab& inverse_quantitee_entrelacee)
{
  Assembleur_P_VEF Assembleur_P0;
  Assembleur_P0.associer_zone_dis_base(z);
  Assembleur_P0.associer_zone_cl_dis_base(zcl);
  Assembleur_P0.remplir(matrice,inverse_quantitee_entrelacee);
  Cerr << "Assemblage P0 OK" << finl;
}

void
updateP0P0(const Zone_dis_base& z,
           const Zone_Cl_dis_base& zcl,
           Matrice& matrice,
           const DoubleTab& inverse_quantitee_entrelacee)
{
  Assembleur_P_VEF Assembleur_P0;
  Assembleur_P0.associer_zone_dis_base(z);
  Assembleur_P0.associer_zone_cl_dis_base(zcl);
  Assembleur_P0.remplir(matrice,inverse_quantitee_entrelacee);
  Cerr << "Update P0 OK" << finl;
}

void assemblerP1P1(const Zone_dis_base& z,
                   const Zone_Cl_dis_base& zcl,
                   Matrice& matrice, const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som)
{
  int dimension=Objet_U::dimension;
  const Zone_VEF_PreP1b& zone_VEF=ref_cast(Zone_VEF_PreP1b, z);
  const Zone& zone=zone_VEF.zone();
  const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF, zcl);
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nint = zone_VEF.premiere_face_int();
  int nb_faces = zone_VEF.nb_faces_tot();
  int nb_som = zone_VEF.zone().nb_som_tot();
  int elem1, elem2, face, ok;
  int nnz=nb_som;
  IntLists voisins(nb_som);
  DoubleLists coeffs(nb_som);
  DoubleVect diag(nb_som);
  ArrOfInt sommets(dimension+2);
  ArrOfInt face_opp1(dimension+2);
  ArrOfInt face_opp2(dimension+2);
  // Faces de bord :
  for(int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot();
      for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
        {
          ok=okface(ind_face, face, la_cl);
          if (ok==-1) break;
          elem1=face_voisins(face, 0);
          elem2=face_voisins(face, 1);
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          sort(sommets, face_opp1, face_opp2);
          if(ok==3)
            {
              contribuer_matrice_NeumannP1P1(zone_VEF,
                                             inverse_quantitee_entrelacee,
                                             face, elem1, sommets,
                                             face_opp1, coef_som,
                                             voisins, coeffs, diag, nnz);
            }
          else if(ok==4)
            {
              contribuer_matrice_SymetrieP1P1(zone_VEF,
                                              inverse_quantitee_entrelacee,
                                              face, elem1, sommets,
                                              face_opp1, coef_som,
                                              voisins, coeffs, diag, nnz);
            }
          else
            contribuer_matriceP1P1(zone_VEF,
                                   inverse_quantitee_entrelacee,
                                   face, elem1, elem2, sommets,
                                   face_opp1, face_opp2, coef_som,
                                   voisins, coeffs, diag, nnz);
        }
    }
  face_associee=-1;
  for(face=nint; face<nb_faces; face++)
    {
      elem1=face_voisins(face, 0);
      elem2=face_voisins(face, 1);
      if (!zone_VEF.est_une_face_virt_bord(face)) // On ne traite que les faces internes
        {
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          sort(sommets, face_opp1, face_opp2);
          contribuer_matriceP1P1(zone_VEF,
                                 inverse_quantitee_entrelacee,
                                 face, elem1, elem2, sommets,
                                 face_opp1, face_opp2, coef_som,
                                 voisins, coeffs, diag, nnz);
        }
    }

  for(int i=0; i<nb_som; i++)
    if(diag(i)==0)
      {
        //Cerr << "On modifie la ligne (sommet) orpheline " << i << finl;
        diag(i)=1.;
      }

  matrice.typer("Matrice_Bloc");
  Matrice_Bloc& matrice_bloc=ref_cast(Matrice_Bloc, matrice.valeur());
  matrice_bloc.remplir(voisins, coeffs, diag, zone.nb_som(), zone.nb_som_tot());
  Cerr << "Assemblage P1 OK" << finl;
}


void updateP1P1(const Zone_dis_base& z,
                const Zone_Cl_dis_base& zcl,
                Matrice& matrice,
                const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som)
{
  int dimension=Objet_U::dimension;
  const Zone_VEF_PreP1b& zone_VEF=ref_cast(Zone_VEF_PreP1b, z);
  //  const Zone& zone=zone_VEF.zone();
  const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF, zcl);
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nint = zone_VEF.premiere_face_int();
  int nb_faces = zone_VEF.nb_faces_tot();
  //  int nb_som = zone_VEF.zone().nb_som_tot();
  int elem1, elem2, face, ok;
  //  int nnz=nb_som;
  ArrOfInt sommets(dimension+2);
  ArrOfInt face_opp1(dimension+2);
  ArrOfInt face_opp2(dimension+2);
  Matrice_Bloc& A=ref_cast(Matrice_Bloc, matrice.valeur());
  Matrice_Morse_Sym& ARR=ref_cast(Matrice_Morse_Sym, A.get_bloc(0,0).valeur());
  Matrice_Morse& ARV=ref_cast(Matrice_Morse, A.get_bloc(0,1).valeur());
  Matrice_Morse& AVR=ref_cast(Matrice_Morse, A.get_bloc(1,0).valeur());
  Matrice_Morse& AVV=ref_cast(Matrice_Morse, A.get_bloc(1,1).valeur());
  // Faces de bord :
  for(int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot();
      for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
        {
          ok=okface(ind_face, face, la_cl);
          if (ok==-1) break;
          elem1=face_voisins(face, 0);
          elem2=face_voisins(face, 1);
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          sort(sommets, face_opp1, face_opp2);
          if(ok==3)
            {
              update_matrice_NeumannP1P1(zone_VEF,
                                         inverse_quantitee_entrelacee,
                                         face, elem1, sommets,
                                         face_opp1, coef_som,
                                         ARR,ARV,AVR,AVV);
            }
          else if(ok==4)
            {
              update_matrice_SymetrieP1P1(zone_VEF,
                                          inverse_quantitee_entrelacee,
                                          face, elem1, sommets,
                                          face_opp1, coef_som,
                                          ARR,ARV,AVR,AVV);
            }
          else
            update_matriceP1P1(zone_VEF,
                               inverse_quantitee_entrelacee,
                               face, elem1, elem2, sommets,
                               face_opp1, face_opp2, coef_som,
                               ARR,ARV,AVR,AVV);
        }
    }
  face_associee=-1;
  for(face=nint; face<nb_faces; face++)
    {
      elem1=face_voisins(face, 0);
      elem2=face_voisins(face, 1);
      if (!zone_VEF.est_une_face_virt_bord(face)) // On ne traite que les faces internes
        {
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          sort(sommets, face_opp1, face_opp2);
          update_matriceP1P1(zone_VEF, inverse_quantitee_entrelacee,
                             face, elem1, elem2, sommets,
                             face_opp1, face_opp2, coef_som,
                             ARR,ARV,AVR,AVV);
        }
    }
  int nb_som=zone_VEF.zone().nb_som();
  for(int i=0; i<nb_som; i++)
    if(ARR(i,i)==0)
      {
        //Cerr << "On modifie la ligne (sommet) orpheline " << i << finl;
        ARR(i,i)=1.;
      }
  Cerr << "Update P1 OK" << finl;
}


void modifieP1P1neumann(const Zone_dis_base& z,
                        const Zone_Cl_dis_base& zcl,
                        Matrice& matrice,
                        const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som)
{

  int dimension=Objet_U::dimension;
  const Zone_VEF_PreP1b& zone_VEF=ref_cast(Zone_VEF_PreP1b, z);
  //  const Zone& zone=zone_VEF.zone();
  const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF, zcl);
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();

  //  int nnz=nb_som;
  ArrOfInt sommets(dimension+2);
  ArrOfInt face_opp1(dimension+2);
  ArrOfInt face_opp2(dimension+2);
  Matrice_Bloc& A=ref_cast(Matrice_Bloc, matrice.valeur());
  Matrice_Morse_Sym& ARR=ref_cast(Matrice_Morse_Sym, A.get_bloc(0,0).valeur());
  // Faces de bord :
  assert(ref_cast(Zone_VEF_PreP1b, z).get_cl_pression_sommet_faible()==0);
  int nb_som_tot=z.nb_som();
  for(int i=0; i<les_cl.size(); i++)
    {

      const Cond_lim_base& la_cl = les_cl[i].valeur();
      if (sub_type(Neumann_sortie_libre,la_cl))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int nb_faces_bord = le_bord.nb_faces_tot();
          const IntTab& faces=zone_VEF.face_sommets();
          int nbsf=faces.dimension(1);
          for(int ind_face=0; ind_face<nb_faces_bord; ind_face++)
            {
              int face=le_bord.num_face(ind_face);
              for(int som=0; som<nbsf; som++)
                {

                  int som_glob=faces(face,som);
                  if (som_glob<nb_som_tot)
                    ARR(som_glob,som_glob)=1e12;
                  //        Cout<<ref_cast(Zone_VEF_PreP1b, z).numero_premier_sommet()<<" ici "<<som_glob<<finl;
                }
            }
        }
    }
  Cerr << "Modifie P1P1 Neumann OK" << finl;
}

void assemblerPaPa(const Zone_dis_base& z,
                   const Zone_Cl_dis_base& zcl,
                   Matrice& matrice,
                   const DoubleTab& inverse_quantitee_entrelacee)
{
  int dimension=Objet_U::dimension;
  const Zone_VEF_PreP1b& zone_VEF=ref_cast(Zone_VEF_PreP1b, z);
  const Zone& zone=zone_VEF.zone();
  const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF, zcl);
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nint = zone_VEF.premiere_face_int();
  int nb_faces = zone_VEF.nb_faces_tot();
  int nb_arete = zone.nb_aretes_tot();
  int elem1, elem2, face, ok;
  int nnz=nb_arete;
  IntLists voisins(nb_arete);
  DoubleLists coeffs(nb_arete);
  DoubleVect diag(nb_arete);
  ArrOfInt sommets(dimension+2);
  ArrOfInt face_opp1(dimension+2);
  ArrOfInt face_opp2(dimension+2);
  // Faces de bord :
  for(int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot();
      for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
        {
          ok=okface(ind_face, face, la_cl);
          if (ok==-1) break;
          elem1=face_voisins(face, 0);
          elem2=face_voisins(face, 1);
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          if(ok==3)
            {
              contribuer_matrice_NeumannPaPa(zone_VEF,
                                             inverse_quantitee_entrelacee,
                                             face, elem1, sommets,
                                             face_opp1,
                                             voisins, coeffs, diag, nnz);
            }
          else if(ok==4)
            {
              contribuer_matrice_SymetriePaPa(zone_VEF,
                                              inverse_quantitee_entrelacee,
                                              face, elem1, sommets,
                                              face_opp1,
                                              voisins, coeffs, diag, nnz);
            }
          else
            contribuer_matricePaPa(zone_VEF,
                                   inverse_quantitee_entrelacee,
                                   face, elem1, elem2, sommets,
                                   face_opp1, face_opp2,
                                   voisins, coeffs, diag, nnz);
        }
    }
  face_associee=-1;
  for(face=nint; face<nb_faces; face++)
    {
      elem1=face_voisins(face, 0);
      elem2=face_voisins(face, 1);
      if (!zone_VEF.est_une_face_virt_bord(face)) // On ne traite que les faces internes
        {
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          contribuer_matricePaPa(zone_VEF,
                                 inverse_quantitee_entrelacee,
                                 face, elem1, elem2, sommets,
                                 face_opp1, face_opp2,
                                 voisins, coeffs, diag, nnz);
        }
    }
  for(int i=0; i<nb_arete; i++)
    if(diag(i)==0)
      {
        // On n'affiche pas car trop sur de gros maillages
        //Cerr << "On modifie la ligne (arete) orpheline " << i << finl;
        diag(i)=1;
      }

  matrice.typer("Matrice_Bloc");
  Matrice_Bloc& matrice_bloc=ref_cast(Matrice_Bloc, matrice.valeur());
  matrice_bloc.remplir(voisins, coeffs, diag, zone.nb_aretes(), zone.nb_aretes_tot());
  Cerr << "Assemblage Pa OK" << finl;
}

void updatePaPa(const Zone_dis_base& z,
                const Zone_Cl_dis_base& zcl,
                Matrice& matrice,
                const DoubleTab& inverse_quantitee_entrelacee)
{
  int dimension=Objet_U::dimension;
  const Zone_VEF_PreP1b& zone_VEF=ref_cast(Zone_VEF_PreP1b, z);
  const Zone& zone=zone_VEF.zone();
  const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF, zcl);
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nint = zone_VEF.premiere_face_int();
  int nb_faces = zone_VEF.nb_faces_tot();
  int nb_arete = zone.nb_aretes();
  int elem1, elem2, face, ok;
  ArrOfInt sommets(dimension+2);
  ArrOfInt face_opp1(dimension+2);
  ArrOfInt face_opp2(dimension+2);
  Matrice_Bloc& A=ref_cast(Matrice_Bloc, matrice.valeur());
  Matrice_Morse_Sym& ARR=ref_cast(Matrice_Morse_Sym, A.get_bloc(0,0).valeur());
  Matrice_Morse& ARV=ref_cast(Matrice_Morse, A.get_bloc(0,1).valeur());
  Matrice_Morse& AVR=ref_cast(Matrice_Morse, A.get_bloc(1,0).valeur());
  Matrice_Morse& AVV=ref_cast(Matrice_Morse, A.get_bloc(1,1).valeur());
  // Faces de bord :
  for(int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot();
      for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
        {
          ok=okface(ind_face, face, la_cl);
          if (ok==-1) break;
          elem1=face_voisins(face, 0);
          elem2=face_voisins(face, 1);
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          if(ok==3)
            {
              update_matrice_NeumannPaPa(zone_VEF,
                                         inverse_quantitee_entrelacee,
                                         face, elem1, sommets,
                                         face_opp1,
                                         ARR, ARV, AVR, AVV);
            }
          else if(ok==4)
            {
              update_matrice_SymetriePaPa(zone_VEF,
                                          inverse_quantitee_entrelacee,
                                          face, elem1, sommets,
                                          face_opp1,
                                          ARR, ARV, AVR, AVV);
            }
          else
            update_matricePaPa(zone_VEF,
                               inverse_quantitee_entrelacee,
                               face, elem1, elem2, sommets,
                               face_opp1, face_opp2,
                               ARR, ARV, AVR, AVV);
        }
    }
  face_associee=-1;
  for(face=nint; face<nb_faces; face++)
    {
      elem1=face_voisins(face, 0);
      elem2=face_voisins(face, 1);
      if (!zone_VEF.est_une_face_virt_bord(face)) // On ne traite que les faces internes
        {
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          update_matricePaPa(zone_VEF,
                             inverse_quantitee_entrelacee,
                             face, elem1, elem2, sommets,
                             face_opp1, face_opp2,
                             ARR,ARV,AVR,AVV);
        }
    }
  for(int i=0; i<nb_arete; i++)
    if(ARR(i,i)==0)
      {
        // On n'affiche pas car trop sur de gros maillages
        //Cerr << "On modifie la ligne (arete) orpheline " << i << finl;
        ARR(i,i)=1;
      }

  Cerr << "Update Pa OK" << finl;
}

void assemblerP0Pa(const Zone_dis_base& z,
                   const Zone_Cl_dis_base& zcl,
                   Matrice& matrice,
                   const DoubleTab& inverse_quantitee_entrelacee)
{
  int dimension=Objet_U::dimension;
  const Zone_VEF_PreP1b& zone_VEF=ref_cast(Zone_VEF_PreP1b, z);
  const Zone& zone=zone_VEF.zone();
  const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF, zcl);
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nint = zone_VEF.premiere_face_int();
  int nb_faces = zone_VEF.nb_faces_tot();
  int nb_elem = zone.nb_elem_tot();
  int elem1, elem2, face, ok;
  int nnz=0;
  IntLists voisins(nb_elem);
  DoubleLists coeffs(nb_elem);
  ArrOfInt sommets(dimension+2);
  ArrOfInt face_opp1(dimension+2);
  ArrOfInt face_opp2(dimension+2);
  // Faces de bord :
  for(int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot();
      for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
        {
          ok=okface(ind_face, face, la_cl);
          if (ok==-1) break;
          elem1=face_voisins(face, 0);
          elem2=face_voisins(face, 1);
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          if(ok==3)
            {
              contribuer_matrice_NeumannP0Pa(zone_VEF,
                                             inverse_quantitee_entrelacee,
                                             face, elem1, sommets,
                                             face_opp1,
                                             voisins, coeffs, nnz);
            }
          else if(ok==4)
            {
              ; // RIEN
            }
          else
            contribuer_matriceP0Pa(zone_VEF, inverse_quantitee_entrelacee, face, elem1, elem2, sommets,
                                   face_opp1, face_opp2,
                                   voisins, coeffs, nnz);
        }
    }
  face_associee=-1;
  for(face=nint; face<nb_faces; face++)
    {
      elem1=face_voisins(face, 0);
      elem2=face_voisins(face, 1);
      if (!zone_VEF.est_une_face_virt_bord(face)) // On ne traite que les faces internes
        {
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          contribuer_matriceP0Pa(zone_VEF,
                                 inverse_quantitee_entrelacee,
                                 face, elem1, elem2, sommets,
                                 face_opp1, face_opp2,
                                 voisins, coeffs, nnz);
        }
    }
  matrice.typer("Matrice_Bloc");
  Matrice_Bloc& matrice_bloc=ref_cast(Matrice_Bloc, matrice.valeur());
  matrice_bloc.remplir(voisins, coeffs, zone.nb_elem(), zone.nb_elem_tot(), zone.nb_aretes(), zone.nb_aretes_tot());
  Cerr << "Assemblage P0Pa OK" << finl;
}

void updateP0Pa(const Zone_dis_base& z,
                const Zone_Cl_dis_base& zcl,
                Matrice& matrice,
                const DoubleTab& inverse_quantitee_entrelacee)
{
  int dimension=Objet_U::dimension;
  const Zone_VEF_PreP1b& zone_VEF=ref_cast(Zone_VEF_PreP1b, z);
  const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF, zcl);
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nint = zone_VEF.premiere_face_int();
  int nb_faces = zone_VEF.nb_faces_tot();
  int elem1, elem2, face, ok;
  ArrOfInt sommets(dimension+2);
  ArrOfInt face_opp1(dimension+2);
  ArrOfInt face_opp2(dimension+2);
  Matrice_Bloc& A=ref_cast(Matrice_Bloc, matrice.valeur());
  Matrice_Morse& ARR=ref_cast(Matrice_Morse, A.get_bloc(0,0).valeur());
  Matrice_Morse& ARV=ref_cast(Matrice_Morse, A.get_bloc(0,1).valeur());
  Matrice_Morse& AVR=ref_cast(Matrice_Morse, A.get_bloc(1,0).valeur());
  Matrice_Morse& AVV=ref_cast(Matrice_Morse, A.get_bloc(1,1).valeur());
  // Faces de bord :
  for(int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot();
      for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
        {
          ok=okface(ind_face, face, la_cl);
          if (ok==-1) break;
          elem1=face_voisins(face, 0);
          elem2=face_voisins(face, 1);
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          if(ok==3)
            {
              update_matrice_NeumannP0Pa(zone_VEF,
                                         inverse_quantitee_entrelacee,
                                         face, elem1, sommets,
                                         face_opp1,
                                         ARR,ARV,AVR,AVV);
            }
          else if(ok==4)
            {
              ; // RIEN
            }
          else
            update_matriceP0Pa(zone_VEF, inverse_quantitee_entrelacee,
                               face, elem1, elem2, sommets,
                               face_opp1, face_opp2,
                               ARR,ARV,AVR,AVV);
        }
    }
  face_associee=-1;
  for(face=nint; face<nb_faces; face++)
    {
      elem1=face_voisins(face, 0);
      elem2=face_voisins(face, 1);
      if (!zone_VEF.est_une_face_virt_bord(face)) // On ne traite que les faces internes
        {
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          update_matriceP0Pa(zone_VEF,
                             inverse_quantitee_entrelacee,
                             face, elem1, elem2, sommets,
                             face_opp1, face_opp2,
                             ARR,ARV,AVR,AVV);
        }
    }
  Cerr << "Update P0Pa OK" << finl;
}

void assemblerP1Pa(const Zone_dis_base& z,
                   const Zone_Cl_dis_base& zcl,
                   Matrice& matrice,
                   const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som)
{
  int dimension=Objet_U::dimension;
  const Zone_VEF_PreP1b& zone_VEF=ref_cast(Zone_VEF_PreP1b, z);
  const Zone& zone=zone_VEF.zone();
  const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF, zcl);
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nint = zone_VEF.premiere_face_int();
  int nb_faces = zone_VEF.nb_faces_tot();
  int elem1, elem2, face, ok;
  int nnz=0;
  int nb_som = zone.nb_som_tot();
  IntLists voisins(nb_som);
  DoubleLists coeffs(nb_som);
  ArrOfInt sommets(dimension+2);
  ArrOfInt face_opp1(dimension+2);
  ArrOfInt face_opp2(dimension+2);
  // Faces de bord :
  for(int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot();
      for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
        {
          ok=okface(ind_face, face, la_cl);
          if (ok==-1) break;
          elem1=face_voisins(face, 0);
          elem2=face_voisins(face, 1);
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          if(ok==3)
            {
              contribuer_matrice_NeumannP1Pa(zone_VEF,
                                             inverse_quantitee_entrelacee,
                                             face, elem1, sommets,
                                             face_opp1,coef_som,
                                             voisins, coeffs, nnz);
            }
          else if(ok==4)
            {
              contribuer_matrice_SymetrieP1Pa(zone_VEF,
                                              inverse_quantitee_entrelacee,
                                              face, elem1, sommets,
                                              face_opp1, coef_som,
                                              voisins, coeffs, nnz);
            }
          else
            contribuer_matriceP1Pa(zone_VEF,
                                   inverse_quantitee_entrelacee,
                                   face, elem1, elem2, sommets,
                                   face_opp1, face_opp2, coef_som,
                                   voisins, coeffs, nnz);
        }
    }
  face_associee=-1;
  for(face=nint; face<nb_faces; face++)
    {
      elem1=face_voisins(face, 0);
      elem2=face_voisins(face, 1);
      if (!zone_VEF.est_une_face_virt_bord(face)) // On ne traite que les faces internes
        {
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          contribuer_matriceP1Pa(zone_VEF,
                                 inverse_quantitee_entrelacee,
                                 face, elem1, elem2, sommets,
                                 face_opp1, face_opp2, coef_som,
                                 voisins, coeffs, nnz);
        }
    }

  matrice.typer("Matrice_Bloc");
  Matrice_Bloc& matrice_bloc=ref_cast(Matrice_Bloc, matrice.valeur());
  matrice_bloc.remplir(voisins, coeffs, zone.nb_som(), zone.nb_som_tot(), zone.nb_aretes(), zone.nb_aretes_tot());
  Cerr << "Assemblage P1Pa OK" << finl;
}

void updateP1Pa(const Zone_dis_base& z,
                const Zone_Cl_dis_base& zcl,
                Matrice& matrice,
                const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som)
{
  int dimension=Objet_U::dimension;
  const Zone_VEF_PreP1b& zone_VEF=ref_cast(Zone_VEF_PreP1b, z);
  const Zone& zone=zone_VEF.zone();
  const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF, zcl);
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nint = zone_VEF.premiere_face_int();
  int nb_faces = zone_VEF.nb_faces_tot();
  int elem1, elem2, face, ok;
  int nb_som = zone.nb_som_tot();
  IntLists voisins(nb_som);
  DoubleLists coeffs(nb_som);
  ArrOfInt sommets(dimension+2);
  ArrOfInt face_opp1(dimension+2);
  ArrOfInt face_opp2(dimension+2);
  Matrice_Bloc& A=ref_cast(Matrice_Bloc, matrice.valeur());
  Matrice_Morse& ARR=ref_cast(Matrice_Morse, A.get_bloc(0,0).valeur());
  Matrice_Morse& ARV=ref_cast(Matrice_Morse, A.get_bloc(0,1).valeur());
  Matrice_Morse& AVR=ref_cast(Matrice_Morse, A.get_bloc(1,0).valeur());
  Matrice_Morse& AVV=ref_cast(Matrice_Morse, A.get_bloc(1,1).valeur());
  // Faces de bord :
  for(int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot();
      for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
        {
          ok=okface(ind_face, face, la_cl);
          if (ok==-1) break;
          elem1=face_voisins(face, 0);
          elem2=face_voisins(face, 1);
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          if(ok==3)
            {
              update_matrice_NeumannP1Pa(zone_VEF,
                                         inverse_quantitee_entrelacee,
                                         face, elem1, sommets,
                                         face_opp1, coef_som,
                                         ARR,ARV,AVR,AVV);
            }
          else if(ok==4)
            {
              update_matrice_SymetrieP1Pa(zone_VEF,
                                          inverse_quantitee_entrelacee,
                                          face, elem1, sommets,
                                          face_opp1, coef_som,
                                          ARR,ARV,AVR,AVV);
            }
          else
            update_matriceP1Pa(zone_VEF,
                               inverse_quantitee_entrelacee,
                               face, elem1, elem2, sommets,
                               face_opp1, face_opp2, coef_som,
                               ARR,ARV,AVR,AVV);
        }
    }
  face_associee=-1;
  for(face=nint; face<nb_faces; face++)
    {
      elem1=face_voisins(face, 0);
      elem2=face_voisins(face, 1);
      if (!zone_VEF.est_une_face_virt_bord(face)) // On ne traite que les faces internes
        {
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          update_matriceP1Pa(zone_VEF,
                             inverse_quantitee_entrelacee,
                             face, elem1, elem2, sommets,
                             face_opp1, face_opp2,coef_som,
                             ARR,ARV,AVR,AVV);
        }
    }

  Cerr << "Update P1Pa OK" << finl;
}

void assemblerP0P1(const Zone_dis_base& z,
                   const Zone_Cl_dis_base& zcl,
                   Matrice& matrice,
                   const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som)
{
  int dimension=Objet_U::dimension;
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF_PreP1b, z);
  const Zone& zone=zone_VEF.zone();
  const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF, zcl);
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nint = zone_VEF.premiere_face_int();
  int nb_faces = zone_VEF.nb_faces_tot();
  int nb_elem = zone.nb_elem_tot();
  int elem1, elem2, face, ok;
  int nnz=0;
  IntLists voisins(nb_elem);
  DoubleLists coeffs(nb_elem);
  ArrOfInt sommets(dimension+2);
  ArrOfInt face_opp1(dimension+2);
  ArrOfInt face_opp2(dimension+2);
  // Faces de bord :
  for(int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot();
      for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
        {
          ok=okface(ind_face, face, la_cl);
          if (ok==-1) break;
          elem1=face_voisins(face, 0);
          elem2=face_voisins(face, 1);
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          sort(sommets, face_opp1, face_opp2);
          if(ok==3)
            {
              contribuer_matrice_NeumannP0P1(zone_VEF,
                                             inverse_quantitee_entrelacee,
                                             face, elem1, sommets,
                                             face_opp1, coef_som,
                                             voisins, coeffs, nnz);
            }
          else if(ok==4)
            {
              ;// RIEN
            }
          else
            contribuer_matriceP0P1(zone_VEF,
                                   inverse_quantitee_entrelacee,
                                   face, elem1, elem2, sommets,
                                   face_opp1, face_opp2, coef_som,
                                   voisins, coeffs, nnz);
        }
    }
  face_associee=-1;
  for(face=nint; face<nb_faces; face++)
    {
      elem1=face_voisins(face, 0);
      elem2=face_voisins(face, 1);
      if (!zone_VEF.est_une_face_virt_bord(face)) // On ne traite que les faces internes
        {
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          sort(sommets, face_opp1, face_opp2);
          contribuer_matriceP0P1(zone_VEF,
                                 inverse_quantitee_entrelacee,
                                 face, elem1, elem2, sommets,
                                 face_opp1, face_opp2, coef_som,
                                 voisins, coeffs, nnz);
        }
    }

  matrice.typer("Matrice_Bloc");
  Matrice_Bloc& matrice_bloc=ref_cast(Matrice_Bloc, matrice.valeur());
  matrice_bloc.remplir(voisins, coeffs, zone.nb_elem(), zone.nb_elem_tot(), zone.nb_som(), zone.nb_som_tot());
  Cerr << "Assemblage POP1 OK" << finl;
}

void updateP0P1(const Zone_dis_base& z,
                const Zone_Cl_dis_base& zcl,
                Matrice& matrice,
                const DoubleTab& inverse_quantitee_entrelacee, const ArrOfDouble& coef_som)
{
  int dimension=Objet_U::dimension;
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF_PreP1b, z);
  const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF, zcl);
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nint = zone_VEF.premiere_face_int();
  int nb_faces = zone_VEF.nb_faces_tot();
  int elem1, elem2, face, ok;
  ArrOfInt sommets(dimension+2);
  ArrOfInt face_opp1(dimension+2);
  ArrOfInt face_opp2(dimension+2);
  Matrice_Bloc& A=ref_cast(Matrice_Bloc, matrice.valeur());
  Matrice_Morse& ARR=ref_cast(Matrice_Morse, A.get_bloc(0,0).valeur());
  Matrice_Morse& ARV=ref_cast(Matrice_Morse, A.get_bloc(0,1).valeur());
  Matrice_Morse& AVR=ref_cast(Matrice_Morse, A.get_bloc(1,0).valeur());
  Matrice_Morse& AVV=ref_cast(Matrice_Morse, A.get_bloc(1,1).valeur());
  // Faces de bord :
  for(int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot();
      for(int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
        {
          ok=okface(ind_face, face, la_cl);
          if (ok==-1) break;
          elem1=face_voisins(face, 0);
          elem2=face_voisins(face, 1);
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          sort(sommets, face_opp1, face_opp2);
          if(ok==3)
            {
              update_matrice_NeumannP0P1(zone_VEF,
                                         inverse_quantitee_entrelacee,
                                         face, elem1, sommets,
                                         face_opp1, coef_som,
                                         ARR,ARV,AVR,AVV);
            }
          else if(ok==4)
            {
              ;// RIEN
            }
          else
            update_matriceP0P1(zone_VEF,
                               inverse_quantitee_entrelacee,
                               face, elem1, elem2, sommets,
                               face_opp1, face_opp2, coef_som,
                               ARR,ARV,AVR,AVV);
        }
    }
  face_associee=-1;
  for(face=nint; face<nb_faces; face++)
    {
      elem1=face_voisins(face, 0);
      elem2=face_voisins(face, 1);
      if (!zone_VEF.est_une_face_virt_bord(face)) // On ne traite que les faces internes
        {
          remplir_sommets(zone_VEF, face, elem1, elem2, sommets,
                          face_opp1, face_opp2);
          sort(sommets, face_opp1, face_opp2);
          update_matriceP0P1(zone_VEF,
                             inverse_quantitee_entrelacee,
                             face, elem1, elem2, sommets,
                             face_opp1, face_opp2, coef_som,
                             ARR,ARV,AVR,AVV);
        }
    }
  Cerr << "Update POP1 OK" << finl;
}


