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

#include <Create_domain_from_sub_domain.h>
#include <Domaine.h>
#include <Param.h>
#include <DomaineCutter.h>
#include <Scatter.h>
#include <Sous_Domaine.h>
#include <SFichier.h>
#include <Synonyme_info.h>

Implemente_instanciable(Create_domain_from_sub_domain,"Create_domain_from_sub_domain|Create_domain_from_sub_domains",Interprete_geometrique_base);
// XD Create_domain_from_sub_domain interprete_geometrique_base Create_domain_from_sub_domain 1 This keyword fills the domain domaine_final with the subdomaine par_sous_zone from the domain domaine_init. It is very useful when meshing several mediums with Gmsh. Each medium will be defined as a subdomaine into Gmsh. A MED mesh file will be saved from Gmsh and read with Lire_Med keyword by the TRUST data file. And with this keyword, a domain will be created for each medium in the TRUST data file.
// 16/10/2017: desactivation du mot-cle par_sous_zones
// Create_domain_from_sub_domain interprete_geometrique_base Create_domain_from_sub_domain 1 This keyword fills the domain domaine_final with the subdomaine par_sous_zone or with several subdomaines par_sous_zones from the domain domaine_init. It is very useful when meshing several mediums with Gmsh. Each medium will be defined as a subdomaine into Gmsh. A MED mesh file will be saved from Gmsh and read with Lire_Med keyword by the TRUST data file. And with this keyword, a domain will be created for each medium in the TRUST data file.

Add_synonym(Create_domain_from_sub_domain, "Create_domain_from_sous_zone");

Sortie& Create_domain_from_sub_domain::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}
Entree& Create_domain_from_sub_domain::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

int Create_domain_from_sub_domain::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="domaines" || mot=="zones")
    {
      noms_sous_domaines.reset(), noms_doms.reset();
      Nom ssz, dom;
      is >> ssz;
      if (ssz != "{") Process::exit("Create_domaine_from_sous_domaine::domaines : { expected");
      for (is >> ssz; ssz != "}"; is >> ssz)
        is >> dom, noms_sous_domaines.add(ssz), noms_doms.add(dom);
      return 1;
    }
  return 0;
}

Entree& Create_domain_from_sub_domain::interpreter_(Entree& is)
{
  Nom nom_dom_org;
  noms_sous_domaines.dimensionner(1), noms_doms.dimensionner(1);
  Param param(que_suis_je());
  param.ajouter("domaine_final",&noms_doms[0]); // XD_ADD_P ref_domaine new domain in which faces are stored
  param.ajouter("par_sous_zone",&noms_sous_domaines[0]); // XD_ADD_P chaine a sub-area allowing to choose the elements
  param.ajouter("domaine_init",&nom_dom_org,Param::REQUIRED); // XD_ADD_P ref_domaine initial domain
  param.ajouter_non_std("domaines|zones", this);
  // 16/10/2017: desactivation du mot-cle par_sous_zones
  //param.ajouter("par_sous_zones",&vec_nom_ssz); // listchaine several sub-domaines allowing to choose the elements
  //param.ajouter_condition("is_read_par_sous_zone_or_is_read_par_sous_zones","Interpreter Create_domain_from_sub_domain: one of the keywords par_sous_zone or par_sous_zones must be specified.");
  param.lire_avec_accolades_depuis(is);


  if (nproc()>1)
    {
      Cerr<<"Options par_sous_zone and par_sous_zones of  "<<que_suis_je()<<" are for sequential." <<finl;
      exit();
    }

  const Domaine& domaine_org=ref_cast(Domaine, objet(nom_dom_org));

  DomaineCutter cutter;
  Noms vide;

  IntTab index(domaine_org.nb_elem()); //0 -> 1er domaine, ..., n_dom + 1 -> le reste
  int nb_dom = noms_doms.size();
  index = 0; //par defaut, on ne prend rien
  for (int i = 0; i < nb_dom; i++)
    {
      const Sous_Domaine& ssz=ref_cast(Sous_Domaine,objet(noms_sous_domaines[i]));
      for (int j = 0; j < ssz.nb_elem_tot(); j++)
        {
          if (index(ssz(j))) /* element deja pris -> erreur */
            Process::exit(Nom("Create_domain_from_sub_domain : collision detected between ") + noms_sous_domaines[i] + " and " + noms_sous_domaines[index(ssz(j)) - 1] + " !");
          index(ssz(j)) = i + 1;
        }
    }

  cutter.initialiser(domaine_org, index, noms_doms.size() + 1,1,vide,1);
  for (int i = 0; i < nb_dom; i++)
    {
      Domaine& dom = ref_cast(Domaine, objet(noms_doms[i]));
      cutter.construire_sous_domaine(i + 1, dom);
      Bords& bords=dom.faces_bord();

      if (cutter.bords_internes().size()>0)
        dom.faces_joint().vide();
      else
        {
          // on transforme les joints en bord
          Joints& joints= dom.faces_joint();
          for (int j=joints.size()-1; j>=0; j--)
            {
              Cout <<"The joint"<<j<<" becomes a boundary"<<finl;
              Bord b;
              ref_cast(Frontiere,b)=ref_cast(Frontiere,joints(j));
              b.nommer(Nom("Couture_")+Nom(j));
              bords.add(b);

              joints.suppr(joints(j));
            }
        }

      for (int b=bords.size()-1; b>=0; b--)
        if (bords(b).nb_faces()==0)
          {
            Cout << bords(b).le_nom()<<" deleted"<<finl;
            bords.suppr(bords(b));
          }
      Raccords& listrac=dom.faces_raccord();

      for (int b=bords.size()-1; b>=0; b--)
        {
          if (cutter.bords_internes().rang(bords(b).le_nom())>=0)
            {
              Cout << bords(b).le_nom() <<" becomes a connection "<<finl;
              Raccord racc_base;
              racc_base.typer("Raccord_local_homogene");
              Raccord_base& racc=racc_base.valeur();
              // on caste en Frontiere pour pouvoir faire la copie ...
              ref_cast(Frontiere,racc)=ref_cast(Frontiere,bords(b));
              listrac.add(racc_base);

              bords.suppr(bords(b));
            }
        }

      //et les sous-domaines?
      const LIST(REF(Sous_Domaine)) & liste_sous_domaines = domaine_org.ss_domaines();
      int nb_sous_domaines = liste_sous_domaines.size();
      const Sous_Domaine& ssz=ref_cast(Sous_Domaine,objet(noms_sous_domaines[i]));
      ArrOfInt rev_ssz(domaine_org.nb_elem());
      rev_ssz = -1;
      for (int j = 0; j < ssz.nb_elem_tot(); j++)
        rev_ssz[ssz[j]] = j;

      Nom jdd(" "), jdd_par(" ");
      int ecr_jdd = 0;
      for (int j = 0; j < nb_sous_domaines; j++)
        if (liste_sous_domaines[j]->le_nom() != noms_sous_domaines[i])
          {
            //liste des elements de la sous-sous-domaine
            ArrOfInt polys;
            polys.set_smart_resize(1);
            for (int k = 0, l; k < liste_sous_domaines[j]->nb_elem_tot(); k++)
              if ((l = rev_ssz[liste_sous_domaines[j].valeur()[k]]) >= 0)
                polys.append_array(l);

            if (!polys.size_array())
              continue; //sous-sous-domaine vide!

            Nom nom_ssz(noms_doms[i] + "_" + liste_sous_domaines[j]->le_nom()), file_ssz(nom_ssz + ".file");

            //contribution aux JDDs des sous-sous-domaines
            jdd += Nom("export Sous_Domaine ") + nom_ssz + "\n";
            jdd += Nom("Associer ") + nom_ssz + " " + noms_doms[i] + "\n";
            jdd += Nom("Lire ") + nom_ssz + " { fichier " + file_ssz + " }" + "\n";
            jdd_par += Nom("export Sous_Domaine ") + nom_ssz + "\n";
            jdd_par += Nom("Associer ") + nom_ssz + " " + noms_doms[i] + "\n";
            jdd_par += Nom("Lire ") + nom_ssz + " { fichier " + nom_ssz + ".ssz }" + "\n";
            ecr_jdd = 1;

            //fichier de la sous-sous-domaine
            SFichier f_ssz(file_ssz);
            f_ssz << polys;
          }

      if (ecr_jdd)
        {
          SFichier f_jdd(noms_doms[i] + "_ssz.geo");
          SFichier f_jdd_par(noms_doms[i] + "_ssz_par.geo");
          f_jdd << jdd;
          f_jdd_par << jdd_par;
        }

      dom.nommer(noms_doms[i]);
      Scatter::init_sequential_domain(dom);
    }

  return is;
}
