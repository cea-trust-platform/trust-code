#ifndef Variable_inclus
#define Variable_inclus

class Variable
{
private:
 double value;
 String *s;


	
public:
Variable()
	{
	value =0.;
	s = new String((char*)"x");
	}
~Variable() { delete s; }
Variable(Variable& v)
	{
	value =v.value;
	s = new String(v.s);
	}

Variable(String& ss)
	{
	value = 0.;
	s = new String(ss);
	}

Variable(char* ss)
	{
	value = 0.;
	s = new String(ss);
	}


double getValue() { return value;}
void setValue(double x) { value = x; }
String& getString() { return *s;}

/*void setValue(String s) 
	{
	System.out.println(s);
	Parser p = new Parser(s);
	p.parseString();
	value = p.eval();
	}
*/
};


#endif 
