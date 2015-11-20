#ifndef Variable_inclus
#define Variable_inclus

class Variable
{
private:
 double value;
 String2 *s;


	
public:
Variable()
	{
	value =0.;
	s = new String2((char*)"x");
	}
~Variable() { delete s; }
Variable(Variable& v)
	{
	value =v.value;
	s = new String2(v.s);
	}

Variable(String2& ss)
	{
	value = 0.;
	s = new String2(ss);
	}

Variable(char* ss)
	{
	value = 0.;
	s = new String2(ss);
	}


double getValue() { return value;}
void setValue(double x) { value = x; }
String2& getString2() { return *s;}

/*void setValue(String2 s) 
	{
	System.out.println(s);
	Parser p = new Parser(s);
	p.parseString2();
	value = p.eval();
	}
*/
};


#endif 
