//! \file
//!
//! Header file for the Physics class.
//!
//! The Physics class defines the particles and physics
//! processes which will be active in the COOKER Monte Carlo simulation.
//!
//! \author D.K. Hasell
//! \version 1.0
//! \date 2010-10-13
//!
//! \ingroup physics

// *+****1****+****2****+****3****+****4****+****5****+****6****+****7****+****

// Ensure this header file is only included once.

#ifndef PHYSICS_H
#define PHYSICS_H

// Include the GEANT4 header files referenced here.

#include "G4VUserPhysicsList.hh"
#include "G4WrapperProcess.hh"
#include "G4VParticleChange.hh"
#include "G4String.hh"

//! Physics class.
//!
//! Declares the Physics class and its member functions.

class Physics: public G4VUserPhysicsList {

public:

     //! Constructor.

     Physics( G4String runType = "", bool fB = true, bool sB = false);

     //! Destructor.

     ~Physics();

		// Bremsstrahlung modes
		bool fullBrem, supBrem;

private:

     G4String Run_Type;

protected:

     //! Member function to construct the particles used.

     void ConstructParticle();

     //! Member function to construct the processes used.

     void ConstructProcess();
 
     //! Member function to set the cuts.

     void SetCuts();

   
protected:

     //! Method to construct the boson family of particles.
 
     void ConstructBosons();

     //! Method to construct the lepton family of particles.
 
     void ConstructLeptons();
 
     //! Method to construct the meson family of particles.
 
     void ConstructMesons();

     //! Method to construct the baryon family of particles.
 
     void ConstructBaryons();

protected:

     //! Method to construct the general physics processes.

     void ConstructGeneral();

     //! Method to construct the electromagnetic physics processes.

     void ConstructEM();
};

// Custom user bremsstrahlung photon suppression class
class bremPhotonSuppression : public G4WrapperProcess
{
	// Overwrite the Geant4 post step action
	G4VParticleChange * PostStepDoIt(const G4Track& track, const G4Step& step);
};

#endif
