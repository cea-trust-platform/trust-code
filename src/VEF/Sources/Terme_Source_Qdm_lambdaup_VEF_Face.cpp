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
// File:        Terme_Source_Qdm_lambdaup_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Sources
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Qdm_lambdaup_VEF_Face.h>
#include <Zone_VEF.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Champ_P1NC.h>
#include <SFichier.h>
#include <Schema_Temps.h>

Implemente_instanciable(Terme_Source_Qdm_lambdaup_VEF_Face,"Source_Qdm_lambdaup_VEF_P1NC",Source_base);



//// printOn
//

Sortie& Terme_Source_Qdm_lambdaup_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Terme_Source_Qdm_lambdaup_VEF_Face::readOn(Entree& is )
{
  lambda=-1.;
  lambda_min=1.e-24;
  lambda_max=2;
  cible=0.01;
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle motlu;
  is >> motlu;
  if(motlu!=accouverte)
    {
      Cerr << motlu << " --> " << finl;
      Cerr << "{ attendue a la lecture du terme source lambda uprime " << finl;
      Cerr << "La syntaxe du mot cle Source_Qdm_lambdaup a change, voir le" << finl;
      Cerr << "manuel utilisateur de la version 1.5.5 ou plus recent." << finl;
      Cerr << "La valeur de lambda, qui peut etre desormais variable" << finl;
      Cerr << "doit etre precedee d'un mot cle:" << finl;
      Cerr << "Ainsi, Source_Qdm_lambdaup valeur devient:" <<finl;
      Cerr << "Source_Qdm_lambdaup { lambda valeur }" <<finl;
      Process::exit();
    }
  Motcles les_mots(4);
  {
    les_mots[0] = "lambda";
    les_mots[1] = "lambda_min";
    les_mots[2] = "lambda_max";
    les_mots[3] = "ubar_umprim_cible";
  }
  lambda=-1;
  while(motlu!=accfermee)
    {
      is>>motlu;
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0:
          {
            is >> lambda;
            lambda_min=lambda;
            lambda_max=lambda;
            break;
          }
        case 1:
          {
            is >> lambda_min;
            break;
          }
        case 2:
          {
            is >> lambda_max;
            break;
          }
        case 3:
          {
            is >> cible;
            break;
          }
        default :
          {
            if(motlu!=accfermee)
              {
                Cerr << motlu
                     << " non compris dans terme source lambda uprime "
                     << finl;
                Process::exit();
              }
          }
        }
    }
  if(lambda < 0)
    lambda=lambda_min;
  if (lambda < 0.)
    {
      Cerr << "Erreur a la lecture de lambda dans " << que_suis_je() << finl;
      Cerr << " lambda doit etre defini et superieur a 0 " << finl;
      exit();
    }
  Cerr << "Sortie du readOn " << finl;
  return is ;
}

void Terme_Source_Qdm_lambdaup_VEF_Face::associer_pb(const Probleme_base& pb)
{
  int nb_eqn = pb.nombre_d_equations();
  int ok=0;
  for (int i=0; i<nb_eqn; i++)
    {
      const Equation_base& eqn = pb.equation(i);
      if  (sub_type(Navier_Stokes_std,eqn))
        {
          la_vitesse = ref_cast(Champ_P1NC,eqn.inconnue().valeur());
          associer_zones(eqn.zone_dis(),eqn.zone_Cl_dis());
          i = nb_eqn;
          ok = 1;
        }
      else
        Cerr << "Lambdaup : " << eqn.que_suis_je() << finl;
    }

  if (!ok)
    {
      Cerr << "Erreur TRUST dans " << que_suis_je()  << finl;
      Cerr << "On ne trouve pas d'equation d'hydraulique dans le probleme" << finl;
      exit();
    }
}

void Terme_Source_Qdm_lambdaup_VEF_Face::associer_zones(const Zone_dis& zone_dis,
                                                        const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VEF = ref_cast(Zone_VEF, zone_dis.valeur());
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF, zone_Cl_dis.valeur());
}


DoubleTab& Terme_Source_Qdm_lambdaup_VEF_Face::ajouter(DoubleTab& resu) const
{
  static double rapport_old=1.;
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const DoubleVect& volumes_entrelaces=zone_VEF.volumes_entrelaces();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const DoubleTab& vitesse=la_vitesse.valeur().valeurs();
  DoubleTab ubar(vitesse);
  DoubleTab uprime(vitesse);
  const int nb_faces = zone_VEF.nb_faces();
  int face, k;

  int nb_comp = resu.line_size();

  la_vitesse.valeur().filtrer_L2(ubar);
  uprime-=ubar;
  double normbar=mp_norme_vect(ubar);
  double normprim=mp_norme_vect(uprime);
  //double rapport=normbar/(normprim+DMINFLOAT) ;
  double rapport=(normprim!=0?normbar/normprim:0);
  double& l=lambda;
  if( (rapport < cible) && (rapport<rapport_old) ) l*=2.;
  if( (rapport < cible) && (rapport>rapport_old) ) l*=1.1;
  if( (rapport>cible) && (rapport>rapport_old)  )  l*=0.95;
  l=std::min(lambda, lambda_max);
  l=std::max(lambda_min, lambda);
  if (Process::je_suis_maitre())
    {
      Cerr << "|ubar| = " << normbar << finl;
      Cerr << "|ubar|/|uprim| = " << rapport << finl;
      Cerr << "lambda " << lambda << finl;
    }
  for(face=0; face<nb_faces; face++)
    for(k=0; k< dimension; k++)
      {
        double v=volumes_entrelaces(face)*porosite_face(face);
        if(nb_comp>1)
          {
            uprime(face,k)*=v;
          }
        else
          {
            uprime(face)*=v;
          }
      }

  double dt=la_vitesse.valeur().equation().schema_temps().pas_de_temps();
  resu.ajoute(-lambda/dt, uprime);
  rapport_old=rapport;
  return resu;
}

DoubleTab& Terme_Source_Qdm_lambdaup_VEF_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

void Terme_Source_Qdm_lambdaup_VEF_Face::mettre_a_jour(double temps)
{
  ;
}

double Terme_Source_Qdm_lambdaup_VEF_Face::norme_H1(const DoubleTab& vitesse) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone& zone = zone_VEF.zone();

  double dnorme_H1,norme_H1_comp,int_grad_elem,norme_grad_elem;
  int face_globale;

  //On va calculer la norme H1 d'une inconnue P1NC.
  //L'algorithme tient compte des contraintes suivantes:
  //- l'inconnue peut avoir plusieurs composantes
  //  (i.e etre un scalaire ou etre un vecteur)
  //- la dimension du probleme est arbitraire (1, 2 ou 3).
  //ATTENTION: les prismes ne sont pas supportes.

  dnorme_H1=0.;
  for (int composante=0; composante<vitesse.line_size(); composante++)
    {
      norme_H1_comp=0.; //pour eviter les accumulations
      for (int K=0; K<zone.nb_elem(); K++) //boucle sur les elements
        {
          norme_grad_elem=0.; //pour eviter les accumulations
          for (int i=0; i<dimension; i++) //boucle sur la dimension du pb
            {
              int_grad_elem=0.; //pour eviter les accumulations
              for (int face=0; face<zone.nb_faces_elem(); face++) //boucle sur les faces d'un "K"
                {
                  face_globale = zone_VEF.elem_faces(K,face);

                  int_grad_elem += vitesse(face_globale,composante)*
                                   zone_VEF.face_normales(face_globale,i)*
                                   zone_VEF.oriente_normale(face_globale,K);
                } //fin du for sur "face"

              norme_grad_elem += int_grad_elem*int_grad_elem;
            } //fin du for sur "i"

          norme_H1_comp += norme_grad_elem/zone_VEF.volumes(K);
        } //fin du for sur "K"

      dnorme_H1 += norme_H1_comp;
    } // fin du for sur "composante"

  return sqrt(dnorme_H1);
}

double Terme_Source_Qdm_lambdaup_VEF_Face::norme_L2_H1(const DoubleTab& u) const
{
  double pas_de_temps = la_vitesse.valeur().equation().schema_temps().pas_de_temps();

  return ((1./pas_de_temps)*norme_L2(u))+norme_H1(u);
}

double Terme_Source_Qdm_lambdaup_VEF_Face::norme_L2(const DoubleTab& u) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const DoubleVect& volumes = zone_VEF.volumes_entrelaces();
  const int nb_faces = zone_VEF.nb_faces();

  int i=0;
  double norme =0;
  int nb_compo_=u.line_size();

  for(; i<nb_faces; i++)
    {
      double s=0;
      for(int j=0; j<nb_compo_; j++)
        s+=u(i,j)*u(i,j);
      norme+=volumes(i)*s;
    }

  return sqrt(norme);
}

