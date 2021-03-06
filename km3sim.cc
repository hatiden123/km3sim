#include "docopt.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4UnitsTable.hh"

//#include "KM3Sim.h"
#include "KM3Physics.h"
#include "KM3PrimaryGeneratorAction.h"
#include "KM3StackingAction.h"
#include "KM3TrackingAction.h"
#include "KM3SteppingAction.h"
#include "KM3EventAction.h"
#include "KM3Detector.h"

/** How to make a simple main:
 *
 * new G4Runmanager
 * rm.SetUnserInit(Detector)
 * rm.SetUnserInit(PhysicsList)
 * rm.SetUnserInit(G4ActionInit)
 * rm.init
 *
 * new uimng
 * uimng.applu(options-verbose)
 *
 * rm.beamon(nevts)
 * del rm
 * return 0
 */

/** how to detector:
 *
 * geometry
 * materials
 * sensitive regions
 * redout schemes of sensitive regions
 */

/** how to physics:
 *
 * particles to be used
 * physics processes tbu
 * range cuts on particles (overrides)
 */

/** how to useraction:
 *
 * add UserActionClasses (see below?)
 * define 1 mandatory UserAction
 */

/* Compilation settings:
 *
 * HADRONIC_COMPILE = True
 * TRACK_INFORMATION = True
 * ENABLE_MIE = True
 * DISABLE_PARAM = True
 * MYFIT_PARAM = False
 * EM_PARAM = False
 * MUON_PARAM = False
 * HA_PARAM = False
 * HAMUON_PARAM = False
 */

static const char USAGE[] =
    R"(km3sim.

  Usage:
    km3sim [--seed=<sd>] -p PARAMS -d DETECTOR -i INFILE -o OUTFILE
    km3sim (-h | --help)
    km3sim --version

  Options:
    -i INFILE         Input .evt file (e.g. from gSeaGen)
    -o OUTFILE        Output .evt file (for JTE)
    -p PARAMS         File with physics (seawater etc.) input parameters.
    -d DETECTOR       File with detector geometry.
    -h --help         Show this screen.
    --seed=<sd>       Set the RNG seed [default: 42].
    --version         Display the current version.
    --no-mie          Disable mie scattering [default: false]
)";

int main(int argc, const char **argv)
{
  std::map<std::string, docopt::value> args =
    docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "KM3Sim 2.0");

  G4long myseed = args["--seed"].asLong();
  CLHEP::HepRandom::setTheSeed(myseed);

  std::string Geometry_File = args["-d"].asString();
  std::string Parameter_File = args["-p"].asString();
  std::string infile_evt = args["-i"].asString();
  std::string outfile_evt = args["-o"].asString();
  G4double ParamEnergy;
  //G4int ParamNumber;
  G4int ParamParticle;

  // sole remaining IO method
  G4bool useHEPEvt = true;

  // argv[3] = old evt outfile
  // argv[9] = old evt infile

  // ALL IO should happen through EvtIO class
  // Other interfaces (savefile, outfile, etc.) are
  // for pythia IO and/or parametrization stuff
  // Usage:
  // EvtIO(infile, outfile)
  // EvtIO->ReadRunHeader()
  // EvtIO->WriteRunHeader()
  // EvtIO->WriteEvent()
  std::cout << "Open evt files..." << std::endl;
  KM3EvtIO *TheEVTtoWrite = new KM3EvtIO(infile_evt, outfile_evt);

  G4RunManager *runManager = new G4RunManager;

  std::cout << "Parsing detector & parameter files..." << std::endl;
  KM3Detector *Mydet = new KM3Detector;
  Mydet->Geometry_File = Geometry_File;
  Mydet->Parameter_File = Parameter_File;
  runManager->SetUserInitialization(Mydet);

  std::cout << "Set physics processes..." << std::endl;
  KM3Physics *MyPhys = new KM3Physics;
  MyPhys->aDetector = Mydet;
  runManager->SetUserInitialization(MyPhys);

  std::cout << "Call primary generator..." << std::endl;
  runManager->SetNumberOfEventsToBeStored(0);
  KM3PrimaryGeneratorAction *myGeneratorAction = new KM3PrimaryGeneratorAction;
  myGeneratorAction->infile_evt = infile_evt;
  myGeneratorAction->idbeam = ParamParticle;
  myGeneratorAction->ParamEnergy = ParamEnergy;
  myGeneratorAction->useHEPEvt = useHEPEvt;
  Mydet->MyGenerator = myGeneratorAction;

  std::cout << "Call TrackingAction..." << std::endl;
  KM3TrackingAction *myTracking = new KM3TrackingAction;
  myTracking->TheEVTtoWrite = TheEVTtoWrite;
  // link between generator and tracking (to provide number of
  // initial particles to trackingAction
  myGeneratorAction->myTracking = myTracking;
  myGeneratorAction->Initialize();
  runManager->SetUserAction(myGeneratorAction);

  std::cout << "Call EventAction..." << std::endl;
  KM3EventAction *event_action = new KM3EventAction;
  event_action->TheEVTtoWrite = TheEVTtoWrite;
  myGeneratorAction->event_action = event_action;
  // generator knows event to set the number of initial particles
  runManager->SetUserAction(event_action);

  Mydet->TheEVTtoWrite = TheEVTtoWrite;

  std::cout << "Call Stack/StepAction..." << std::endl;
  KM3StackingAction *myStacking = new KM3StackingAction;
  KM3SteppingAction *myStepping = new KM3SteppingAction;
  myStacking->SetDetector(Mydet);
  myStepping->myStDetector = Mydet;
  myStepping->event_action = event_action;
  runManager->SetUserAction(myStacking);
  runManager->SetUserAction(myTracking);
  runManager->SetUserAction(myStepping);

  // Initialize G4 kernel
  std::cout << "Init G4 Kernel..." << std::endl;
  runManager->Initialize();

  // Ummm dont use UI
  // get the pointer to the UI manager and set verbosities
  //G4UImanager *UI = G4UImanager::GetUIpointer();
  //G4UIsession *session = 0;
  //session = new G4UIterminal();
  // inactivate the parametrization
  //UI->ApplyCommand("/process/inactivate G4FastSimulationManagerProcess");

  // start a run
  std::cout << "Start a run..." << std::endl;
  runManager->SetVerboseLevel(10);
  runManager->BeamOn(myGeneratorAction->nevents);

  delete TheEVTtoWrite;

  delete runManager;
  return 0;
}
