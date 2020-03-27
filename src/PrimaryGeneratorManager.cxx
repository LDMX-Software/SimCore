/**
 * @file PrimaryGeneratorManager.cxx
 * @brief Class that manages the generators used to fire particles. 
 * @author Omar Moreno, SLAC National Accelerator Laboratory
 */

#include "SimApplication/PrimaryGeneratorManager.h"

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <algorithm> 
#include <string>
#include <vector>
#include <dlfcn.h>

/*~~~~~~~~~~~~~~~*/
/*   Exception   */
/*~~~~~~~~~~~~~~~*/
#include "Exception/Exception.h" 

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimApplication/GeneralParticleSource.h"
#include "SimApplication/LHEPrimaryGenerator.h"
#include "SimApplication/LHEReader.h"
#include "SimApplication/MultiParticleGunPrimaryGenerator.h"
#include "SimApplication/RootPrimaryGenerator.h"
#include "SimApplication/ParticleGun.h"

ldmx::PrimaryGeneratorManager ldmx::PrimaryGeneratorManager::instance_ __attribute__((init_priority(100)));

namespace ldmx { 

    PrimaryGeneratorManager& PrimaryGeneratorManager::getInstance() { return instance_; }

    void PrimaryGeneratorManager::registerGenerator(const std::string& className , PrimaryGeneratorBuilder* builder) {

        GeneratorInfo info;
        info.className_ = className;
        info.builder_   = builder;

        generatorMap_[className] = info;
    }

    void PrimaryGeneratorManager::createGenerator(const std::string& className , const std::string& instanceName, Parameters& parameters) {
        auto it{generatorMap_.find(className)};
        if ( it == generatorMap_.end()) {
            EXCEPTION_RAISE( "CreateGenerator" , "Failed to create generator '" + className + "'." );
        }

        auto generator{it->second.builder_(instanceName, parameters)};

        //now that the generator is built --> put it on active list
        generators_.push_back( generator );
    }

} // ldmx
