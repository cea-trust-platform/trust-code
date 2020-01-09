from xqt import *
from TRIOUGUI_Extension import *

__MARGIN_SIZE__    = 11
__SPACING_SIZE__   = 6
__TIMER_INTERVAL__ = 1000

class SolverDlg(QDialog):
    def __init__( self, extension, parent = None, name = None, modal = 0, flags = 0 ):
        QDialog.__init__( self, parent, name, modal, flags )
        self.setWFlags( Qt.WDestructiveClose )
        self.setCaption( self.tr( "Solver Control" ) )

        self.extension = extension
        self.study = getStudy()

        topLayout = QGridLayout( self )
        topLayout.setMargin( __MARGIN_SIZE__ )
        topLayout.setSpacing( __SPACING_SIZE__ )

        self.frame = QButtonGroup( self.tr( "Solver input" ), self )
        self.frame.setColumnLayout( 0, Qt.Vertical )
        self.frame.layout().setMargin( __MARGIN_SIZE__ )
        self.frame.layout().setSpacing( __SPACING_SIZE__ )

        self.studyRB = QRadioButton( self.tr( "Study" ), self.frame )
        self.fileRB = QRadioButton( self.tr( "External file" ), self.frame )
        self.studyRB.setChecked( 1 )
        self.connect( self.frame, SIGNAL( "clicked(int)" ), self.onInputChanged )

        self.filePath = QLineEdit( self.frame )
        self.filePath.setMinimumWidth( 200 )

        self.browseBtn = QPushButton( self.tr( "Browse..." ), self.frame )
        self.connect( self.browseBtn, SIGNAL( "clicked()" ), self.onBrowse )

        self.workDirLab = QLabel( self.tr( "Woking directory:" ), self.frame )
        self.workDir = QLineEdit( self.frame )
        self.workDir.setMinimumWidth( 200 )

        self.browseDirBtn = QPushButton( self.tr( "Browse..." ), self.frame )
        self.connect( self.browseDirBtn, SIGNAL( "clicked()" ), self.onBrowseDir )

        frameLayout = QGridLayout( self.frame.layout() )
        frameLayout.setAlignment( Qt.AlignTop )
        frameLayout.setMargin( 0 ); frameLayout.setSpacing( __SPACING_SIZE__ )

        frameLayout.addWidget( self.studyRB,      0, 0 )
        frameLayout.addWidget( self.fileRB,       1, 0 )
        frameLayout.addWidget( self.filePath,     1, 1 )
        frameLayout.addWidget( self.browseBtn,    1, 2 )
        frameLayout.addWidget( self.workDirLab,   2, 0 )
        frameLayout.addWidget( self.workDir,      2, 1 )
        frameLayout.addWidget( self.browseDirBtn, 2, 2 )

        self.autoClose = QCheckBox( self.tr( "Auto close after finishing" ), self )
        self.autoClose.setChecked( 1 )

        self.status = QLabel( self )
        self.status.setAlignment( Qt.AlignCenter )
        self.status.setFrameStyle( QFrame.Panel | QFrame.Sunken )

        self.startBtn = QPushButton( self.tr( "&Start" ), self )
        self.connect( self.startBtn, SIGNAL( "clicked()" ), self.onStart )

        self.pauseBtn = QPushButton( self.tr( "&Pause" ), self )
        self.pauseBtn.setEnabled( 0 )
        self.connect( self.pauseBtn, SIGNAL( "clicked()" ), self.onPause )

        self.stopBtn = QPushButton( self.tr( "S&top" ), self )
        self.stopBtn.setEnabled( 0 )
        self.connect( self.stopBtn, SIGNAL( "clicked()" ), self.onStop )

        self.closeBtn = QPushButton( self.tr( "&Close" ), self )
        self.connect( self.closeBtn, SIGNAL( "clicked()" ), self.close )

        btnLayout = QHBoxLayout(); btnLayout.setMargin( 0 ); btnLayout.setSpacing( __SPACING_SIZE__ )
        btnLayout.addWidget( self.startBtn )
        btnLayout.addWidget( self.pauseBtn )
        btnLayout.addWidget( self.stopBtn )
        btnLayout.addSpacing( 10 )
        btnLayout.addStretch()
        btnLayout.addWidget( self.closeBtn )

        topLayout.addWidget( self.frame,     0, 0 )
        topLayout.addWidget( self.autoClose, 1, 0 )
        topLayout.addWidget( self.status,    2, 0 )
        topLayout.addLayout( btnLayout,      3, 0 )

        self.timer = QTimer( self )
        self.connect( self.timer, SIGNAL( "timeout()" ), self.onTimer )

        self.onInputChanged()
        pass

    def onInputChanged( self ):
        self.filePath.setEnabled( not self.studyRB.isChecked() )
        self.browseBtn.setEnabled( not self.studyRB.isChecked() )

    def onBrowse( self ):
        aFile = self.filePath.text()
        aFilters = QStringList(); aFilters.append( self.tr( "Data files (*.data)" ) ); aFilters.append( self.tr( "All files (*.*)" ) )
        aFile = getSalomePyQt().getFileName( self, aFile, aFilters, "Select solver input file", 1 )
        if not aFile.isEmpty():
            self.filePath.setText( aFile )
        pass

    def onBrowseDir( self ):
        aDir = self.workDir.text()
        aDir = getSalomePyQt().getExistingDirectory( self, aDir, "Select working directory" )
        if not aDir.isEmpty():
            self.workDir.setText( aDir )
        pass

    def onStart( self ):
        if self.extension is None or self.extension.IsSolverRunning() or self.extension.IsSolverPaused():
            return

        aFile = ""
        if self.fileRB.isChecked():
            aFile = str( self.filePath.text() )
        aSolverPath = str( getSalomePyQt().getSetting( "TRIOU:SolverFile" ) )
        aWorkDir = str( self.workDir.text() )
        status = self.extension.StartSolver( self.study, aFile, aSolverPath, aWorkDir )
        if not status:
            QError( self, self.tr( "Error" ), self.tr( "Can't start solver!" ), self.tr( "&OK" )  )
            return

        self.startBtn.setEnabled( 0 )
        self.frame.setEnabled( 0 )
        self.pauseBtn.setEnabled( 1 )
        self.stopBtn.setEnabled( 1 )
        self.status.setText( self.tr( "Solver is running..." ) )

        self.timer.start( __TIMER_INTERVAL__ )
        pass

    def onPause( self ):
        if self.extension is None or not self.extension.IsSolverRunning() or self.extension.IsSolverPaused():
            return

        # try to pause solver
        self.extension.PauseSolver()
        # if failed, return
        if not self.extension.IsSolverPaused():
            return

        self.pauseBtn.setText( self.tr( "&Resume" ) )
        try:
            self.disconnect( self.pauseBtn, SIGNAL( "clicked()" ), self.onPause )
        except:
            pass
        self.connect( self.pauseBtn, SIGNAL( "clicked()" ), self.onResume )
        self.status.setText( self.tr( "Solver paused" ) )
        self.timer.stop()
        pass

    def onResume( self ):
        if self.extension is None or not self.extension.IsSolverPaused():
            return
        self.extension.ContinueSolver()
        self.pauseBtn.setText( self.tr( "&Pause" ) )
        try:
            self.disconnect( self.pauseBtn, SIGNAL( "clicked()" ), self.onResume )
        except:
            pass
        self.connect( self.pauseBtn, SIGNAL( "clicked()" ), self.onPause )
        self.status.setText( self.tr( "Solver is running..." ) )
        self.timer.start( __TIMER_INTERVAL__ )
        pass

    def onStop( self ):
        if self.extension is None:
            return
        self.startBtn.setEnabled( 1 )
        self.frame.setEnabled( 1 )
        self.pauseBtn.setEnabled( 0 )
        self.stopBtn.setEnabled( 0 )
        self.pauseBtn.setText( self.tr( "&Pause" ) )
        try:
            self.disconnect( self.pauseBtn, SIGNAL( "clicked()" ), self.onPause )
        except:
            pass
        try:
            self.disconnect( self.pauseBtn, SIGNAL( "clicked()" ), self.onResume )
        except:
            pass
        self.connect( self.pauseBtn, SIGNAL( "clicked()" ), self.onPause )
        self.status.setText( "" )
        if self.extension.IsSolverRunning():
            self.extension.StopSolver()
        self.timer.stop()
        pass

    def onTimer( self ):
        if self.extension is None:
            return

        if not self.extension.IsSolverRunning():
            self.onStop()
            status = self.extension.GetStatus()
            if status == TRIOU_CORBA.OK_NO_VISU:
                QWarning( self, self.tr( "Warning" ), self.tr( "VISU module is not found!\nResults are not published." ), self.tr( "&OK" ) )
                pass
            elif status == TRIOU_CORBA.FAILED_TO_PUBLISH:
                QError( self, self.tr( "Error" ), self.tr( "Can't publish the results." ), self.tr( "&OK" ) )
                pass
            elif status == TRIOU_CORBA.OK_NO_MED_FILES:
                # do not process this case
                pass
            elif status == TRIOU_CORBA.OK_WITH_ERRORS:
                QWarning( self, self.tr( "Warning" ), self.tr( "Errors occured when publishing of the results." ), self.tr( "&OK" ) )
                pass
            if self.autoClose.isChecked():
                self.close()
            getSalomeSwig().updateObjBrowser( 0 )
        pass

    def closeEvent( self, event ):
        if not self.extension is None and self.extension.IsSolverRunning():
            self.extension.StopSolver()

        self.timer.stop()
        getSalomeSwig().updateObjBrowser( 0 )
        QDialog.closeEvent( self, event )
        pass
