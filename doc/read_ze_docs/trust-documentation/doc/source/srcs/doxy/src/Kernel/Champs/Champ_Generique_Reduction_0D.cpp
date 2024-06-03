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

#include <Champ_Generique_Reduction_0D.h>
#include <Discretisation_base.h>
#include <TRUSTTab_parts.h>
#include <communications.h>
#include <Probleme_base.h>
#include <Synonyme_info.h>
#include <Champ_Fonc.h>
#include <Domaine_VF.h>
#include <Param.h>

Implemente_instanciable(Champ_Generique_Reduction_0D,"Reduction_0D",Champ_Gen_de_Champs_Gen);
Add_synonym(Champ_Generique_Reduction_0D,"Champ_Post_Reduction_0D");
// XD reduction_0d champ_post_de_champs_post reduction_0d -1 To calculate the min, max, sum, average, weighted sum, weighted average, weighted sum by porosity, weighted average by porosity, euclidian norm, normalized euclidian norm, L1 norm, L2 norm of a field.

Sortie& Champ_Generique_Reduction_0D::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

//cf Champ_Gen_de_Champs_Gen::readOn
Entree& Champ_Generique_Reduction_0D::readOn(Entree& s )
{
  LIST(Motcle) mot_compris;
  mot_compris.add("min");
  mot_compris.add("max");
  mot_compris.add("euclidian_norm"); // new name for norme_L2
  mot_compris.add("normalized_euclidian_norm"); // new name for normalized_norm_L2
  mot_compris.add("moyenne");
  mot_compris.add("somme");
  mot_compris.add("moyenne_ponderee");
  mot_compris.add("somme_ponderee");
  mot_compris.add("moyenne_ponderee_porosite");
  mot_compris.add("somme_ponderee_porosite");
  mot_compris.add("valeur_a_gauche");
  mot_compris.add("L2_norm");  // L2 norm
  mot_compris.add("L1_norm");  // L1 norm
  mot_compris.add("average");  // new name for moyenne
  mot_compris.add("sum");  // new name for somme
  mot_compris.add("weighted_average"); // new name for moyenne_ponderee
  mot_compris.add("weighted_sum"); // new name weighted_sum for somme_ponderee
  mot_compris.add("weighted_average_porosity");  // new name for moyenne_ponderee_porosite
  mot_compris.add("weighted_sum_porosity");  // new name for somme_ponderee_porosite
  mot_compris.add("left_value");  // new name for valeur_a_gauche

  Champ_Gen_de_Champs_Gen::readOn(s);
  if (mot_compris.rang(methode_)<0)
    {
      Cerr << "Method " << methode_ << " is an unknown option for methode keyword in "<< que_suis_je() << "." << finl;
      Cerr << "Choose from " << mot_compris << finl;
      Process::exit();
    }
  return s ;
}

//  methode : indique le type de reduction qui va etre realisee
//              (min, max, moyenne, moyenne_ponderee_volume_elem, somme, somme_ponderee)
void Champ_Generique_Reduction_0D::set_param(Param& param)
{
  Champ_Gen_de_Champs_Gen::set_param(param);
  param.ajouter("methode",&methode_,Param::REQUIRED); // XD_ADD_P chaine(into=["min","max","moyenne","average","moyenne_ponderee","weighted_average","somme","sum","somme_ponderee","weighted_sum","somme_ponderee_porosite","weighted_sum_porosity","euclidian_norm","normalized_euclidian_norm","L1_norm","L2_norm","valeur_a_gauche","left_value"]) name of the reduction method: NL2 - min for the minimum value, NL2 - max for the maximum value, NL2 - average (or moyenne) for a mean, NL2 - weighted_average (or moyenne_ponderee) for a mean ponderated by integration volumes, e.g: cell volumes for temperature and pressure in VDF, volumes around faces for velocity and temperature in VEF, NL2 - sum (or somme) for the sum of all the values of the field, NL2 - weighted_sum (or somme_ponderee) for a weighted sum (integral), NL2 - weighted_average_porosity (or moyenne_ponderee_porosite) and weighted_sum_porosity (or somme_ponderee_porosite) for the mean and sum weighted by the volumes of the elements, only for ELEM localisation, NL2 - euclidian_norm for the euclidian norm, NL2 - normalized_euclidian_norm for the euclidian norm normalized, NL2 - L1_norm for norm L1, NL2 - L2_norm for norm L2
}

void Champ_Generique_Reduction_0D::completer(const Postraitement_base& post)
{
  Champ_Gen_de_Champs_Gen::completer(post);
  //Champ source_espace_stockage;
  Motcle directive;
  directive = get_directive_pour_discr();

  if (directive=="pression")
    {
      const Noms& nom_champ = get_property("nom");
      const Noms& nom_source = get_source(0).get_property("nom");
      Cerr<<"Problem with the "<<que_suis_je()<<" post processing field named "<<nom_champ[0]<<"."<<finl;
      Cerr<<"The source field named "<<nom_source[0]<<" of this last "<<que_suis_je()<<" field "<<finl;
      Cerr<<"must be previously interpolated at the elem or som location."<<finl;
      Cerr<<"Please use instead the syntax : "<<finl;
      Cerr<<"..."<<que_suis_je()<<" { source Champ_Post_Interpolation { ... } ... }"<<finl;
      Cerr<<"or contact TRUST support."<<finl;
      exit();
    }
  if (methode_=="valeur_a_gauche" || methode_=="left_value")
    {
      numero_proc_=-1;
    }

  Journal()<<"METHODE "<<methode_<<finl;
  if ((methode_=="valeur_a_gauche" || methode_=="left_value")&&(numero_proc_==-1))
    {
      const Domaine_dis_base& domaine_dis = get_source(0).get_ref_domaine_dis_base();

      const Domaine_VF& zvf = ref_cast(Domaine_VF,domaine_dis);
      // position la plus a gauche
      const DoubleTab& coords=zvf.domaine().les_sommets();
      const IntTab& conn=zvf.domaine().les_elems();
      double minp=mp_min_vect(coords);
      //Cerr<<" uu "<<minp<<finl;
      minp=Process::mp_min(minp);
      // Cerr<<" uu "<<minp<<finl;
      const Domaine& domaine =zvf.domaine();
      int elemin=-1;
      double dmin=DMAXFLOAT;
      int nb_elem=domaine.nb_elem();
      //ArrOfDouble xp(dimension);
      for (int ele=0; ele<nb_elem; ele++)
        {

          double d=0;
          for (int dir=0; dir<dimension; dir++)
            {
              double xp=0;
              int nb_som_elem=conn.dimension(1);
              for (int s=0; s<nb_som_elem; s++)
                {
                  int som=conn(ele,s);
                  xp+=coords(som,dir);
                }
              xp/=nb_som_elem;
              d+=(xp-minp)*(xp-minp);
            }
          if (d<dmin)
            {
              dmin=d;
              elemin=ele;
            }
        }
      double dming=mp_min(dmin);
      // on, envoye au maitre
      if (!je_suis_maitre())
        {
          envoyer(dmin,0,97);
          envoyer(elemin,0,97);
        }
      if (je_suis_maitre())
        {
          numero_proc_=me();
          numero_elem_=elemin;
          for (int p=1; p<nproc(); p++)
            {
              double dminloc;
              int eleminloc;
              recevoir(dminloc,p,97);
              recevoir(eleminloc,p,97);

              if (dminloc<dmin)
                {
                  dmin=dminloc;
                  numero_proc_=p;
                  numero_elem_=eleminloc;
                }
            }

          if (!est_egal(dmin,dming))
            {
              Cerr<<" iiiiiiiiiii "<<dmin<<" "<<dming<<finl;
              exit();
            }
        }
      envoyer_broadcast(numero_proc_,0);
      envoyer_broadcast(numero_elem_,0);

    }

}

const Champ_base& Champ_Generique_Reduction_0D::get_champ_without_evaluation(Champ& espace_stockage) const
{

  Champ source_espace_stockage;
  const Champ_base& source = get_source(0).get_champ_without_evaluation(source_espace_stockage);
  Nature_du_champ nature_source = source.nature_du_champ();
  int nb_comp = source.nb_comp();

  Champ_Fonc es_tmp;
  espace_stockage = creer_espace_stockage(nature_source,nb_comp,es_tmp);
  return espace_stockage.valeur();
}
/*! @brief Reduction_0D du champ source (au sens qu on le rend uniforme) en fonction de la methode (min, max, moyenne, moyenne_ponderee_volume_elem, somme, somme_ponderee)
 *
 *  Dans le cas ou le champ possede plusieurs composantes, elles sont traitees une par une
 *
 */
const Champ_base& Champ_Generique_Reduction_0D::get_champ(Champ& espace_stockage) const
{

  Champ source_espace_stockage;
  const Champ_base& source = get_source(0).get_champ(source_espace_stockage);
  const Domaine_dis_base& domaine_dis = get_source(0).get_ref_domaine_dis_base();
  Nature_du_champ nature_source = source.nature_du_champ();
  int nb_comp = source.nb_comp();

  // dimension() sur le tableau de valeurs des champs PolyMAC_P0P1NC renvoie -1 (plusieurs supports)
  // ToDo: reecrire completement cette methode (horrible, tres mal ecrite) en deportant les methodes min/max/sum/... pour chaque Champ !
  if (source.que_suis_je()=="Champ_Face_PolyMAC_P0P1NC" || source.que_suis_je()=="Champ_Face_PolyMAC_P0")
    Process::exit("PolyMAC_P0P1NC/PolyMAC_P0 face field not supported yet for Reduction_0D");

  Champ_Fonc es_tmp;
  espace_stockage = creer_espace_stockage(nature_source,nb_comp,es_tmp);

  int nb_dim = source.valeurs().nb_dim();
  // correction pour le 3D
  if (nb_dim==2)
    nb_dim= source.valeurs().dimension(1);

  ConstDoubleTab_parts valeurs_source_parts(source.valeurs()); // pour ignorer les variables auxiliaires
  const DoubleTab& valeurs_source = valeurs_source_parts[0];   // de PolyMAC_P0P1NC (sinon : min, moyenne FAUX)
  DoubleTab&  espace_valeurs = espace_stockage->valeurs();
  const Domaine_VF& zvf = ref_cast(Domaine_VF,domaine_dis);
  //DoubleVect val_extraites(nb_comp);
  double val_extraite=-100.;

  if (nb_comp==1)
    {
      extraire(val_extraite,valeurs_source);
      espace_valeurs = val_extraite;
    }
  else
    {
      for (int comp=0; comp<nb_comp; comp++)
        {
          int size_vect = valeurs_source.dimension(0);
          if (nb_dim!=nb_comp) //Cas des Champ_Face_VDF
            {
              size_vect=0;
              //const IntVect& ori = zvf.orientation();
              for (int i=0; i<valeurs_source.dimension(0); i++)
                //if (ori(i)==comp)
                if (zvf.orientation(i)==comp)
                  ++size_vect;
            }


          DoubleVect vect_source;
          //Pour l'option somme vect_source doit avoir une structure parallele
          //pour appliquer val_extraite = mp_prodscal(vect_source,un)
          //Sa dimension est alors fixee par rapport au nombre d items de la source
          //ex : zvf.nb_faces() si loc==FACE
          if (methode_=="somme" || methode_=="moyenne" || methode_=="sum" || methode_=="average")
            {
              Entity loc;
              loc = get_localisation();
              if (loc==ELEMENT)
                zvf.domaine().creer_tableau_elements(vect_source,Array_base::NOCOPY_NOINIT);
              else if (loc==NODE)
                zvf.domaine().creer_tableau_sommets(vect_source,Array_base::NOCOPY_NOINIT);
              else if (loc==FACE)
                zvf.creer_tableau_faces(vect_source,Array_base::NOCOPY_NOINIT);
              vect_source = 0.;
            }
          else
            //Dans le cas VDF avec nb_dim!=nb_comp
            //Sa dimension est fixee par rapport aux nb_faces dont l orientation est comp
            vect_source.resize(size_vect);

          // Remplissage
          if (nb_dim==nb_comp)
            {
              for (int i=0; i<size_vect; i++)
                vect_source(i) = valeurs_source(i,comp);
            }
          else
            {
              int k=0;
              //const IntVect& ori = zvf.orientation();
              for (int i=0; i<valeurs_source.dimension(0); i++)
                //if (ori(i)==comp)
                if (zvf.orientation(i)==comp)
                  {
                    if (methode_=="somme" || methode_=="moyenne" || methode_=="sum" || methode_=="average")
                      vect_source(i) = valeurs_source(i);
                    else
                      {
                        vect_source(k) = valeurs_source(i);
                        k++;
                      }
                  }
            }
          // Passage si necessaire de la composante pour les Champ_face
          extraire(val_extraite,vect_source,(nb_dim==nb_comp?-1:comp));

          if (nb_dim==nb_comp)
            {
              for (int i=0; i<size_vect; i++)
                espace_valeurs(i,comp) = val_extraite;
            }
          else
            {
              //const IntVect& ori = zvf.orientation();
              for (int i=0; i<valeurs_source.dimension(0); i++)
                //if (ori(i)==comp)
                if (zvf.orientation(i)==comp)
                  espace_valeurs(i) = val_extraite;
            }
        }
    }
  espace_valeurs.echange_espace_virtuel();

  return espace_stockage.valeur();
}

//Extrait la valeur du vecteur val_source dans val_extraite
void Champ_Generique_Reduction_0D::extraire(double& val_extraite,const DoubleVect& val_source, const int composante_VDF) const
{
  if (methode_=="min")
    {
      val_extraite = mp_min_vect(val_source);
    }
  else if (methode_=="max")
    {
      val_extraite = mp_max_vect(val_source);
    }
  /*
    else if (methode_=="moyenne") {
    // On n'utilise pas mp_moyenne_vect car probleme en VDF avec un champ vectoriel pour faire un val_source distribue
  // Pour calculer la moyenne, on utilise somme(val_source)/somme(1)
  val_extraite = mp_moyenne_vect(val_source);
  } */
  else if (methode_=="euclidian_norm")
    {
      val_extraite = mp_norme_vect(val_source);
    }
  else if (methode_=="normalized_euclidian_norm")
    {
      DoubleVect val_un(val_source);
      val_un=1.;
      val_extraite = mp_norme_vect(val_source)/mp_norme_vect(val_un);
    }
  else if (methode_ =="L1_norm" || methode_ =="L2_norm")
    {
      // Si on est :
      // - au ELEM -> on pondere par les volumes des elements,
      // - au FACE -> on pondere par les volumes entrelaces (on ne prend pas en compte les volumes etendues car on n'y a pas acces),
      // - au NODE -> on pondere par les volumes de controle nodal [Vol(som)= Somme_sur_elem_entourant_som(Vol_elem/nb_som_par_elem)].
      const Domaine_dis_base& domaine_dis = get_ref_domaine_dis_base();
      const Domaine_VF& zvf = ref_cast(Domaine_VF,domaine_dis);
      double sum=0;
      const DoubleVect& volumes = zvf.volumes();
      //int volumes_size_tot = mp_sum(volumes.size_array());
      if (volumes.size_array()<zvf.nb_elem())
        {
          Cerr << "The mesh volumes of the domain " << zvf.domaine().le_nom() << " are not available yet." << finl;
          Cerr << "It is not implemented yet." << finl;
          exit();
        }

      // au ELEM
      if (get_localisation()==ELEMENT)
        {
          int nb_elem = zvf.nb_elem();
          if (methode_ =="L1_norm")
            {
              for (int i=0; i<nb_elem; i++)
                {
                  sum+=std::fabs(val_source(i))*volumes(i);
                }
            }
          else if (methode_ =="L2_norm")
            {
              for (int i=0; i<nb_elem; i++)
                {
                  sum+=val_source(i)*val_source(i)*volumes(i);
                }
            }
          else
            {
              Cerr << "Error in Champ_Generique_Reduction_0D::extraire" << finl;
              exit();
            }
        }

      // au FACE
      if (get_localisation()==FACE)
        {
          // Calcul des volumes de controle a chaque face
          int nb_face = zvf.nb_faces();
          if (!volume_controle_.size())
            {
              volume_controle_.resize(nb_face);
              volume_controle_=0;
              int nb_faces_par_elem = zvf.elem_faces().dimension_tot(1);
              int nb_elem = zvf.nb_elem();
              for (int i=0; i<nb_elem; i++)
                for (int j=0; j<nb_faces_par_elem; j++)
                  {
                    int face=zvf.elem_faces(i,j);
                    volume_controle_(face)+=volumes(i)/nb_faces_par_elem;
                  }
            }
          if (composante_VDF>=0)
            {
              const IntVect& ori = zvf.orientation();
              int k=0;
              if (methode_ =="L1_norm")
                {
                  for (int i=0; i<nb_face; i++)
                    {
                      if (ori(i)==composante_VDF)
                        {
                          sum+=std::fabs(val_source(k))*volume_controle_(i);
                          k++;
                        }
                    }
                }
              else if (methode_ =="L2_norm")
                {
                  for (int i=0; i<nb_face; i++)
                    {
                      if (ori(i)==composante_VDF)
                        {
                          sum+=val_source(k)*val_source(k)*volume_controle_(i);
                          k++;
                        }
                    }
                }
              else
                {
                  Cerr << "Error in Champ_Generique_Reduction_0D::extraire" << finl;
                  exit();
                }
            }
          else
            {
              if (methode_ =="L1_norm")
                {
                  for (int i=0; i<nb_face; i++)
                    {
                      sum+=std::fabs(val_source(i))*volume_controle_(i);
                    }
                }
              else if (methode_ =="L2_norm")
                {
                  for (int i=0; i<nb_face; i++)
                    {
                      sum+=val_source(i)*val_source(i)*volume_controle_(i);
                    }
                }
              else
                {
                  Cerr << "Error in Champ_Generique_Reduction_0D::extraire" << finl;
                  exit();
                }
            }
        }

      // au NODE
      if (get_localisation()==NODE)
        {
          // Calcul des volumes de controle a chaque sommet
          int nb_som = zvf.nb_som();
          if (!volume_controle_.size())
            {
              volume_controle_.resize(nb_som);
              volume_controle_=0;
              int nb_som_par_elem = zvf.domaine().les_elems().dimension_tot(1);
              int nb_elem = zvf.nb_elem();
              for (int i=0; i<nb_elem; i++)
                for (int j=0; j<nb_som_par_elem; j++)
                  {
                    int som=zvf.domaine().sommet_elem(i,j);
                    volume_controle_(som)+=volumes(i)/nb_som_par_elem;
                  }
            }
          if (methode_ =="L1_norm")
            {
              for (int i=0; i<nb_som; i++)
                {
                  sum+=std::fabs(val_source(i))*volume_controle_(i);
                }
            }
          else if (methode_ =="L2_norm")
            {
              for (int i=0; i<nb_som; i++)
                {
                  sum+=val_source(i)*val_source(i)*volume_controle_(i);
                }
            }
          else
            {
              Cerr << "Error in Champ_Generique_Reduction_0D::extraire" << finl;
              exit();
            }
        }
      val_extraite = mp_sum(sum);
      if (methode_ =="L2_norm")
        {
          val_extraite = sqrt(val_extraite);
        }
    }

  else if (methode_=="weighted_average" || methode_=="weighted_sum" || methode_=="moyenne_ponderee" || methode_=="somme_ponderee")
    {
      // Si on est :
      // - au ELEM -> on pondere par les volumes des elements,
      // - au FACE -> on pondere par les volumes entrelaces (on ne prend pas en compte les volumes etendues car on n'y a pas acces),
      // - au NODE -> on pondere par les volumes de controle nodal [Vol(som)= Somme_sur_elem_entourant_som(Vol_elem/nb_som_par_elem)].

      const Domaine_dis_base& domaine_dis = get_ref_domaine_dis_base();
      const Domaine_VF& zvf = ref_cast(Domaine_VF,domaine_dis);
      double sum=0;
      double volume=0;
      const DoubleVect& volumes = zvf.volumes();
      //int volumes_size_tot = mp_sum(volumes.size_array());
      if (volumes.size_array()<zvf.nb_elem())
        {
          Cerr << "The mesh volumes of the domain " << zvf.domaine().le_nom() << " are not available yet." << finl;
          Cerr << "It is not implemented yet." << finl;
          exit();
        }

      // au ELEM
      if (get_localisation()==ELEMENT)
        {
          int nb_elem = zvf.nb_elem();
          for (int i=0; i<nb_elem; i++)
            {
              sum+=val_source(i)*volumes(i);
              volume+=volumes(i);
            }
        }

      // au FACE
      if (get_localisation()==FACE)
        {
          // Calcul des volumes de controle a chaque face
          int nb_face = zvf.nb_faces();
          if (!volume_controle_.size())
            {
              volume_controle_.resize(nb_face);
              volume_controle_=0;
              int nb_faces_par_elem = zvf.elem_faces().dimension_tot(1);
              int nb_elem = zvf.nb_elem();
              for (int i=0; i<nb_elem; i++)
                for (int j=0; j<nb_faces_par_elem; j++)
                  {
                    int face=zvf.elem_faces(i,j);
                    volume_controle_(face)+=volumes(i)/nb_faces_par_elem;
                  }
            }
          if (composante_VDF>=0)
            {
              //const IntVect& ori = zvf.orientation();
              int k=0;
              for (int i=0; i<nb_face; i++)
                //if (ori(i)==composante_VDF)
                if (zvf.orientation(i)==composante_VDF)
                  {
                    sum+=val_source(k)*volume_controle_(i);
                    volume+=volume_controle_(i);
                    k++;
                  }
            }
          else
            for (int i=0; i<nb_face; i++)
              {
                sum+=val_source(i)*volume_controle_(i);
                volume+=volume_controle_(i);
              }
        }

      // au NODE
      if (get_localisation()==NODE)
        {
          // Calcul des volumes de controle a chaque sommet
          int nb_som = zvf.nb_som();
          if (!volume_controle_.size())
            {
              volume_controle_.resize(nb_som);
              volume_controle_=0;
              int nb_som_par_elem = zvf.domaine().les_elems().dimension_tot(1);
              int nb_elem = zvf.nb_elem();
              for (int i=0; i<nb_elem; i++)
                for (int j=0; j<nb_som_par_elem; j++)
                  {
                    int som=zvf.domaine().sommet_elem(i,j);
                    volume_controle_(som)+=volumes(i)/nb_som_par_elem;
                  }
            }
          for (int i=0; i<nb_som; i++)
            {
              sum+=val_source(i)*volume_controle_(i);
              volume+=volume_controle_(i);
            }
        }
      val_extraite = mp_sum(sum);
      if (methode_=="moyenne_ponderee" || methode_=="weighted_average")
        val_extraite /= mp_sum(volume);
    }
  else if (methode_=="moyenne_ponderee_porosite" || methode_=="somme_ponderee_porosite" || methode_=="weighted_average_porosity" || methode_=="weighted_sum_porosity")
    {
      // - au ELEM -> on pondere par les volumes des elements *porosite_volumique
      // si on n'est pas aux ELEM erreur

      const Domaine_dis_base& domaine_dis = get_ref_domaine_dis_base();
      const Domaine_VF& zvf = ref_cast(Domaine_VF,domaine_dis);
      double sum=0;
      double volume=0;
      const DoubleVect& volumes = zvf.volumes();
      // int volumes_size_tot = mp_sum(volumes.size_array());
      if (volumes.size_array()<zvf.nb_elem())
        {
          Cerr << "The mesh volumes of the domain " << zvf.domaine().le_nom() << " are not available yet." << finl;
          Cerr << "It is not implemented yet." << finl;
          exit();
        }
      if (get_nb_sources()!=2)
        {
          Cerr<<" you must define the porosity "<<finl;
          exit();
        }

      // au ELEM
      if (get_localisation()==ELEMENT)
        {
          Champ source_espace_stockage2;
          const Champ_base& source2 = get_source(1).get_champ(source_espace_stockage2);
          Motcle nom_source_1(get_source(1).get_nom_post());
          if (!(nom_source_1.debute_par("porosite_volumique")||(nom_source_1.debute_par("beta"))))
            {
              Cerr<<" Error with option pondere_porosite !!! "<< get_source(1).get_nom_post()<<finl;
              exit();

            }
          const DoubleVect& poro= source2.valeurs();
          assert(volumes.size_array()==poro.size_array());
          int nb_elem = zvf.nb_elem();
          for (int i=0; i<nb_elem; i++)
            {
              sum+=val_source(i)*volumes(i)*poro(i);
              volume+=volumes(i)*poro(i);
            }
        }
      else
        {
          Cerr<<que_suis_je()<<" not implemented for this localisation "<<finl;
          exit();

        }

      val_extraite = mp_sum(sum);
      if (methode_=="moyenne_ponderee_porosite" || methode_=="weighted_average_porosity")
        val_extraite /= mp_sum(volume);
    }

  else if (methode_=="somme" || methode_=="moyenne" || methode_=="sum" || methode_=="average")
    {

      const Domaine_dis_base& domaine_dis = get_source(0).get_ref_domaine_dis_base();
      const Domaine_VF& zvf = ref_cast(Domaine_VF,domaine_dis);
      DoubleVect un;
      Entity loc;
      loc = get_localisation();
      if (loc==ELEMENT)
        zvf.domaine().creer_tableau_elements(un,Array_base::NOCOPY_NOINIT);
      else if (loc==NODE)
        zvf.domaine().creer_tableau_sommets(un,Array_base::NOCOPY_NOINIT);
      else if (loc==FACE)
        zvf.creer_tableau_faces(un,Array_base::NOCOPY_NOINIT);
      un = 1.;
      if (methode_=="somme" || methode_=="sum")
        {
          val_extraite = mp_prodscal(val_source,un);
          // Pourquoi ne pas utiliser val_extraite = mp_somme_vect(val_source); ?
        }
      else if (methode_=="moyenne" || methode_=="average")
        {
          if (loc==FACE && composante_VDF>=0)
            {
              // Dans le cas vectoriel en VDF, il ne faut compter que les
              // faces de la composante etudiee :
              int nb_face = zvf.nb_faces();
              const IntVect& ori = zvf.orientation();
              for (int i=0; i<nb_face; i++)
                if (ori(i)!=composante_VDF)
                  un(i) = 0;
            }
          val_extraite = mp_somme_vect(val_source) / mp_somme_vect(un);
        }
      else
        {
          Cerr << "Error in Champ_Generique_Reduction_0D::extraire" << finl;
          exit();
        }
    }
  else if (methode_=="valeur_a_gauche" || methode_=="left_value")
    {

      assert(numero_proc_!=-1);
      if (me()==numero_proc_)
        {
          val_extraite=val_source(numero_elem_);
          envoyer(val_extraite,me(),-1,98);
        }
      else
        recevoir(val_extraite,numero_proc_,me(),98 );
    }
  else
    {
      Cerr << "Method " << methode_ << " is an unknown option for methode keyword." << finl;
      exit();
    }
}

const Noms Champ_Generique_Reduction_0D::get_property(const Motcle& query) const
{

  Motcles motcles(1);
  motcles[0] = "composantes";
  int rang = motcles.search(query);
  switch(rang)
    {

    case 0:
      {
        Noms source_compos = get_source(0).get_property("composantes");
        int nb_comp = source_compos.size();
        Noms compo(nb_comp);

        for (int i=0; i<nb_comp; i++)
          {
            Nom nume(i);
            compo[i] = nom_post_+nume;
          }

        return compo;
      }

    }
  return Champ_Gen_de_Champs_Gen::get_property(query);
}

//Nomme le champ en tant que source par defaut
//"Reduction_0D_"+nom_champ_source
void Champ_Generique_Reduction_0D::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source, nom_champ_source;
      const Noms nom = get_source(0).get_property("nom");
      nom_champ_source = nom[0];
      nom_post_source =  "Reduction_0D_";
      nom_post_source +=  nom_champ_source;
      nommer(nom_post_source);
    }
}


