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

#include <TRUSTVect.h>
#include <Memoire.h>
#include <Nom.h>

static int compteur=0;
Memoire* Memoire::_instance = 0;
int Memoire::prems=0;
int Memoire::step=4096;
//struct mallinfo minfo;
static int max_sz_mem=0;
static int min_sz_mem=0;

/*! @brief Retourne un pointeur sur l'instance de la memoire Cree un nouvel objet memoire si aucune instance n'a deja ete creee
 *
 * @return (Memoire*) pointeur sur l'instance de la memoire
 */
Memoire& Memoire::Instance()
{
  if (_instance == 0) _instance = new Memoire;

  return *_instance;
}

/*! @brief Constructeur Initialize une zone de travail pour les Objet_U, les "double" et les "int"
 *
 */
Memoire::Memoire() : size(step), data(new Memoire_ptr[step]),
  trav_double(new Double_ptr_trav()), trav_float(new Float_ptr_trav()), trav_int(new Int_ptr_trav())
{
  for(int i=prems; i<size; i++) data[i].next=i+1;
}

/*! @brief Ajoute un Objet_U dans la Memoire de TRUST
 *
 * @param (Objet_U* obj) pointeur sur l'Objet_U a ajouter
 * @return (int) le numero affecte a l'objet dans la memoire
 */
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

/*! @brief Suppression de la memoire de l'Objet_U de numero num L'Objet_U n'est pas supprime, seul son pointeur dans la memoire l'est.
 *
 * @param (int num) le numero de l'Objet_U a supprimer
 * @return (int) code de retour, retourne toujours 1
 */
int Memoire::suppr(int num)
{
  //   Cerr << "Suppression de " << num << finl;
  assert(!data[num].libre());
  data[num].next=prems;
  prems=num;
  data[num].set(0);
  compteur--;
#ifndef _COMPILE_AVEC_PGCC
  if((size>4*compteur)&&(size>step))
    //if((size-compteur)>step)
    {
      compacte(); // Plantage obscur avec le compilateur NVidia a la fin du calcul
    }
#endif
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
          delete trav_float;
          delete trav_int;
          data=0;
          delete _instance;
          _instance=0;

        }
    }
  return 1;
}


/*! @brief Retourne le rang dans la memoire de l'objet de type et de nom indiques
 *
 * @param (const Nom& type) le type de l'objet
 * @param (const Nom& nom) le nom de l'objet
 * @return (int) le rang de l'objet s'il est trouve dans la memoire, -1 sinon
 */
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


/*! @brief Retourne le rang dans la memoire de l'objet de nom indique
 *
 * @param (const Nom& nom) le nom de l'objet
 * @return (int) le rang de l'objet s'il est trouve dans la memoire, -1 sinon
 */
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


/*! @brief Retoune une reference sur l'Objet_U de rang num dans la memoire
 *
 * @param (int num) le rang de l'objet dans la memoire
 * @return (Objet_U&) reference sur l'Objet_U trouve
 * @throws Sort en erreur si la memoire comporte une erreur a la case de rang num
 */
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


/*! @brief Retoune une reference constante sur l'Objet_U de rang num dans la memoire
 *
 * @param (int num) le rang de l'objet dans la memoire
 * @return (const Objet_U&) reference sur l'Objet_U trouve
 * @throws Sort en erreur si la memoire comporte une erreur a la case de rang num
 */
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


/*! @brief Retoune un pointeur sur l'Objet_U de rang num dans la memoire
 *
 * @param (int num) le rang de l'objet dans la memoire
 * @return (Objet_U&) reference sur l'Objet_U trouve
 * @throws Sort en erreur si la memoire comporte une erreur a la case de rang num
 */
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


/*! @brief Retoune un pointeur constant sur l'Objet_U de rang num dans la memoire
 *
 * @param (int num) le rang de l'objet dans la memoire
 * @return (Objet_U&) reference sur l'Objet_U trouve
 * @throws Sort en erreur si la memoire comporte une erreur a la case de rang num
 */
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

/*! @brief Compacte la memoire Ce compactage est effectue automatiquement lorsqu'il devient necessaire
 *
 */
void Memoire::compacte()
{
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
}

/*! @brief Imprime un etat sur la memoire sur la sortie des erreurs
 *
 * @return (int) code de retour; retourne toujours 1
 */
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
              if (sub_type(Objet_U_ptr, obj))
                {
                  const Objet_U_ptr& x=ref_cast(Objet_U_ptr, obj);
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

/*! @brief Verifie le contenu de toutes les cases de la memoire
 *
 * @return (int) code de retour; retourne toujours 1
 */
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

/*! @brief Operateur d'affichage d'un etat de la memoire mem sur le flot de sortie os
 *
 * @param (Sortie& os) le flot de sortie a utiliser
 * @param (const Memoire& mem) la memoire a examiner
 * @return (Sortie& le flot de sortie modifie)
 */
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

        const ArrOfInt* intA = dynamic_cast<const ArrOfInt*>(&obj);
        const ArrOfDouble* intAD = dynamic_cast<const ArrOfDouble*>(&obj);
        if(intA)
          {
            const ArrOfInt& toto = *intA;
            tmp = obj.taille_memoire() + (int) ((toto.size_array()*sizeof(int))/toto.ref_count());
            os << " TAILLE ArrOfInt : " << tmp<< " octets ";

            const IntVect* intV = dynamic_cast<const IntVect*>(&obj);
            if(intV)
              {
                const IntVect& titi = *intV;
                tmp1 = obj.taille_memoire() + (int) ((titi.size()*sizeof(int))/titi.ref_count());
                os << " dont : " << (tmp-tmp1) << " octets virtuels ";
              }
            os << "\n";
          }
        else if (intAD)
          {
            const ArrOfDouble& toto = *intAD;
            tmp = obj.taille_memoire() + (int) ((toto.size_array()*sizeof(double))/toto.ref_count());
            os << " TAILLE ArrOfDouble : " << tmp<< " octets ";
            /* PL: Plante en P1Bulle donc je commente
             * const DoubleVect* intVD = dynamic_cast<const DoubleVect*>(&obj);
               if(intVD){
               const DoubleVect& titi = *intVD;
               tmp1 = obj.taille_memoire() + (int) ((titi.size()*sizeof(double))/titi.ref_count());
               os << " dont : " << (tmp-tmp1) << " octets virtuels ";
               } */
            os << "\n";
          }
        else
          {
            const ArrOfFloat* intAF = dynamic_cast<const ArrOfFloat*>(&obj);
            if(intAF)
              {
                const ArrOfFloat& toto = *intAF;
                tmp = obj.taille_memoire() + (int) ((toto.size_array()*sizeof(float))/toto.ref_count());
                os << " TAILLE ArrOfFloat : " << tmp<< " octets ";
                os << "\n";
              }
            else
              {
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

/*! @brief Destruction de la memoire Supprime les zone de travail pour les Objet_U, les double et les int
 *
 *     Sort (exit) avec un code 0
 *
 */
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
      delete trav_float;
      delete trav_int;
      data=0;
    }
  Cerr << "Memory destruction " << finl;
  Cerr << "Max Memory Size : " << max_sz_mem << finl;
  Cerr << "Memory size min : " << min_sz_mem << finl;
  Cerr << "Memory size used in Ko : " << (max_sz_mem-min_sz_mem)/1024 << finl;
  Process::exit();
}
