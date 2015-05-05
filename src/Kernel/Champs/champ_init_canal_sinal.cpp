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
// File:        champ_init_canal_sinal.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <champ_init_canal_sinal.h>

Implemente_instanciable(champ_init_canal_sinal,"champ_init_canal_sinal",Champ_Don_base);

Sortie& champ_init_canal_sinal::printOn(Sortie& os) const
{
  return os;
}

Entree& champ_init_canal_sinal::readOn(Entree& is)
{
  // Initialisation par defaut des minimums et maximums
  // pour generaliser l'initialisation du canal pour
  // n'importe quelle position dans l'espace.
  min_dir_flow=0.;
  min_dir_wall=0.;
  dir_flow=0;
  dir_wall=1;

  // Lecture de la dimension
  int dim;
  dim=lire_dimension(is,que_suis_je());
  dimensionner(1,dim);
  if (abs(dim)>3)
    {
      Cerr << "Problem while reading the dimension of field champ_init_canal_sinal" << finl;
      Cerr << "Check that it was correctly indicated : dim = " << dim << finl;
      exit();
    }
  // FIN Lecture de la dimension
  ArrOfInt Verif(9);
  Verif[3]=1;
  Verif[5]=1;
  Verif[6]=1;
  Verif[7]=1;
  Verif[8]=1;
  Motcles les_mots(9);
  Motcle mot_lu;
  {
    les_mots[0]="Ucent";
    les_mots[1]="h";
    les_mots[2]="ampli_bruit";
    les_mots[3]="ampli_sin";
    les_mots[4]="omega";
    les_mots[5]="dir_flow";
    les_mots[6]="dir_wall";
    les_mots[7]="min_dir_flow";
    les_mots[8]="min_dir_wall";
  }
  Motcle acc_ouverte("{");
  Motcle acc_fermee("}");
  is >> mot_lu;
  if(mot_lu != acc_ouverte)
    {
      Cerr << "Expecting a { instead of " << mot_lu
           << "while reading intial condition" << finl;
    }
  is >> mot_lu;
  while(mot_lu != acc_fermee)
    {
      int rang=les_mots.search(mot_lu);
      switch(rang)
        {
        case 0 :
          is >> Ucent;
          Cerr << "Centerline Longitudinal Velocity = " << Ucent << finl;
          // valeur par defaut de ampli_sin = Ucent/10.
          ampli_sin = Ucent/10.;
          Verif[0] = 1;
          break;
        case 1  :
          is >> h;
          Cerr << "Channel Half-height = " << h << finl;
          Verif[1] = 1;
          break;
        case 2  :
          is >> ampli_bruit;
          Cerr << "Noise Amplitude = " << ampli_bruit << finl;
          Verif[2] = 1;
          break;
        case 3  :
          is >> ampli_sin;
          Cerr << "Sine Perturbation Amplitude = " << ampli_sin << finl;
          Verif[3] = 1;
          break;
        case 4  :
          is >> omega;
          Cerr << "Omega = " << omega  << finl;
          Verif[4] = 1;
          break;
        case 5  :
          is >> dir_flow;
          Cerr << "Flow direction : " << dir_flow << finl;
          Verif[5] = 1;
          break;
        case 6  :
          is >> dir_wall;
          Cerr << "The specified Wall direction : " << dir_wall  << finl;
          Verif[6] = 1;
          break;
        case 7  :
          is >> min_dir_flow;
          Cerr << "Minimum value position in flow direction : " << min_dir_flow  << finl;
          Verif[7] = 1;
          break;
        case 8  :
          is >> min_dir_wall;
          Cerr << "Minimum value position in wall direction : " << min_dir_wall  << finl;
          Verif[8] = 1;
          break;
        default :
          {
            Cerr << "Error in champ_init_canal_sinal::readOn " << finl;
            Cerr << mot_lu << " is not understood by velocity initial condition generation." << finl;
            Cerr << "The understood keywords are : " << les_mots << finl;
            exit();
          }
        }
      is >> mot_lu;
    }

  if(dir_flow==dir_wall)
    {
      Cerr << "Error in champ_init_canal_sinal::readOn " << finl;
      Cerr << "You gave the same direction for a wall" << finl;
      Cerr << "and the flow direction : Impossible !" << finl;
      exit();
    }

  if((dir_wall>2)||(dir_flow>2))
    {
      Cerr << "Error in champ_init_canal_sinal::readOn " << finl;
      Cerr << "One of the specified directions are not possible." << finl;
      Cerr << "Use : 0 for X, 1 for Y and 2 for Z !" << finl;
      exit();
    }

  int valmin = min_array(Verif);
  int i;
  if (valmin == 0)
    {
      for (i=0; i<5; i++)
        {
          if (Verif[i]==0)
            {
              Cerr << "You didn't give a value for " << les_mots[i] << finl;
            }
        }
      Cerr << "Error in champ_init_canal_sinal::readOn " << finl;
      Cerr << "User needs to specify values for the parameters below.";
      Cerr << "The syntax is : Ucent val_Ucent   h val_h   ampli_bruit val_ampli_bruit [ampli_sin val_ampli_sin] omega val_omega" << finl;
      Cerr << "dir_flow val_dir_flow   dir_wall val_dir_wall" << finl;
      exit();
    }

  return is;
}



// Description:
//
Champ_base& champ_init_canal_sinal::affecter(const Champ_base& ch)
{
  Champ_base::affecter_erreur();
  return *this;
}

// Description:
// Renvoie la valeur en un point.
DoubleVect& champ_init_canal_sinal::valeur_a(const DoubleVect& positions,
                                             DoubleVect& tab_valeurs) const
{
  Cerr<<"values = "<<tab_valeurs<<finl;
  Cerr << "In champ_init_canal_sinal::valeur_a nb_compo_=" << nb_compo_ << finl;
  tab_valeurs.resize(nb_compo_);
  switch(dimension)
    {
    case(1):
      {
        assert(nb_compo_==1);
        tab_valeurs(dir_flow)= fx(positions(dir_flow));
        break;
      }
    case(2):
      {
        assert(nb_compo_<=2);
        tab_valeurs(dir_flow)= fx(positions(dir_flow), positions(dir_wall));
        if(nb_compo_==2)
          tab_valeurs(dir_wall)= fy(positions(dir_flow), positions(dir_wall));
        break;
      }
    case(3):
      {
        assert(nb_compo_<=3);
        tab_valeurs(dir_flow)= fx(positions(dir_flow), positions(dir_wall), positions(3-dir_flow-dir_wall));
        if(nb_compo_>=2)
          tab_valeurs(dir_wall)= fy(positions(dir_flow), positions(dir_wall), positions(3-dir_flow-dir_wall));
        if(nb_compo_>=3)
          tab_valeurs(3-dir_flow-dir_wall)= fz(positions(dir_flow), positions(dir_wall), positions(3-dir_flow-dir_wall));
        break;
      }
    }
  return tab_valeurs;
}

// Description:
//
DoubleVect& champ_init_canal_sinal::valeur_a_elem(const DoubleVect& positions,
                                                  DoubleVect& tab_valeurs,
                                                  int poly) const
{
  return valeur_a(positions, tab_valeurs);
}

// Description:
//
double champ_init_canal_sinal::valeur_a_elem_compo(const DoubleVect& positions,
                                                   int ,int ncomp) const
{
  double val=0.;
  switch(dimension)
    {
    case(1):
      {
        val=fx(positions(dir_flow));
      }
      break;
    case(2):
      {
        if(ncomp==1+dir_flow)
          val=fx(positions(dir_flow),positions(dir_wall));
        else
          val=fy(positions(dir_flow),positions(dir_wall));
      }
      break;
    case(3):
      {
        if(ncomp==1+dir_flow)
          val=fx(positions(dir_flow),positions(dir_wall),positions(3-dir_flow-dir_wall));
        else if(ncomp==1+dir_wall)
          val=fy(positions(dir_flow),positions(dir_wall),positions(3-dir_flow-dir_wall));
        else
          val=fz(positions(dir_flow),positions(dir_wall),positions(3-dir_flow-dir_wall));
      default:
        val=-1;
        assert(0);
        exit();
        break;
      }
      break;
    }
  return val;
}

// Description:
//
DoubleTab& champ_init_canal_sinal::valeur_aux(const DoubleTab& positions,
                                              DoubleTab& tab_valeurs) const
{
  assert(tab_valeurs.size_totale() == dimension*positions.dimension(0));
  int i;
  switch(dimension)
    {
    case(1):
      {
        assert(nb_compo_==1);
        for(i=0; i<tab_valeurs.dimension(0); i++)
          {
            tab_valeurs(i,dir_flow)= fx(positions(i,dir_flow));
          }
        break;
      }
    case(2):
      {
        assert(nb_compo_<=2);
        for(i=0; i<tab_valeurs.dimension(0); i++)
          {
            tab_valeurs(i,dir_flow)= fx(positions(i,dir_flow), positions(i,dir_wall));
            if(nb_compo_==2)
              tab_valeurs(i,dir_wall)= fy(positions(i,dir_flow), positions(i,dir_wall));
          }
        break;
      }
    case(3):
      {
        assert(nb_compo_<=3);
        for(i=0; i<tab_valeurs.dimension(0); i++)
          {
            tab_valeurs(i,dir_flow)= fx(positions(i,dir_flow), positions(i,dir_wall), positions(i,3-dir_flow-dir_wall));
            if(nb_compo_>=2)
              tab_valeurs(i,dir_wall)= fy(positions(i,dir_flow), positions(i,dir_wall), positions(i,3-dir_flow-dir_wall));
            if(nb_compo_>=3)
              tab_valeurs(i,3-dir_flow-dir_wall)= fz(positions(i,dir_flow), positions(i,dir_wall), positions(i,3-dir_flow-dir_wall));
          }
        break;
      }
    }
  return tab_valeurs;
}

// Description:
//
DoubleVect& champ_init_canal_sinal::valeur_aux_compo(const DoubleTab& positions,
                                                     DoubleVect& tab_valeurs, int ncomp) const
{
  int nb_pos=positions.dimension(0);
  tab_valeurs.resize(nb_pos);
  assert(ncomp<3);
  int i;
  switch(dimension)
    {
    case 1:
      {
        for(i=0; i<nb_pos; i++)
          tab_valeurs(i)= fx(positions(i,dir_flow));
        break;
      }
    case 2:
      {
        if(ncomp==dir_flow)
          for(i=0; i<nb_pos; i++)
            tab_valeurs(i)= fx(positions(i,dir_flow), positions(i,dir_wall));
        else
          for(i=0; i<nb_pos; i++)
            tab_valeurs(i)= fy(positions(i,dir_flow), positions(i,dir_wall));
        break;
      }
    case 3:
      {
        if(ncomp==dir_flow)
          for(i=0; i<nb_pos; i++)
            tab_valeurs(i)= fx(positions(i,dir_flow), positions(i,dir_wall), positions(i,3-dir_flow-dir_wall));
        else if(ncomp==dir_wall)
          for(i=0; i<nb_pos; i++)
            tab_valeurs(i)= fy(positions(i,dir_flow), positions(i,dir_wall), positions(i,3-dir_flow-dir_wall));
        else
          for(i=0; i<nb_pos; i++)
            tab_valeurs(i)= fz(positions(i,dir_flow), positions(i,dir_wall), positions(i,3-dir_flow-dir_wall));
        break;
      }
    }
  return tab_valeurs;
}

// Description:
//
DoubleTab& champ_init_canal_sinal::valeur_aux_elems(const DoubleTab& positions,
                                                    const IntVect& ,
                                                    DoubleTab& tab_valeurs) const
{
  return valeur_aux(positions, tab_valeurs);
}

// Description:
//
DoubleVect& champ_init_canal_sinal::valeur_aux_elems_compo(const DoubleTab& positions,
                                                           const IntVect& ,
                                                           DoubleVect& tab_valeurs,
                                                           int ncomp) const
{
  return valeur_aux_compo(positions, tab_valeurs, ncomp);
}
