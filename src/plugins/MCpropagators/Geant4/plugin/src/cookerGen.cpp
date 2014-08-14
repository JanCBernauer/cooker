#include "cookerGen.h"

#include "G4ParticleTable.hh"
#include "G4PrimaryVertex.hh"
#include "generatortree.h"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"

cookerGen::cookerGen(GeneratorEvent *ge):genEvent(ge)
{
  //find particle defs for e+,e-,proton,pi+,pi-
  G4ParticleTable *pt=G4ParticleTable::GetParticleTable();
  particleDefs["e+"]=pt->FindParticle("e+");
  particleDefs["e-"]=pt->FindParticle("e-");
  particleDefs["proton"]=pt->FindParticle("proton");
  particleDefs["neutron"]=pt->FindParticle("neutron");
  particleDefs["mu+"]=pt->FindParticle("mu+");
  particleDefs["mu-"]=pt->FindParticle("mu-");
  particleDefs["pi+"]=pt->FindParticle("pi+");
  particleDefs["pi-"]=pt->FindParticle("pi-");
  particleDefs["pi0"]=pt->FindParticle("pi0");
  particleDefs["gamma"]=pt->FindParticle("gamma");
}

void cookerGen::GeneratePrimaries(G4Event *anEvent)
{
  G4PrimaryVertex* vertex=new G4PrimaryVertex(G4ThreeVector(genEvent->vertex.x(),genEvent->vertex.y(),genEvent->vertex.z()),0);
  G4ThreeVector zero;
  for (std::vector<GeneratorParticle>::iterator iter=genEvent->particles.begin();
       iter!=genEvent->particles.end();iter++)
    {
      G4PrimaryParticle *particle=new G4PrimaryParticle(particleDefs[iter->particle]);
      particle->Set4Momentum(iter->momentum.Px(),iter->momentum.Py(),iter->momentum.Pz(),iter->momentum.E());
      particle->SetCharge(particleDefs[iter->particle]->GetPDGCharge());
      particle->SetPolarization(zero);
      vertex->SetPrimary(particle);
    }
  anEvent->AddPrimaryVertex(vertex);
}
