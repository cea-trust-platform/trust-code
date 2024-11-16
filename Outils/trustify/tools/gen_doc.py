#!/usr/bin/env python

"""
This script generates the RST reference documentation for all the TRUST (or baltik) keywords.
It uses only the generated Python module (this is the easiest since the module contains all the
relevant docstrings and inheritance relationships), not the TRAD2 file, nor the C++ sources.
"""

import os, sys

class TRUSTDocGenerator:
    # For those classes, we want a specific type display, and we do not want a full entry in the
    # reference manual (those are the basic root types) - TODO review this
    TYPE_MAP =  { "objet_u": "Objet_U",
                  "listobj": "list of objects"
                 }

    TRUST_BASE_CLS = None #: will be filled once the module is loaded.

    def __init__(self, py_mod=None):
        """ @param py_mod (str): path to the automatically generated Python module
        """
        import re, os
        self.py_mod = py_mod
        if py_mod is None:
            if "TRUST_ROOT" not in os.environ:
                print("Generated module not specified and TRUST_ROOT not defined! Exiting.")
                sys.exit(-1)
            self.py_mod = os.path.join(os.environ["TRUST_ROOT"], "Outils", "trustpy", "install", "generated", "trustpy_gen.py")
        # Regexp to replace '\input' directives in the core of the description
        self.re_input = re.compile(r"\\input{{([a-z]+)}}")
        # Regexp to replace '\image` directives:
        self.re_img = re.compile(r"\\includepng{{([a-z]+).[a-z]+}}{{[0-9.]+}}")
        self.script_dir = os.path.dirname(os.path.realpath(__file__))
        self.trust_base_cls = None

    def get_top_parent(self, c):
        """ Get ultimate parent class """
        # Recurse of course
        def get_parent(cls):
            bases = cls.__bases__
            if len(bases) != 1:
                return cls
            if bases[0] is self.TRUST_BASE_CLS:
                return cls
            return get_parent(bases[0])

        return get_parent(c)

    def extract_type_and_desc(self, fld_nfo):
        """ From the pydantic type given in fld_nfo.annotation into a nice output like
           'string into ["lml","lata","lata_v2","med"]'
           @return description
           @return formatted type
        """
        from trustify.base import Builtin_Parser
        from trustify.misc_utilities import break_type

        ann = fld_nfo.rebuild_annotation()
        if Builtin_Parser.IsBuiltin(ann):
            pars_inst = Builtin_Parser.InstanciateFromBuiltin(ann)
            nice_typ = pars_inst.getFormattedType()
        else:
            _, typ, _ = break_type(ann)
            nice_typ = f":ref:`{typ[0].__name__.lower()}`"
        return nice_typ, fld_nfo.description

    def process_input_clauses(self, keyw, doc):
        r""" Find and replace all '\input{{toto}} clauses to insert instead the content
        of the file extra_rst/toto.rst
        """
        import os
        for match in self.re_input.finditer(doc):
            g0, g1 = match.group(0), match.group(1)
            file_nam = os.path.join(self.script_dir, "..", "doc", "extra_rst", f"{g1}.rst")
            # Try to open file
            if not os.path.isfile(file_nam):
                raise ValueError(f"When parsing documentation for keyword '{keyw}', unable to find requested file '{file_nam}' !!")
            with open(file_nam) as f:
                s = f.read()
                doc = doc.replace(g0, s)
        return doc

    def process_images(self, doc):
        r""" Find and replace all '\includepng{{triangulerh.pdf}}{{10}}' directives to replace them with proper RST syntax
        """
        for match in self.re_img.finditer(doc):
            g0, g1 = match.group(0), match.group(1)
            fil_nam = os.path.join("images", f"{g1}.jpeg")
            s = f"""

.. image:: {fil_nam}
  :alt: {fil_nam}
  :scale: 150%
  :align: center

"""
            doc = doc.replace(g0, s)
        return doc

    def doc_single(self, c, parent):
        """ Generate full RST string for a single keyword """
        from trustify.misc_utilities import break_type
        # Main name and synonyms
        nam = c.__name__.lower()
        s = f".. _{nam}:\n\n"
        s += f"**{nam}**\n"
        s += "-" * (len(nam)+4) + "\n\n"
        if len(c._synonyms[None]):
            syno = c._synonyms[None]
            s += "**Synonyms:** %s\n\n" % ", ".join(syno)
        # # Inheritance (if not parent)
        # if not c is parent and parent.__name__ != "Objet_u":
        #     par = ":ref:`%s`" % parent.__name__.lower()
        #     s += "**Inherits from:** %s \n\n" % par

        # Core description
        core_doc = self.process_input_clauses(nam, c.__doc__)
        t = core_doc.split("\n")
        t2 = [self.process_images(tt.lstrip()) for tt in t]
        s += "\n".join(t2)
        # If this is a list we can stop here, no attributes
        if c.__name__.startswith("List"):
            return s
        if len(c._synonyms.keys()) > 1:
            # Striclty greater than 1, because we always have the key 'None' for the synos of the keyword itself
            s += "\nParameters are:\n\n"
        # Attributes, in the proper order which was set in the _synonyms member:
        for a, asyno in c._synonyms.items():
            if a is None: continue # see comment above, None is for the keyword itself
            all_att = " | ".join([a]+asyno)
            hd_a = ""
            # Get corresponding pydantic info:
            fld_nfo = c.model_fields[a]
            opt, _, _ = break_type(fld_nfo.annotation)
            if opt:
                hd_a += f"- **[{all_att}]** "
            else:
                hd_a += f"- **{all_att}** "
            typ, dsc = self.extract_type_and_desc(fld_nfo)
            s += f"{hd_a} (*type:* {typ}) {dsc}\n\n"
        return s

    def generate_doc(self, out_dir):
        """ Generate MD documentation for all classes in TRUST """
        # Import generated module:
        from trustify.misc_utilities import import_generated_module, ClassFactory
        ze_mod = import_generated_module(self.py_mod)
        self.__class__.TRUST_BASE_CLS = ze_mod.Objet_u

        # Identify top classes in the module
        all_cls = ClassFactory.GetAllConstrainBasePyd()
        top_cls = {}
        # Keep valid classes and find their parent class:
        for c in all_cls:
            p = self.get_top_parent(c)
            cn, pn = c.__name__.lower(), p.__name__.lower()
            # Classes in self.TYPE_MAP (and their children) should simply be skipped:
            if pn in self.TYPE_MAP or cn in self.TYPE_MAP:
                # print("skiping ", pn)
                continue
            top_cls[c] = p

        # Unique parents, sorted alphabetically
        prt = list(set(top_cls.values()))
        prt.sort(key=lambda c: c.__name__)
        # Invert dict top_cls:
        inv_top = {}
        for k, v in top_cls.items():
            inv_top.setdefault(v, []).append(k)
        # Prepare the return value:
        single_s = ""
        # One MD file per parent:
        for j, p in enumerate(prt):
            lst = inv_top[p]
            lst.sort(key=lambda c: c.__name__)
            par = p.__name__.lower()
            s = f"**Keywords derived from {par}**\n"
            s += "=" * (len(s)-1) + "\n\n"
            for i, c in enumerate(lst):
                if i != 0:
                    # Horizontal line
                    s += "\n\n----\n\n"
                else:
                    # At first iteration, generate parent class doc, if not there already
                    if p not in lst:
                        s += self.doc_single(p, p)
                        s += "\n\n----\n\n"
                s += self.doc_single(c, p)
            fname = os.path.join(out_dir, par + ".rst")
            with open(fname, "w") as f:
                f.write(s)
            if j != 0:
                # Horizontal line
                single_s += "\n\n----\n\n"
            single_s += s

        # One big file with everything:
        fname_single = os.path.join(out_dir, "all.rst")
        with open(fname_single, "w") as f:
            f.write(single_s)

if __name__ == "__main__":
    from pathlib import Path
    pm = None
    if len(sys.argv) < 3:
        print("Usage: ./gen_doc.py <generated_module> <out_directory>")
        sys.exit(-1)
    pm, out_dir = Path(sys.argv[1]), Path(sys.argv[2])
    if not out_dir.is_dir():
        out_dir.mkdir()
    tdg = TRUSTDocGenerator(py_mod=pm)
    #s = """[chaine(into=["lml","lata","lata_v2","med"])] generated file post_med.data use format"""
    #print(tdg.extractTypeAndRef(s))
    tdg.generate_doc(out_dir)
