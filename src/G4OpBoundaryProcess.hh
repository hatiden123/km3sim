//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: G4OpBoundaryProcess.hh,v 1.14 2006/06/29 21:08:38 gunter Exp $
// GEANT4 tag $Name: geant4-08-01-patch-01 $
//
//
////////////////////////////////////////////////////////////////////////
// Optical Photon Boundary Process Class Definition
////////////////////////////////////////////////////////////////////////
//
// File:        G4OpBoundaryProcess.hh
// Description: Discrete Process -- reflection/refraction at
//                                  optical interfaces
// Version:     1.1
// Created:     1997-06-18
// Modified:    2005-07-28 add G4ProcessType to constructor
//              1999-10-29 add method and class descriptors
//              1999-10-10 - Fill NewMomentum/NewPolarization in
//                           DoAbsorption. These members need to be
//                           filled since DoIt calls
//                           aParticleChange.SetMomentumChange etc.
//                           upon return (thanks to: Clark McGrew)
//
// Author:      Peter Gumplinger
//              adopted from work by Werner Keil - April 2/96
// mail:        gum@triumf.ca
//
// CVS version tag:
////////////////////////////////////////////////////////////////////////

#ifndef G4OpBoundaryProcess_h
#define G4OpBoundaryProcess_h 1

/////////////
// Includes
/////////////

#include "globals.hh"
#include "templates.hh"
#include "geomdefs.hh"
#include "Randomize.hh"
#include "G4Step.hh"
#include "G4VDiscreteProcess.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4OpticalPhoton.hh"
#include "G4TransportationManager.hh"

// Class Description:
// Discrete Process -- reflection/refraction at optical interfaces.
// Class inherits publicly from G4VDiscreteProcess.
// Class Description - End:

/////////////////////
// Class Definition
/////////////////////

enum G4OpBoundaryProcessStatus {
  Undefined,
  FresnelRefraction,
  FresnelReflection,
  TotalInternalReflection,
  LambertianReflection,
  LobeReflection,
  SpikeReflection,
  BackScattering,
  Absorption,
  Detection,
  NotAtBoundary,
  SameMaterial,
  StepTooSmall,
  NoRINDEX
};

class G4OpBoundaryProcess : public G4VDiscreteProcess {

private:
  //////////////
  // Operators
  //////////////

  // G4OpBoundaryProcess& operator=(const G4OpBoundaryProcess &right);

  // G4OpBoundaryProcess(const G4OpBoundaryProcess &right);

public: // Without description
  ////////////////////////////////
  // Constructors and Destructor
  ////////////////////////////////

  G4OpBoundaryProcess(const std::string &processName = "OpBoundary",
                      G4ProcessType type = fOptical);

  ~G4OpBoundaryProcess();

  ////////////
  // Methods
  ////////////

public: // With description
  bool IsApplicable(const G4ParticleDefinition &aParticleType);
  // Returns true -> 'is applicable' only for an optical photon.

  double GetMeanFreePath(const G4Track &, double,
                           G4ForceCondition *condition);
  // Returns infinity; i. e. the process does not limit the step,
  // but sets the 'Forced' condition for the DoIt to be invoked at
  // every step. However, only at a boundary will any action be
  // taken.

  G4VParticleChange *PostStepDoIt(const G4Track &aTrack, const G4Step &aStep);
  // This is the method implementing boundary processes.

  G4OpticalSurfaceModel GetModel() const;
  // Returns the optical surface mode.

  G4OpBoundaryProcessStatus GetStatus() const;
  // Returns the current status.

  void SetModel(G4OpticalSurfaceModel model);
  // Set the optical surface model to be followed
  // (glisur || unified).

private:
  void G4Swap(double *a, double *b) const;

  void G4Swap(G4Material *a, G4Material *b) const;

  void G4VectorSwap(G4ThreeVector *vec1, G4ThreeVector *vec2) const;

  bool G4BooleanRand(const double prob) const;

  G4ThreeVector G4IsotropicRand() const;

  G4ThreeVector G4LambertianRand(const G4ThreeVector &normal);

  G4ThreeVector G4PlaneVectorRand(const G4ThreeVector &normal) const;

  G4ThreeVector GetFacetNormal(const G4ThreeVector &Momentum,
                               const G4ThreeVector &Normal) const;

  void DielectricMetal();
  void DielectricDielectric();

  void ChooseReflection();
  void DoAbsorption();
  void DoReflection();

private:
  double thePhotonMomentum;

  G4ThreeVector OldMomentum;
  G4ThreeVector OldPolarization;

  G4ThreeVector NewMomentum;
  G4ThreeVector NewPolarization;

  G4ThreeVector theGlobalNormal;
  G4ThreeVector theFacetNormal;

  G4Material *Material1;
  G4Material *Material2;

  G4OpticalSurface *OpticalSurface;

  double Rindex1;
  double Rindex2;

  double cost1, cost2, sint1, sint2;

  G4OpBoundaryProcessStatus theStatus;

  G4OpticalSurfaceModel theModel;

  G4OpticalSurfaceFinish theFinish;

  double theReflectivity;
  double theEfficiency;
  double prob_sl, prob_ss, prob_bs;

  double kCarTolerance;
};

////////////////////
// Inline methods
////////////////////

inline void G4OpBoundaryProcess::G4Swap(double *a, double *b) const {
  // swaps the contents of the objects pointed
  // to by 'a' and 'b'!

  double temp;

  temp = *a;
  *a = *b;
  *b = temp;
}

inline void G4OpBoundaryProcess::G4Swap(G4Material *a, G4Material *b) const {
  // ONLY swaps the pointers; i.e. what used to be pointed
  // to by 'a' is now pointed to by 'b' and vice versa!

  G4Material *temp = a;

  a = b;
  b = temp;
}

inline void G4OpBoundaryProcess::G4VectorSwap(G4ThreeVector *vec1,
                                              G4ThreeVector *vec2) const {
  // swaps the contents of the objects pointed
  // to by 'vec1' and 'vec2'!

  G4ThreeVector temp;

  temp = *vec1;
  *vec1 = *vec2;
  *vec2 = temp;
}

inline bool G4OpBoundaryProcess::G4BooleanRand(const double prob) const {
  /* Returns a random boolean variable with the specified probability */

  return (G4UniformRand() < prob);
}

inline G4ThreeVector G4OpBoundaryProcess::G4IsotropicRand() const {
  /* Returns a random isotropic unit vector. */

  G4ThreeVector vect;
  double len2;

  do {

    vect.setX(G4UniformRand() - 0.5);
    vect.setY(G4UniformRand() - 0.5);
    vect.setZ(G4UniformRand() - 0.5);

    len2 = vect.mag2();

  } while (len2 < 0.01 || len2 > 0.25);

  return vect.unit();
}

inline G4ThreeVector
G4OpBoundaryProcess::G4LambertianRand(const G4ThreeVector &normal) {
  /* Returns a random lambertian unit vector. */

  G4ThreeVector vect;
  double ndotv;

  do {
    vect = G4IsotropicRand();

    ndotv = normal * vect;

    if (ndotv < 0.0) {
      vect = -vect;
      ndotv = -ndotv;
    }

  } while (!G4BooleanRand(ndotv));
  return vect;
}

inline G4ThreeVector
G4OpBoundaryProcess::G4PlaneVectorRand(const G4ThreeVector &normal) const

/* This function chooses a random vector within a plane given
   by the unit normal */
{
  G4ThreeVector vec1 = normal.orthogonal();

  G4ThreeVector vec2 = vec1.cross(normal);

  double phi = twopi * G4UniformRand();
  double cosphi = std::cos(phi);
  double sinphi = std::sin(phi);

  return cosphi * vec1 + sinphi * vec2;
}

inline bool
G4OpBoundaryProcess::IsApplicable(const G4ParticleDefinition &aParticleType) {
  return (&aParticleType == G4OpticalPhoton::OpticalPhoton());
}

inline G4OpticalSurfaceModel G4OpBoundaryProcess::GetModel() const {
  return theModel;
}

inline G4OpBoundaryProcessStatus G4OpBoundaryProcess::GetStatus() const {
  return theStatus;
}

inline void G4OpBoundaryProcess::SetModel(G4OpticalSurfaceModel model) {
  theModel = model;
}

inline void G4OpBoundaryProcess::ChooseReflection() {
  double rand = G4UniformRand();
  if (rand >= 0.0 && rand < prob_ss) {
    theStatus = SpikeReflection;
    theFacetNormal = theGlobalNormal;
  } else if (rand >= prob_ss && rand <= prob_ss + prob_sl) {
    theStatus = LobeReflection;
  } else if (rand > prob_ss + prob_sl && rand < prob_ss + prob_sl + prob_bs) {
    theStatus = BackScattering;
  } else {
    theStatus = LambertianReflection;
  }
}

inline void G4OpBoundaryProcess::DoAbsorption() {
  theStatus = Absorption;

  if (G4BooleanRand(theEfficiency)) {

    // EnergyDeposited =/= 0 means: photon has been detected
    theStatus = Detection;
    aParticleChange.ProposeLocalEnergyDeposit(thePhotonMomentum);
  } else {
    aParticleChange.ProposeLocalEnergyDeposit(0.0);
  }

  NewMomentum = OldMomentum;
  NewPolarization = OldPolarization;

  //              aParticleChange.ProposeEnergy(0.0);
  aParticleChange.ProposeTrackStatus(fStopAndKill);
}

inline void G4OpBoundaryProcess::DoReflection() {
  if (theStatus == LambertianReflection) {

    NewMomentum = G4LambertianRand(theGlobalNormal);
    theFacetNormal = (NewMomentum - OldMomentum).unit();

  } else if (theFinish == ground) {

    theStatus = LobeReflection;
    theFacetNormal = GetFacetNormal(OldMomentum, theGlobalNormal);
    double PdotN = OldMomentum * theFacetNormal;
    NewMomentum = OldMomentum - (2. * PdotN) * theFacetNormal;

  } else {

    theStatus = SpikeReflection;
    theFacetNormal = theGlobalNormal;
    double PdotN = OldMomentum * theFacetNormal;
    NewMomentum = OldMomentum - (2. * PdotN) * theFacetNormal;
  }
  double EdotN = OldPolarization * theFacetNormal;
  NewPolarization = -OldPolarization + (2. * EdotN) * theFacetNormal;
}

#endif /* G4OpBoundaryProcess_h */
