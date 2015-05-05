from SuperV import *

MARGE_Y=20
BOX_Y=110
SPACE_Y=110

MARGE_X=90
LOOP_X=250
BOX_X=180
SPACE_X=20

#DDL
listOfUseCase="liste_instructions"
MachineToLaunch=["localhost","crolles"]
MachineToLaunch=["localhost"]
import os
res=os.getenv("DIST_LANCE")
if res:
    from string import split
    MachineToLaunch=split(res,",")

print MachineToLaunch 
ContainerOfDistributor="FactoryServer"

def DefaNewDataFlow(listOfUseCase2, listOfContainers, distribCont):
    aNewDataFlow = Graph('aNewDataFlow')
    aNewDataFlow.SetName('aNewDataFlow')
    aNewDataFlow.SetAuthor( 'A Geay' )
    aNewDataFlow.SetComment( '' )
    aNewDataFlow.Coords( 0 , 0 )
    lgthOfListOfCont=len(listOfContainers)
    ###
    init = aNewDataFlow.FNode( 'DISTRIBUTOR' , 'DISTRIBUTOR_Gen' , 'init' )
    init.SetName( 'init' )
    init.SetAuthor( '' )
    init.SetContainer(distribCont)
    init.SetComment( 'init from DISTRIBUTOR_Gen' )
    YPOS=lgthOfListOfCont*(BOX_Y+SPACE_Y)/2-SPACE_Y+MARGE_Y
    XPOS=MARGE_X
    init.Coords(MARGE_X,YPOS )#Coord
    IfileOfTests = init.GetInPort('fileOfTests')
    IfileOfTests.Input(listOfUseCase)
    OinitGate = init.GetOutPort( 'Gate' )
    # Generation de Wait for all
    i=1
    Pywait_for_all = []
    def_instr="def wait_for_all("
    for container in listOfContainers:
        def_instr+="in"+str(i)+","
        i=i+1
        pass
    Pywait_for_all.append(def_instr[:-1]+'):   ')
    Pywait_for_all.append( '    print "All finished"   ' )
    Pywait_for_all.append( '    pass   ' )
    wait_for_all = aNewDataFlow.INode( 'wait_for_all' , Pywait_for_all )
    wait_for_all.SetName( 'wait_for_all' )
    wait_for_all.SetAuthor( '' )
    wait_for_all.SetComment( 'Compute Node' )
    XFIN=6*(MARGE_X+BOX_X)+MARGE_X
    wait_for_all.Coords( XFIN, YPOS) # , 175 )
    #Iwait_for_allin1 = wait_for_all.InPort( 'in1' , 'string' )
    #Iwait_for_allin2 = wait_for_all.InPort( 'in2' , 'string' )
    Iwait_for_allGate = wait_for_all.GetInPort( 'Gate' )
    Owait_for_allGate = wait_for_all.GetOutPort( 'Gate' )

    cat_res = aNewDataFlow.FNode( 'SHELLCOMPO' , 'SHELLCOMPO_Gen' , 'execLocal' )
    cat_res.SetName( 'cat_res' )
    cat_res.SetAuthor( '' )
    cat_res.SetContainer( 'localhost/FactoryServer' )
    cat_res.SetComment( 'execLocal from SHELLCOMPO' )
    cat_res.Coords( XFIN+ MARGE_X+BOX_X, YPOS )
    Icat_rescmd = cat_res.GetInPort( 'cmd' )
    Icat_rescmd.Input( 'cat res' )
    Icat_resGate = cat_res.GetInPort( 'Gate' )
    Ocat_resreturn = cat_res.GetOutPort( 'return' )
    Ocat_resGate = cat_res.GetOutPort( 'Gate' )
    Lwait_for_allGatecat_resGate = aNewDataFlow.Link( Owait_for_allGate , Icat_resGate )
    
    ###
    i=1
    initLaunch=[]
    loops=[]
    perfLaunch=[]
    switchs=[]
    OcurEndLoopDoLoop=0
    for container in listOfContainers:
        XPOS=MARGE_X
        YPOS=(i-1)*(BOX_Y+SPACE_Y)+MARGE_Y
        curPyLoop=[]
        curPyLoop.append( '   ' )
        curPyMoreLoop=[]
        curPyMoreLoop.append('import CORBA ' )
        curPyMoreLoop.append('import LifeCycleCORBA ' )
        curPyMoreLoop.append('import DISTRIBUTOR_ORB ' )
        curPyMoreLoop.append('orb = CORBA.ORB_init([], CORBA.ORB_ID) ')
        curPyMoreLoop.append('lcc = LifeCycleCORBA.LifeCycleCORBA(orb) ')
        curPyMoreLoop.append('comp=lcc.FindOrLoadComponent("'+distribCont+'","DISTRIBUTOR") ')
        curPyMoreLoop.append('def More'+`i`+'(case): ')
        curPyMoreLoop.append('	 (DoLoop,case)=comp.getNextElement() ')
        curPyMoreLoop.append('	 return DoLoop,case ')
        curPyMoreLoop.append('    ')
        curPyNextLoop=[]
        curPyNextLoop.append( '     ' )
        curPyEndLoop=[]
        curLoop,curEndLoop = aNewDataFlow.LNode( '' , curPyLoop , 'More'+`i` , curPyMoreLoop , '' , curPyNextLoop )
        curEndLoop.SetName( 'EndLoop_'+`i`)
        curEndLoop.SetAuthor( '' )
        curEndLoop.SetComment( 'Compute Node' )
        curEndLoop.Coords(XFIN-MARGE_X-BOX_X, YPOS)#Coord
        PycurEndLoop = []
        curEndLoop.SetPyFunction( 'EndLoop_'+`i` , curPyEndLoop )
        IcurLoopDoLoop = curLoop.GetInPort( 'DoLoop' )
        IcurLoopcase = curLoop.InPort( 'case' , 'string' )
        IcurLoopcase.Input("ee")
        IcurLoopGate = curLoop.GetInPort( 'Gate' )
        OcurLoopDoLoop = curLoop.GetOutPort( 'DoLoop' )
        OcurLoopcase = curLoop.GetOutPort( 'case' )
        IcurEndLoopDoLoop = curEndLoop.GetInPort( 'DoLoop' )
        IcurEndLoopcase = curEndLoop.GetInPort( 'case' )
        IcurEndLoopGate = curEndLoop.GetInPort( 'Gate' )
        OcurEndLoopDoLoop = curEndLoop.GetOutPort( 'DoLoop' )
        OcurEndLoopcase = curEndLoop.GetOutPort( 'case' )
        OcurEndLoopGate = curEndLoop.GetOutPort( 'Gate' )
        curLoop.SetName( 'Loop_'+`i` )
        curLoop.SetAuthor( '' )
        curLoop.SetComment( 'Compute Node' )
        XPOS+=MARGE_X+BOX_X
        curLoop.Coords(XPOS, YPOS)#Coord
        loops.append((curLoop,curEndLoop))
        aNewDataFlow.Link(OinitGate,IcurLoopGate)
        ## copie
           
        copiecas1 = aNewDataFlow.FNode( 'SHELLCOMPO' , 'SHELLCOMPO_Gen' , 'copie_cas' )
        copiecas1.SetName( 'copiecas_'+str(i) )
        copiecas1.SetAuthor( '' )
        copiecas1.SetContainer( 'localhost/FactoryServer' )
        copiecas1.SetComment( 'copiecas1 from SHELLCOMPO' )
        XPOS+=MARGE_X+BOX_X
        copiecas1.Coords( XPOS,YPOS )
        Icopiecas1cmd = copiecas1.GetInPort( 'cmd' )
        Icopiecas1machine = copiecas1.GetInPort( 'machine' )
        Icopiecas1Gate = copiecas1.GetInPort( 'Gate' )
        Ocopiecas1return = copiecas1.GetOutPort( 'return' )
        Ocopiecas1Gate = copiecas1.GetOutPort( 'Gate' )
        Icopiecas1machine.Input(container)
        ###
        exec1 = aNewDataFlow.FNode( 'SHELLCOMPO' , 'SHELLCOMPO_Gen' , 'exec' )
        exec1.SetName( 'exec_'+str(i) )
        exec1.SetAuthor( '' )
        #exec1.SetContainer( "localhost/FactoryServer"+str(i))
        exec1.SetContainer( "localhost/FactoryServer")
        exec1.SetComment( 'exec1 from SHELLCOMPO' )
        XPOS+=MARGE_X+BOX_X
        exec1.Coords(XPOS, YPOS)
        Iexec1cmd = exec1.GetInPort( 'cmd' )
        Iexec1machine = exec1.GetInPort( 'machine' )
        Iexec1Gate = exec1.GetInPort( 'Gate' )
        Oexec1strOutput = exec1.GetOutPort( 'strOutput' )
        Oexec1return = exec1.GetOutPort( 'return' )
        Oexec1Gate = exec1.GetOutPort( 'Gate' )
        Iexec1machine.Input(container)
  
        ##
        SaveResult = aNewDataFlow.FNode( 'DISTRIBUTOR' , 'DISTRIBUTOR_Gen' , 'SaveResult' )
        SaveResult.SetName( 'SaveResult'+str(i) )
        SaveResult.SetAuthor( '' )
        SaveResult.SetContainer( distribCont )
        SaveResult.SetComment( 'SaveResult from DISTRIBUTOR' )
        XPOS+=MARGE_X+BOX_X
        SaveResult.Coords(XPOS, YPOS)
        ISaveResultflag1 = SaveResult.GetInPort( 'flag1' )
        ISaveResultflag2 = SaveResult.GetInPort( 'flag2' )
        ISaveResultpartOfFile = SaveResult.GetInPort( 'partOfFile' )
        ISaveResultGate = SaveResult.GetInPort( 'Gate' )
        OSaveResultGate = SaveResult.GetOutPort( 'Gate' )
        aNewDataFlow.Link(OcurLoopcase,Iexec1cmd)
        aNewDataFlow.Link(OcurLoopcase,Icopiecas1cmd )
        ##
        aNewDataFlow.Link(OSaveResultGate,IcurEndLoopGate)

        aNewDataFlow.Link( Ocopiecas1Gate , Iexec1Gate )
        ## on ajoute le port sur wait_all et le lien
        Iwait_for_allin = wait_for_all.InPort( 'in'+str(i) , 'string' )
        LEndLoop_1casewait_for_allin1 = aNewDataFlow.Link(  OcurEndLoopGate , Iwait_for_allin )
        aNewDataFlow.Link(Oexec1return,ISaveResultflag1)
        aNewDataFlow.Link(Oexec1return,ISaveResultflag2)
        aNewDataFlow.Link(Oexec1strOutput,ISaveResultpartOfFile)
        i=i+1
        pass
#    Iwait_for_allGate = wait_for_all.GetInPort( 'Gate' )
#    Owait_for_allGate = wait_for_all.GetOutPort( 'Gate' )
    return aNewDataFlow


TEST = DefaNewDataFlow(listOfUseCase, MachineToLaunch, ContainerOfDistributor)
print "Generation du fichier de supervision"
TEST.Export("TEST.xml")
print "Taper TEST.Run() pour lancer en batch"

