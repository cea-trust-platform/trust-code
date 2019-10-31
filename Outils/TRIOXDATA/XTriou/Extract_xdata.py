from optparse import OptionParser
import os,sys
import itertools
import re


def readSrc(src_dir):
    lines=[]
    for root, dirs, files in os.walk(src_dir):
        for file in files:
            if file.endswith(".cpp"):
                lines+=["New_file "+ file]
                lines_file = open(os.path.join(root, file)).read().splitlines()
                lines+=lines_file
                pass
            pass
        pass
    return lines

def writeRunLog(dico, filename):
    st=""
    for clas in dico.keys():
        st+="class : "+clas+"\n"
        st+="=======\n"
        st+="  - Desc : "+dico[clas]["desc"]+"\n"
        if (len(dico[clas]["parameters"].keys())>0):
            st+="  - Params : \n"
            st+="  ********** \n"
            pass
        for param in dico[clas]["parameters"].keys():
            st+="     + Param : "+param+"  ==> Desc : "+dico[clas]["parameters"][param]["desc"]+"\n"
            st+="       -----\n"
            if (len(dico[clas]["parameters"][param]["dict"].keys())>0):
                st+="        + Dicts : \n"
                st+="          +++++ \n"
                pass
            for dic in dico[clas]["parameters"][param]["dict"].keys():
                st+="            Dict : "+dic+"  ==> Desc : "+dico[clas]["parameters"][param]["dict"][dic]["desc"]+"\n"
                st+="            ----\n"
                pass
            pass
        pass

    fi=open(filename, "w")
    fi.write(st)
    fi.close()

    return

def getLinesWithRegExp(lines):
    dico={}
    for xd in ["XD","2XD","3XD"]:
        debut=0
        for line in lines:
            # on rajoute un blanc pour avoir le dernier mot des commentaires
            line+=" "
            if ((len(line.strip())>=8) and (line.split()[0]=="New_file")):
                debut=1
                filename=line.split()[1]
            # revoir les comm ne marchent pas pour mpcube
            # elif (re.findall("//.*//[ ]*"+xd,line)):
            # continue
            elif (re.findall("//[ ]*"+xd+"[ ]+",line)):
        # traitement des classes
                li=re.findall(re.escape(xd)+"(.*)"+re.escape(' '),line)[0].split(' ')
                li = [x for x in li if x.strip()]
                desc=re.split("//[ ]*"+xd+"[ ]+",line)[-1]
                if li[0]=="attr":
                    if (debut<2):
                        raise Exception("error in "+filename+" first line XD "+line)
                    # print dico[nameClass]
                    desc2=li[1:]
                    dico_p={"desc":' '.join(desc2)}
                    dico_p["dict"]={}
                    dico_p["numero"]=len(dico[nameClass]["parameters"])
                    dico[nameClass]['parameters'][li[1]]=dico_p
            # print li
                  #  print desc2
                    #1/0
                elif li[0]=="ref":
                    if (debut<2):
                        raise Exception("error in "+filename+" first line XD "+line)
                    # print nameClass, line
                    dico[nameClass]["refs"].append([li[1],li[2]])
                    #  1/0
                else:
                    nameClass=li[0]

                    dico[nameClass]={"desc":desc,"parameters":{},"refs":[]}
                    debut=2
                pass
            elif re.findall("//[ ]*"+xd+"_ADD_P+",line):
                # traitement des parametres
                if (debut<2):
                    raise Exception("error in "+filename+" first line XD "+line)
                dico_param={}
                optionnel=True
                if (re.findall("Param::REQUIRED",line)):
                    optionnel=False
                    pass
                param=line.split('"')[1].lower()
                mparam=param.split("|")[0]
                if mparam=="lambda":
                    mparam="lambda_u"
                dico_param["mparm"]=mparam
                dico_param["optionnel"]=optionnel
                dr=line.split(xd+"_ADD_P")[-1].split()
                desc=param+" "+dr[0]+" "+mparam+" "+str(int(optionnel))+" "+' '.join(dr[1:])
                dico_param["desc"]=desc
                dico_param["numero"]=len(dico[nameClass]["parameters"])
                dico_param["dict"]={}
                dico[nameClass]["parameters"][param]=dico_param
                pass
            elif re.findall("//[ ]*"+xd+"_ADD_DICO+",line):
                # traitement des dictionnaires
                if (debut<2):
                    raise 'jjjjjjjj'
                dr=line.split(xd+"_ADD_P")[-1].split()
                dico_dict={}
                dico_dict["desc"]=line
                dict_name=line.split('"')[1].lower()
                dico[nameClass]["parameters"][param]["dict"][dict_name]=dico_dict
                pass
            pass
    return dico

def writeOutPutFile(dico, filename,st_add=""):
    st=""
    for clas in dico.keys():
        st+=dico[clas]["desc"]+"\n"
        Params=dico[clas]["parameters"]
        for i in xrange(len(Params.keys())):
            ok=0
            for j,param in enumerate(Params.keys()):
                if (i==Params[param]["numero"]):
                    ok=1
                    break
            if (ok==0):
                print "pb",clas,"nmero",i,"params",Params
                1/0
            if (len(Params[param]["dict"].keys())==0):
                st+="  attr "+Params[param]["desc"]+"\n"
                pass
            str_dico="  attr "+param+" chaine(into=["
            for dic in Params[param]["dict"].keys():
                str_dico+='"'+dic+'",'
                pass
            if (len(Params[param]["dict"].keys())>0):
                desc=Params[param]["desc"].split()[2:]
                st+=str_dico+"]) "+' '.join(desc)+"\n"
                pass
            pass

        for ref in dico[clas]["refs"]:
            st+="  ref "+ref[0]+" "+ref[1]+"\n"
            pass
        pass
    st=st.replace(" double "," floattant ")
    st=st.replace(" flag "," rien ")
    st=st.replace(" int "," entier ")
    st=st.replace(r"'",r"\'")
    st=st.replace(r"\\'",r"\'")
    #st="\\'".join(st.split("'"))
    #st="\\'".join(st.split("\\\\'"))

    fi=open(filename, "w")
    fi.write(st_add)
    fi.write(st)
    fi.write("\n")
    fi.close()

    return


def run(result_dir, src_dir):
    lines=readSrc(src_dir)
    dico=getLinesWithRegExp(lines)

    run_log=os.path.join(result_dir,"run.log")
    writeRunLog(dico, run_log)

    trad_org=os.path.join(result_dir,"TRAD_2.org")
    fi=open(trad_org,"r")
    st_org=fi.read()
    fi.close()
    st=st_org

    trad_add=os.path.join(result_dir,"TRAD2_ajout0")
    if (os.path.exists(trad_add)):
        fi=open(trad_add,"r")
        st+=fi.read()
        fi.close()


    trad_ajout=os.path.join(result_dir,"TRAD_2")
    writeOutPutFile(dico,trad_ajout,st)

    return

def options_script(argv):

    parser = OptionParser(usage="usage: %prog [options]")

    parser.add_option("-r", "--result", dest="result_dir", type="string",
                      metavar="<result_dir>",
                      help="choose results directory")

    parser.add_option("-s", "--src", dest="src_dir", type="string",
                      metavar="<src_dir>",
                      help="choose src directory")

    parser.set_defaults(result_dir=os.getcwd())
    parser.set_defaults(src_dir=os.getcwd())

    (options, args) = parser.parse_args(argv)

    if len(args) > 0:
        parser.print_help()
        sys.exit(1)
        pass

    if options.result_dir != os.getcwd():
        options.result_dir=os.path.join(os.getcwd(),options.result_dir)
        if not os.path.isdir(options.result_dir):
            os.mkdir(options.result_dir)
            pass
        pass

    result_dir = os.path.expanduser(options.result_dir)
    result_dir = os.path.expandvars(result_dir)
    result_dir = os.path.abspath(result_dir)
    if not os.path.isdir(result_dir):
        sys.stderr.write('Error: result dir \"' + result_dir + '\" is not a directory\n')
        sys.exit(1)
        pass

    src_dir = options.src_dir
    if src_dir!=None:
        os.path.expanduser(options.src_dir)
        src_dir = os.path.expandvars(src_dir)
        src_dir = os.path.abspath(src_dir)
        if not os.path.isdir(src_dir):
            sys.stderr.write('Error: source dir \"' + src_dir + '\" is not a directory\n')
            sys.exit(1)
            pass
        pass

    return result_dir, src_dir

def main(argv):
    """
    Main function.
    """
    result_dir, src_dir = options_script(argv)
    run(result_dir, src_dir)

if __name__ == "__main__":
    main(sys.argv[1:])
