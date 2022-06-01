"""
Victor Banon Garcia / Adrien Bruneton
CEA Saclay - DM2S/STMF/LGLS
03/2021

We provide here a Python package that can be used to run validation files from jupyterlab.

IMPORTANT: we assume that the methods of this module are invoked from the root directory of a
validation form (i.e. a directory containing at least a 'src' subdirectory).

"""

import os
import subprocess
from string import Template


def saveFormOutput():
    """ Dummy method to indicate that the output of the notebook should be saved.
    This method does nothing here, but its invokation is detected by the pre_save hook
    registered in the TRUST Jupyter configuration.
    By default, outputs of the validation forms are not saved.
    """
    pass

def getLastLines_(err_pth):
    """ Get last 20 lines of an error file ...
    """
    if os.path.exists(err_pth):
        with open(err_pth) as f:
            s = "(....)"
            txt = f.readlines()
            err = "".join(txt[-20:])
    else:
        err = "Error file %s can not be opened!" % err_pth
    return err


def displayMD(txt):
    """ Display text as markdown in Jupyter notebook.
    """
    try:
        from IPython.display import display, Markdown

        ok = True
    except:
        ok = False
    if ok:
        display(Markdown(txt))


def useMEDCoupling():
    """ Load MEDCoupling environment in the PYTHONPATH so that 'import medcoupling' can work.
    """
    import sys

    mcr = os.environ["TRUST_MEDCOUPLING_ROOT"]
    sub = "lib/python%d.%d/site-packages" % (sys.version_info.major, sys.version_info.minor)
    sys.path.append(os.path.join(mcr, sub))
    sys.path.append(os.path.join(mcr, "bin"))
    try:
        import medcoupling
    except:
        raise Exception("Could not load MEDCoupling environment!")

######## PRIVATE STUFF #########
ORIGIN_DIRECTORY = os.getcwd()

def _initBuildDir():
    """
    Private. Initialize build directory: this can be overriden with the "-dest" option in env variable JUPYTER_OPTIONS
    Change the build directory if -dest option provided. This is mostly for NR test and validation
    """
    ret = os.path.join(ORIGIN_DIRECTORY,"build")
    # Compute correct build directory if overriden:
    opt = os.environ.get("JUPYTER_RUN_OPTIONS", "")
    a = opt.split(" ")
    if "-dest" in a:
        idx = a.index("-dest")
        if idx >= 0 and len(a) >= idx + 2:
            ret = os.path.join(a[idx + 1], "build")
    return ret

defaultSuite_ = None  # a TRUSTSuite instance
BUILD_DIRECTORY = _initBuildDir()

def _runCommand(cmd, verbose):
    """ Private method to run a command, print the logs if verbose or if it fails, and throw an exception
    if it fails.
    """
    # Run by redirecting stderr to stdout
    complProc = subprocess.run(cmd, shell=True, executable="/bin/bash", stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if verbose or complProc.returncode != 0:
        print(cmd)
        print(complProc.stdout.decode('utf-8'))
    # Throw if return code non-zero:
    if complProc.returncode:
        # Display message through a custom exception so that jupyter-nbconvert also shows it properly in the console:
        msg = "\nExecution of following command failed!!\n"
        msg += "  " + cmd
        msg += "\nwith return code %d\n" % complProc.returncode
        msg += "and with following output:\n\n"
        msg += complProc.stdout.decode('utf-8')
        raise RuntimeError(msg)

######## End PRIVATE STUFF #########

class TRUSTCase(object):
    """ Class which allows the user to load and execute a validation case
    """

    UNIQ_ID_START = -1

    def __init__(self, directory, datasetName, nbProcs=1, record=False):
        """ Initialisation of the class
        @param directory: str : Path of the file
        @param dataName: str: Path of the case we want to run, relatively to the 'src' folder.
        @param nbProcs : int : Number of processors to use to run the case.
        @param record : bool : whether to add the case to the global list of cases to run
        """
        TRUSTCase.UNIQ_ID_START += 1
        self.id_ = TRUSTCase.UNIQ_ID_START  # Unique ID
        self.dir_ = os.path.normpath(directory)
        self.name_ = datasetName.split(".data")[0]  # always w/o the trailing .data
        self.nbProcs_ = nbProcs
        self.last_run_ok_ = -255  # exit status of the last run of the case
        self.last_run_err_ = ""  # error message returned when last running the case
        if record:
            self._ListCases.append(self)

    def fullDir(self):
        """
        @return full directory of the test case
        """
        return os.path.normpath(os.path.join(BUILD_DIRECTORY, self.dir_))

    def fullPath(self):
        """
        @return full path of the test case in the build directory
        """
        fullPath = os.path.join(self.fullDir(), self.name_)
        return fullPath + ".data"

    def substitute(self, find, replace):
        """ Substitute (in place and in the build directory) a part of the dataset
        @param find : str : Text we want to substitute.
        @param replace : str: Text to insert in replacement.
        """
        path = self.fullPath()
        text_out = ""
        with open(path, "r") as f:
            text_in = f.read()
            text_out += text_in.replace(find, replace)

        with open(path, "w") as f:
            f.write(text_out)

    def substitute_template(self, subs_dict):
        """ Substitute (in place and in the build directory) a part of the dataset
        @param subs_dict : dict : Text we want to substitute.
        """
        path = self.fullPath()
        with open(path, "r") as file: filedata = Template(file.read())
        result = filedata.substitute(subs_dict)
        with open(path, "w") as file: file.write(result)

    def copy(self, targetName, directory=None, nbProcs=1):
        """ Copy a TRUST Case.
        """
        if directory is None:
            directory = self.dir_
        # Create the directory if not there:
        fullDir2 = os.path.join(BUILD_DIRECTORY, directory)
        if not os.path.exists(fullDir2):
            os.makedirs(fullDir2, exist_ok=True)
        pthTgt = os.path.join(BUILD_DIRECTORY, directory, targetName)
        # And copy the .data file:TRUSTSuite
        from shutil import copyfile

        copyfile(self.fullPath(), pthTgt)

        return TRUSTCase(directory, targetName, nbProcs=nbProcs, record=False)

    def dumpDataset(self, user_keywords=[]):
        """ Print out the .data file. TODO handle upper / lower case
        @param list_Trust_user_words : list(str) : List of word the user wants to color in red.
        """
        ###############################################################
        # Voici un lien qui explique comment on manipule les couleurs #
        # https://stackabuse.com/how-to-print-colored-text-in-python/ #
        ###############################################################

        ### Class that I found interesting to change theirs colors. Victor ###
        list_Trust_classe = [
            "fields",
            "Partition",
            "Read",
            "Conduction",
            "solveur implicite",
            "solveur gmres",
            "Post_processing",
            "diffusion",
            "initial_conditions",
            "boundary_conditions",
            "Probes",
        ]

        ### Import and change de layout of TRUST Keyword ###
        keywords = os.path.join(os.getenv("TRUST_ROOT"), "doc", "TRUST", "Keywords.txt")
        f = open(keywords, "r")
        tmp = []
        for i in f.readlines():
            i = i.replace("|", " ")
            i = i.replace("\n", " ")
            if len(i) == 2:
                i = i.replace(i, "")
            else:
                tmp.append(i)
        list_Trust_keywork = tmp
        f.close()

        ### Import and Underline important words of the data file  ###
        f = open(self.fullPath(), "r")
        tmp = f.readlines()
        test = []
        for j in tmp:
            flag = 0
            if j[0] == "#":
                flag = 1
                j = "\033[38;5;88m" + j + "\033[0;0m"
            j = " " + j
            j = j.replace("\t", "\t ")
            if flag == 0:
                for i in list_Trust_keywork:
                    j = j.replace(i, "\033[38;5;28m" + i + "\033[0;0m")
            if flag == 0:
                for i in list_Trust_classe:
                    j = j.replace(i, "\033[38;5;4m" + i + "\033[0;0m")
            if flag == 0:
                for i in user_keywords:
                    # tmp=tmp.replace(i, "\033[196;5;30m"+i+"\033[0;0m ") # en gras
                    j = j.replace(i, "\033[38;5;196m" + i + "\033[0;0m")  # en rouge
            test.append(j)
        f.close()

        ### Print the coloured .data file ###
        print("".join(test))

    def _runScript(self, scriptName, verbose=False):
        """ Internal. Run a shell script if it exists.
        """
        pth = "./%s" % scriptName
        if os.path.exists(pth):
            subprocess.check_output("chmod u+x %s" % pth, shell=True)
            cmd = "%s %s" % (pth, self.name_)
            _runCommand(cmd, verbose)

    def preRun(self, verbose):
        self._runScript("pre_run", verbose)

    def postRun(self, verbose):
        self._runScript("post_run", verbose)

    def generateExecScript(self):
        """ Generate a shell script doing the
            - pre_run
            - launching the case
            - and doing hte post_run
        """
        uniq_id = "{:04d}".format(self.id_)
        scriptFl = os.path.join(BUILD_DIRECTORY, "cmds_%s.sh" % uniq_id)

        logName = "cmds_%s.log" % uniq_id
        n, d = self.name_, self.dir_
        fullD, fullL = os.path.join(BUILD_DIRECTORY, d), os.path.join(BUILD_DIRECTORY, logName)
        para = ""
        if self.nbProcs_ != 1:
            para = str(self.nbProcs_)
        with open(scriptFl, "w") as f:
            s = "#!/bin/bash\n"
            s += "( echo;\n"
            s += '  echo "-> Running the calculation of the %s data file in the %s directory ...";\n' % (n, d)
            s += "  cd %s ; \n" % fullD
            s += '  [ -f pre_run ] && chmod +x pre_run && echo "-> Running the pre_run script in the %s directory ..." && (./pre_run %s || exit -1);\n' % (d, n)
            s += "  trust %s %s 1>%s.out 2>%s.err;\n" % (n, para, n, n)
            s += "  if [ ! $? -eq 0 ]; then exit -1; fi; \n"
            s += '  [ -f post_run ] && chmod +x post_run && echo "-> Running the post_run script in the %s directory ..." && (./post_run %s || exit -1);\n' % (d, n)
            s += "  exit 0;"
            s += ") 1>%s 2>&1 \n" % fullL
            f.write(s)
        os.chmod(scriptFl, 0o755)
        return scriptFl, fullL

    def runCase(self, verbose=False):
        """ Move to the case directory and execute the current test case:
        - calls pre_run if any
        - run the case
        - calls post_run if any
        @param verbose
        @return None
        The results of the run are stored in members self.last_run_ok_ and self.last_run_err_
        """
        ok, err = True, ""
        os.chdir(self.fullDir())

        ### Run pre_run ###
        self.preRun(verbose)

        ### Run Case ###
        err_file = self.name_ + ".err"
        out_file = self.name_ + ".out"
        cmd = "trust %s %s 2>%s 1>%s" % (self.name_, str(self.nbProcs_), err_file, out_file)
        output = subprocess.run(cmd, shell=True, executable="/bin/bash", stderr=subprocess.STDOUT)
        if verbose:
            print(cmd)
            print(output.stdout)
        if output.returncode != 0:
            ok = False
            err = getLastLines_(self.name_ + ".err")

        ### Run post_run ###
        if ok:
            self.postRun(verbose)

        ### Return to initial directory ###
        os.chdir(ORIGIN_DIRECTORY)
        self.last_run_ok_, self.last_run_err_ = ok, err
        return ok, err

    def addPerfToTable(self, zeTable):
        """ Extract performances for this case and add it to the global table
            passed in parameter.
        """
        os.chdir(self.fullDir())
 
        opt = os.environ.get("JUPYTER_RUN_OPTIONS", None)
        # Very specific to the validation process - we need to keep build there:
        if not opt is None and "-not_run" in opt:
            cmd = '.'
        else:
            cmd = os.environ["TRUST_ROOT"] + "/Validation/Outils/Genere_courbe/scripts/extract_perf " + self.name_
            _runCommand(cmd, False)

        f = open(self.name_ + ".perf", "r")
        row = f.readlines()[0].replace("\n", "").split(" ")[1:]
        f.close()

        if len(row) == 6:
            row_arrange = row[0:5]
            row_arrange[4] = str(row[4]) + "-" + str(row[5])
            row = row_arrange

        zeTable.addLigne([row], self.dir_ + "/" + self.name_)
        os.chdir(ORIGIN_DIRECTORY)
        
        ## Save the file
        saveFileAccumulator(self.dir_ + "/" + self.name_ + ".perf")


class TRUSTSuite(object):
    """ A set of TRUST cases to be run for the validation form.
    """
    def __init__(self, runPrepare=True):
        self.cases_ = []

        self.copySrc()
        if runPrepare:
            self.executeScript("prepare")

    def copySrc(self):
        """ Copy content of src directory into build directory.
        WARNING: this is where we expect to be at the root of the validation form.
        """
        if not os.path.exists("src"):
            raise Exception("Not a coherent validation form directory: 'src' subdirectory not found.")
        # Mimick what is done in 'prepare_gen' TRUST script:
        subprocess.run("mkdir -p %s && cp -a src/* %s 2>/dev/null" % (BUILD_DIRECTORY, BUILD_DIRECTORY), shell=True)  # Note the '*' !!

    def executeScript(self, scriptName, verbose=False):
        """ Execute scriptName if any.
        """
        os.chdir(BUILD_DIRECTORY)
        pth = "./" + scriptName
        if os.path.exists(pth):
            cmd = pth
            subprocess.check_output("chmod u+x %s" % pth, shell=True)
            _runCommand(cmd, verbose)
        os.chdir(ORIGIN_DIRECTORY)

    def executeCommand(self, cmd, verbose=False):
        """ Execute bash command (in the build directory)
        """
        os.chdir(BUILD_DIRECTORY)
        _runCommand(cmd, verbose)
        os.chdir(ORIGIN_DIRECTORY)

    def addCase(self, case):
        self.cases_.append(case)

    def getCases(self):
        return self.cases_

    def detectSserver(self):
        """ Detect whether the Sserver is running on the machine
        """
        squeue = os.path.join(os.environ["TRUST_ROOT"], "bin", "Sjob", "Squeue")
        try:
            subprocess.check_call([squeue], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            return True
        except subprocess.CalledProcessError:
            return False

    def runCases(self, verbose=False, preventConcurrent=False):
        """ Launch all cases for the current suite.
        @param verbose: bool : whether to print the console output of the run.
        @param preventConcurrent: run the cases in the order they were provided, even if the Sserver is up and running, and the -parallel_sjob option
        was passed.
        """
        opt = os.environ.get("JUPYTER_RUN_OPTIONS", "")
        # Very specific to the validation process. Sometimes we want the core
        # method 'runCases()' not to do anything ... see script 'archive_resultat' for example.
        if "-not_run" in opt:
            return
        ## Check run environment - should we run // ?
        ## We do so if JUPYTER_RUN_OPTIONS is not there, or if it is there with value '-parallel_sjob'
        ## Hence, s.o. who runs the Sserver on its machine will benefit from it directly, and on the other hand
        ## the validation process can control this finely.
        runParallel = not preventConcurrent and ((opt == "" or "-parallel_sjob" in opt.split(" ")) and self.detectSserver())
        extra = {True: "**with Sserver**", False: ""}[runParallel]
        print("Running %s..." % extra)

        from time import time

        t0 = time()
        stream = os.popen("echo Returned output")
        if verbose:
            print(stream.read())

        ### Change the root to build file ###

        allOK = True
        lstC = defaultSuite_.getCases()

        th_lst, log_lst = [], []
        err_msg = "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        err_msg += "Case '%s/%s.data' FAILED !! Here are the last 20 lines of the log file:\n"
        err_msg += "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        if runParallel:
            salloc = os.path.join(os.environ["TRUST_ROOT"], "bin", "Sjob", "Salloc")
            for case in lstC:
                (script, logFile,) = case.generateExecScript()  # Generate the shell script doing pre_run, case and post_run
                log_lst.append(logFile)
                # Invoke Salloc to schedule test case execution:
                cmdLst = [salloc, "-n", str(case.nbProcs_), script]
                #   We don't track Salloc output (should we?)
                th = subprocess.Popen(cmdLst, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                th_lst.append(th)
            for i, th in enumerate(th_lst):
                th.wait()
                if th.returncode != 0:
                    allOK, case = False, lstC[i]
                    print(err_msg % (case.dir_, case.name_))
                    print(getLastLines_(log_lst[i]))
        else:
            for case in lstC:
                try:
                    case.runCase()
                    allOK = allOK and case.last_run_ok_
                    if not allOK:
                        print(err_msg % (case.dir_, case.name_))
                        print(case.last_run_err_)
                except Exception as e:
                    os.chdir(ORIGIN_DIRECTORY)  # Restore initial directory
                    raise e
        t1 = time()
        if allOK:
            dt = t1 - t0
            # Validation process - we do not output variable information:
            if os.environ.get("JUPYTER_IS_VALIDATION", False):
                dt = 0
            print("  => A total of %d cases were (successfully) run in %.1fs." % (len(lstC), dt))

def readFile(data):
    """
    Method to open and read file with Save in FileAccumulator"
    """
    path = os.getcwd()
    os.chdir(BUILD_DIRECTORY)
    
    f = open(data,"r")
    print(f.read())
    f.close()
    
    os.chdir(path)

    saveFileAccumulator(data)

def saveFileAccumulator(data):
    """ Method for saving files.
    @param data : str : name of the file we want to save.
    """
    from .filelist import FileAccumulator

    path = os.getcwd()
    os.chdir(BUILD_DIRECTORY)

    FileAccumulator.active = True
    FileAccumulator.Append(data)
    FileAccumulator.WriteToFile("used_files", mode="a")

    os.chdir(path)

def introduction(auteur, creationDate=None):
    """ Function that creates an introduction cell Mardown
    @param auteur : str : Name of the author of the test case.
    """
    from datetime import datetime

    format = "%d/%m/%Y"
    today = datetime.today()
    # Validation process - we do not output variable information:
    if os.environ.get("JUPYTER_IS_VALIDATION", False): dat = ""
    else:                                              dat = today.strftime(format)

    displayMD("## Introduction \n Validation made by : " + auteur + "\n")
    if creationDate:
        try:
            datetime.strptime(creationDate, format)
        except ValueError:
            print("This is the incorrect date string format. It should be DD/MM/YYYY")
        displayMD("\n Report created : " + creationDate + "\n")
    displayMD("\n Report generated " + dat)


def description(text):
    """ Function that creates a Description cell Mardown
    @param text : str : Description test.
    """
    displayMD("### Description \n" + text)


def TRUST_parameters(version="", param=[]):
    """ Function that creates a cell Mardown giving TRUST parameters (version, binary)
    @param version : str : version of trust - if void TRUST_VERSION read
    @param : str list : List of Parameter used in this test case
    """
    # Validation process - we do not output variable information:
    builtOn = BUILD_DIRECTORY
    if os.environ.get("JUPYTER_IS_VALIDATION", False):
        binary, version, builtOn = "BINARY", "VERSION", "BUILDDIR"
    elif version == "":
        binary, version = os.environ.get("exec", "UNKNOWN"), os.environ.get("TRUST_VERSION", "UNKNOWN")
    else:
        binary = os.environ.get("exec", "UNKNOWN")

    text = "### TRUST parameters \n * Version TRUST: " + version + "\n * Binary used: " + binary + " (built in directory " + builtOn + ")"
    for i in param:
        text = text + "\n" + i
    displayMD(text)


def dumpDataset(fiche, list_Trust_user_words=[]):
    """ Print out the .data file.
    @param fiche: str : Path of the file
    @param list_Trust_user_words : list(str) : List of word the user wants to color in red.
    """
    c = TRUSTCase(directory=BUILD_DIRECTORY, datasetName=fiche)
    c.dumpDataset(list_Trust_user_words)


def dumpData(fiche, list_keywords=[]):
    """ Print out the file.
    @param fiche: str : Path of the file
    """
    ## Save the file
    name = os.path.join(BUILD_DIRECTORY, fiche)
    saveFileAccumulator(fiche)

    f = open(name, "r")
    tmp = f.readlines()
    test = []
    for j in tmp:
        flag = 0
        if j[0] == "#":
            flag = 1
            j = "\033[38;5;88m" + j + "\033[0;0m"
        j = " " + j
        j = j.replace("\t", "\t ")
        if flag == 0:
            for i in list_keywords:
                # tmp=tmp.replace(i, "\033[196;5;30m"+i+"\033[0;0m ") # en gras
                j = j.replace(i, "\033[38;5;28m" + i + "\033[0;0m")  # en rouge
        test.append(j)
    f.close()

    print("".join(test))

def addCaseFromTemplate(datasetName, directory, d, nbProcs=1):
    """ Add a case to run to the list of globally recorded cases.
    @param directoryOrTRUSTCase: str : directory where the case is stored (relative to build/)
    @param datasetName: str : Name of the case we want to run.
    @param nbProcs : int : Number of proceseurs
    @return case: a TRUSTcase instance. Trust we want to launch.
    """
    global defaultSuite_
    if defaultSuite_ is None:
        # When called for the first time, will copy src to build, and execute 'prepare' script if any
        defaultSuite_ = TRUSTSuite()
    fullDir = os.path.join(BUILD_DIRECTORY, datasetName)
    fullDir2 = os.path.join(BUILD_DIRECTORY, directory)
    if not os.path.exists(fullDir2):
        os.makedirs(fullDir2, exist_ok=True)
    pthTgt = os.path.join(BUILD_DIRECTORY, directory, datasetName)
    # And copy the .data file:
    from shutil import copyfile

    copyfile(fullDir, pthTgt)
    tc = addCase(directory, datasetName, nbProcs)
    tc.substitute_template(d)
    return tc


def addCase(directoryOrTRUSTCase, datasetName="", nbProcs=1):
    """ Add a case to run to the list of globally recorded cases.
    @param directoryOrTRUSTCase: str : directory where the case is stored (relative to build/)
    @param datasetName: str : Name of the case we want to run.
    @param nbProcs : int : Number of proceseurs
    @return case: a TRUSTcase instance. Trust we want to launch.
    """
    global defaultSuite_
    if isinstance(directoryOrTRUSTCase, TRUSTCase):
        if datasetName != "":
            raise ValueError("addCase() method can either be called with a single argument (a TRUSTCase object) or with at least 2 arguments (directory and case name)")
        tc = directoryOrTRUSTCase
    elif isinstance(directoryOrTRUSTCase, str):
        if datasetName == "":
            raise ValueError("addCase() method can either be called with a single argument (a TRUSTCase object) or with at least 2 arguments (directory and case name)")
        tc = TRUSTCase(directoryOrTRUSTCase, datasetName, nbProcs)
    initCaseSuite()
    defaultSuite_.addCase(tc)
    return tc

def initCaseSuite():
    """ Instantiate a default suite of cases """
    global defaultSuite_
    if defaultSuite_ is None:
        # When called for the first time, will copy src to build, and execute 'prepare' script if any
        defaultSuite_ = TRUSTSuite()

def reset():
    """ Wipe out build directory completly and reset default suite.
    """
    opt = os.environ.get("JUPYTER_RUN_OPTIONS", None)
    # Very specific to the validation process - we need to keep build there:
    if not opt is None and "-not_run" in opt:
        return

    import shutil

    global defaultSuite_
    defaultSuite_ = None
    if os.path.exists(BUILD_DIRECTORY):
        shutil.rmtree(BUILD_DIRECTORY)


def getCases():
    global defaultSuite_
    if defaultSuite_ is None:
        return []
    return defaultSuite_.getCases()

def executeScript(scriptName, verbose=False):
    """ Execute a script shell in the BUILD_DIRECTORY
    """
    global defaultSuite_
    
    opt = os.environ.get("JUPYTER_RUN_OPTIONS", None)
    if not opt is None and "-not_run" in opt:
        return []
    if defaultSuite_ is None:
        print("ERROR: defaultSuite_ is not initialized. Please call initCaseSuite() first.")
        return []
    opt = os.environ.get("JUPYTER_RUN_OPTIONS", None)
    # No additional modification of the results file with new script
    if not opt is None and "-not_run" in opt:
        return []
    return defaultSuite_.executeScript(scriptName, verbose)

def executeCommand(cmd, verbose=False):
    """ Execute a bash command in the BUILD_DIRECTORY
    """
    global defaultSuite_
    if defaultSuite_ is None:
        print("ERROR: defaultSuite_ is not initialized. Please call initCaseSuite() first.")
        return []
    opt = os.environ.get("JUPYTER_RUN_OPTIONS", None)
    # No additional modification of the results file with new script
    if not opt is None and "-not_run" in opt:
        return []
    return defaultSuite_.executeCommand(cmd, verbose)


def printCases():
    text = "### Test cases \n"
    for c in getCases():
        text += "* " + c.dir_ + "/" + c.name_ + ".data : \n"
    displayMD(text)


def extractNRCases():
    """ Prints out the list of cases in a suitable format for processing by validation and lance_test
    tools.
    WARNING: do not modify this without looking at scripts get_list_cas_nr and get_nb_cas_nr in
       Validation/Outils/Genere_Courbe/scripts
    """
    import numpy as np

    list_exclu_nr = []
    if os.path.exists("src/liste_cas_exclu_nr"):
        list_cases = np.loadtxt("src/liste_cas_exclu_nr", dtype=str)
        list_exclu_nr = list(map(lambda a: os.path.normpath(a), list_cases))

    for c in getCases():
        if c.dir_ != ".":
            t = os.path.join(c.dir_, c.name_ + ".data")
            # t = c.dir_ + "/" + c.name_ + ".data"
        else:
            t = c.name_ + ".data"
        t = os.path.normpath(t)
        if not t in list_exclu_nr:
            print("@@@CAS_NR_JY@@@ " + t)


def runCases(verbose=False, preventConcurrent=False):
    """ Launch all TRUST cases for the current validation form.
    @param verbose: bool : whether to print the console output of the run.
    @param preventConcurrent: run the cases in the order they were provided, even if the Sserver is up and running, and the -parallel_sjob option
    was passed.
    """
    global defaultSuite_
    if defaultSuite_ is None:
        raise Exception("No test cases currently recorded! Call 'addCase' first ...")

    defaultSuite_.runCases(verbose, preventConcurrent)


def tablePerf():
    """ Prints the table of performance
    """
    from . import plot

    columns = ["host", "system", "Total CPU Time", "CPU time/step", "number of cells"]
    zeTable = plot.Table(columns)
    for case in getCases():
        try:
            case.addPerfToTable(zeTable)
        except Exception as e:
            raise e

    zeTable.sum("Total CPU Time")

    return zeTable.df


def extractHistogram(domain, Out, file):
    """
    Extract the histogram.

    @param domain : str : domain name
    @param out    : str: Output.
    @param file   :str : file in which we save the data.
    """
    raise NotImplementedError  # TODO TODO review the code for this one ...

    os.chdir(BUILD_DIRECTORY)

    tmp = ""
    flag = False
    document = open(Out, "r")
    lines = document.readlines()
    text = "Histogram of the largest angle of each element found into the mesh %s :\n" % (domain)

    for i in lines:
        if flag:
            tmp = tmp + i
        if i == text:
            flag = True
        if i == "\n":
            flag = False

    if os.path.exists(file):
        os.remove(file)

    f = open(file, "a")
    f.write(tmp)
    f.close()

    os.chdir(ORIGIN_DIRECTORY)
