#ifndef __GENERATOR_TREE_
#define __GENERATOR_TREE_

#include "cookerrawtree.h"

#include "TVector3.h"
#include "TLorentzVector.h"
#include <string>
#include <map>

class GeneratorParticle:public CRTBase
{
  public:
    TLorentzVector momentum;
    std::string particle;
    GeneratorParticle();
    ~GeneratorParticle();

    ClassDef(GeneratorParticle,1);
};

// Handles multiple weights for each event
class GeneratorWeights:public CRTBase
{
  private:
    double defaultweight;
    std::map<std::string, double> extraweights;
  public:
    GeneratorWeights();
    ~GeneratorWeights();
    GeneratorWeights & operator=(const double &weight); // Set default weight
    GeneratorWeights & operator=(const GeneratorWeights &weights); // Copy all weights
    double get_default() const;
    double get_extra(std::string id) const;
    std::map<std::string, double> get_extra_weights() const;
    void set_default(double weight);
    void set_extra(std::string id, double weight);
    void set_extra_weights(std::map<std::string, double> weights);
    void clear();

    ClassDef(GeneratorWeights,1);
};

class GeneratorEvent:public CRTBase
{
  public:
    TVector3 vertex; //the Vertex 
    GeneratorWeights weight; // the weight (should ALSO be saved into EventInfo)
    GeneratorParticle lepton_prescatter; //the simulated particle momentum pre scatter (with external energy loss, WITHOUT internal energy loss)
    std::vector<GeneratorParticle> particles;
    GeneratorEvent();
    ~GeneratorEvent();
    ClassDef(GeneratorEvent,2);
};

#endif
