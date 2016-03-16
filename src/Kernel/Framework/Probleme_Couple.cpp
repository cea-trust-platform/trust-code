/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Probleme_Couple.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/62
//
//////////////////////////////////////////////////////////////////////////////

#include <Probleme_Couple.h>
#include <Zone_VF.h>
#include <DoubleTabs.h>
#include <LecFicDistribue.h>
#include <Debog.h>
#include <Postraitement.h>
#include <Milieu_base.h>
#include <Equation_base.h>
#include <Schema_Euler_Implicite.h>
#include <List_List_Nom.h>
#include <sys/stat.h>
#include <communications.h>
#include <EcrFicCollecte.h>

Implemente_instanciable(Probleme_Couple,"Probleme_Couple",Couplage_U);

///////////////////////////////////////////////
//                                           //
// Implementation de l'interface de Problem  //
//                                           //
///////////////////////////////////////////////

bool Probleme_Couple::initTimeStep(double dt)
{
  /*
  double& residu_max = schema_temps().residu();
  double test=residu_max;
  for (int i=1; i<nb_problemes(); i++)
    residu_max = max(residu_max,sch_clones[i]->residu());
  if (test!=residu_max) abort();
  */
  bool ok =  Couplage_U::initTimeStep(dt);
  return ok;
}

double Probleme_Couple::computeTimeStep(bool& stop) const
{
  double dt_=DMAXFLOAT;

  // On prend le dt min.
  // On stoppe si l'un des problemes veut stopper.
  for (int i=0; i<nb_problemes(); i++)
    {
      double dt1=probleme(i).computeTimeStep(stop);
      if (stop)
        return 0;
      if (dt1<dt_)
        dt_=dt1;
    }
  return dt_;
}



bool Probleme_Couple::solveTimeStep()
{
  // WEC : A changer !!!!
  if (sub_type(Schema_Euler_Implicite,schema_temps()))
    {
      Schema_Euler_Implicite& sch_eul_imp=ref_cast(Schema_Euler_Implicite,schema_temps());
      bool ok= sch_eul_imp.faire_un_pas_de_temps_pb_couple(*this);
      // on propage un certain nombre de choses vers les clones
      for (int i=1; i<nb_problemes(); i++)
        {
          sch_clones[i].facteur_securite_pas()=schema_temps().facteur_securite_pas();
          sch_clones[i]->set_stationnaire_atteint()=schema_temps().stationnaire_atteint();
        }
      return ok;
    }

  bool ok=Couplage_U::solveTimeStep();
  double& residu_max = schema_temps().residu();
  for (int i=1; i<nb_problemes(); i++)
    residu_max = max(residu_max,sch_clones[i]->residu());
  return ok;
}


bool Probleme_Couple::iterateTimeStep(bool& converged)
{
  bool ok=true;
  converged=true;

  int debut_gr=0;
  int fin_gr=0;
  int gr=0;
  while(fin_gr<nb_problemes())
    {

      if (gr<groupes.size_array())
        fin_gr += groupes[gr++];
      else
        fin_gr=nb_problemes();

      for(int i=debut_gr; i<fin_gr; i++)
        ok = ok && probleme(i).updateGivenFields();

      for(int i=debut_gr; i<fin_gr; i++)
        {
          bool cv;
          ok = ok && probleme(i).iterateTimeStep(cv);
          converged = converged && cv;
        }

      debut_gr=fin_gr;
    }

  return ok;
}



////////////////////////////////////////////////////////
//                                                    //
// Fin de l'implementation de l'interface de Problem  //
//                                                    //
////////////////////////////////////////////////////////



Entree& Probleme_Couple::readOn(Entree& is)
{

  Cerr << "Reading of Probleme_Couple " << le_nom() << finl;

  Motcle motlu;
  is >> motlu;
  if (motlu != Motcle("{"))
    {
      Cerr << "We expected { to start to read the Probleme_Couple" << finl;
      exit();
    }

  is >> motlu;
  while (motlu!=Motcle("}"))   // fin du readOn
    {

      if (motlu != Motcle("groupes"))
        {
          Cerr << "The keyword " << motlu << " is not understood" << finl;
          exit();
        }

      if (nb_problemes())
        {
          Cerr << "We can associate problems to Probleme_Couple" << endl;
          Cerr << "* either by \"associer prob_couple pb\" (in which case they are all in the same group)" << endl;
          Cerr << "* either by the keyword \"groupes\" while reading the object Probleme_Couple" << endl;
          Cerr << "but not both!" << endl;
        }
      assert(nb_problemes()==0);

      LIST(LIST(Nom)) les_noms;
      is >> les_noms;

      groupes.resize_array(les_noms.size());
      for (int i=0; i<les_noms.size(); i++)
        {
          groupes[i]=les_noms[i].size();
          for (int j=0; j<les_noms[i].size(); j++)
            {
              Nom nom_pb=les_noms[i][j];
              Objet_U& ob=Interprete::objet(nom_pb);
              Probleme_base& pb=ref_cast(Probleme_base,ob);
              ajouter(pb);
            }
        }

      is >> motlu;
    }

  return is;
}


// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    Imprime les problemes couples sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie sur lequel imprimer
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition:
Sortie& Probleme_Couple::printOn(Sortie& os) const
{
  for(int i=1; i< nb_problemes(); i++)
    os << probleme(i) << finl;

  return os;
}


bool Probleme_Couple::updateGivenFields()
{

  // Pas de champs provenant de l'exterieur du couplage.
  // Les echanges internes se font pendant iterateTimeStep.

  return true;

}


// Description:
//    Ajoute un probleme a la liste des problemes couples.
//    Met en place la reference du probleme vers this.
//    Verifie que l'ordre conduction, thHyd est respecte.
// Precondition:
// Parametre: Probleme_base& pb
//    Signification: le probleme a ajouter au couplage
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Probleme_Couple::ajouter(Probleme_base& pb)
{
  addProblem(pb);
  pb.set_coupled(1);

  int nb_pb=nb_problemes();
  Nom nom_pb=probleme(nb_pb-1).que_suis_je();
  if (nb_pb!=1
      && probleme(0).que_suis_je()=="Pb_Conduction"
      && nom_pb.prefix("Turbulent") != probleme(nb_pb-1).que_suis_je())
    {
      Cerr << finl;
      Cerr << "Warning !" << finl;
      Cerr << "You define a conduction problem named "<<probleme(0).le_nom()<<" before your turbulent thermalhydraulic problem named "<<probleme(nb_pb-1).le_nom()<<"." << finl;
      Cerr << "Please, define first your turbulent thermalhydraulic problem in your data file like:" << finl;
      Cerr << finl;
      Cerr << "Probleme_Couple "<<le_nom()<< finl;
      for (int i=nb_pb-1; i>=0; i--)
        Cerr << "Associer "<<le_nom()<<" "<<probleme(i).le_nom()<<finl;
      Cerr << finl;
      exit();
    }
}


// Description:
//    Surcharge Objet_U::associer_(Objet_U&)
//    Associe un objet au probleme couple, en verifiant le type
//    dynamiquement. L'objet peut-etre:
//      - un schema en temps (Schema_Temps_base), et on l'associe aux problemes
//      - un probleme (Probleme_base), on l'ajoute a la liste
// Precondition:
// Parametre: Objet_U& ob
//    Signification: l'objet a associer
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: 1 si l'association a reussie 0 sinon
//    Contraintes:
// Exception: l'objet n'est pas d'un type attendu
// Effets de bord:
// Postcondition:
int Probleme_Couple::associer_(Objet_U& ob)
{
  if( sub_type(Schema_Temps_base, ob))
    {
      associer_sch_tps_base(ref_cast(Schema_Temps_base, ob));
      return 1;
    }
  else if( sub_type(Probleme_base, ob))
    {
      Probleme_base& pb = ref_cast(Probleme_base, ob);
      ajouter(pb);
      return 1;
    }
  else
    return 0;
}


// Description:
//    Associe une copie du schema en temps a chaque probleme
//    du Probleme couple.
// Precondition:
// Parametre: Schema_Temps_base& sch
//    Signification: le schema en temps a associer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord: les problemes du probleme couple ont un schema en temps
//                 associe
// Postcondition:
void Probleme_Couple::associer_sch_tps_base(Schema_Temps_base& sch)
{
  sch_clones.dimensionner(nb_problemes());
  for (int i=0; i<nb_problemes(); i++)
    {
      sch_clones[i]=sch; // Clonage du schema
      Probleme_base& pb=ref_cast(Probleme_base,probleme(i));
      pb.associer_sch_tps_base(sch_clones[i]); // association
      //On attribue la valeur 1 a schema_impr_ pour le schema du probleme 0
      //et 0 pour les autres. Un seul schema doit imprimer.
      if (i!=0) pb.schema_temps().schema_impr()=0;
    }
}
// Description:
//    Renvoie le schema en temps associe aux problemes couples.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Schema_Temps_base&
//    Signification: le schema en temps associe
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Schema_Temps_base& Probleme_Couple::schema_temps() const
{
  if (nb_problemes()==0)
    {
      Cerr << "You forgot to associate problems to the coupled problem named " << le_nom() << finl;
      Process::exit();
    }
  const Probleme_base& pb=ref_cast(Probleme_base,probleme(0));
  return pb.schema_temps();
}


// Description:
//    Renvoie le schema en temps associe aux problemes couples.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Schema_Temps_base&
//    Signification: le schema en temps associe
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Schema_Temps_base& Probleme_Couple::schema_temps()
{
  if (nb_problemes()==0)
    {
      Cerr << "You forgot to associate problems to the coupled problem named " << le_nom() << finl;
      Process::exit();
    }
  Probleme_base& pb=ref_cast(Probleme_base,probleme(0));
  return pb.schema_temps();
}

// Description:
//    Associe une discretisation a tous les problemes du probleme
//    couple.
//    Appelle Probleme_Base::discretiser(const Discretisation_base&)
//    sur chacun des problemes du probleme couple.
//    voir Probleme_Base::discretiser(const Discretisation_base&)
// Precondition:
// Parametre: Discretisation_base& dis
//    Signification: une discretisation pour tous les problemes
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord: tous les problemes du probleme couple sont discretises
// Postcondition:
void Probleme_Couple::discretiser(const Discretisation_base& dis)
{
  // Loop to discretize each problem of the coupled problem:
  for(int i=0; i< nb_problemes(); i++)
    {
      Cerr<<"The problem that we are starting to discretize is "<<probleme(i).le_nom()<<finl;
      Probleme_base& pb=ref_cast(Probleme_base,probleme(i));
      pb.discretiser(dis);
    }
}

