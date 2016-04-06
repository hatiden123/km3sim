#ifndef G4OpMie_h
#define G4OpMie_h 1

#include "globals.h"
#include "templates.h"
#include "Randomize.h"
#include "G4ThreeVector.h"
#include "G4ParticleMomentum.h"
#include "G4Step.h"
#include "G4VDiscreteProcess.h"
#include "G4DynamicParticle.h"
#include "G4Material.h"
#include "G4OpticalPhoton.h"
#include "G4PhysicsTable.h"
#include "G4PhysicsOrderedFreeVector.h"
#include <vector>

struct PhaseFactors {
  double c0;
  double c1;
  double c2;
  double c3;
  double c4;
  double c5;
  double c6;
};

class G4OpMie : public G4VDiscreteProcess {
 private:
 public:
  G4OpMie(const G4String &processName = "OpMie", G4ProcessType type = fOptical);

  ~G4OpMie();

 public:  // With description
  G4bool IsApplicable(const G4ParticleDefinition &aParticleType);
  // Returns true -> 'is applicable' only for an optical photon.

  G4double GetMeanFreePath(const G4Track &aTrack, G4double, G4ForceCondition *);
  // Returns the mean free path for Mie scattering in water.
  // --- Not yet implemented for other materials! ---

  G4VParticleChange *PostStepDoIt(const G4Track &aTrack, const G4Step &aStep);
  // This is the method implementing Mie scattering.

 private:
  void BuildThePhysicsTable(void);
  G4double SampleAngle(void);
  G4double PhaseFunction(G4double angle);

 private:
  G4int IndexPhaseFunction;
  std::vector<PhaseFactors *> *thePhaseFactors;
  CLHEP::RandGeneral *PhaseRand;
  G4double PhaseArray[1801];
};

inline G4bool G4OpMie::IsApplicable(const G4ParticleDefinition &aParticleType) {
  return (&aParticleType == G4OpticalPhoton::OpticalPhoton());
}

#endif /* G4OpMie_h */