import nbformat as nbf
import argparse
from datetime import datetime
import os

parser = argparse.ArgumentParser(description="Create jupyter notebook")
parser.add_argument('-i','--inputfile', required=True, metavar="File", help = "inputfile")
args = parser.parse_args()

nb = nbf.v4.new_notebook()

title = "# %s test case\n" %args.inputfile

intro = "## Introduction\n Validation made by: " + os.getlogin() + "\n\n"
intro += " Report generated " + datetime.now().strftime("%d/%m/%Y")

description = "### Description\n "

description += "Sketch of a classic jupyter validation form.\n\n"
description += "For additional information, please consult validation form documentation with <code>Run_fiche --doc</code>."

binary, version = os.environ.get("exec", "[UNKNOWN]"), os.environ.get("TRUST_VERSION", "[UNKNOWN]")

origin = os.getcwd()
path = os.path.join(origin, "build")

parameters = "### TRUST parameters \n * Version TRUST: " + version + "\n * Binary used: " + binary + " (built on TRUST " + path + ")"

testCases = "from trustutils import run\n \n"

testCases += "run.addCase(\".\",\"%s.data\",nbProcs=1) \n"%args.inputfile
testCases += "run.printCases()\n"
testCases += "run.runCases()"


graph_t = "## Visualization of the probes"

graph = "from trustutils import plot \n \n"

graph += "graph1=plot.Graph(\"Sonde point\",size=[15,8])\n"
graph += "graph1.addPoint(\"%s_SONDE_PRESSION.son\",compo=0,marker=\"+\")\n\n"%args.inputfile

graph += "graph2=plot.Graph(\"Sonde segment\",size=[15,8])\n"
graph += "graph2.addSegment(\"%s_SONDE_VITESSE.son\",compo=0,marker=\"-x\")\n"%args.inputfile


visit_t = "## Visualization via VisIt"

visit = "from trustutils import visit\n \n"
visit += "fig=visit.Show(\"%s.lata\",\"Pseudocolor\",\"PRESSION_ELEM_dom\",nX=1,nY=1,mesh=\"dom\",title=\"\",time=-1)\n"%args.inputfile
visit += "fig.plot()"

tab_t = "## Table" 
tab = "from trustutils import plot\n \n" 
tab += "tab = plot.Table(['column1','column2'])\n"
tab += "data = plot.loadText(\"file.dat\",transpose=False)\n"
tab += "tab.addLigne([[data[0],data[1]]],\"label\")\n"
tab += "tab.df"

perf_t = "## Computer Performance"
perf = "run.tablePerf()"

data_t = "## Data"
data = "run.dumpDataset(\"%s.data\")"%args.inputfile

nb['cells'] = [nbf.v4.new_markdown_cell(title),
        nbf.v4.new_markdown_cell(intro),
        nbf.v4.new_markdown_cell(description),
        nbf.v4.new_markdown_cell(parameters),
        nbf.v4.new_code_cell(testCases),
        nbf.v4.new_markdown_cell(graph_t),
        nbf.v4.new_code_cell(graph),
        nbf.v4.new_markdown_cell(visit_t),
        nbf.v4.new_code_cell(visit),
        nbf.v4.new_markdown_cell(tab_t),
        nbf.v4.new_code_cell(tab),
        nbf.v4.new_markdown_cell(perf_t),
        nbf.v4.new_code_cell(perf),
        nbf.v4.new_markdown_cell(data_t),
        nbf.v4.new_code_cell(data),
        ]

nbf.write(nb, "%s.ipynb" % args.inputfile)


