/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Pilote_ICoCo.cpp
// Directory:   $TRUST_ROOT/src/Kernel/ICoCo
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Pilote_ICoCo.h>
#include <ICoCoTrioField.h>
#include <ProblemTrio.h>
#include <Param.h>
#include <Probleme_U.h>
/*
  #include <vector>
  #include <string>
  #include <iostream>
*/
//using namespace std;
using std::string;
using std::vector;
using ICoCo::ProblemTrio;
using ICoCo::TrioField;

Implemente_instanciable(Pilote_ICoCo,"Pilote_ICoCo",Interprete);


// Description:
//    Simple appel a:
//      Interprete::printOn(Sortie&)
//    Imprime l'interprete sur un flot de sortie
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Pilote_ICoCo::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


// Description:
//    Simple appel a:
//      Interprete::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Pilote_ICoCo::readOn(Entree& is)
{
  return Interprete::readOn(is);
}


// Description:
//    Fonction principale de l'interprete:
//      resoudre un probleme
//    Le probleme est pilote via l'interface ICoCo
//    On lui fournit a chaque pas de temps les champs
//    d'entree "pression_sortie" et "vitesse_entree"
//    qui sont respectivement un ch_front_input et un ch_front_input_uniforme
//    Montre l'utilisation de l'avance en temps et des input fields

//     Utilisation dans le JDD : Pilote_ICoCo_1 pb
//     Utilise dans le cas test U_in_var_impl_ICoCo
void main_pilote_icoco_1(Probleme_U& pb_to_solve)
{

  ProblemTrio pb;

  Nom pb_name=pb_to_solve.le_nom();
  pb.initialize_pb(pb_to_solve);

  bool stop=false; // Does the Problem want to stop ?
  bool ok=true; // Is the time interval successfully solved ?

  // Compute the first time step length
  double dt=pb.computeTimeStep(stop);

  // Loop on the time steps
  while(!stop)
    {

      ok=false; // Is the time interval successfully solved ?

      // Loop on the time interval tries
      while (!ok && !stop)
        {

          // Prepare the next time step
          ok=pb.initTimeStep(dt);
          if (!ok)
            break;

          // Example of field exchange
          {
            vector<string> sv=pb.getInputFieldsNames();
            cout << "Input Fields for " << pb_name << " :" << endl;
            for (unsigned i=0; i<sv.size(); i++)
              std::cout << i << ". " << sv[i] << std::endl;

            TrioField afield;
            int nb_elems,nb_comp;

            // Get the right geometry & nbcomp for "pression_sortie"
            pb.getInputFieldTemplate("pression_sortie",afield);
            // Allocate memory for the values (size=nb_elems*nb_comp)
            afield.set_standalone();
            // Fill the values
            nb_elems=afield._nb_elems;
            nb_comp=afield._nb_field_components;
            assert(nb_comp==1);
            for (int i=0; i<nb_elems; i++)
              afield._field[i]=0;
            // Give the field to the problem
            pb.setInputField("pression_sortie",afield);

            // Get the right geometry & nbcomp for "vitesse_entree"
            pb.getInputFieldTemplate("vitesse_entree",afield);
            // Allocate memory for the values (size=nb_elems*nb_comp)
            afield.set_standalone();
            // Fill the values
            nb_elems=afield._nb_elems;
            nb_comp=afield._nb_field_components;
            assert(nb_elems==1); // ch_front_input_uniforme in the data file
            double t=pb.presentTime();
            t+=dt;
            double vx=(t>100)?100:t;
            for (int i=0; i<nb_elems; i++)
              for (int j=0; j<afield._nb_field_components; j++)
                afield._field[i*nb_comp+j]=(j==0)?vx:0;
            // Give the field to the problem
            pb.setInputField("vitesse_entree",afield);
          }

          // Solve the next time step
          ok=pb.solveTimeStep();

          if (!ok)   // The resolution failed, try with a new time interval.
            {
              pb.abortTimeStep();
              dt=pb.computeTimeStep(stop);
            }

          else // The resolution was successful, validate and go to the
            // next time step.
            pb.validateTimeStep();
        }

      if (!ok) // Impossible to solve the next time step, the Problem
        break; // has given up

      // Compute the next time step length
      dt=pb.computeTimeStep(stop);

      // Stop the resolution if the Problem is stationnary
      if (pb.isStationary())
        stop=true;
    }

  pb.terminate();

}
// Description:
//    Fonction principale de l'interprete:
//      resoudre un probleme
//    Le probleme est pilote via l'interface ICoCo.
//    On lui fournit a chaque pas de temps le champ
//    d'entree "puissance" qui est un Champ_input_P0
//    Montre l'utilisation de l'avance en temps et des input fields

//     Utilisation dans le JDD : Pilote_ICoCo_2 pb
//     Utilise dans le cas test ChDonXYZ_ICoCo
void main_pilote_icoco_2(Probleme_U& pb_to_solve)
{

  ProblemTrio pb;

  Nom pb_name=pb_to_solve.le_nom();
  pb.initialize_pb(pb_to_solve);

  bool stop=false; // Does the Problem want to stop ?
  bool ok=true; // Is the time interval successfully solved ?

  // Compute the first time step length
  double dt=pb.computeTimeStep(stop);

  // Loop on the time steps
  while(!stop)
    {

      ok=false; // Is the time interval successfully solved ?

      // Loop on the time interval tries
      while (!ok && !stop)
        {

          // Prepare the next time step
          ok=pb.initTimeStep(dt);
          if (!ok)
            break;

          // Example of field exchange
          {
            vector<string> sv=pb.getInputFieldsNames();
            cout << "Input Fields for " << pb_name << " :" << endl;
            for (unsigned i=0; i<sv.size(); i++)
              std::cout << i << ". " << sv[i] << std::endl;

            TrioField afield;
            int nb_elems;

            // Get the right geometry & nbcomp for "puissance"
            pb.getInputFieldTemplate("puissance",afield);
            // Allocate memory for the values (size=nb_elems*nb_comp)
            afield.set_standalone();
            // Fill the values
            nb_elems=afield._nb_elems;
            //nb_comp=afield._nb_field_components;
            assert(afield._nb_field_components==1);
            assert(afield._space_dim==2);
            for (int i=0; i<nb_elems; i++)
              {
                // Find gravity center of the element.
                double x=0,y=0;
                for (int j=0; j<afield._nodes_per_elem; j++)
                  {
                    int som=afield._connectivity[afield._nodes_per_elem*i+j];
                    x+=afield._coords[som*2];
                    y+=afield._coords[som*2+1];
                  }
                x/=afield._nodes_per_elem;
                y/=afield._nodes_per_elem;
                afield._field[i]=(((x-0.5)*(x-0.5)+(y-0.5)*(y-0.5))<0.3*0.3)*10;
              }
            // Give the field to the problem
            pb.setInputField("puissance",afield);
          }

          // Solve the next time step
          ok=pb.solveTimeStep();

          if (!ok)   // The resolution failed, try with a new time interval.
            {
              pb.abortTimeStep();
              dt=pb.computeTimeStep(stop);
            }

          else // The resolution was successful, validate and go to the
            // next time step.
            pb.validateTimeStep();
        }

      if (!ok) // Impossible to solve the next time step, the Problem
        break; // has given up

      // Compute the next time step length
      dt=pb.computeTimeStep(stop);

      // Stop the resolution if the Problem is stationnary
      if (pb.isStationary())
        stop=true;
    }

  pb.terminate();
}



// Description:
//    Fonction principale de l'interprete:
//      resoudre un probleme
//    Le probleme est pilote via l'interface ICoCo
//    Le troisieme pas de temps est effectue deux fois
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void main_abort( Probleme_U& pb_to_solve)
{
  //  char* pb_name=nom1.getChar();
  ProblemTrio pb;
  //  pb.setDataFile(pb_name);

  pb.initialize_pb(pb_to_solve);

  bool stop=false; // Does the Problem want to stop ?
  bool ok=true; // Is the time interval successfully solved ?
  int n=0;
  //loop on time step
  while (!stop)                       // Loop on timesteps
    {

      ok=false;

      while (!ok && !stop)                  // Loop on timestep tries
        {

          //compute time step length
          double dt=pb.computeTimeStep(stop);

          if (stop)
            break;

          //prepare the new time step
          pb.initTimeStep(dt);

          //---------------------
          //Solve next time step
          //---------------------

          {
            TrioField afield;
            int nb_elems;
            pb.getInputFieldTemplate("temperature_bord",afield);
            afield.set_standalone();
            nb_elems=afield._nb_elems;
            for (int i=0; i<nb_elems; i++)
              afield._field[i]=600;
            pb.setInputField("temperature_bord",afield);
          }


          bool ok2=pb.solveTimeStep();

          n++;
          if (!ok2 || n==3)   // The resolution failed, try with a new time interval
            {
              Cerr<<" call of abortTimeStep" <<finl;
              pb.abortTimeStep();
            }
          else   // validate and go to the next time step
            {
              pb.validateTimeStep();
            }
        }                                     // End loop on timestep size
    }                                   // End loop on timesteps

  pb.terminate();
}
Entree& Pilote_ICoCo::interpreter(Entree& is)
{
  Param param(que_suis_je());
  Nom nom1;
  param.ajouter("pb_name",&nom1,Param::REQUIRED);
  int methode=-1;
  param.ajouter("main",&methode,Param::REQUIRED);
  param.dictionnaire("abort_time_step",0);
  param.dictionnaire("Pilote_ICoCo_1",1);
  param.dictionnaire("Pilote_ICoCo_2",2);
  param.lire_avec_accolades_depuis(is);
  Probleme_U& pb_to_solve=ref_cast(Probleme_U,objet(nom1));
  switch (methode)
    {
    case 0:
      main_abort(pb_to_solve);
      break;
    case 1:
      main_pilote_icoco_1(pb_to_solve);
      break;
    case 2:
      main_pilote_icoco_2(pb_to_solve);
      break;
    default:
      {
        Cerr<<que_suis_je()<<" main numero "<<methode<<" not implemnted"<<finl;
        exit();
      }
    }
  return is;
}

