
#include <stdio.h>

#include "G4UIExecutive.hh"

int main(int argc, char** argv) {

    // Start an interactive session
    auto ui{new G4UIExecutive(argc, argv)};
    ui->SessionStart(); 

    delete ui; 

}
