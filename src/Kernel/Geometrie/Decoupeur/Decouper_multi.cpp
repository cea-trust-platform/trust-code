/****************************************************************************
* Copyright (c) 2025, CEA
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
// XD partition_multi interprete decouper_multi 0 allows to partition multiple domains in contact with each other in parallel: necessary for resolution monolithique in implicit schemes and for all coupled problems using PolyMAC_P0P1NC. By default, this keyword is commented in the reference test cases.
// XD attr aco chaine(into=["{"]) aco 0 Opening curly bracket.
// XD attr domaine1 chaine(into=["domaine"]) domaine1 0 not set.
// XD attr dom ref_domaine dom 0 Name of the first domain to be cut.
// XD attr blocdecoupdom1 bloc_decouper blocdecoupdom1 0 Partition bloc for the first domain.
// XD attr domaine2 chaine(into=["domaine"]) domaine2 0 not set.
// XD attr dom2 ref_domaine dom2 0 Name of the second domain to be cut.
// XD attr blocdecoupdom2 bloc_decouper blocdecoupdom2 0 Partition bloc for the second domain.
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
      const Nom& nom = decoup.domaine().le_nom();
      if (decoupeurs.count(nom.getString())) //decoupeur deja lu
        Process::exit(Nom("Decouper_multi: domain ") + nom + "already read!");
      else decoupeurs[nom.getString()] = decoup; //sinon, on l'ajoute
    }
  else return -1;
  return 0;
}


Entree& Decouper_multi::interpreter(Entree& is)
{
  //lecture des domaines et des raccords
  Param param(que_suis_je());
  param.ajouter_non_std("domaine",(this),Param::REQUIRED);
  param.ajouter("tolerance", &tolerance);
  param.lire_avec_accolades_depuis(is);

  /* partition des domaines */
  std::vector<Decouper*> v_dec; //decoupeurs
  std::vector<const Domaine*> v_dom; //domaines
  std::vector<Static_Int_Lists> v_se; //connectivites
  std::vector<MCAuto<DataArrayDouble>> v_da; //DataArrayDouble de coordonnes
  std::vector<const DataArrayDouble*> v_pda; //et des pointeurs (ppur Aggregate)
  std::vector<int> off = { 0 }; //offset des sommets de chaque domaine dans le tableau aggrege
  for (auto &&n_d : decoupeurs)
    {
      Decouper& dec = n_d.second;
      const Domaine& dom = dec.domaine();
      const DoubleTab& coord = dom.coord_sommets();
      v_dec.push_back(&dec);
      v_dom.push_back(&dom);
      v_se.push_back(Static_Int_Lists());
      v_da.push_back(DataArrayDouble::New());
      v_da.back()->useExternalArrayWithRWAccess((double *) coord.addr(), coord.dimension(0), coord.dimension(1));
      v_pda.push_back(v_da.back());
      off.push_back(off.back() + coord.dimension(0));

      Partitionneur_base& partitionneur = dec.deriv_partitionneur_.valeur();
      partitionneur.declarer_bords_periodiques(dec.liste_bords_periodiques_);
      partitionneur.construire_partition(dec.elem_part_, dec.nb_parts_tot_);
      construire_connectivite_som_elem(dom.nb_som(), dom.les_elems(), v_se.back(), 1);
    }

#ifdef MEDCOUPLING_
  /* concatenation des coordonnees et recherche de sommets coincidents */
  Cerr << "Decouper_multi: searching for coinciding vertices ... ";
  MCAuto<DataArrayDouble> da(DataArrayDouble::Aggregate(v_pda)); //tous les sommets!
  DataArrayIdType* S_i = nullptr, *S = nullptr; //groupes de sommets coincidants : S([S_i(i), S_i(i + 1)[)
  da->findCommonTuples(tolerance, -1, S, S_i); // * heavy lifting *

  /* pour chaque groupe de sommets, recherche de tous les procs les touchant et ajout a chaque sommet des procs qui ne le possedent pas encore */
  std::vector<std::map<int, std::set<int>>> v_sp(v_dec.size()); //v_sp[d][s] = { processeurs supplementaires ayant besoin du sommet s du domaine d }
  std::vector<std::set<int>> procs; //processeurs possedant chaque sommet du groupe...
  std::set<int> u_procs; //et leur union
  std::vector<std::array<int, 2>> v_ds; //liste (domaine, num sommet local)
  int ns, l, d, count = 0;
  mcIdType j, s;
  for (mcIdType i = 0; i + 1 < S_i->getNumberOfTuples(); i++)
    if ((ns = static_cast<int>(S_i->getIJ(i + 1, 0) - S_i->getIJ(i, 0))) > 1) //pas besoin de traiter les sommets seuls
      {
        count++;
        procs.resize(ns);
        v_ds.resize(ns);
        int k = 0;
        for (j = S_i->getIJ(i, 0); j < S_i->getIJ(i + 1, 0); j++, k++)
          {
            // Retrouve le numero de dom dans lequel le sommet doublon est localise:
            for (s = S->getIJ(j, 0), d = 0; s >= off[d + 1]; )
              d++; //d : indice du domaine contenant s
            const IntVect& elem_part = v_dec[d]->elem_part_;
            mcIdType som_loc0 = s - off[d]; // son numero local
            assert(som_loc0 < std::numeric_limits<int>::max());
            int som_loc = static_cast<int>(som_loc0);
            v_ds[k] = {{ d, som_loc }}; //stockage du couple (d, s)
            procs[k].clear();
            for (l = 0; l < v_se[d].get_list_size(som_loc); l++)
              procs[k].insert(elem_part(v_se[d](som_loc, l))); //processeurs connectes
          }
        for (u_procs.clear(), k = 0; k < ns; k++)
          for (auto &&pr : procs[k])
            u_procs.insert(pr); //union
        for (k = 0; k < ns; k++)  /* on ajoute a chaque sommet les procs auxquels il n'est pas deja connecte */
          if (procs[k].size() < u_procs.size())
            {
              std::set<int>& dest = v_sp[v_ds[k][0]][v_ds[k][1]]; //ou on doit inserer
              std::set_difference(u_procs.begin(), u_procs.end(), procs[k].begin(), procs[k].end(), std::inserter(dest, dest.end()));
            }
      }
  S_i->decrRef(), S->decrRef();
  Cerr << count << " sommets trouves!" << finl;

#else
  Process::exit("Decouper_multi requires MEDCoupling!");
#endif

  /* ecriture des domaines avec les sommets raccord qu'on a trouves */
  for (d = 0; d < (int) v_dec.size(); d++)
    {
      Static_Int_Lists som_raccord;
      ArrOfInt sizes(v_dom[d]->nb_som()); //tailles des listes par sommet
      sizes = 0;
      for (auto && s_p : v_sp[d]) sizes[s_p.first] = (int)s_p.second.size();
      som_raccord.set_list_sizes(sizes);
      for (auto && s_p : v_sp[d])
        {
          int i = 0;
          for (auto &&p : s_p.second)
            {
              som_raccord.set_value(s_p.first, i, p);
              i++;
            }
        }
      v_dec[d]->ecrire(&som_raccord); // * heavier lifting *
    }
  return is;
}
