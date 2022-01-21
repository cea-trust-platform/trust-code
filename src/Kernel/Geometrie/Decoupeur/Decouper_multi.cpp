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
// File:        Decouper_multi.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////
#include <Decouper_multi.h>
#include <Domaine.h>
#include <Param.h>
#include <Connectivite_som_elem.h>
#include <Static_Int_Lists.h>

#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDCouplingMemArray.hxx>

using namespace MEDCoupling;
#endif

Implemente_instanciable(Decouper_multi,"Decouper_multi|Partition_multi",Interprete);
// XD partition_multi interprete decouper_multi 0 allows to partition multiple domains while accounting for connections via Raccords (allows for easier implementation of thermique_monolithique in parallel). By default, this keyword is commented in the reference test cases.
// XD attr aco chaine(into=["{"]) aco 0 Opening curly bracket.
// XD attr domaine1 chaine(into=["domaine"]) domaine1 0 not set.
// XD attr dom ref_domaine dom 0 Name of the first domain to be cut.
// XD attr blocdecoupdom1 bloc_decouper blocdecoupdom1 0 Partition bloc for the first domain.
// XD attr domaine2 chaine(into=["domaine"]) domaine2 0 not set.
// XD attr dom2 ref_domaine dom2 0 Name of the second domain to be cut.
// XD attr blocdecoupdom2 bloc_decouper blocdecoupdom2 0 Partition bloc for the second domain.
// XD attr raccords chaine(into=["raccords"]) raccords 0 not set.
// XD attr blocracc bloc_lecture blocracc 0 Indicates the joints between both domains. The syntax is { dom1 bord1 dom2 bord2 ... }
// XD attr acof chaine(into=["}"]) acof 0 Closing curly bracket.


Sortie& Decouper_multi::printOn(Sortie& os) const
{
  exit();
  return os;
}

Entree& Decouper_multi::readOn(Entree& is)
{
  exit();
  return is;
}

int Decouper_multi::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="domaine") //parametres de decoupage d'un domaine
    {
      Decouper decoup;
      decoup.lire(is);
      if (decoupeurs.count(decoup.nom_domaine.getString())) //decoupeur deja lu
        Process::exit(Nom("Decouper_multi: domain ") + decoup.nom_domaine + "already read!");
      else decoupeurs[decoup.nom_domaine.getString()] = decoup; //sinon, on l'ajoute
    }
  else if (mot == "raccords") //liste de raccords
    {
      Nom dom[2], racc[2];
      is >> dom[0];
      if (dom[0] != "{") Process::exit(Nom("Decouper_multi : { expected after 'raccords' instead of ") + dom[0]);
      for (is >> dom[0]; dom[0] != "}"; is >> dom[0])
        {
          is >> racc[0] >> dom[1] >> racc[1];
          for (int i = 0; i < 2; i++) if (!objet_existant(dom[i]) || !sub_type(Domaine, objet(dom[i])))
              Process::exit(Nom("Decouper_multi : domain ") + dom[i] + "does not exist!");
          const Domaine *pdom[2] = { &ref_cast(Domaine, objet(dom[0])), &ref_cast(Domaine, objet(dom[1])) };
          for (int i = 0; i < 2; i++) if (!pdom[i]->zone(0).faces_raccord().contient(racc[i]))
              Process::exit(Nom("Decouper_multi : domain ") + dom[i] + "does not contain a Raccord named " + racc[i] + " . Have you used modif_bord_to_raccord?");
          //on peut ajouter la paire de raccords a la liste
          racc_pairs.push_back({{ &pdom[0]->zone(0).raccord(racc[0]).valeur(), &pdom[1]->zone(0).raccord(racc[1]).valeur() }});
        }
    }
  else return -1;
  return 0;
}


Entree& Decouper_multi::interpreter(Entree& is)
{
  int i, j, k, d, D = dimension;
  //lecture des domaines et des raccords
  Param param(que_suis_je());
  param.ajouter_non_std("domaine",(this),Param::REQUIRED);
  param.ajouter_non_std("raccords", (this));
  param.ajouter("tolerance", &tolerance);
  param.lire_avec_accolades_depuis(is);

  /* partition des domaines */
  std::map<std::string, IntVect> m_elem_part; //m_elem_part[nom_domaine] = sa partition
  for (auto &&n_d : decoupeurs)
    {
      Partitionneur_base& partitionneur = n_d.second.deriv_partitionneur.valeur();
      partitionneur.declarer_bords_periodiques(n_d.second.liste_bords_periodiques);
      partitionneur.construire_partition(m_elem_part[n_d.first], n_d.second.nb_parts_tot);
    }

  /* on aura besoin des connectivites som-elem de tous les domaines */
  std::map<std::string, Static_Int_Lists> m_som_elem;
  Cerr << "Decouper_multi: connectivite som-elem... ";
  for (auto &&n_d : decoupeurs)
    {
      const Domaine& dom = n_d.second.ref_domaine.valeur();
      int nb_som = dom.nb_som();
      const IntTab& elems = dom.zone(0).les_elems();
      construire_connectivite_som_elem(nb_som, elems, m_som_elem[n_d.first], 1);
    }
  Cerr << "OK" << finl;

  /* pour chaque paire de raccords : correspondance entre sommets par coordonnees, puis contribution aux structures
    "som_raccord" des domaines de chaque cote */
  std::map<std::string, std::map<int, std::set<int>>> m_som_raccord; //s_som_raccord[nom_dom][som] = { process raccordes a som }
  for (auto && racc_pair : racc_pairs)
    {
#ifdef MEDCOUPLING_
      const Domaine *dom[2];                //domaines de chaque cote
      std::vector<int> racc_som[2];         //liste de sommets de chaque cote et leurs coordonnees
      MCAuto<DataArrayDouble> racc_coord[2];//leurs coordonnees
      for (i = 0; i < 2; i++)
        {
          /* sommets */
          const IntTab& f_s = racc_pair[i]->les_sommets_des_faces();
          std::set<int> s_racc_som; //pour dedupliquer
          for (j = 0; j < f_s.size(); j++) if (f_s.addr()[j] >= 0) s_racc_som.insert(f_s.addr()[j]);
          racc_som[i].assign(s_racc_som.begin(), s_racc_som.end());

          /* coordonnees */
          dom[i] = &racc_pair[i]->zone().domaine();
          const DoubleTab& coord = dom[i]->coord_sommets();
          for (racc_coord[i] = DataArrayDouble::New(), racc_coord[i]->alloc(racc_som[i].size(), D), j = 0; j < (int) racc_som[i].size(); j++)
            for (d = 0; d < D; d++) racc_coord[i]->setIJ(j, d, coord(racc_som[i][j], d));
        }

      if (racc_som[0].size() != racc_som[1].size()) //si nombre de sommets differents, probleme!
        Process::exit(Nom("Decouper_multi : different number of vertices found for ") + dom[0]->le_nom() + "/" + racc_pair[0]->le_nom()
                      + " and " + dom[1]->le_nom() + "/" + racc_pair[1]->le_nom() + " ! ( " + Nom((int) racc_som[0].size()) + " vs " + Nom((int) racc_som[1].size()) + " )");

      //correspondance racc[0] -> racc[1]
      MCAuto<DataArrayInt> corr(DataArrayInt::New());
      corr = racc_coord[1]->findClosestTupleId(racc_coord[0]);

      //pour chaque sommet : verification de la coincidence, contribution a som_raccord
      Static_Int_Lists *som_elem[2];                //connectivites som-elem de chaque cote
      std::map<int, std::set<int>> *som_raccord[2]; //som_raccord de chaque cote
      IntVect *elem_part[2];                       //partitions de chaque cote
      for (i = 0; i < 2; i++) som_elem[i] = &m_som_elem[dom[i]->le_nom().getString()];
      for (i = 0; i < 2; i++) som_raccord[i] = &m_som_raccord[dom[i]->le_nom().getString()];
      for (i = 0; i < 2; i++) elem_part[i] = &m_elem_part[dom[i]->le_nom().getString()];

      double dmax = 0;
      for (i = 0; i < (int) racc_som[0].size(); i++)
        {
          for (d = 0; d < D; d++) dmax = std::max(dmax, std::fabs(racc_coord[0]->getIJ(i, d) - racc_coord[1]->getIJ(corr->getIJ(i, 0), d)));
          int s[2] = { racc_som[0][i], racc_som[1][corr->getIJ(i, 0)] }; //sommets du cote 0/1
          //boucle sur les cotes : pour j = 0, on regarde du cote 1 pour ajouter des procs demandant le sommet du cote 0
          for (j = 0; j < 2; j++) for (k = 0; k < som_elem[!j]->get_list_size(s[!j]); k++)
              (*som_raccord[j])[s[j]].insert((*elem_part[!j])[(*som_elem[!j])(s[!j], k)]); //on ajoute le proc possedant cet element cote distant a la liste de s_l
        }
      if (dmax > tolerance) Process::exit(Nom("Decouper_multi : non-coincident vertices found between ") + dom[0]->le_nom() + "/" + racc_pair[0]->le_nom()
                                            + " and " + dom[1]->le_nom() + "/" + racc_pair[1]->le_nom() + "! dmax = " + Nom(dmax) + " tol = " + Nom(tolerance));
#else
      Process::exit("Decouper_multi requires MEDCoupling!");
#endif
    }

  /* ecriture des domaines avec les sommets raccord qu'on a trouves */
  for (auto && n_d : decoupeurs)
    {
      Static_Int_Lists som_raccord;
      ArrOfInt sizes(n_d.second.ref_domaine->nb_som()); //tailles des listes par sommet
      sizes = 0;
      for (auto && s_p : m_som_raccord[n_d.first]) sizes[s_p.first] = s_p.second.size();
      som_raccord.set_list_sizes(sizes);
      for (auto && s_p : m_som_raccord[n_d.first])
        {
          i = 0;
          for (auto &&p : s_p.second) som_raccord.set_value(s_p.first, i, p), i++;
        }
      n_d.second.ecrire(m_elem_part[n_d.first], &som_raccord); // * heacy lifting *
    }
  return is;
}
