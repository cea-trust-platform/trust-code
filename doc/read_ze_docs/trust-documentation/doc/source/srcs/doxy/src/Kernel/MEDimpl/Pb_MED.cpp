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

#include <Pb_MED.h>
#include <Champ_Fonc_MED.h>
#include <LireMED.h>
#include <Postraitement.h>
#include <Interprete_bloc.h>
#include <Domaine_dis_cache.h>
#include <trust_med_utils.h>

Implemente_instanciable(Pb_MED,"Pb_MED",Probleme_base);
Implemente_instanciable(Pbc_MED,"Pbc_MED",Probleme_Couple);

Sortie& Pb_MED::printOn(Sortie& s ) const { return s; }
Sortie& Pbc_MED::printOn(Sortie& s ) const { return s; }

Entree& Pbc_MED::readOn(Entree& is )
{

  int nmax=10,npb=0;
  sch_.typer("Schema_Euler_explicite");
  //Schema_Euler_explicite& sch=ref_cast(Schema_Euler_explicite,sch_.valeur());
  Schema_Temps_base& sch=ref_cast(Schema_Temps_base,sch_.valeur());
  sch.changer_temps_courant(-1.);
  associer_sch_tps_base(sch);
  //Pb_MED* listpb;
  // On cree une liste de DerObjU car c'est eux que l'on peut ajouter a l'interprete...
  DerObjU* listob =new DerObjU[nmax];
  Motcle accouv("{"),accfer("}"),virg(","),motlu;
  is >> motlu;
  if (motlu!=accouv)
    {
      Cerr<<" One expects { to begin the read of Pbc_Med"<<finl;
      exit();
    }
  Interprete_bloc inter;
  while (motlu!=accfer)
    {
      assert((motlu==virg)||(motlu==accouv));
      if (npb>=nmax)
        {
          Cerr<<"The number of coupled problems is to large"<<finl;
          exit();
        }
      Nom nom_numero(npb);
      Nom nompb("pbMED_");
      nompb+=nom_numero;
      DerObjU& ob= listob[npb];
      ob.typer("Pb_MED");

      inter.ajouter(nompb,ob);
      Pb_MED& pb=ref_cast(Pb_MED,inter.objet_global(nompb));
      pb.nommer(nompb);
      associer_(pb);

      pb.associer_sch_tps_base(sch);
      is >> pb;
      npb++;
      is >> motlu;
      if ((motlu!=accfer)&&(motlu!=virg))
        {
          Cerr<<"One expects } or , between the problems "<<finl;
          Cerr<<"and not "<<motlu<<finl;
          exit();
        }
    }

  for (int i=0; i<nb_problemes(); i++)
    ref_cast(Probleme_base,probleme(i)).init_postraitements();

  // On oublie les axi, car les inconnues ont deja eu le traitement
  // apres l'init pour ecrire correctement les geom
  bidim_axi=0;
  axi=0;

  //const Probleme_base& pb1=les_problemes[0].valeur();
  Nom name_spe("pbMED_0");
  const ArrOfDouble& temps_sauv=ref_cast(Pb_MED,inter.objet_global(name_spe)).temps_sauv();
  int nbpasdetemps=temps_sauv.size_array();
  Cerr<<"nbpasdetemps "<<nbpasdetemps<<finl;
  for (int i=0; i<nbpasdetemps; i++)
    {
      schema_temps().mettre_a_jour();
      schema_temps().changer_temps_courant(temps_sauv[i]);
      postraiter();
    }

  if (nbpasdetemps==1)
    {
      //pour eviter bug meshtv
      //  postraiter();
    }
  for (int i=0; i<nb_problemes(); i++)
    ref_cast(Probleme_base,probleme(i)).finir();
  delete [] listob;
  return is;
}

/*! @brief Simple appel a: Probleme_base::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Pb_MED::readOn(Entree& is )
{
  dis_bidon.typer("VF_inst");
  la_discretisation=dis_bidon.valeur();
  is >> nom_fic;
  Nom nom_dom;
  is >> nom_dom;

  // on retire _0000 si il existe et on cree le bon fichier
  traite_nom_fichier_med(nom_fic);

  Domaine& dom=ref_cast(Domaine, Interprete::objet(nom_dom));
  associer_domaine(dom);

  // Read domain in MED file:
  LireMED lire(nom_fic, nom_dom);
  lire.associer_domaine(dom);
  lire.lire_geom(true);
  dom.reordonner();

  Nom typ = "NO_FACE_Domaine_VF_inst";
  le_domaine_dis = Domaine_dis_cache::Build_or_get(typ, dom);

  Cerr<<"Reading the name of existing fields in "<<nom_fic<<finl;
  read_med_field_names(nom_fic, nomschampmed, temps_sauv_);
  Cerr<<"temps_sauv "<<temps_sauv_<<finl;
  Probleme_base::readOn(is);
  return is;
}


/*! @brief Renvoie le nombre d'equations du probleme.
 *
 * Toujours egal a 1 pour un probleme de conduction standart.
 *
 * @return (int) le nombre d'equations du probleme
 */
int Pb_MED::nombre_d_equations() const
{
  return 0;
}

/*! @brief Renvoie l'equation de type MED si i = 0,
 *
 *     Provoque une erreur sinon car le probleme
 *     n'a qu'une seule equation.
 *     (version const)
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation de type MED
 */
const Equation_base& Pb_MED::equation(int i) const
{
  assert(0);
  exit();
  assert (i==0);
  //pour les compilos

  return Probleme_base::equation("bidon");
}

/*! @brief Renvoie l'equation de type MED si i = 0,
 *
 *     Provoque une erreur sinon car le probleme
 *     n'a qu'une seule equation.
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation de type MED
 */
Equation_base& Pb_MED::equation(int i)
{
  assert(0);
  exit();
  assert (i==0);
  //pour les compilos
  return Probleme_base::equation("bidon") ;
}



int Pb_MED::comprend_champ(const Motcle& mot) const
{
  //  Cerr<<" Pb_MED::comprend_champ"<<mot <<" " <<nomschampmed<<finl;
  int nbchampmed=nomschampmed.size();
  int ind=0;
  for (int ch=0; ch<nbchampmed; ch++)
    {
      if (mot==nomschampmed[ch]) ind= 1;
    }

  if (ind == 1)
    return ind;

  if (ind == 0)
    {
      Cerr << "Error when reading the data for post-processing" << finl;
      Cerr << "The field " << mot << " is not recognized by the problem" << finl;
      Cerr << "recognized words are "<<nomschampmed<<finl;
      exit();
    }
  return -1;
}

void Pb_MED::creer_champ(const Motcle& motlu)
{
  Cerr<<"Pb_MED::creer_champ "<< motlu<<finl;
  Noms liste_noms;
  get_noms_champs_postraitables(liste_noms);
  Champ_Fonc toto; // on ajoute toto et on le type apres pour eviter des copies qui ne marchent pas ...
  Champ_Fonc& le_ch_fonc= champs_fonc_post.add(toto);
  le_ch_fonc.typer("Champ_Fonc_MED");
  int nbchampmed=nomschampmed.size();

  Nom localisation;
  Motcle es;
  le_ch_fonc.valeur().nommer(motlu);

  int flag=0;
  Nom test("_");
  test+=domaine().le_nom();

  for (int ch=0; ch<nbchampmed; ch++)
    {
      Nom pp(nomschampmed[ch]);
      pp.prefix(test);
      //Cerr<<test<<" ICI "<<pp<<" "<<motlu<<finl;
      if (motlu==pp)
        {
          es=pp;
          es.prefix("_SOM");
          if (es!=pp)
            localisation="som";
          else
            {
              es.prefix("_ELEM");
              if (es!=pp)
                localisation="elem";
            }
          Cerr<<"One wishes to read the field "<<es<<" choice "<<localisation<<" readen word in the data set "<<motlu<<finl;
          le_ch_fonc.valeur().nommer(es);
          flag=1;
        }
    }
  Champ_Fonc_MED& chmed=ref_cast(Champ_Fonc_MED,le_ch_fonc.valeur());
  const Domaine& dom_med = domaine();
  chmed.creer(nom_fic,dom_med,localisation, temps_sauv_);

  if (flag)
    {
      chmed.le_champ().nommer(le_ch_fonc.valeur().le_nom());
      chmed.le_champ().corriger_unite_nom_compo();
      chmed.le_champ().nommer(motlu);
    }

  le_ch_fonc.valeur().nommer(motlu);
  chmed.le_champ().nommer(motlu);
  if (!le_ch_fonc.non_nul())
    {
      Cerr << "Error when reading data for post-processing" << finl;
      Cerr << "The creation of the field " << motlu << " has failed." << finl;
      exit();
    }
}

const Champ_base& Pb_MED::get_champ(const Motcle& un_nom) const
{
  REF(Champ_base) ref_champ;

  double temps_courant = schema_temps().temps_courant();
  Motcle nom_champ;
  int ok_post=1;

  for (const auto& itr : champs_fonc_post)
    {
      Champ_Fonc_MED& ch_med = ref_cast_non_const(Champ_Fonc_MED,itr.valeur());
      nom_champ = Motcle(itr.le_nom());
      if ((nom_champ==un_nom) && (ok_post==1))
        {
          ref_champ = itr;
          if (ch_med.temps()!=temps_courant)
            ch_med.mettre_a_jour(temps_courant);
          return ch_med.le_champ();
        }
      else
        {
          int nb_composantes = itr->nb_comp();
          for (int i=0; i<nb_composantes; i++)
            {
              nom_champ = Motcle(itr->nom_compo(i));
              if ((nom_champ==un_nom) && (ok_post==1))
                {
                  ref_champ = itr;
                  if (ch_med.temps()!=temps_courant)
                    ch_med.mettre_a_jour(temps_courant);
                  return ch_med.le_champ();
                }
            }
        }
    }

  Cerr<<"The indicated name "<<un_nom<<" do not correspond to a field understood by the problem"<<finl;
  Cerr<<"Check the field name to indicate in the post-processing set"<<finl;
  exit();

  //Pour compilation
  return ref_champ;
}

void Pb_MED::get_noms_champs_postraitables(Noms& noms,Option opt) const
{
  //La methode surcharge celle de Probleme_base
  if (opt==DESCRIPTION)
    Cerr<<"Pb_MED : : "<<nomschampmed<<finl;
  else
    noms.add(nomschampmed);
}
