#include "generatortree.h"

// Default to a weight of 1
GeneratorWeights::GeneratorWeights(){
  set_default(1);
}

// Set just the default weight while keeping the others intact
GeneratorWeights & GeneratorWeights::operator=(const double &weight){
  set_default(weight);
  return *this;
}

// Copy the default weight and the extra weights
GeneratorWeights & GeneratorWeights::operator=(const GeneratorWeights &weights){
  if (this != &weights){
    set_default(weights.get_default());
    //set_extra_weights(weights.get_extra_weights());
  }
  return *this;
}

// Set the default weight
void GeneratorWeights::set_default(double weight){
  defaultweight = weight;
}

// Set an extra weight
void GeneratorWeights::set_extra(std::string id, double weight){
  extraweights[id] = weight;
}

// Set all of the extra weights
void GeneratorWeights::set_extra_weights(std::map<std::string, double> weights){
  extraweights = weights;
}

// Return the default weight
double GeneratorWeights::get_default() const {
  return defaultweight;
}

// Return one of the extra weights 
double GeneratorWeights::get_extra(std::string id) const {
  return extraweights.find(id)->second;
}

// Return all of the extra weights
std::map<std::string, double> GeneratorWeights::get_extra_weights() const {
  return extraweights;
}

// Clear the weights 
void GeneratorWeights::clear(){
  // Set default to 1
  set_default(1);
  // Erase the map
  extraweights.clear();
}

GeneratorWeights::~GeneratorWeights()
{}

ClassImp(GeneratorWeights);

GeneratorEvent::GeneratorEvent()
{}

GeneratorEvent::~GeneratorEvent()
{}

ClassImp(GeneratorEvent);

GeneratorParticle::GeneratorParticle()
{}

GeneratorParticle::~GeneratorParticle()
{}

ClassImp(GeneratorParticle);

