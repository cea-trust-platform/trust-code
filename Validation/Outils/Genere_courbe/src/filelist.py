from glob import glob

class FileAccumulator(object):
    """ Static accumulator listing all external files used by the PRM.
    This is then used to generate the archive of the validation form.
    """
    file_list = set()
    active = False        # Whether the accumulation functionalities are turned on or off.
    last_LATA_used = ""

    @classmethod
    def _ParseLataDB(cls, file, dom, var, loc, cycles):
        """ Extract subfile from a LATA database. Take the last time step.
        var or loc might be None in which case the last field found will be returned (useful for plane
        probes)
        @param file LATA file name
        @param dom domain name
        @param var variable name to process (PRESSURE, etc ...)
        @param loc one of the valid LATA discretisation: SOM, ELEM
        @param cycles list of integers describing times to take
        @return a list of files to save for archiving purposes
        """
#         print("****  DGB: called with %s / %s / %s / %s" % (str(dom), str(var),str(loc), str(cycles)))
        # TODO: this method is called several times if there are several visu directives in a Visu block
        # this should be improved ...
        with open(file) as f:
            ll = f.readlines()
            # There can be several match for one variable (e.g. FrontTracking lata files do not properly differentiate
            # VITESSE on ELEM or on SOM). We potentially save several file references for one time, one variable.
            records, fNames, init = [], [], True
            for l in ll:
                a = l.split()
                # Start really parsing lines when the first TEMPS is encountered:
                if a[0] in ["TEMPS", "FIN"]:
                    if not init:
                        records.append(fNames)
                        fNames = []
                    init = False
                    if a[0] == "FIN": break
                if len(a) >= 3 and a[0].upper() == "CHAMP":
                    # Parse the rest of the line (geometrie= ... localisation= ...)
                    attr = {}
                    for aa in a[3:]:
                        sp = aa.split("=")
                        if len(sp) == 2: attr[sp[0]] = sp[1]
                    # At least the variable name has to match (or has to be found in "noms_compo")
                    varOk = False
                    if var is None or cls._VariableMatch(var, a[1]):
                        varOk = True
#                     elif "noms_compo" in attr.keys() and not (loc is None or dom is None):
#                         compoNames = [c.upper() for c in attr["noms_compo"].split(",")]
#                         fullCompo = "%s_%s_%s" % (var, loc, dom)
#                         varOk = fullCompo.upper() in compoNames
                    # Localisation and domain are condsidered matching if:
                    #   either the parameter passed to the function is None
                    #   or the attribute is not present in the file
                    #   or both parameter and attribute are set and they are equal
                    if varOk:
                        if dom is None or attr.get("geometrie", dom) == dom:
                            if loc is None or attr.get("localisation", loc).upper() == loc.upper():
                                fNames.append(a[2])
            if a[0] != "FIN":
                records.append(fNames)
                print("WARNING: Expected keyword FIN at end of file %s -- found '%s'" % (file, a[0]))
                 #raise Exception("Expected keyword FIN at end of file %s -- found '%s'" % (file, a[0]))
            # Extract relevant cycles and flatten list of lists:
            ret = []
            for c in cycles:
                r = records[c]
                if len(r) == 0:
                    raise ValueError("Variable %s for domain %s at loc %s (cycle %d) not found in %s" %(var, str(dom), str(loc), c, file))
                ret.extend(r)
            return ret

    @classmethod
    def _GetRealDomName(cls, dom):
        """ Pre-process the domain name:
          * my_domain_dual -> my_domain
          * dom/Inlet  -> dom_Inlet
        """
        dom = dom.replace("/", "_")
        a = dom.split("_")
        if a[-1] in ["dual", "centerfaces"]:  # specific keywords that can be appended to the domain name
            dom = "_".join(a[:-1])
        return dom

    @classmethod
    def _VariableMatch(cls, varName, token):
        """ For a given variable name (e.g. VITESSE) check whether the 'Champ' name found in the LATA file matches
        Sometimes we ask for VITESSE_SOLUTION, and the LATA contains component fields like VITESSE_SOLUTION_X, VITESSE_SOLUTION_Y
        We have to handle those as well ...
        """
        if varName.upper() == token.upper():
            return True
        for ext in ["_X", "_Y", "_Z"]:
            if varName.upper() + ext == token.upper():
                return True
        return False

    @classmethod
    def ParseDirectName(cls, name):
        """ From PRESSION_PA_ELEM_dom2_global_dual  return ('PRESSION_PA', 'ELEM', 'dom2_global') 
        And from ERROR_ELEM_ELEM_DOM returns ('ERROR_ELEM', 'ELEM', 'DOM')
        """
        a = name.split("_")
        if a[-1] == "dual": a = a[:-1]
        # Find localisation:
        idx = -1
        reva = a[:]; reva.reverse()
        for loc in ["ELEM", "SOM", "FACES"]:
            try:
                idx = reva.index(loc)  # take last loc to accept var name such as ERROR_ELEM
                idx = len(a)-1-idx
                break
            except: pass
        if idx == -1:
            raise ValueError("Invalid field name: %s" % name)
        loc = a[idx]
        var, dom = "_".join(a[:idx]), "_".join(a[idx+1:])
        return var, loc, dom

    @classmethod
    def GetList(cls):
        l = list(cls.file_list)
        l.sort()
        return l

    @classmethod
    def Append(cls, file):
        if not cls.active: return
        cls.file_list.add(file)

    @classmethod
    def AppendVisuMesh(cls, file, dom_name):
        """ Handle 'mesh' option of 'visu { ... }' block
        """
#         print("*** DBG AppendVisuMesh: %s / %s" % (file, dom_name))
        if not cls.active: return
        cls.Append(file)
        tab = file.split(".")
        ext = tab[-1]
        if ext == "case":
            noExt = ".".join(tab[:-1])
            cls.Append(noExt + ".geo")
        if ext == "lata" and dom_name is not None:
            cls.last_LATA_used = file
            dom_name = cls._GetRealDomName(dom_name)
            cls.Append(file + "." + dom_name)
            # By default take all timesteps for the domain (simplify things for ALE):
            cls.Append(file + "." + dom_name + ".*")
            # Extract first "Geometrie" from LATA - this domain is always needed by VisIt for example in case of Front Tracking
            direc = "/".join(file.split("/")[:-1])
            with open(file) as f:
                for l in f.readlines():
                    if l.startswith("Geometrie "):
                        s = [st.strip() for st in l.split(" ") if st != ""]
                        base_file, base_dom = direc + "/" + s[-1], s[-2]
                        cls.AppendVisuMesh(base_file, base_dom)
                        break
            # TODO: the 2 items below only seen on InwardField so far (??)
            faceF = file + ".FACES.FACES." + dom_name + ".*"
            if len(glob(faceF)):       cls.Append(faceF)
            faceF = file + ".ELEM_FACES.ELEMENTS." + dom_name + ".*"
            if len(glob(faceF)):       cls.Append(faceF)
            # Joint files - never too big (?)
            jointFiles = file + ".JOINTS_*.." + dom_name + ".*"
            if len(glob(jointFiles)):  cls.Append(jointFiles)
            jointFiles2 = file + ".decal_*." + dom_name    # Seen only on FT (Sloshing ...)
            if len(glob(jointFiles2)):  cls.Append(jointFiles2)
            # Interface files for Front Tracking! Need all of them(?)
            interfFiles = file + ".INTERFACES.*"
            if len(glob(interfFiles)):  cls.Append(interfFiles)

    @classmethod
    def AppendVisuComplex(cls, file, dom, var, loc, cycles=""):
        """ Handle all other visu types in a 'visu { ... }' block (pseudocolor, etc ...)
        Last postprocessing time of the LATA database for the given field is added:
        @param file LATA file name
        @param dom domain name
        @param var variable name to process (PRESSURE, etc ...)
        @param loc one of the valid LATA discretisation: SOM, ELEM
        @param cycles string containing a list of integers describing times to take (defaut to empty means last time)
        """
#         print("****  DGB0: called with %s %s %s" % (str(dom), str(var),str(loc)))
        if not cls.active: return
        # Append domain first:
        dom = cls._GetRealDomName(dom)
        cls.AppendVisuMesh(file, dom)
        tab = file.split(".")
        ext = tab[-1]
        # The CASE case ... not very important.
        if ext == "case":
            noExt = ".".join(tab[:-1])
            fName = noExt + "*.%s_%s_%s" % (var.upper(), loc.upper(), dom)
            cls.Append(fName)
        # Then append relevant element of the LATA database:
        if ext == "lata":
            # Extract base directory for all other subfiles that will be read from LATA db:
            drs = file.split("/")
            baseDir = "."
            if not var is None:
                var = var.upper()     # Variable name is always upper case in LATA file name and records
            if len(drs) > 1:
                baseDir = "/".join(drs[:-1])
            if not var is None:
                a = var.split("_")
                if a[-1].upper() in ["X", "Y", "Z", "0", "1"]:  # for example VITESSE_X or KEPS_0
                    var = "_".join(a[:-1])                      # keep VITESSE only
                if a[0].upper() == "NORME":                     # same for norme_VITESSE, keep VITESSE only
                    var = "_".join(a[1:])
            # Take all the required time steps in the LATA database:
            if cycles == "":
                cyc = [-1]
            else:
                cyc = [int(s) for s in cycles.split()]
            try:
                fNames = cls._ParseLataDB(file, dom, var, loc, cyc)
                for fNa in fNames:
#                     print("@@@ VIS COMP adding :%s for var %s" % (fNa, var))
                    cls.Append(baseDir + "/" + fNa)
            except ValueError as e:
                # Do not raise - in some PRM, custom "instruction_visit" can lead to funny field names:
                print("WARNING: " + str(e))

    @classmethod
    def AppendTestCase(cls, case, dir):
        """ Append all the (small) files created for every case run. """
        if not cls.active: return
        cls.Append(dir + "/" + case + ".data")
        cls.Append(dir + "/" + case + ".err")
        # Commenting those 2 - they might be huge if many timesteps:
        #  cls.Append(dir + "/" + case + ".out")
        #  cls.Append(dir + "/" + case + ".dt_ev")
        cls.Append(dir + "/" + case + ".sons")  # mandatory to spot 'plane' probes
        cls.Append(dir + "/" + case + ".perf")
        cls.Append(dir + "/.tmp/*")          # mandatory for "make validation"

    @classmethod
    def AppendFromProbe(cls, param):
        """ Handle 'segment', 'points' (but not 'plane'!!) keywords in Curve
        """
        if not cls.active: return
        a = param.split()
        a[1] = a[1].upper() # variable name is always upper case in the file name
        b = a[0].split(".")
        if b[-1] == "data":
            a[0] = ".".join(b[:-1])
        probe_file = "%s_%s.son" % (a[0], a[1])  # "segment toto/tutu VITX 0" in the PRM ---> will produce file name "toto/tutu_VITX.son"
        cls.Append(probe_file)
        cls.Append("%s.data" % a[0])    # Little hack Jet_impingement_on_a_hot_flat_plate (because Trust is executed in the prepare)

    @classmethod
    def AppendFromPlaneProbe(cls, param):
        """ Handle 'plane' keyword in Curve. This is very much like a LATA reading.
        Plane probes in TRUST are really a hack around LATA stuff ...
        """
        if not cls.active: return
        a = param.split()
        a[1] = a[1].upper() # variable name is always upper case in the file name
        probe_file_base = "%s_%s" % (a[0], a[1])
        # Here the domain name is really the probe name ...
        cls.AppendVisuComplex(probe_file_base + ".lata", a[1], None, None)
        cls.Append(a[0] + ".sons")
        cls.Append(probe_file_base + ".plan")

    @classmethod
    def AppendFromInstructionVisit(cls, param, cycles=""):
        """ Try to recognise some of the common fields in a set of Visit instructions """
        if not cls.active: return
        if cls.last_LATA_used == "": return
        import re
        # Parse most common fields:
        # .*? in the regex corresponds to a lazy quantifier thus allowing to have several matches in a expression.
        # Regexp can be tested here:
        #    https://regex101.com/
        # With following examples:
        #         DefineScalarExpression("Tl_ELEM_dom","if(or(INDICATRICE_INTERF_ELEM_dom=1, INDICATRICE_INTERF_ELEM_dom=0), TEMPERATURE_THERMIQUE_ELEM_dom*INDICATRICE_INTERF_ELEM_dom,-(INDICATRICE_INTERF_ELEM_dom)^2/2*TEMPERATURE_GRAD_THERMIQUE_ELEM_dom*1.e-5)")
        #         DefineVectorExpression("v_proj_obli45", "{VITESSE_X_ELEM_dom/2+VITESSE_Z_ELEM_dom/2,VITESSE_Y_ELEM_dom,VITESSE_X_ELEM_dom/2+VITESSE_Z_ELEM_dom/2}")
        #         DefineScalarExpression("ERREUR_P_ELEM_dom","PRESSION_PA_ELEM_dom-P_ANALYTIQUE_ELEM_dom")
        #         DefineScalarExpression("ERREUR_SOM_dom","TEMPERATURE_SOM_dom-(-3./(6.*0.25)*(coord(dom)[0])/2.*((coord(dom)[0])*(coord(dom)[0])-2.*2.))")
        #         DefineScalarExpression("delta_TEMPERATURE_FACES_dom_pb_dual","TEMPERATURE_FACES_dom_pb_dual-pos_cmfe(<Temp_imposee.lata[0]id:TEMPERATURE_FACES_dom_pb_dual>,dom_pb_dual,1000.)")
        exp = re.compile(r'''(TEMPERATURE.*?|VITESSE|PRESSION|PRESSION_PA|INDICATRICE.*?)_(ELEM|SOM|FACES)_([A-Za-z_]+)''')
        mo, start = 1, 0
        while mo:   # as long as the parameter string contains variable that are recognised.
            mo = exp.search(param, start)
            if mo:
                var, loc, dom = mo.group(1), mo.group(2),mo.group(3)   # group 0 is the whole match !
                dom = cls._GetRealDomName(dom)
                start = mo.end()
                cls.AppendVisuComplex(cls.last_LATA_used, dom, var, loc, cycles)

    @classmethod
    def WriteToFile(cls, filename):
        with open(filename, "w") as f:
            for it in cls.GetList():
                f.write("%s\n" % it)
