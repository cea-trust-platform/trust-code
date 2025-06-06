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

#include <RegroupeBord.h>
#include <EFichier.h>

Implemente_instanciable_32_64(RegroupeBord_32_64,"RegroupeBord",Interprete_geometrique_base_32_64<_T_>);
// XD regroupebord interprete regroupebord -1 Keyword to build one boundary new_bord with several boundaries of the domain named domaine.
// XD   attr domaine ref_domaine domain 0 Name of domain
// XD   attr new_bord chaine new_bord 0 Name of the new boundary
// XD   attr bords bloc_lecture bords 0 { Bound1 Bound2 }

template <typename _SIZE_>
Sortie& RegroupeBord_32_64<_SIZE_>::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

template <typename _SIZE_>
Entree& RegroupeBord_32_64<_SIZE_>::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

template <typename _SIZE_>
Entree& RegroupeBord_32_64<_SIZE_>::interpreter_(Entree& is)
{
  Nom nom;
  LIST(Nom) nlistbord;
  this->associer_domaine(is);
  is >> nom;

  //is>>nlistbord;
  // recodage pour ne pas mettre les virgules
  {
    Nom mot;
    is >>mot;
    if (mot!="{")
      {
        Cerr<< "we expected { and not "<<mot<<finl;
        this->exit();
      }
    is >>mot;
    while (mot!="}")
      {
        nlistbord.add(mot);
        is>>mot;
      }
  }

  Domaine_t& dom = this->domaine();
  // test pour savoir si la frontiere a regrouper est valide
  // i.e : si frontiere est une frontiere du domaine  : on applique regroupe_bord()
  LIST(Nom) nlistbord_dom; // liste stockant tous les noms de frontiere du domaine
  int nbfr=dom.nb_front_Cl();
  for (int b=0; b<nbfr; b++)
    {
      Frontiere_t& org=dom.frontiere(b);
      nlistbord_dom.add(org.le_nom());
    }
  for (int i=0; i<nlistbord.size(); i++)
    {
      if (!nlistbord_dom.contient(nlistbord[i]))
        {
          Cerr << "Problem in the RegroupeBord instruction " << finl;
          Cerr << "The boundary named " << nlistbord[i] << " is not a boundary of the domain " << dom.le_nom() << finl;
          this->exit();

        }
    }

  regroupe_bord(dom,nom,nlistbord);
  return is;
}

template <typename _SIZE_>
void RegroupeBord_32_64<_SIZE_>::rassemble_bords(Domaine_t& dom)
{
  int nbfr=dom.nb_front_Cl();
  for (int b=0; b<nbfr; b++)
    {

      Frontiere_t& org=dom.frontiere(b);
      const Nom& nom_bord=org.le_nom();

      LIST(Nom) listn;
      listn.add(nom_bord);
      regroupe_bord(dom,nom_bord,listn);
      // si on a detruit des bords on revient sur b
      if (nbfr!=dom.nb_front_Cl())
        {
          Cerr<<"Boundary "<<nom_bord<<" has been collected"<<finl;
          nbfr=dom.nb_front_Cl();
          b--;
        }
    }
}

template <typename _SIZE_>
void RegroupeBord_32_64<_SIZE_>::regroupe_bord(Domaine_t& dom, Nom nom,const LIST(Nom)& nlistbord)
{
  int nbfr=dom.nb_front_Cl();
  int_t nbfacestot=0;
  int prem_b=-1;

  for (int b=0; b<nbfr; b++)
    {
      Frontiere_t& org=dom.frontiere(b);

      if (nlistbord.contient(org.le_nom()))
        {
          if (prem_b==-1) prem_b=b;
          //Frontiere& org=domaine.frontiere(b);
          nbfacestot+=org.nb_faces();
        }
    }
  Frontiere_t& newb=dom.frontiere(prem_b);


  Faces_t& newfaces=newb.faces();
  //newfaces.typer(domaine.frontiere(0).faces().type_face());
  int nbsom=newfaces.nb_som_faces();
  int_t nbfacestot0=newfaces.nb_faces();
  newfaces.dimensionner(nbfacestot);
  IntTab_t& newsommet=newfaces.les_sommets();
  nbfacestot = nbfacestot0;
  for (int b=0; b<nbfr; b++)
    {
      if (b!=prem_b)
        {
          Frontiere_t& org=dom.frontiere(b);
          Nom nombord=org.le_nom();

          if (nlistbord.contient(nombord))
            {
              const IntTab_t& sommets=org.faces().les_sommets();
              int_t nbface=org.nb_faces();
              for (int_t fa=0; fa<nbface; fa++)
                for (int s=0; s<nbsom; s++)
                  newsommet(fa+nbfacestot,s)=sommets(fa,s);
              nbfacestot+=org.nb_faces();
            }
        }
    }
  dom.frontiere(prem_b).nommer("bord_a_conserver_coute_que_coute");
  Nom bordcons =dom.frontiere(prem_b).le_nom();

  for (int b=0; b<nbfr; b++)
    {
      // la recup des bords et des raccords est dans la boucle
      // pour pouvoir supprimer ...
      Bords_t& listbord=dom.faces_bord();
      Raccords_t& listrac=dom.faces_raccord();
      //const Nom& nombord=nlistbord[b];
      Frontiere_t& org=dom.frontiere(b);
      const Nom& nombord=org.le_nom();

      if (nlistbord.contient(nombord))
        if (nombord!=bordcons)
          {

            int num_b=dom.rang_frontiere(nombord);
            if (num_b<listbord.size())
              listbord.suppr(dom.bord(nombord));
            else
              listrac.suppr(dom.raccord(nombord));

            // on supprime un bord
            nbfr--;
            b--; // on veut verifie que le suivant n'est pas dans la liste

          }

    }
  newb.nommer(nom);
  dom.fixer_premieres_faces_frontiere();
}

template class RegroupeBord_32_64<int>;
#if INT_is_64_ == 2
template class RegroupeBord_32_64<trustIdType>;
#endif
