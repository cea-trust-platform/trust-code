
#include <LataLoader.h>

#include <string>
#include <fstream>
#include <iostream>

#include <MEDCouplingUMesh.hxx>
#include <MEDCouplingMemArray.hxx>
#include <MEDCouplingFieldDouble.hxx>
#include <MEDCouplingRefCountObject.hxx>


#include <LmlReader.h>
#include <LataJournal.h>
using namespace ParaMEDMEM;


int main(int argc,char ** argv)
{
  cerr<<"Usage : "<<argv[0]<<": file.lata field_name [ numero_temps ] [ numero_bock ] "<<endl;
  LataLoader toto(argv[1]);
  int numero_temps=-1;
  int nblock=-1;
  if (argc>=4) numero_temps=atoi(argv[3]);
  if (argc==5) nblock=atoi(argv[4]);
  MEDCouplingFieldDouble*  field= toto.GetFieldDouble(argv[2],numero_temps,nblock); 
  cerr<< field->getNumberOfValues()<< " "<<field->getArray()->getPointer()[0]<<endl;
  
  field->decrRef();
  return 0;
}
