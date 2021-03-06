#ifndef KM3EvtIO_h
#define KM3EvtIO_h

#include "seaweed.h"
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// following is for hepevt interface
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
//#include "G4ThreeVector.h"
#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

class KM3EvtIO {
 public:
  KM3EvtIO(std::string infile, std::string outfile);
  ~KM3EvtIO();

  void ReadRunHeader();
  void WriteRunHeader();
  // taken from writer
  //void ReadEvent();
  void WriteEvent();
  void AddHit(int id, int PMTid, double pe, double t, int trackid, int npepure,
              double ttpure, int creatorProcess);
  void AddNumberOfHits(int hitnumber);
  void AddMuonPositionInfo(int tracknumber, int positionnumber, double posx,
                           double posy, double posz, double momx, double momy,
                           double momz, double mom, double time);
  void AddMuonPositionInfo(int tracknumber, int positionnumber, double posx,
                           double posy, double posz, double time);
  void AddMuonDecaySecondaries(int trackID, int parentID, double posx,
                               double posy, double posz, double dx, double dy,
                               double dz, double energy, double time,
                               int idPDG);

  void AddMuonEnergyInfo(const std::vector<double> &info);

  // taken from reader
  int GetNumberOfEvents();
  // this will actually get used if im right
  void ReadEvent(void);  // possible duplication
  void GetNeutrinoInfo(int &idneu, int &idtarget, double &xneu, double &yneu,
                       double &zneu, double &pxneu, double &pyneu,
                       double &pzneu, double &t0);
  int GetNumberOfParticles();
  void GetParticleInfo(int &idbeam, double &xx0, double &yy0, double &zz0,
                       double &pxx0, double &pyy0, double &pzz0, double &t0);
  bool IsNeutrinoEvent(void);

  void GeneratePrimaryVertex(G4Event *anEvent);

 private:
  seaweed::event *evt;
  std::ifstream infile;
  std::ofstream outfile;
  bool RunHeaderIsRead;
  bool RunHeaderIsWrite;
  int ParticlesHEPNumber[210000];
  int ParticlesIdNumber[210000];
  bool isneutrinoevent;
  bool hasbundleinfo;
  void GetArgs(std::string &chd, int &argnumber, double *args);
  int NumberOfParticles;

  // taken from reader
  int nevents;
  int ICONPDG[174];
  double PDGMASS[174];
  void InitPDGTables(void);
  int ConvertHEPToPDG(int hepcode);
  double GetParticleMass(int hepcode);
  bool UseEarthLepton;
  bool ReadNeutrinoVertexParticles;
};
#endif   // KM3EvtIO_h

