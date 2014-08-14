//! \file
//!
//! Source file for the Physics class.
//!
//! The Physics class is used for defining the particles and physics
//! processes which will be active in the COOKER Monte Carlo simulation.
//!
//! \author D.K. Hasell
//! \version 1.0
//! \date 2011-10-26
//!
//! \ingroup physics

// *+****1****+****2****+****3****+****4****+****5****+****6****+****7****+****

// Include the Physics header file.

#include "Physics.h"

// Include the GEANT4 header files referenced here.

#include "globals.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4CoulombScattering.hh"
#include "G4RayleighScattering.hh"

#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuMultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hMultipleScattering.hh"
#include "G4hIonisation.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"

#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"

#include "G4Decay.hh"

// Constructor.

Physics::Physics( G4String runType, bool fB, bool sB ) : G4VUserPhysicsList() {

   if( runType == "Recon" ) Run_Type = runType;
   else Run_Type = "MonteCarlo";

	fullBrem = fB;
	supBrem  = sB;

   // Set the default cut value.

   defaultCutValue = 1.0 * mm;

   // Set the verbosity.

   SetVerboseLevel(0);
}

// Destructor.

Physics::~Physics() {}

// Member function to construct the particles used.

void Physics::ConstructParticle() {

   // Construct the various families of particles.

   ConstructBosons();
   ConstructLeptons();
   ConstructMesons();
   ConstructBaryons();
}

// Method to construct the boson family of particles.
 
void Physics::ConstructBosons() {

   // Pseudo particles.

   G4Geantino::GeantinoDefinition();
   G4ChargedGeantino::ChargedGeantinoDefinition();

   // Photon.

   G4Gamma::GammaDefinition();
}

// Method to construct the lepton family of particles.
 
void Physics::ConstructLeptons() {

   // Electron and positron.

   G4Electron::ElectronDefinition();
   G4Positron::PositronDefinition();

   // Charged muons.

   G4MuonPlus::MuonPlusDefinition();
   G4MuonMinus::MuonMinusDefinition();

   // Electron neutrinos.

   G4NeutrinoE::NeutrinoEDefinition();
   G4AntiNeutrinoE::AntiNeutrinoEDefinition();

   // Muon neutrinos.

   G4NeutrinoMu::NeutrinoMuDefinition();
   G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
}

// Method to construct the meson family of particles.
 
void Physics::ConstructMesons() {

   // Mesons.

   G4PionPlus::PionPlusDefinition();
   G4PionMinus::PionMinusDefinition();
   G4PionZero::PionZeroDefinition();

//   G4Eta::EtaDefinition();
//   G4EtaPrime::EtaPrimeDefinition();

//   G4KaonPlus::KaonPlusDefinition();
//   G4KaonMinus::KaonMinusDefinition();
//   G4KaonZero::KaonZeroDefinition();
//   G4AntiKaonZero::AntiKaonZeroDefinition();
//   G4KaonZeroLong::KaonZeroLongDefinition();
//   G4KaonZeroShort::KaonZeroShortDefinition();
}

// Method to construct the baryon family of particles.
 
void Physics::ConstructBaryons() {

   G4Proton::ProtonDefinition();
//   G4AntiProton::AntiProtonDefinition();

   G4Neutron::NeutronDefinition();
//   G4AntiNeutron::AntiNeutronDefinition();
}

// Member function to construct the processes used.

void Physics::ConstructProcess() {
   AddTransportation();
   ConstructEM();
   ConstructGeneral();
}

// Method to construct the general physics processes.

void Physics::ConstructGeneral() {

   // Add Decay Process

   G4Decay * theDecayProcess = new G4Decay();
   theParticleIterator->reset();

   while( ( *theParticleIterator )() ){

      G4ParticleDefinition * particle = theParticleIterator->value();
      G4ProcessManager * pmanager = particle->GetProcessManager();

      if( theDecayProcess->IsApplicable( *particle ) ) {
 
         pmanager ->AddProcess( theDecayProcess );
         // set ordering for PostStepDoIt and AtRestDoIt
         pmanager ->SetProcessOrdering( theDecayProcess, idxPostStep );
         pmanager ->SetProcessOrdering( theDecayProcess, idxAtRest );
      }
   }
}

// Method to construct the electromagnetic physics processes.

void Physics::ConstructEM() {

   theParticleIterator->reset();

   while( ( *theParticleIterator )() ) {

      G4ParticleDefinition * particle = theParticleIterator->value();
      G4ProcessManager * pmanager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();
     
      if (particleName == "gamma") {

         pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
         pmanager->AddDiscreteProcess(new G4ComptonScattering); 
         pmanager->AddDiscreteProcess(new G4GammaConversion);
         pmanager->AddDiscreteProcess(new G4CoulombScattering);
         pmanager->AddDiscreteProcess(new G4RayleighScattering);
      
      }
      else if( particleName == "e-" ) {

         if( Run_Type == "Recon" ) {
            pmanager->AddProcess(new G4eIonisation,          -1,  1, 1 );
         }
         else {
            pmanager->AddProcess(new G4eMultipleScattering,  -1,  1, 1 );
            pmanager->AddProcess(new G4eIonisation,          -1,  2, 2 );
				if (fullBrem && supBrem)
				{
					std::cout<<"\n\nCANNOT USE FULL AND SUPPRESSED PHOTON BREMSSTRAHLUNG SIMULTANEOUSLY!\n\n";
					std::cout<<"USING FULL BREMSSTRAHLUNG FOR ELECTRONS!\n\n";
					supBrem = false;
				}
				else if (fullBrem)
				{
					pmanager->AddProcess(new G4eBremsstrahlung,     -1, -1, 3 );
				}
				else if (supBrem)
				{
					G4eBremsstrahlung * eBremP = new G4eBremsstrahlung;
            	bremPhotonSuppression * bremSup = new bremPhotonSuppression;
					bremSup->RegisterProcess(eBremP);
       			pmanager->AddProcess(bremSup,                    -1, -1, 3 );
				}
				else
				{
					std::cout<<"\n\nNO ELECTRON BREMSSTRAHLUNG INCLUDED!\n\n";
				}
         }

      }
      else if( particleName == "e+" ) {

         if( Run_Type == "Recon" ) {
            pmanager->AddProcess(new G4eIonisation,          -1,  1, 1 );
         }
         else {
            pmanager->AddProcess(new G4eMultipleScattering,  -1,  1, 1 );
            pmanager->AddProcess(new G4eIonisation,          -1,  2, 2 );
		if (fullBrem && supBrem)
		{
			std::cout<<"\n\nCANNOT USE FULL AND SUPPRESSED PHOTON BREMSSTRAHLUNG SIMULTANEOUSLY!\n\n";
			std::cout<<"USING FULL BREMSSTRAHLUNG FOR POSITRONS!\n\n";
			supBrem = false;
		}
		else if (fullBrem)
		{
			pmanager->AddProcess(new G4eBremsstrahlung,     -1, -1, 3 );
		}
		else if (supBrem)
		{
			G4eBremsstrahlung * eBremP = new G4eBremsstrahlung;
			bremPhotonSuppression * bremSup = new bremPhotonSuppression;
			bremSup->RegisterProcess(eBremP);
			pmanager->AddProcess(bremSup,                    -1, -1, 3 );
		}
		else
		{
			std::cout<<"\n\nNO POSITRON BREMSSTRAHLUNG INCLUDED!\n\n";
		}
            pmanager->AddProcess(new G4eplusAnnihilation,     0, -1, 4 );
         }

      }
      else if( particleName == "mu+" ||
               particleName == "mu-" ) {

         pmanager->AddProcess(new G4MuMultipleScattering, -1,  1, 1 );
         pmanager->AddProcess(new G4MuIonisation,         -1,  2, 2 );
			if (fullBrem && supBrem)
			{
				std::cout<<"\n\nCANNOT USE FULL AND SUPPRESSED PHOTON BREMSSTRAHLUNG SIMULTANEOUSLY!\n\n";
				std::cout<<"USING FULL BREMSSTRAHLUNG FOR MUONS!\n\n";
				supBrem = false;
			}
			else if (fullBrem)
			{
				pmanager->AddProcess(new G4MuBremsstrahlung,     -1, -1, 3 );
			}
			else if (supBrem)
			{
				G4MuBremsstrahlung * eBremP = new G4MuBremsstrahlung;
         	bremPhotonSuppression * bremSup = new bremPhotonSuppression;
				bremSup->RegisterProcess(eBremP);
    			pmanager->AddProcess(bremSup,                    -1, -1, 3 );
			}
			else
			{
				std::cout<<"\n\nNO MUON BREMSSTRAHLUNG INCLUDED!\n\n";
			}

         pmanager->AddProcess(new G4MuPairProduction,     -1, -1, 4 );       
     
      }

      // All others charged particles except chargedgeantino.

      else if( !( particle->IsShortLived() ) && 
               particle->GetPDGCharge() != 0.0 && 
               particle->GetParticleName() != "chargedgeantino" ) {

         if( Run_Type == "Recon" ) {
            pmanager->AddProcess(new G4hIonisation,          -1,  1, 1 );
         }
         else {
            pmanager->AddProcess(new G4hMultipleScattering,  -1,  1, 1 );
            pmanager->AddProcess(new G4hIonisation,          -1,  2, 2 );
				if (fullBrem && supBrem)
				{
					std::cout<<"\n\nCANNOT USE FULL AND SUPPRESSED PHOTON BREMSSTRAHLUNG SIMULTANEOUSLY!\n\n";
					std::cout<<"USING FULL BREMSSTRAHLUNG FOR HADRONS!\n\n";
					supBrem = false;
				}
				else if (fullBrem)
				{
					pmanager->AddProcess(new G4hBremsstrahlung,     -1, -1, 3 );
				}
				else if (supBrem)
				{
					G4hBremsstrahlung * eBremP = new G4hBremsstrahlung;
            	bremPhotonSuppression * bremSup = new bremPhotonSuppression;
					bremSup->RegisterProcess(eBremP);
       			pmanager->AddProcess(bremSup,                    -1, -1, 3 );
				}
				else
				{
					std::cout<<"\n\nNO HADRON BREMSSTRAHLUNG INCLUDED!\n\n";
				}

            pmanager->AddProcess(new G4hPairProduction,      -1, -1, 4 );
         }

         // Step limit.
         //pmanager->AddProcess(new G4StepLimiter,         -1, -1, 3 );         
         //pmanager->AddProcess(new G4UserSpecialCuts,     -1, -1, 4 );  
      }
   }
}

// Bremsstrahlung photon suppression wrapper process
G4VParticleChange * bremPhotonSuppression::PostStepDoIt(const G4Track& track, const G4Step& step)
{
	G4VParticleChange * particleChange = pRegProcess->PostStepDoIt(track,step);
	particleChange->SetNumberOfSecondaries(0);

	return particleChange;
};


// Member function to set the cuts.

void Physics::SetCuts() {

   // G4VUserPhysicsList::SetCutsWithDefault method sets 
   // the default cut value for all particle types.

   SetCutsWithDefault();
     
   if( verboseLevel > 0 ) DumpCutValuesTable();
}
