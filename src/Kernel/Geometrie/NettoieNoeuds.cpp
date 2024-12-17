/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <NettoieNoeuds.h>
#include <Domaine.h>
#include <Scatter.h>

Implemente_instanciable_32_64(NettoieNoeuds_32_64, "NettoiePasNoeuds", Interprete_geometrique_base_32_64<_T_>);
// XD nettoiepasnoeuds interprete nettoiepasnoeuds 0 Keyword NettoiePasNoeuds does not delete useless nodes (nodes without elements) from a domain.
// XD  attr domain_name ref_domaine domain_name 0 Name of domain.

template <typename _SIZE_>
Sortie& NettoieNoeuds_32_64<_SIZE_>::printOn(Sortie& os) const { return Interprete::printOn(os); }

template <typename _SIZE_>
Entree& NettoieNoeuds_32_64<_SIZE_>::readOn(Entree& is) { return Interprete::readOn(is); }

template<> int NettoieNoeuds_32_64<int>::NettoiePasNoeuds=0;
#if INT_is_64_ == 2
template<> int NettoieNoeuds_32_64<trustIdType>::NettoiePasNoeuds=0;
#endif

/*! @brief Fonction principale de l'interprete NettoieNoeuds_32_64 Structure du jeu de donnee (en dimension 2) :
 *
 *     NettoieNoeuds_32_64 dom alpha
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree
 * @throws l'objet a mailler n'est pas du type Domaine
 */
template <typename _SIZE_>
Entree& NettoieNoeuds_32_64<_SIZE_>::interpreter_(Entree& is)
{
  this->associer_domaine(is);

  if (this->domaine().les_sommets().size()!=0)
    {
      Cerr << "The keyword NettoiePasNoeuds must be placed before the ";
      Cerr << "reading of the domain to avoid the cleaning of the double";
      Cerr << "nodes of the domain." << finl;
      Process::exit();
    }
  NettoiePasNoeuds=1;
  Cerr << "NettoiePasNoeuds=" << NettoiePasNoeuds << " for the domain " << this->domaine().le_nom() << finl;

  return is;
}

template <typename _SIZE_>
void NettoieNoeuds_32_64<_SIZE_>::nettoie(Domaine_t& dom)
{
  if (NettoiePasNoeuds==1)
    return;

  // Autorise la modification de structure des tableaux sommets et elements:
  Scatter::uninit_sequential_domain(dom);

  DoubleTab_t& coord_sommets=dom.les_sommets();
  IntTab_t& les_elems=dom.les_elems();
  int_t nb_som_init=coord_sommets.dimension(0);
  int_t nb_elem=les_elems.dimension(0);
  int nb_som_elem=(int)les_elems.dimension(1);
  ArrOfInt_t renum_som_old2new(nb_som_init);

  int_t compteur=0;

  {
    //int_t i;
    for (int_t i=0; i<nb_som_init ; i++)
      renum_som_old2new[i]=-1;

    for (int_t i=0; i<nb_elem ; i++)
      for (int j=0; j< nb_som_elem; j++)
        {
          // GF dans le cas ou on a des polyedres
          if (les_elems(i,j)==-1) break;
          int_t& tmp=renum_som_old2new[les_elems(i,j)];
          assert (tmp < compteur);
          if(tmp==-1)
            tmp=compteur++;
        }
    // ajout GF sinon un sommet double sur un bord devient -1
    int nb_bords=dom.nb_front_Cl();
    for (int ii=0; ii<nb_bords; ii++)
      {
        const Frontiere_t& front= dom.frontiere(ii);
        const Faces_t& faces=front.faces();
        const IntTab_t& faces_sommets=faces.les_sommets();
        int_t nb_faces=faces_sommets.dimension(0);
        int nb_som_face=(int)faces_sommets.dimension(1);
        for(int_t i=0; i<nb_faces; i++)
          for(int j=0; j<nb_som_face; j++)
            {
              // dans le cas ou l'on a des polygones
              if (faces_sommets(i,j)==-1) break;
              int_t& tmp=renum_som_old2new[faces_sommets(i,j)];
              assert (tmp < compteur);
              if(tmp==-1)
                tmp=compteur++;
            }
      }
  }
  int_t nb_som_new=compteur;
  Cerr << "Elimination of " << nb_som_init - nb_som_new << " unnecessary nodes in the domain " << dom.le_nom() << finl;
  if(nb_som_new!=nb_som_init)
    {
      ArrOfInt_t renum_som_new2old(nb_som_new);
      {
        int_t j;
        for (int_t i=0; i<nb_som_init ; i++)
          if((j=renum_som_old2new[i])!=-1)
            renum_som_new2old[j]=i;
      }
      {
        int_t i;
        int j;
        DoubleTab_t new_coord(nb_som_new, Objet_U::dimension);

        for (i=0; i<nb_som_new ; i++)
          for (j=0; j<Objet_U::dimension ; j++)
            {
              new_coord(i,j)=coord_sommets(renum_som_new2old[i],j);
            }
        coord_sommets=new_coord;

        for (i=0; i<nb_elem ; i++)
          for (j=0; j<nb_som_elem ; j++)
            {
              int_t num_som=les_elems(i,j);
              if (num_som!=-1)
                les_elems(i,j)=renum_som_old2new[num_som];
              else
                les_elems(i,j)=-1;
            }
      }

      // On recupere les bords :
      for (auto &itr : dom.faces_bord())
        {
          Frontiere_t& front = itr;
          Faces_t& faces = front.faces();
          IntTab_t& faces_sommets = faces.les_sommets();
          IntTab_t old_faces_sommets(faces.les_sommets());
          int_t nb_faces = faces_sommets.dimension(0);
          int nb_som_face = (int)faces_sommets.dimension(1);
          for (int_t i = 0; i < nb_faces; i++)
            for (int j = 0; j < nb_som_face; j++)
              {
                int_t som = old_faces_sommets(i, j);
                if (som != -1)
                  faces_sommets(i, j) = renum_som_old2new[som];
                else
                  faces_sommets(i, j) = som;
              }
        }

      // Handy renum lambda:
      auto renum_lamb = [&](auto& itr_arg)
      {
        Frontiere_t& front = itr_arg;
        Faces_t& faces = front.faces();
        IntTab_t& faces_sommets = faces.les_sommets();
        IntTab_t old_faces_sommets(faces.les_sommets());
        int_t nb_faces = faces_sommets.dimension(0);
        int nb_som_face = (int)faces_sommets.dimension(1);
        for (int_t i = 0; i < nb_faces; i++)
          for (int j = 0; j < nb_som_face; j++)
            faces_sommets(i, j) = renum_som_old2new[old_faces_sommets(i, j)];
      };

      // Les Bords Internes :
      for (auto &itr : dom.bords_int())
        renum_lamb(itr);
      // Les Raccords
      for (auto &itr : dom.faces_raccord())
        renum_lamb(itr);
      // Les Groupes de Faces_t :
      for (auto &itr : dom.groupes_faces())
        renum_lamb(itr);
    }
  Scatter::init_sequential_domain(dom);
}

/*! @brief regarde si on n'a pas des noeuds doubles
 *
 */
template <typename _SIZE_>
void NettoieNoeuds_32_64<_SIZE_>::verifie_noeuds(const Domaine_t& dom)
{
  Cerr<<finl<<"Beginning of the search of identical nodes (can be long, algorithm in n^2)..."<<finl;
  const DoubleTab_t& coord_sommets=dom.les_sommets();
  int_t ns=coord_sommets.dimension(0);
  //  int nbsomelem=coord_sommets.dimension(1);
  //  const IntTab_t& les_elems= dom.les_elems();
  int err=0;
  // la version avec chercher element plus rapide a priori
  // n log(n) a des chances de rater des sommets doubles
  // on revien ta un algo en n*n/2
  /*  for (int sommet=0;sommet<ns;sommet++)
      {

      double x,y,z=0;
      x=coord_sommets(sommet,0);
      y=coord_sommets(sommet,1);
      if (dimension>2)
      z=coord_sommets(sommet,0);

      int elem2=dom.chercher_elements(x,y,z);
      if (elem2==-1)
      {
      Cerr<<"unable to find node "<<finl;
      Process::exit();
      }
      int som;
      for ( som=0;som<nbsomelem;som++)
      {
      int sommet2=les_elems(elem2,som);
      int ok=1;
      for (int dir=0;dir<Objet_U::dimension;dir++)
      ok=ok&&(est_egal(coord_sommets(sommet,dir),coord_sommets(sommet2,dir)));
      if (ok)
      {
      if (sommet!=sommet2)
      {
      Cerr<<" the nodes "<<sommet<<" and "<<sommet2<<" seem the same"<<finl;
      err=1;
      }
      }
      }
      }
  */
  int_t tmp,pourcent=0;
  for (int_t sommet=0; sommet<ns; sommet++)
    {
      tmp =(sommet*100)/ns;
      if(tmp>=pourcent)
        {
          pourcent = tmp;
          Cerr << pourcent << "% of nodes have been verified\r " << flush;
        }
      for (int_t sommet2=sommet+1; sommet2<ns; sommet2++)
        {
          int ok=1;
          for (int dir=0; dir<Objet_U::dimension; dir++)
            ok=ok&&(est_egal(coord_sommets(sommet,dir),coord_sommets(sommet2,dir)));
          if (ok)
            {
              if (sommet!=sommet2)
                {
                  Cerr<< finl << "The nodes "<<sommet<<" and "<<sommet2<<" are the same !"<<finl;
                  err=1;
                }
            }
        }
    }
  Cerr<<"End of the search of identical nodes."<<finl;
  if (err)
    Process::exit();
}


template class NettoieNoeuds_32_64<int>;
#if INT_is_64_ == 2
template class NettoieNoeuds_32_64<trustIdType>;
#endif


