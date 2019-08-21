// Data exchange channel for trio
// TrioDEC.h
// version 0.0 06/06/2014

#ifndef _TrioDEC_included_
#define _TrioDEC_included_

#include <InterpKernelDEC.hxx>

namespace ICoCo
{
  class MEDDoubleField;
  class TrioField;
  class Field;
}

namespace MEDCoupling
{
  class TrioDEC : public InterpKernelDEC
  {
  public:  
    TrioDEC();
    TrioDEC(ProcessorGroup& source_group, ProcessorGroup& target_group);
    TrioDEC(const std::set<int>& src_ids, const std::set<int>& trg_ids, const MPI_Comm& world_comm=MPI_COMM_WORLD);
    void attachLocalField(ICoCo::TrioField *field);
    void attachLocalField(ICoCo::Field *field);
    inline void attachLocalField(MEDCouplingFieldDouble *field)
	{ InterpKernelDEC::attachLocalField(field); }

    inline void attachLocalField(const ICoCo::MEDDoubleField *field)
	{ InterpKernelDEC::attachLocalField(field); }
    virtual ~TrioDEC();
  private:
    void releaseInternalPointer();
  private :
    ICoCo::MEDDoubleField *_my_traduced_field;
  };
}

#endif
