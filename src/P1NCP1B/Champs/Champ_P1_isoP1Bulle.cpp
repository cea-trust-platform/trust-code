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

#include <Champ_P1_isoP1Bulle.h>
#include <Domaine.h>
#include <Debog.h>
#include <TRUSTTab_parts.h>

Implemente_instanciable(Champ_P1_isoP1Bulle,"Champ_P1_isoP1Bulle",Champ_Inc_base);

// printOn et readOn

Sortie& Champ_P1_isoP1Bulle::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Champ_P1_isoP1Bulle::readOn(Entree& is )
{
  lire_donnees(is) ;
  return is ;
}

int Champ_P1_isoP1Bulle::fixer_nb_valeurs_nodales(int n)
{
  // On ne doit pas specifier nb_ddl !
  assert(n < 0);
  const Zone_VEF_PreP1b& zvef = zone_vef();
  const MD_Vector& md = zvef.md_vector_p1b();
  creer_tableau_distribue(md);
  return n;
}

double Champ_P1_isoP1Bulle::norme_L2(const Domaine& dom) const
{
  const Zone_VEF_PreP1b& zvef = zone_vef();
  const IntTab& som_elem=zvef.zone().les_elems();
  const DoubleVect& volumes=zvef.volumes();

  int prs=zvef.numero_premier_sommet();
  DoubleTab q(valeurs());
  double norme=0, s=0;
  double alpha =1./(dimension*(dimension+1));
  int i, k;
  double somme=0;
  int sz=q.size();
  for(i=prs; i<sz; i++)
    somme+=q(i);
  somme/=(sz-prs);
  for(i=0; i<prs; i++)
    q(i)+=alpha*somme;
  for(i=prs; i<sz; i++)
    q(i)-=somme;
  for(i=0; i<prs; i++)
    {
      double m=0;
      for(k=0; k<(dimension+1); k++)
        m+=q(prs+som_elem(i,k));
      m*=alpha;
      m+=q(i);
      s+= m*volumes(i);
      norme+= volumes(i);
    }
  Cerr << "norme L1 : " << s << finl;

  s/=norme;
  for(i=0; i<prs; i++)
    q(i)-=s;
  s=norme=0;
  for(i=0; i<prs; i++)
    {
      double m=0;
      for(k=0; k<(dimension+1); k++)
        m+=q(prs+som_elem(i,k));
      m*=alpha;
      m+=q(i);
      m*=m;
      norme+= volumes(i)*m;

      for(k=0; k<(dimension+1); k++)
        {
          double v=0;
          for(int l=0; l<(dimension+1); l++)
            {
              if(k!=l)
                v+=q(prs+som_elem(i,l));
            }
          v*=alpha;
          v*=v;
          v/=(dimension+1);
          norme+= volumes(i)*v;
        }
    }
  return sqrt(norme);
}
Champ_base& Champ_P1_isoP1Bulle::affecter_(const Champ_base& ch)
{
  DoubleTab noeuds;
  const Zone_VEF_PreP1b& zvef = zone_vef();

  remplir_coord_noeuds(noeuds);

  DoubleTab_parts parties_P(valeurs());
  Debog::verifier("affecter::champ ch", ch.valeurs());
  if (zvef.get_alphaE() && zvef.get_alphaS())
    {


      DoubleTab& Pk = parties_P[0];  // partie elements
      DoubleTab& Ps = parties_P[1];  // partie sommets

      //const Zone_VEF_PreP1b& zvef=zone_vef();
      const Zone& la_zone=zvef.zone();
      const Domaine& dom=la_zone.domaine();
      const DoubleTab& coord_sommets=dom.coord_sommets();
      const DoubleTab& xg = zvef.xp();
      ch.valeur_aux(xg, Pk);
      ch.valeur_aux(coord_sommets, Ps);
    }
  else
    ch.valeur_aux(noeuds, valeurs());

  //  abort();
  Debog::verifier("affecter::champ brut", valeurs());
  // GF on retire la partie moyenne aux elements si element et sommet
  if (zvef.get_alphaE() && zvef.get_alphaS())
    {

      //int nb_som=zvef.nb_som();
      DoubleVect& Pk = parties_P[0];  // partie elements
      DoubleVect& Ps = parties_P[1];  // partie sommets

      /*
      // on regarde si Ps vaut 0 on recupere la p aux elems
      // bidouille ....
      const DoubleTab& coord=zvef.zone().domaine().coord_sommets();
      for (int s=0;s<nb_som;s++)
      if (Ps(s)==0)
      {
      abort();
      double x=coord(s,0);
      double y=coord(s,1);
      double z=0;
      if (dimension==3)
      z=coord(s,2);
      int elem=zvef.zone().chercher_elements(x,y,z);
      Ps(s)=valeurs()(elem);
      }
      */

      // on retire d'abord la moyenne des sommets a chaque element
      const IntTab& som_elem=zvef.zone().les_elems();

      int prs=zvef.numero_premier_sommet();
      int i, k ;
      double alpha =1./(dimension);
      //DoubleTab& p = valeurs();
      for(i=0; i<prs; i++)
        {
          double m=0;
          for(k=0; k<(dimension+1); k++)
            m+=Ps(som_elem(i,k));
          m*=alpha;
          Pk(i)-=m;
        }
      Debog::verifier("affecter::champ milieu", valeurs());
      // Tableaux d'acces aux valeurs Pk et Ps


      double moyenne_K = mp_moyenne_vect(Pk);           // Calcul de la moyenne du champ aux elements
      Pk -= moyenne_K;
      valeurs().echange_espace_virtuel();


      Debog::verifier("affecter::champ final", valeurs());
      //Cerr<<"minPS "<<min_abs(Ps)<<finl;exit();
    }

  return *this;

}

const Zone_dis_base& Champ_P1_isoP1Bulle::zone_dis_base() const
{
  return la_Zone_VEF_PreP1b.valeur();
}

void
Champ_P1_isoP1Bulle::associer_zone_dis_base(const Zone_dis_base& une_zone_dis_base)
{
  la_Zone_VEF_PreP1b = ref_cast(Zone_VEF_PreP1b, une_zone_dis_base);
}

DoubleTab& Champ_P1_isoP1Bulle::remplir_coord_noeuds(DoubleTab& coord) const
{
  return Champ_P1iP1B_implementation::remplir_coord_noeuds(coord);
}

DoubleVect& Champ_P1_isoP1Bulle::valeur_a_elem(const DoubleVect& position,
                                               DoubleVect& val,
                                               int le_poly) const
{
  return Champ_P1iP1B_implementation::valeur_a_elem(position, val, le_poly);
}

double Champ_P1_isoP1Bulle::valeur_a_elem_compo(const DoubleVect& position,
                                                int le_poly, int ncomp) const
{
  return Champ_P1iP1B_implementation::valeur_a_elem_compo(position, le_poly, ncomp);
}

DoubleTab& Champ_P1_isoP1Bulle::valeur_aux_elems(const DoubleTab& positions,
                                                 const IntVect& les_polys,
                                                 DoubleTab& val) const
{
  return Champ_P1iP1B_implementation::valeur_aux_elems(positions, les_polys, val);
}

DoubleVect& Champ_P1_isoP1Bulle::valeur_aux_elems_compo(const DoubleTab& positions,
                                                        const IntVect& les_polys,
                                                        DoubleVect& val,
                                                        int ncomp) const
{
  return Champ_P1iP1B_implementation::valeur_aux_elems_compo(positions, les_polys, val, ncomp);
}

DoubleTab& Champ_P1_isoP1Bulle::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps,int distant) const
{
  if (!est_egal(temps_,tps))
    {
      Cerr<<"La methode Champ_P1_isoP1Bulle::trace effectue une interpolation des valeurs"<<finl;
      Cerr<<"du champ dont on veut prendre la trace sur la frontiere"<<finl;
      Cerr<<"Cette interpolation utilise par defaut les valeurs au temps present "<<temps_<<finl;
      Cerr<<"de ce champ qui ne correspond pas au temps passe en parametre "<<tps<<finl;
      exit();
    }
  return Champ_P1iP1B_implementation::trace(fr, valeurs(tps), x,distant);
}

double Champ_P1_isoP1Bulle::valeur_au_bord(int face) const
{
  const DoubleTab& val = valeurs();
  const Zone_VEF_PreP1b& zone_VEF=la_Zone_VEF_PreP1b.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntTab& som_elem=zone_VEF.zone().les_elems();
  int nps=zone_VEF.numero_premier_sommet();

  int elem = face_voisins(face,0);
  if (face_voisins(face,1)!=-1)
    {
      Cerr << "Erreur dans Champ_P1_isoP1Bulle::valeur_au_bord." << finl;
      Cerr << "La face n'est pas une face de bord." << finl;
      exit();
    }

  int som_opp=zone_VEF.get_num_fac_loc(face, 0);
  double val_bord = 0;
  for(int i=0; i<(dimension+1); i++)
    if(i!=som_opp)
      {
        val_bord+=val[nps+som_elem(elem, i)];
      }
  val_bord/=dimension;
  return val_bord;
}

void Champ_P1_isoP1Bulle::completer(const Zone_Cl_dis_base& zcl)
{
  Champ_P1iP1B_implementation::completer(zcl);
}
