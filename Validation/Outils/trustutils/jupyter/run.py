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
    arch = os.environ["TRUST_ARCH"]
    sub = "%s_opt/lib/python%d.%d/site-packages" % (arch, sys.version_info.major, sys.version_info.minor)
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
    # add universal_newlines as in https://stackoverflow.com/questions/41171791/how-to-suppress-or-capture-the-output-of-subprocess-run
    complProc = subprocess.run(cmd, shell=True, executable="/bin/bash", stdout=subprocess.PIPE, stderr=subprocess.STDOUT,universal_newlines = True)
    if verbose:
        print(cmd)
        print(complProc.stdout)
    # Throw if return code non-zero:
    if complProc.returncode != 0 and complProc.stdout:
        # Display message through a custom exception so that jupyter-nbconvert also shows it properly in the console:
        msg = "\nExecution of following command failed!!\n"
        msg += "  " + cmd
        msg += "\nwith return code %d\n" % complProc.returncode
        msg += "and with following output:\n\n"
        msg += (complProc.stdout if complProc.stdout else "")
        raise RuntimeError(msg)

######## End PRIVATE STUFF #########

class TRUSTCase(object):
    """ 
    Class which allows the user to load and execute a validation case
    """

    _UNIQ_ID_START = -1

    def __init__(self, directory, datasetName, nbProcs=1, execOptions="", record=False):
        """ 
        Initialisation of the class

        Parameters
        ---------

        directory: str 
            Path of the file
        datasetName: str
            Path of the case we want to run, relatively to the 'src' folder.
        nbProcs : int
            Number of processors to use to run the case.
        record : bool 
            whether to add the case to the global list of cases to run
        execOptions : str
            TRUST Options to add at the execution of the test case 

        Returns
        ------

        """
        TRUSTCase._UNIQ_ID_START += 1
        self.id_ = TRUSTCase._UNIQ_ID_START  # Unique ID
        self.dir_ = os.path.normpath(directory)
        self.name_ = datasetName.split(".data")[0]  # always w/o the trailing .data
        self.dataFileName_ = self.name_ #Change in case of parallel case
        self.nbProcs_ = nbProcs
        self.last_run_ok_ = -255  # exit status of the last run of the case
        self.last_run_err_ = ""  # error message returned when last running the case
        self.execOptions = execOptions
        if record:
            self._ListCases.append(self)

    def _fullDir(self):
        """
        full directory of the test case
        """
        return os.path.normpath(os.path.join(BUILD_DIRECTORY, self.dir_))

    def _fullPath(self):
        """
        full path of the test case in the build directory
        """
        fullPath = os.path.join(self._fullDir(), self.name_)
        return fullPath + ".data"

    def substitute(self, find, replace):
        """ 
        Substitute (in place and in the build directory) a part of the dataset

        Parameters
        ---------

        find: str 
            Text we want to substitute.
        replace: str
            Text to insert in replacement.

        """
        path = self._fullPath()
        text_out = ""
        with open(path, "r") as f:
            text_in = f.read()
            text_out += text_in.replace(find, replace)

        with open(path, "w") as f:
            f.write(text_out)

    def substitute_template(self, subs_dict):
        """ 
        Substitute (in place and in the build directory) a part of the dataset

        Parameters
        --------

        subs_dict: dict 
            Text we want to substitute with python Template formalism (character identified with $ in datafile)
        """
        path = self._fullPath()
        with open(path, "r") as file: filedata = Template(file.read())
        result = filedata.substitute(subs_dict)
        with open(path, "w") as file: file.write(result)

    def copy(self, targetName, targetDirectory=None, nbProcs=1, execOptions=""):
        """ 
            Copy a TRUST Case

        Parameters
        ---------

        targetName: str
            New name
        nbProcs: int
            number of procs

        """
        if targetDirectory is None:
            targetDirectory = self.dir_
        # Create the directory if not there:
        fullDir2 = os.path.join(BUILD_DIRECTORY, targetDirectory)
        if not os.path.exists(fullDir2):
            os.makedirs(fullDir2, exist_ok=True)
        pthTgt = os.path.join(BUILD_DIRECTORY, targetDirectory, targetName)
        # And copy the .data file:TRUSTSuite
        from shutil import copyfile

        copyfile(self._fullPath(), pthTgt)

        return TRUSTCase(targetDirectory, targetName, nbProcs=nbProcs, execOptions=execOptions, record=False)

    def dumpDataset(self, user_keywords=[]):
        """ 
        Print out the .data file. TODO handle upper / lower case

        Parameters
        --------
        
        list_Trust_user_words: list(str) 
            List of word the user wants to color in red.
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
        list_Trust_keywords = tmp
        f.close()

        ### Import and Underline important words of the data file  ###
        f = open(self._fullPath(), "r")
        tmp = f.readlines()
        test = []
        comment_flag = False
        for j in tmp:
            flag = 0
            if j.count("#") == 1 and  not comment_flag:
                comment_flag = True
                flag = 1
                j = "\033[38;5;88m" + j + "\033[0;0m"
            elif j.count("#") == 1 and  comment_flag :
                comment_flag = False
                j = "\033[38;5;88m" + j + "\033[0;0m"
                flag = 1
            elif j.count("#") == 2 or comment_flag:
                j = "\033[38;5;88m" + j + "\033[0;0m"
                flag = 1


            if "end" in j.lower():
                flag = 1
                j = "\033[38;5;88m" + j + "\033[0;0m"
            j = " " + j
            j = j.replace("\t", "\t ")
            if flag == 0:
                j = j.replace("{", "\033[196;5;30m{\033[0;0m") # en gras
                j = j.replace("}", "\033[196;5;30m}\033[0;0m") # en gras

                for i in list_Trust_keywords:
                    j = j.replace(i, "\033[38;5;28m" + i + "\033[0;0m")

                for i in list_Trust_classe:
                    j = j.replace(i, "\033[38;5;4m" + i + "\033[0;0m")

                for i in user_keywords:
                    # tmp=tmp.replace(i, "\033[196;5;30m"+i+"\033[0;0m ") # en gras
                    j = j.replace(i, "\033[38;5;196m" + i + "\033[0;0m")  # en rouge
            test.append(j)
        f.close()

        ### Print the coloured .data file ###
        print("".join(test))

    def dumpDatasetMD(self):
        """
        Print out the .data file in a basic MarkDown format.
        This will not highlight TRUST keywords.
        """

        f = open(self._fullPath(), "r").read()
        displayMD( "```\n" + f + "\n```" )

    def _runScript(self, scriptName, verbose=False):
        """ Internal. Run a shell script if it exists.
        """
        pth = "./%s" % scriptName
        if os.path.exists(pth):
            subprocess.check_output("chmod u+x %s" % pth, shell=True)
            cmd = "%s %s" % (pth, self.name_)
            _runCommand(cmd, verbose)

    def _preRun(self, verbose):
        self._runScript("pre_run", verbose)

    def _postRun(self, verbose):
        self._runScript("post_run", verbose)

    def _generateExecScript(self):
        """ Generate a shell script doing the
            - pre_run
            - launching the case
            - and doing hte post_run
        """
        uniq_id = "{:04d}".format(self.id_)
        scriptFl = os.path.join(BUILD_DIRECTORY, "cmds_%s.sh" % uniq_id)

        logName = "cmds_%s.log" % uniq_id
        n, d, e = self.dataFileName_, self.dir_, self.execOptions
        fullD, fullL = os.path.join(BUILD_DIRECTORY, d), os.path.join(BUILD_DIRECTORY, logName)
        para = ""
        if self.nbProcs_ != 1:
            para = str(self.nbProcs_)
        with open(scriptFl, "w") as f:
            s = "#!/bin/bash\n"
            s += "( echo;\n"
            s += '  echo "-> Running the calculation of the %s data file in the %s directory ...";\n' % (n, d)
            s += "  cd %s ; \n" % fullD
            # Running and checking pre_run was OK:
            s += "  if [ -f pre_run ]; then\n"
            s += "     chmod +x pre_run\n"
            s +=f'     echo "-> Running the pre_run script in the {d} directory ..."\n'
            s +=f"     (./pre_run {n} || (echo '  FAILED!' && exit -1)) || exit -1 \n"
            s += "fi\n"

            # Running case
            s += "  trust %s %s %s 1>%s.out 2>%s.err;\n" % (n, para, e, n, n)
            s += "  if [ ! $? -eq 0 ]; then exit -1; fi; \n"

            # Running and checking post_run was OK:
            s += "  if [ -f post_run ]; then\n"
            s += "     chmod +x post_run\n"
            s +=f'     echo "-> Running the post_run script in the {d} directory ..."\n'
            s +=f"     (./post_run {n} || (echo '  FAILED!' && exit -1)) || exit -1\n"
            s += "fi\n"

            s += "  exit 0;"
            s += ") 1>%s 2>&1 \n" % fullL
            f.write(s)
        os.chmod(scriptFl, 0o755)
        return scriptFl, fullL

    def partition(self, verbose=False):
        """ 
        Apply partitioning of specified test case with trust -partition if nbProcs>1 only
        This avoids to modify trust -partition to be able to call it with 1 cpu

        Parameters
        ---------
        verbose: bool
        
        """
        ok = True
        path = os.getcwd()
        os.chdir(self._fullDir())
        opt = os.environ.get("JUPYTER_RUN_OPTIONS", "")
        self.dataFileName_ = "PAR_"+self.name_
        if "-not_run" in opt:
            return
        if self.nbProcs_ == 1:
            err_msg = "Not allowed to call case.partition() on case = addCase(%s,..., nbProcs=1) \n" % (self.name_)
            err_msg += "You cannot run parallel computation on 1 proc!"
            raise ValueError(err_msg)

        err_file = self.name_ + "_partition.err"
        out_file = self.name_ + "_partition.out"
        cmd = "trust -partition %s %s 2>%s 1>%s" % (self.name_, str(self.nbProcs_), err_file, out_file)
        output = subprocess.run(cmd, shell=True, executable="/bin/bash", stderr=subprocess.STDOUT)
        if verbose:
            print(cmd)
        if output.returncode != 0:
            ok = False
            err = getLastLines_(err_file)

        os.chdir(path)

        baseName = os.path.join(self.dir_, self.dataFileName_)

        saveFileAccumulator(f"{baseName}.dt_ev")
        saveFileAccumulator(f"{baseName}.newton_evol")
        saveFileAccumulator(f"{baseName}.data")
        saveFileAccumulator(f"{baseName}.out")
        saveFileAccumulator(f"{baseName}_*.out")
        saveFileAccumulator(f"{baseName}.err")
        saveFileAccumulator(f"{baseName}_*.son")
        saveFileAccumulator(f"{baseName}.TU")
        saveFileAccumulator(f"{baseName}_csv.TU")

        return ok

    def runCase(self, verbose=False):
        """ 
        Move to the case directory and execute the current test case:
        - calls pre_run if any
        - run the case
        - calls post_run if any

        Parameters
        ---------
        verbose: bool
        
        The results of the run are stored in members self.last_run_ok_ and self.last_run_err_
        """
        ok, err = True, ""
        os.chdir(self._fullDir())

        ### Run pre_run ###
        self._preRun(verbose)

        ### Run Case ###
        err_file = self.dataFileName_ + ".err"
        out_file = self.dataFileName_ + ".out"
        para = ""
        if self.nbProcs_ != 1:
            para = str(self.nbProcs_)
        cmd = "trust %s %s %s 2>%s 1>%s" % (self.dataFileName_, para, self.execOptions, err_file, out_file)
        output = subprocess.run(cmd, shell=True, executable="/bin/bash", stderr=subprocess.STDOUT)
        if verbose:
            print(cmd)
            print(output.stdout)
        if output.returncode != 0:
            ok = False
            err = getLastLines_(self.dataFileName_ + ".err")

        ### Run post_run ###
        if ok:
            self._postRun(verbose)

        ### Return to initial directory ###
        os.chdir(ORIGIN_DIRECTORY)
        self.last_run_ok_, self.last_run_err_ = ok, err
        return ok, err

    def _addPerfToTable(self, zeTable):
        """ Extract performances for this case and add it to the global table
            passed in parameter.
        """
        os.chdir(self._fullDir())

        opt = os.environ.get("JUPYTER_RUN_OPTIONS", None)
        # Very specific to the validation process - we need to keep build there:
        if not opt is None and "-not_run" in opt:
            cmd = '.'
        else:
            cmd = os.environ["TRUST_ROOT"] + "/Validation/Outils/Genere_courbe/scripts/extract_perf " + self.dataFileName_
            _runCommand(cmd, False)

        f = open(self.dataFileName_ + ".perf", "r")
        row = f.readlines()[0].replace("\n", "").split(" ")[1:]
        f.close()

        if len(row) == 6:
            row_arrange = row[0:5]
            row_arrange[4] = str(row[4])
            if (row[5] != row[4]):
                row_arrange[4] += "-" + str(row[5])
            row = row_arrange

        zeTable.addLine([row[:5]], self.dir_ + "/" + self.dataFileName_)
        os.chdir(ORIGIN_DIRECTORY)

        ## Save the file
        saveFileAccumulator(self.dir_ + "/" + self.dataFileName_ + ".perf")


class TRUSTSuite(object):
    """ A set of TRUST cases to be run for the validation form.
    """
    def __init__(self, runPrepare=True):
        self.cases_ = []
        os.chdir(ORIGIN_DIRECTORY)
        self.copySrc()
        if runPrepare:
            executeScript("prepare")

    def copySrc(self):
        """ Copy content of src directory into build directory.
        WARNING: this is where we expect to be at the root of the validation form.
        """
        if not os.path.exists("src"):
            raise Exception("Not a coherent validation form directory: 'src' subdirectory not found.")
        # Mimick what is done in 'prepare_gen' TRUST script:
        subprocess.run("mkdir -p %s && cp -a src/* %s 2>/dev/null" % (BUILD_DIRECTORY, BUILD_DIRECTORY), shell=True)  # Note the '*' !!
        # Add image file that can be in src file for archiving
        os.chdir(BUILD_DIRECTORY)
        import pathlib
        desktop = pathlib.Path("./")
        image_files = list(desktop.rglob("*.png"))
        image_files.append(list(desktop.rglob("*.jpg")))
        image_files.append(list(desktop.rglob("*.jpeg")))
        for image in image_files:
            saveFileAccumulator(str(image))

    def addCase(self, case):
        self.cases_.append(case)

        baseName = os.path.join(case.dir_, case.name_)

        saveFileAccumulator(f"{baseName}.dt_ev")
        saveFileAccumulator(f"{baseName}.newton_evol")
        saveFileAccumulator(f"{baseName}.data")
        saveFileAccumulator(f"{baseName}.out")
        saveFileAccumulator(f"{baseName}_*.out")
        saveFileAccumulator(f"{baseName}.err")
        saveFileAccumulator(f"{baseName}_*.son")
        saveFileAccumulator(f"{baseName}.TU")
        saveFileAccumulator(f"{baseName}_csv.TU")

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
        
        Parameters
        ---------
        verbose: bool 
            whether to print the console output of the run.
        preventConcurrent: bool 
            run the cases in the order they were provided, even if the Sserver is up and running, and the -parallel_sjob option was passed.
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
        if "-parallel_run" in opt:
            runParallel = not preventConcurrent
        else:
            runParallel = not preventConcurrent and ((opt == "" or "-parallel_sjob" in opt.split(" ")) and self.detectSserver())
            extra = {True: "**with Sserver**", False: ""}[runParallel]
            print("Running %s..." % extra)

        from time import time, sleep

        t0 = time()
        stream = os.popen("echo Returned output")
        if verbose:
            print(stream.read())

        ### Change the root to build file ###

        allOK = True
        lstC = self.getCases()

        th_lst, log_lst = [], []
        err_msg = "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        err_msg += "Case '%s/%s.data' FAILED !! Here are the last 20 lines of the log file:\n"
        err_msg += "(If you don't see anything suspicious, also check pre/post_run scripts!!)\n"
        err_msg += "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"

        if runParallel:
            salloc = os.path.join(os.environ["TRUST_ROOT"], "bin", "Sjob", "Salloc")
            for case in lstC:
                (script, logFile,) = case._generateExecScript()  # Generate the shell script doing pre_run, case and post_run
                log_lst.append(logFile)
                if "-parallel_run" in opt:
                    host = os.environ.get("TRUST_WITHOUT_HOST","")
                    nb_procs = int(os.environ.get("TRUST_NB_PROCS",""))
                    # On personnal computer, limit the number of process
                    while (host=="1"):
                        current_number = 0
                        for th in th_lst:
                            if th.poll() is None:
                                current_number += case.nbProcs_
                        if (current_number < nb_procs):
                            break
                        if (case.nbProcs_ > nb_procs):
                            print("Insufficient number of processors to perform parallel calculations.\n Increase TRUST_NB_PROCS")
                            break
                        sleep(1)
                    cmdLst = ["bash", script, "&"]
                else:
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
                    case.runCase(verbose=verbose)
                    allOK = allOK and case.last_run_ok_
                    if not allOK:
                        print(err_msg % (case.dir_, case.name_))
                        print(case.last_run_err_)
                        raise ValueError ("at least one case has failed ! open notebook to get more information ")

                except Exception as e:
                    os.chdir(ORIGIN_DIRECTORY)  # Restore initial directory
                    raise e
        t1 = time()
        if allOK:
            dt = t1 - t0
            # Validation process - we do not output variable information:
            if os.environ.get("JUPYTER_IS_VALIDATION", False):
                dt = 0
                # Extract total time for all the runs of datasets
                tr = os.environ.get("TRUST_ROOT", "")
                gtt = os.path.join(tr, "Validation", "Outils", "Genere_courbe", "scripts", "get_total_time")
                l = [c._fullPath() for c in lstC]
                cmd = gtt + " " + " ".join(l)
                os.chdir(BUILD_DIRECTORY)
                _runCommand(cmd, False)
                os.chdir(ORIGIN_DIRECTORY)

            print("  => A total of %d cases were (successfully) run in %.1fs." % (len(lstC), dt))

    def tablePerf(self):
        """ Prints the table of performance
        """
        from . import plot
        columns = ["host", "system", "Total CPU Time", "CPU time/step", "number of cells"]
        zeTable = plot.Table(columns)
        for case in self.getCases():
            try:
                case._addPerfToTable(zeTable)
            except Exception as e:
                raise e
        zeTable.sum("Total CPU Time")
        return zeTable.df

    def extractNRCases(self):
        """
        Prints out the list of cases in a suitable format for processing by validation and lance_test tools.

        WARNING:
        do not modify this without looking at scripts get_list_cas_nr and get_nb_cas_nr in Validation/Outils/Genere_Courbe/scripts
        """
        import numpy as np

        list_exclu_nr = []
        if os.path.exists("src/liste_cas_exclu_nr"):
            list_cases = np.loadtxt("src/liste_cas_exclu_nr", dtype=str)
            list_exclu_nr = list(map(lambda a: os.path.normpath(a), list_cases))

        for c in self.getCases():
            if c.dir_ != ".":
                t = os.path.join(c.dir_, c.name_ + ".data")
                # t = c.dir_ + "/" + c.name_ + ".data"
            else:
                t = c.name_ + ".data"
            t = os.path.normpath(t)
            if not t in list_exclu_nr:
                print("@@@CAS_NR_JY@@@ " + t)

    def printCases(self):
        """
        display testCases
        """
        text = "### Test cases \n"
        for c in self.getCases():
            text += "* " + c.dir_ + "/" + c.dataFileName_ + ".data "
            if (int(c.nbProcs_) > 1):
                text += "with " + str(c.nbProcs_)  + " procs"
            text += "\n"
        displayMD(text)

def saveFileAccumulator(data):
    """ Method for saving files for the Non Regression test

    Parameters
    ---------
    data: str
        name of the file we want to save.
    """
    from .filelist import FileAccumulator

    path = os.getcwd()
    os.chdir(BUILD_DIRECTORY)

    FileAccumulator.active = True
    new = FileAccumulator.Append(data)
    if new==True:
        FileAccumulator.WriteToFile("used_files")

    os.chdir(path)

def introduction(auteur, creationDate=None):
    """ Function that creates an introduction cell Mardown

    Parameters
    ---------

    author: str 
        Name of the author of the test case.
    creationDate: date 
        format dd/mm/YYYY 
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


def TRUST_parameters(version="", param=[]):
    """ Function that creates a cell Mardown giving TRUST parameters (version, binary)
    
    Parameters
    ----------

    version: str 
        version of TRUST - if void TRUST_VERSION read
    param: list(str) 
        List of Parameter used in this test case
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
    """ 
    Print out the .data file.

    Parameters
    ---------
    fiche: str 
        Path of the file
    list_Trust_user_words: list(str) 
        List of word the user wants to color in red.
    """
    c = TRUSTCase(directory=BUILD_DIRECTORY, datasetName=fiche)
    c.dumpDataset(list_Trust_user_words)


def dumpDatasetMD(data):
    """
    Print the .data file in MD format.

    Parameters
    ---------
    datafile: str
        relative path (to build directory) + name of datafile
    """

    c = TRUSTCase(directory=BUILD_DIRECTORY, datasetName=data)
    c.dumpDatasetMD()


def dumpText(fiche, list_keywords=[]):
    """ Print out the file.

    Parameters
    --------
    fiche: str 
        Path of the file
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

def addCaseFromTemplate(templateData, targetDirectory, dic, nbProcs=1, targetData=None, execOptions=""):
    """ Add a case to run to the list of globally recorded cases.
    
    Parameters
    ----------
    targetDirectory: str 
        targetDirectory where the case is stored (relative to build/)
    templateData: str
        Name of the template datafile
    dic: dictionary
        substitution of term (key identified with $ in datafile) by value in a new data file
    targetData : str
        if provided, templateData will be copied as targetData + dictionary applied + added to TRUSTSuite()
    nbProcs : int 
        Number of processors

    Returns
    -------
    a new TRUSTcase instance we want to launch.
    """
    global defaultSuite_
    if defaultSuite_ is None:
        # When called for the first time, will copy src to build, and execute 'prepare' script if any
        defaultSuite_ = TRUSTSuite()

    if targetData is None:
        targetData = templateData
    else:
        if targetData[-5:] != ".data":
            raise ValueError("targetData should ends with .data in addCaseFromTemplate!!!")

    fullDir = os.path.join(BUILD_DIRECTORY, templateData)
    fullDir2 = os.path.join(BUILD_DIRECTORY, targetDirectory)
    if not os.path.exists(fullDir2):
        os.makedirs(fullDir2, exist_ok=True)
    pthTgt = os.path.join(BUILD_DIRECTORY, targetDirectory, targetData)
    # And copy the .data file:
    from shutil import copyfile

    copyfile(fullDir, pthTgt)
    tc = addCase(targetDirectory, targetData, nbProcs, execOptions)
    tc.substitute_template(dic)
    return tc


def addCase(directoryOrTRUSTCase, datasetName="", nbProcs=1, execOptions=""):
    """ 
    Add a case to run to the list of globally recorded cases.

    Parameters
    ---------

    directoryOrTRUSTCase: str 
        directory where the case is stored (relative to build/)
    datasetName: str 
        Name of the case we want to run.
    nbProcs : int 
        Number of processors
    execOptions : str
        TRUST Options to add at the execution of the test case 

    Returns
    -------
    TRUSTcase instance we want to launch.
    """
    global defaultSuite_
    if isinstance(directoryOrTRUSTCase, TRUSTCase):
        if datasetName != "":
            raise ValueError("addCase() method can either be called with a single argument (a TRUSTCase object) or with at least 2 arguments (directory and case name)")
        tc = directoryOrTRUSTCase
        initCaseSuite()
        defaultSuite_.addCase(tc)
        return
    elif isinstance(directoryOrTRUSTCase, str):
        if datasetName == "":
            raise ValueError("addCase() method can either be called with a single argument (a TRUSTCase object) or with at least 2 arguments (directory and case name)")
        tc = TRUSTCase(directoryOrTRUSTCase, datasetName, nbProcs,execOptions=execOptions)
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
    """ 
    Wipe out build directory completly and reset default suite.
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

def executeScript(scriptName, verbose=False, nonRegression=False):
    """ 
    Execute a script shell in the BUILD_DIRECTORY

    Parameters
    ----------

    scriptName: str
        Name of the exec script
    verbose: bool
    nonRegression: bool
        by default executeScript is inactive when option -not_run is applied (for non regression test)
    """
    opt = os.environ.get("JUPYTER_RUN_OPTIONS", "")
    # Very specific to the validation process. Sometimes we want the core
    # method 'runCases()' not to do anything ... see script 'archive_resultat' for example.
    if "-not_run" in opt and not nonRegression :
        return

    os.chdir(BUILD_DIRECTORY)
    pth = "./" + scriptName
    if os.path.exists(pth):
        cmd = pth
        subprocess.check_output("chmod u+x %s" % pth, shell=True)
        _runCommand(cmd, verbose)
    os.chdir(ORIGIN_DIRECTORY)

def executeCommand(cmd, verbose=False, nonRegression=False):
    """ Execute a bash command in the BUILD_DIRECTORY
    Parameters
    ----------

    cmd: command to execute
    verbose: bool
    nonRegression: bool
    """
    opt = os.environ.get("JUPYTER_RUN_OPTIONS", "")
    # Very specific to the validation process. Sometimes we want the core
    # method 'runCases()' not to do anything ... see script 'archive_resultat' for example.
    if "-not_run" in opt and not nonRegression :
        return
    os.chdir(BUILD_DIRECTORY)
    _runCommand(cmd, verbose)
    os.chdir(ORIGIN_DIRECTORY)


def printCases():
    """
    display testCases
    """
    global defaultSuite_
    return defaultSuite_.printCases()


def extractNRCases():
    """
    Prints out the list of cases in a suitable format for processing by validation and lance_test tools.

    WARNING:
    do not modify this without looking at scripts get_list_cas_nr and get_nb_cas_nr in Validation/Outils/Genere_Courbe/scripts
    """
    global defaultSuite_
    return defaultSuite_.extractNRCases()


def runCases(verbose=False, preventConcurrent=False):
    """ Launch all TRUST cases for the current validation form.

    Parameters
    ---------
    verbose: bool 
        whether to print the console output of the run.
    preventConcurrent: bool
        run the cases in the order they were provided, even if the Sserver is up and running, and the -parallel_sjob option was passed.
    """
    global defaultSuite_
    if defaultSuite_ is None:
        raise Exception("No test cases currently recorded! Call 'addCase' first ...")

    defaultSuite_.runCases(verbose, preventConcurrent)


def tablePerf():
    """ Prints the table of performance
    """
    global defaultSuite_
    return defaultSuite_.tablePerf()

def initBuildDirectory():
    """ triggers build directory creation and copy src stuff into it
    """
    global defaultSuite_
    defaultSuite_ = TRUSTSuite()
