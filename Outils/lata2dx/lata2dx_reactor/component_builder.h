#ifndef _COMPONENT_BUILDER_H_
#define _COMPONENT_BUILDER_H_

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <map>

#include "reactor.h"
#include "Parser.h"
#include "LataStructures.h"

using namespace std;

string custom_field_name(const LataField_base & f);

class component_builder {

  friend istream& operator>>(istream&, component_builder&);
  friend ostream& operator<<(ostream&, const component_builder&);

  public :

  component_builder();
  component_builder(const Domain&, vector<Field<FloatTab> >&);
  ~component_builder();
 
  void init(const Domain&, vector<Field<FloatTab> >&);

  const vector< vector<string> >& vector_of_parameters_names() const;
  const vector< vector<Parser*> >& vector_of_parsersPtr() const;
  const vector<int>& vector_of_nbplaces() const;
  const vector<int>& vector_of_shape() const;
  const vector<string>& names() const;
  const vector<string>& labels() const;
  const vector<string>& types() const;

  void build_components();
  void build_components(const reactors&);

  protected :

  void update_components_names() const;

  private :

  /* Attributes of class component_builder */
  vector< vector<string> > vector_of_parameters_names_;
  vector< vector<Parser*> > vector_of_parsersPtr_;
  vector<int> vector_of_nbplaces_;
  vector<int> vector_of_shape_;
  vector<string> names_;
  vector<string> labels_;
  vector<string> types_;
  const Domain* domain_;
  vector<Field<FloatTab> > *latafields_;

  map<string, unsigned int> keywords_;
  mutable map<string, unsigned int> components_names_;

  /* Functions used only when creating component_builder object with operator>> */
  void skip(istream&, string);
  void read_function_name(istream&); 
  void read_function_type(istream&); 
  void read_function_label(istream&);
  void read_function_parameters(istream&);
  void read_function_definition(istream&);

  void add_nbplaces();
  void add_shape();

  bool are_names_unknown() const;
  bool are_function_parameters_unknown() const;
  bool are_information_ok() const;
  bool are_data_ok() const;

  /* Functions used only in class component_builder */
  void build_keywords();
  void build_components_names();

  unsigned int search_keyword(const string&) const;
  unsigned int search_component_name(const string&) const;

  LataField_base::Elem_som identity(const string&) const;

};


inline istream& operator>>(istream& is, component_builder& builder) 
{ 
  string read_word;

  while (is >> read_word)
    {
      const int where_in_keywords = builder.search_keyword(read_word); //to do to_upper

      switch(where_in_keywords)
	{
	case 1 : //NAME of function 

	  builder.read_function_name(is);
	  cerr << "Function name : " << builder.names_.back() << endl; 
	  break;

	case 2 : //TYPE of function

	  builder.read_function_type(is);
	  cerr << "Type of function : " << builder.types_.back() << endl;
	  break;

	case 3 : //LABEL of function

	  builder.read_function_label(is);
	  cerr << "Label of function : " << builder.labels_.back() << endl;
	  break;

	case 4 : //PARAMETERS of function

	  builder.read_function_parameters(is);

	  cerr << "Parameters of function : ";
	  for (unsigned int i=0; i<builder.vector_of_parameters_names_.back().size(); i++)
	    cerr << builder.vector_of_parameters_names_.back()[i] << ", ";
	  cerr << endl;

	  builder.read_function_definition(is);
	  break;

	case 5 : //COMMENTARY

	  builder.skip(is,"#"); 
	  break; 

	default : 

	  cerr << "Error in component_builder::operator>>" << endl;
	  cerr << "We should not be here" << endl;
	  cerr << "Exiting program" << endl;
	  exit(-1);

	  break;

	}//end of switch

    }//fin du while 

  if (!builder.are_data_ok())
    {
      cerr << "Error component_builder::operator>>" << endl;
      cerr << "Data are not valid" << endl;
      cerr << "Check your lata file(s) or your data file" << endl;
      cerr << "Exiting programm" << endl;
      exit(-1);
    }

  builder.add_nbplaces();
  builder.add_shape();

  return is; 
}


inline ostream& operator<<(ostream& os, const component_builder& builder) { return os; } //to do

inline const vector< vector<string> >& component_builder::vector_of_parameters_names() const
{
  return vector_of_parameters_names_;
}

inline const vector< vector<Parser*> >& component_builder::vector_of_parsersPtr() const
{
  return vector_of_parsersPtr_;
}

inline const vector<string>& component_builder::types() const
{
  return types_;
}   

inline const vector<string>& component_builder::names() const
{
  return names_;
}   

inline const vector<string>& component_builder::labels() const
{
  return labels_;
}   

inline const vector<int>& component_builder::vector_of_nbplaces() const
{
  return vector_of_nbplaces_;
}

inline const vector<int>& component_builder::vector_of_shape() const
{
  return vector_of_shape_;
}

#endif
