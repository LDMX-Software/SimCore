#include "SimCore/UserTrackInformation.h"

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <iostream>

namespace simcore {

UserTrackInformation::UserTrackInformation() {}

UserTrackInformation::~UserTrackInformation() {}

void UserTrackInformation::Print() const {
  std::cout << "Saving track: " << saveFlag_ << "\n"
            << "Is brem candidate: " << isBremCandidate_ << "\n"
            << std::endl;
}
}  // namespace simcore
