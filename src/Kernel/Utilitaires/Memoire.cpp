/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Memoire.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Memoire.h>
#include <DoubleVect.h>

static int compteur=0;
Memoire* Memoire::_instance = 0;
int Memoire::prems=0;
int Memoire::step=4096;
//struct mallinfo minfo;
static int max_sz_mem=0;
static int min_sz_mem=0;


// Description:
//    Retourne un pointeur sur l'instance de la memoire
//    Cree un nouvel objet memoire si aucune instance n'a deja ete creee
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Memoire*
//    Signification: pointeur sur l'instance de la memoire
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Memoire& Memoire::Instance()
{
  if (_instance == 0)
    {
      _instance = new Memoire;
    }
  return *_instance;
}


// Description:
//    Constructeur
//    Initialize une zone de travail pour les Objet_U, les "double" et les "int"
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Memoire::Memoire() : size(step), data(new Memoire_ptr[step]),
  trav_double(new Double_ptr_trav()), trav_int(new Int_ptr_trav())
{
  // Cerr << "prems="<<prems<<" step="<<step<<finl;
  for(int i=prems; i<size; i++)
    data[i].next=i+1;
  //   minfo = mallinfo();
  //   min_sz_mem = minfo.arena;
  //   max_sz_mem = min_sz_mem;
}


// Description:
//    Ajoute un Objet_U dans la Memoire de TRUST
// Precondition:
//    La memoire doit avoir prealablement ete instanciee
// Parametre: Objet_U* obj
//    Signification: pointeur sur l'Objet_U a ajouter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le numero affecte a l'objet dans la memoire
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Memoire::add(Objet_U* obj)
{
  assert(_instance != 0);
  compteur++;
  if(prems<size)
    {
      const int num=prems;
      assert(data[num].libre());
      data[num].set(obj);
      prems=data[num].next;
      data[num].next = -1;
      assert(prems>=0);
      //      minfo = mallinfo();
      //      int sz_mem = minfo.arena;
      //      if(sz_mem > max_sz_mem) max_sz_mem = sz_mem;
      return num;
    }
  int old_size=size;
  int i;
  size*=2;
  // Cerr<<"size "<<size<<" "<<size/step<<finl;
  Memoire_ptr* newdata=new Memoire_ptr[size];
  if(!newdata)
    {
      Cerr << "Not enough memory " << finl;
      Process::exit();
    }
  Memoire_ptr  newptr(obj);
  for(i=0; i<old_size; i++)
    {
      newdata[i]=data[i];
      data[i].set(0);
    }
  newdata[old_size]=newptr;
  for(i=old_size; i<size; i++)
    newdata[i].next=i+1;
  if(data)
    delete[] data;
  data=newdata;
  prems=old_size+1;
  //   minfo = mallinfo();
  //   int sz_mem = minfo.arena;
  //   if(sz_mem > max_sz_mem) max_sz_mem = sz_mem;
  return old_size;
}


// Description:
//    Suppression de la memoire de l'Objet_U de numero num
//    L'Objet_U n'est pas supprime, seul son pointeur dans la memoire l'est.
// Precondition:
// Parametre: int num
//    Signification: le numero de l'Objet_U a supprimer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour, retourne toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Memoire::suppr(int num)
{
  //   Cerr << "Suppression de " << num << finl;
  assert(!data[num].libre());
  data[num].next=prems;
  prems=num;
  data[num].set(0);
  compteur--;
  if((size>4*compteur)&&(size>step))
    //if((size-compteur)>step)
    {
      compacte();
    }
  /*
    static int deb=0;
    if (compteur >500) deb=1;
    if ((deb)&&(compteur<3))
    {
    Cerr<<"here "<<compteur<<finl;

    int i;
    for(i=0; i<size; i++)
    if(! data[i].libre())
    {
    Cerr << finl;
    Cerr << i << " ";
    const Objet_U& obj=objet_u(i);
    Cerr << " :: ";
    Cerr << "TYPE :" << obj.le_type()<<finl;
    if(sub_type(Nom,obj)) Cerr << " NAME : " << obj.le_nom() <<finl;

    }
    }
  */

  //GF quand on n'a plus d'objet_u on detruit tout

  if (compteur==0)
    {
      //Cerr<<"delete data"<<finl;
      if (data)
        {
          delete[] data;
          delete trav_double;
          delete trav_int;
          data=0;
          delete _instance;
          _instance=0;

        }
    }
  return 1;
}


// Description:
//    Retourne le rang dans la memoire de l'objet de type et de nom indiques
// Precondition:
// Parametre: const Nom& type
//    Signification: le type de l'objet
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const Nom& nom
//    Signification: le nom de l'objet
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le rang de l'objet s'il est trouve dans la memoire, -1 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Memoire::rang(const Nom& type, const Nom& nom) const
{
  Memoire_ptr* x=data;
  for(int i=0; i<size; i++)
    {
      if( !( x->libre() ) )
        if ( ((x->obj()).que_suis_je()==type) &&
             ((x->obj()).le_nom()==nom) )
          return i;
      x++;
    }
  return -1;
}


// Description:
//    Retourne le rang dans la memoire de l'objet de nom indique
// Precondition:
// Parametre: const Nom& nom
//    Signification: le nom de l'objet
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le rang de l'objet s'il est trouve dans la memoire, -1 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Memoire::rang(const Nom& nom) const
{
  Memoire_ptr* x=data;
  for(int i=0; i<size; i++)
    if( !( x->libre() ) )
      {
        if (((x->obj()).le_nom())==nom)
          return i;
        x++;
      }
  return -1;
}


// Description:
//    Retoune une reference sur l'Objet_U de rang num dans la memoire
// Precondition:
//    num doit etre positif ou nul et inferieur au nombre d'objets de la memoire
// Parametre: int num
//    Signification: le rang de l'objet dans la memoire
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Objet_U&
//    Signification: reference sur l'Objet_U trouve
//    Contraintes:
// Exception:
//    Sort en erreur si la memoire comporte une erreur a la case de rang num
// Effets de bord:
// Postcondition:
Objet_U& Memoire::objet_u(int num)
{
  assert(num >=0);
  assert(num < size);
  assert(!data[num].libre());
  Objet_U& objet = data[num].obj();
  const int num_obj = objet.numero();
  if (num_obj != num)
    {
      Cerr << "Error in Objet_U & Memoire::objet_u(int num) " << finl;
      Cerr << " num                       = " << num << finl;
      std::cerr << " data[num].o_ptr           = " << &objet << std::endl;
      Cerr << " data[num].o_ptr->numero() = " << num_obj << finl;
      assert(0);
      Process::exit();
    }
  return objet;
}


// Description:
//    Retoune une reference constante sur l'Objet_U de rang num dans la memoire
// Precondition:
//    num doit etre positif ou nul et inferieur au nombre d'objets de la memoire
// Parametre: int num
//    Signification: le rang de l'objet dans la memoire
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: const Objet_U&
//    Signification: reference sur l'Objet_U trouve
//    Contraintes:
// Exception:
//    Sort en erreur si la memoire comporte une erreur a la case de rang num
// Effets de bord:
// Postcondition:
const Objet_U& Memoire::objet_u(int num) const
{
  assert(num >=0);
  assert(num < size);
  assert(!data[num].libre());
  Objet_U& objet = data[num].obj();
  const int num_obj = objet.numero();
  if (num_obj != num)
    {
      Cerr << "Error in const Objet_U & Memoire::objet_u(int num) " << finl;
      Cerr << " num                       = " << num << finl;
      std::cerr << " data[num].o_ptr           = " << &objet << std::endl;
      Cerr << " data[num].o_ptr->numero() = " << num_obj << finl;
      assert(0);
      Process::exit();
    }
  return objet;
}


// Description:
//    Retoune un pointeur sur l'Objet_U de rang num dans la memoire
// Precondition:
//    num doit etre positif ou nul et inferieur au nombre d'objets de la memoire
// Parametre: int num
//    Signification: le rang de l'objet dans la memoire
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Objet_U&
//    Signification: reference sur l'Objet_U trouve
//    Contraintes:
// Exception:
//    Sort en erreur si la memoire comporte une erreur a la case de rang num
// Effets de bord:
// Postcondition:
Objet_U* Memoire::objet_u_ptr(int num)
{
  assert(num >=0);
  assert(num < size);
  assert(!data[num].libre());
  Objet_U& objet = data[num].obj();
  const int num_obj = objet.numero();
  if (num_obj != num)
    {
      Cerr << "Error in Objet_U * Memoire::objet_u(int num) " << finl;
      Cerr << " num                       = " << num << finl;
      std::cerr << " data[num].o_ptr           = " << &objet << std::endl;
      Cerr << " data[num].o_ptr->numero() = " << num_obj << finl;
      assert(0);
      Process::exit();
    }
  return &objet;
}


// Description:
//    Retoune un pointeur constant sur l'Objet_U de rang num dans la memoire
// Precondition:
//    num doit etre positif ou nul et inferieur au nombre d'objets de la memoire
// Parametre: int num
//    Signification: le rang de l'objet dans la memoire
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Objet_U&
//    Signification: reference sur l'Objet_U trouve
//    Contraintes:
// Exception:
//    Sort en erreur si la memoire comporte une erreur a la case de rang num
// Effets de bord:
// Postcondition:
const Objet_U* Memoire::objet_u_ptr(int num)const
{
  assert(num >=0);
  assert(num < size);
  assert(!data[num].libre());
  Objet_U& objet = data[num].obj();
  const int num_obj = objet.numero();
  if (num_obj != num)
    {
      Cerr << "Error in const Objet_U * Memoire::objet_u(int num) " << finl;
      Cerr << " num                       = " << num << finl;
      std::cerr << " data[num].o_ptr           = " << &objet << std::endl;
      Cerr << " data[num].o_ptr->numero() = " << num_obj << finl;
      assert(0);
      Process::exit();
    }
  return &objet;
}


// Description:
//    Compacte la memoire
//    Ce compactage est effectue automatiquement lorsqu'il devient necessaire
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Memoire::compacte()
{
  //   Cerr << "On compacte la memoire " << finl;
  //   Cerr << "size prems " << size << " " << prems << finl;
  //   Cerr << "avant" << finl;
  int i, compte=0;
  int* newnum=new int[size];
  if(!newnum)
    {
      Cerr << "Not enough memory " << finl;
      Process::exit();
    }
  for(i=0; i<size; i++)
    if(data[i].libre())
      newnum[i]=-1;
    else
      newnum[i]=compte++;
  for(i=0; i<size; i++)
    if(newnum[i]>-1)
      {
        objet_u(i).change_num(newnum);
      }
  Memoire_ptr* newdata=new Memoire_ptr[compte];
  if(!newdata)
    {
      Cerr << "Not enough memory " << finl;
      Process::exit();
    }
  for(i=0; i< size; i++)
    if(newnum[i]>-1)
      newdata[newnum[i]]=data[i];
  delete[] data;
  delete[] newnum;
  data=newdata;
  size=prems=compte;
  for(i=0; i<size; i++)
    data[i].next=i+1;
  verifie();
  //   Cerr << "la memoire est compactee" << finl;
  //   Cerr << "size prems " << size << " " << prems << finl;
}


// Description:
//    Imprime un etat sur la memoire sur la sortie des erreurs
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour; retourne toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Memoire::imprime() const
{
  assert(prems <=size);
  int i, tmp=0;
  for(i=0; i<size; i++)
    if(! data[i].libre()) tmp++;
  Cerr << "\n There are " << size << " memory squares";
  Cerr << "\n of which " << tmp << " ==(const char*)" << compteur << " are used" << finl;
  for(i=0; i<size; i++)
    {
      Cerr << i << " :: " << flush;
      if(! (data[i].libre()) )
        {
          const Objet_U& obj=data[i].obj();
          if (obj.numero() != i)
            Cerr << "error";
          else
            {
              Cerr << "TYPE :" << obj.le_type()<< flush;
              if (sub_type(O_U_Ptr, obj))
                {
                  const O_U_Ptr& x=ref_cast(O_U_Ptr, obj);
                  Cerr << " key: " << x << flush;
                }
              else
                Cerr << " NAME : " << obj.le_nom() << flush;
              Cerr << " address : " << (long)(&(obj))<< flush;
              Cerr << " SIZE : " << (int)(obj.taille_memoire())<< " octets "<< flush;
            }
        }
      else
        Cerr << "free ";
      Cerr << finl;
    }
  return 1;
}


// Description:
//    Verifie le contenu de toutes les cases de la memoire
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour; retourne toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Memoire::verifie() const
{
  assert(prems <=size);
  for(int i=0; i<size; i++)
    {
      if(! (data[i].libre()) )
        {
          if ((data[i].obj()).numero() != i)
            {
              Cerr << "ERROR at the square " << i << finl;
              imprime();
              Process::exit();
            }
        }
    }
  return 1;
}


// Description:
//    Operateur d'affichage d'un etat de la memoire mem sur le flot de sortie os
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const Memoire& mem
//    Signification: la memoire a examiner
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie& le flot de sortie modifie
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& operator << (Sortie& os, const Memoire& mem)
{
  int i;
  int total=0;
  int tmp=0;
  int tmp1=0;
  for(i=0; i<mem.size; i++)
    if(! mem.data[i].libre()) tmp++;
  os << "\n il y a " << mem.size << " cases memoires";
  os << "\n dont " << tmp << " ==(const char*)" << compteur << " sont occupees" << finl;
  for(i=0; i<mem.size; i++)
    if(! mem.data[i].libre())
      {
        os << i << " ";
        const Objet_U& obj=mem.objet_u(i);
        os << " :: ";
        os << "TYPE :" << obj.le_type();
        if(sub_type(Nom,obj)) os << " NOM : " << obj.le_nom() ;
        os << " adresse : " << (long)(&(obj));
        if(sub_type(ArrOfInt,obj))
          {
            const ArrOfInt& toto = static_cast<const ArrOfInt&>(obj);
            tmp = obj.taille_memoire() +
                  (int) ((toto.size_array()*sizeof(int))/toto.ref_count());
            os << " TAILLE ArrOfInt : " << tmp<< " octets ";
            if(sub_type(IntVect,obj))
              {
                const IntVect& titi = ref_cast(IntVect,obj);
                tmp1 = obj.taille_memoire() +
                       (int) ((titi.size()*sizeof(int))/titi.ref_count());
                os << " dont : " << (tmp-tmp1) << " octets virtuels ";
              }
            os << "\n";
          }
        else
          {
            if(sub_type(ArrOfDouble,obj))
              {
                const ArrOfDouble& toto = static_cast<const ArrOfDouble&>(obj);
                tmp = obj.taille_memoire() +
                      (int) ((toto.size_array()*sizeof(double))/toto.ref_count());
                os << " TAILLE ArrOfDouble : " << tmp<< " octets ";
                /* PL: Plante en P1Bulle donc je commente
                   if(sub_type(DoubleVect,obj)){
                   const DoubleVect& titi = ref_cast(DoubleVect,obj);
                   tmp1 = obj.taille_memoire() +
                   (int) ((titi.size()*sizeof(double))/titi.ref_count());
                   os << " dont : " << (tmp-tmp1) << " octets virtuels ";
                   } */
                os << "\n";
              }
            else
              {
                /*
                  if(sub_type(Matrice_Morse_Sym,obj)){
                  const Matrice_Morse_Sym& toto = ref_cast(Matrice_Morse_Sym,obj);
                  tmp = obj.taille_memoire() +
                  (int) (toto.nb_lignes()*sizeof(int)) +
                  (int) ((toto.nb_coeff()+1)*sizeof(int)) +
                  (int) (toto.nb_coeff()*sizeof(double));
                  os << " TAILLE Matrice_Morse_Sym : " << tmp<< " octets ";
                  os << "\n";
                  }
                  else
                */
                os << " TAILLE : " << (tmp=obj.taille_memoire())<< " octets \n ";
              }
          }
        total += tmp;
        os.flush();
      }
  os << "Taille memoire max: " <<  max_sz_mem << finl;
  os << "Taille memoire min: " <<  min_sz_mem << finl;
  os << "Taille memoire occupee en Mo: " <<  (max_sz_mem-min_sz_mem)/1024/1024 << finl;

  return os << "Occupation taille memoire totale en Mo: " << total/1024/1024 << finl;
}




// Description:
//    Destruction de la memoire
//    Supprime les zone de travail pour les Objet_U, les double et les int
//    Sort (exit) avec un code 0
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Memoire::~Memoire()
{
  // GF on a deja tout detruit avant.
  return;
  if (data)
    {
      int i;
      for(i=0; i<size; i++)
        if(! data[i].libre())
          {
            Cerr << finl;
            Cerr << i << " ";
            const Objet_U& obj=objet_u(i);
            Cerr << " :: ";
            Cerr << "TYPE :" << obj.le_type() << finl;
          }

      delete[] data;
      delete trav_double;
      delete trav_int;
      data=0;
    }
  Cerr << "Memory destruction " << finl;
  Cerr << "Max Memory Size : " << max_sz_mem << finl;
  Cerr << "Memory size min : " << min_sz_mem << finl;
  Cerr << "Memory size used in Ko : " << (max_sz_mem-min_sz_mem)/1024 << finl;
  Process::exit();
}

