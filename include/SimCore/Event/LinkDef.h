#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ namespace ldmx;
#pragma link C++ namespace ldmx::SimParticle+;
#pragma link C++ namespace std::map<int,ldmx::SimParticle>+;
#pragma link C++ namespace ldmx::SimCalorimeterHit+;
#pragma link C++ namespace std::vector<ldmx::SimCalorimeterHit>+;
#pragma link C++ namespace ldmx::SimTrackerHit+;
#pragma link C++ namespace std::vector<ldmx::SimTrackerHit>+;

#endif
