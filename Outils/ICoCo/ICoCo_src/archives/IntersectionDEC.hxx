#ifndef IntersectionDEC_hxx
#define IntersectionDEC_hxx
#include  <InterpKernelDEC.hxx>
namespace ParaMEDMEM
{

class IntersectionDEC : public InterpKernelDEC
{
	public: 
    inline IntersectionDEC(ProcessorGroup& source_group, ProcessorGroup& target_group)  : InterpKernelDEC(source_group,target_group) {  };
};
}
//#define IntersectionDEC InterpKernelDEC
#endif
