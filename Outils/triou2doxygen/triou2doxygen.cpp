//
// Transforme un fichier avec des commentaires TrioU
// en fichier avec des commentaires Doxygen.
// 
// ( )// ... => pris en compte comme commentaire de ce qui suit
//              (l'espace est imperatif)
// ... // ... => pris en compte comme commentaire de la ligne qui precede
//               (l'espace est imperatif)
// 
// ... // ... est remplace par ... //!< ...
// 
// dans :
// // ...
// // ...
// * la ".DESCRIPTION " ou "Description: " se retrouve en //!
// * la ".SECTION voir aussi " est transforme en \sa dans /** */
// * les Parametre: sont transformes en \param dans /** */
// * les Retour: sont transformes en \return dans /** */
// * les .autre_chose sont laisses tels quels
// * les sauts de ligne sont inchanges dans /** */, transformes en <br> dans //!
// 
// Le nombre de lignes de commentaires est conserve
// Ce qui est entre /* */ est inchange
// 

#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>

#define MAX_LINE_LENGTH 1024

// Entourloupes pour compiler avec GCC 2.96
// (string::clear() manque et l'ordre des arguments de
// string::compare est different)
// compare ne retourne pas la meme valeur et ca pose probleme...
// compare(0,2,"//") retourne 0 avec gcc 3 et 2 avec gcc 2.96
// Ainsi //.DESCRIPTION n'est pas corrige 
#ifdef __GNUC__
#if __GNUC__ < 3
#define compare(a,b,c) compare(c,a,b)
#define clear() resize(0)
#endif
#endif

using namespace std;

// Add \ before \ @ $ & ~ < > # % 
void strip_special(string& comment) {
  string::size_type pos=0;
  while (1) {
    pos = comment.find_first_of("\\@$&~<>#%",pos+2);
    if (pos==string::npos)
      break;
    comment.insert(pos,"\\");
  }
}

// Takes the concatenation of consecutive full line comments (without "( )// "),
// and interprets it.
void PrintComment(string& comment) {
  string::size_type current_pos=0;
  string short_comment;
  string long_comment;
  int short_lines=0;


  // Means [current_pos,next_pos[ should go to short/long comment.
  // Otherwise stay as standard C++ comment
  bool in_short=false;
  bool in_long=false;
  while (current_pos<comment.size()) {
    string::size_type next_pos=comment.find("\n",current_pos);
    string the_line=comment.substr(current_pos,next_pos-current_pos);
    if (!the_line.compare(0,strlen("File:"),"File:")) {
      in_short=false;
      in_long=false;
    }
    else if (!the_line.compare(0,strlen("Directory:"),"Directory:")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen(".NOM"),".NOM")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen(".ENTETE"),".ENTETE")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen(".LIBRAIRIE"),".LIBRAIRIE")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen(".FILE"),".FILE")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen(".DESCRIPTION"),".DESCRIPTION")) {
      in_short=true;
      in_long=false;
      the_line.erase(0,strlen(".DESCRIPTION"));
    }
    else if (!the_line.compare(0,strlen(".SECTION voir aussi"),".SECTION voir aussi")) {
      the_line.erase(0,strlen(".SECTION voir aussi"));
      while (the_line[0]==' ' || the_line[0]=='\t')
	the_line.erase(the_line.begin());
      if (the_line.size()) {
	in_short=false;
	in_long=true;
	the_line="\\sa "+the_line;
      }
      else
	in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen(".CONTRAINTES"),".CONTRAINTES")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen(".INVARIANTS"),".INVARIANTS")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen(".HTML"),".HTML")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen(".EPS"),".EPS")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen("Description:"),"Description:")) {
      in_short=true;
      in_long=false;
      the_line.erase(0,strlen(".DESCRIPTION"));
    }
    else if (!the_line.compare(0,strlen("Precondition:"),"Precondition:")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen("Parametre:"),"CCCCmetre:")) {
      the_line.erase(0,strlen("Parametre:"));
      while (the_line[0]==' ' || the_line[0]=='\t')
	the_line.erase(the_line.begin());
      if (the_line.size()) {
	in_short=false;
	in_long=true;
	// inversion des mots : le dernier en premier, les autres ensuite
	istringstream is(the_line.c_str());
	string tmp_line;
	string tmp_word1,tmp_word2;
	while (1) {
	  is >> tmp_word2;
	  if(!is)
	    break;
	  tmp_line+=tmp_word1+" ";
	  tmp_word1=tmp_word2;
	}
	the_line="\\param "+tmp_word2+tmp_line;
      }
      else
	in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen("Retour:"),"Retour:")) {
      the_line.erase(0,strlen("Retour:"));
      while (the_line[0]==' ' || the_line[0]=='\t')
	the_line.erase(the_line.begin());
      if (the_line.size()) {
	in_short=false;
	in_long=true;
	the_line="\\return "+the_line;
      }
      else
	in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen("Exception:"),"Exception:")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen("Effets de bord:"),"Effets de bord:")) {
      in_short=in_long=false;
    }
    else if (!the_line.compare(0,strlen("Postcondition:"),"Postcondition:")) {
      in_short=in_long=false;
    }
    current_pos=next_pos+1;
    while (the_line[0]==' ' || the_line[0]=='\t')
      the_line.erase(the_line.begin());
    if (in_short) {
      short_lines++;
      if (the_line.size()) {
	strip_special(the_line);
	if (short_comment.size())
	  short_comment+="<br>";
	short_comment+=the_line;
      }
    }
    else if (in_long) {
      if (the_line.size()) {
	strip_special(the_line);
	if (long_comment.size())
	  long_comment+="<br>\n";
	long_comment+=the_line;
      }
      else
	short_lines++;
    }
    else {
      cout << "// " << the_line << endl;
    }
  }
  comment.clear();
  short_lines+=(short_comment.size()==0);
  while(--short_lines>0)
    cout << "//" << endl;
  if (short_comment.size()) {
    cout << "//! " << short_comment << endl;
  }
  if (long_comment.size()) {
    cout << "/** " << long_comment << " */" << endl;
  }
}

// Returns the number of times pattern is in s
int nfind(string& s, const char* pattern) {
  int n=0;
  string::size_type pos=0;
  while (pos!=string::npos) {
    pos=s.find(pattern,pos);
    if (pos!=string::npos) {
      pos+=s.size();
      n++;
    }
  }
  return n;
}

int main(int argc,char **argv) {

  if (argc!=2)
    return -1;
  ifstream fic(argv[1]);
  if (!fic)
    return -1;

  char line[MAX_LINE_LENGTH];
  line[MAX_LINE_LENGTH-1]=0;
  string comment;
  string end_comment;
  bool endcomment=false;
  int in_c_comment=0; // (are we in a /* */ block at the end of this line ?)
  int nlinesend=0;
  while (fic) {
    fic.getline(line,MAX_LINE_LENGTH-1);
    int firstchar=0;
    while(line[firstchar]==' ' || line[firstchar]=='\t')
      firstchar++;
    string ligne=line+firstchar;

    // Do not alter lines within /* */
    in_c_comment+=nfind(ligne,"/*");
    in_c_comment-=nfind(ligne,"*/");
    if (in_c_comment>0) {
      cout << line << endl;
      continue;
    }

    // Add a space after initial "//" if not present, unless it's a "///"
    if (ligne=="//")
      ligne="// ";
    if (ligne.size()>=3 && !ligne.compare(0,2,"//") && ligne[2]!=' ' && ligne[2]!='/')
      ligne.replace(0,2,"// ");

    string::size_type pos=ligne.find("// ");
    if (pos==0) { // Full line comment or continuing end comment
      if (endcomment) {
	end_comment+=" ";
	end_comment+=ligne.substr(pos+3);
	nlinesend++;
      }
      else {
	comment+=ligne.substr(pos+3);
	comment+='\n';
      }
    }
    else if (pos==string::npos) { // No comment in the line
      if (endcomment) { // Dump previous endline comments if any
	cout << end_comment << endl;
	end_comment.clear();
	while (nlinesend--)
	  cout << "//" << endl;
	endcomment=false;
      }
      PrintComment(comment); // Dump previous comments if any
      if (!ligne.compare(0,3,"///"))
	cout << "//" << endl;
      else {
	cout << line;
	// GF et non cela ne marche pas si on veut interpreter les macros...
	//if (ligne.find("declare_")!=string::npos || ligne.find("implemente_")!=string::npos) cout << ';';
	cout << endl;
      }
    }
    else { // End of line comment
      if (endcomment) { // Dump previous endline comments if any
	cout << end_comment << endl;
	end_comment.clear();
	while (nlinesend--)
	  cout << "//" << endl;
      }
      PrintComment(comment); // Dump previous comments if any
      ligne=line; // get back initial spaces
      pos=ligne.find("// ");
      ligne.replace(pos,3,"//!< ");
      end_comment+=ligne;
      endcomment=true;
      nlinesend=0;
    }
  }
  
}
