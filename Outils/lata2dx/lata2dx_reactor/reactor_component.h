#ifndef _REACTOR_COMPONENT_H_
#define _REACTOR_COMPONENT_H_

#include <string>
#include <vector>
#include <assert.h>
using namespace std;

class reactor_component {

 public:

  //Functions that give read only acces to reactor_component attributes
  const vector<double>& values() const;
  string name() const;
  string name_of_special_calculation() const;

  //Functions that modify reactor_component attributes
  vector<double>& values();
  string& name();
  string& name_of_special_calculation();

 private :

  vector<double> values_;
  string name_;
  string name_of_special_calculation_;
};

inline const vector<double>& reactor_component::values() const { return values_; }
inline vector<double>& reactor_component::values() { return values_; }

inline string reactor_component::name() const { return name_; }
inline string& reactor_component::name() { return name_; }

inline string reactor_component::name_of_special_calculation() const { return name_of_special_calculation_; }
inline string& reactor_component::name_of_special_calculation() { return name_of_special_calculation_; }

#endif
