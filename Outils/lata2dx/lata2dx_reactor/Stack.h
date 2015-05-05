
#ifndef Stack_inclus
#define Stack_inclus
using std::cerr;
using std::endl;

template<class T> class Stack
{
public :

Stack(const int t=10);
~Stack();

int push(const T&);
T& pop(void);
T& peek(void);
int isEmpty() const ;


T* getBase() { return base; }




private:



T *top ;
T *base ;
int taille ;

} ;
template<class T>
inline Stack<T>::Stack(const int t) : taille(t)
{
base = new T [t];
top = base;
}

template<class T>
inline Stack<T>::~Stack(void) {
 delete[] base ; 
}

template<class T>
T& Stack<T>::pop(void)  
{
if (top!=base) return *(--top) ; 
return *base;
}

template<class T>
T& Stack<T>::peek(void)  
{
return *(top-1) ; 
}


template<class T>
int Stack<T>::push(const T& valeur) 
{
if ((top-base)==taille) 
	{
	cerr << "Stack trop petite : on double la taille !!" << endl;
	T* tmp = new T[2*taille];
	for (int i=0;i<taille;i++)
		tmp[i] = base[i];
	delete base;
	base = tmp;
	top = base+taille;
	taille=2*taille;
	}
*top++=valeur ;
return 1;
}


template<class T>
int Stack<T>::isEmpty() const { return top==base ;}



#endif
