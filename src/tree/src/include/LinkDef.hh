#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class std::vector<unsigned short>+;
#pragma link C++ class std::pair<unsigned short, std::string>+;
#pragma link C++ class std::map<unsigned short, std::string>+;
#pragma link C++ class CRTBase+;
#pragma link C++ class CRTConfigFile+;
#pragma link C++ class CRTRunInfo+;


#pragma link C++ class CRTEventInfo+;
#pragma link C++ class CRTBinaryBlob+;

#pragma link C++ class std::map<std::string, double>+;

#pragma link C++ class GeneratorEvent+;
#pragma link C++ class GeneratorParticle+;
#pragma link C++ class GeneratorWeights+;
#pragma link C++ class std::vector<GeneratorParticle>+;


#pragma link C++ class MCDetectorHit+;
#pragma link C++ class std::vector<MCDetectorHit>+;
#pragma link C++ class MCDetectorData;

#endif
