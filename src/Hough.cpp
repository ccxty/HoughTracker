#include "Hough.h"

#include "Track.h"
namespace Hough {
void MergeTracks(Tracks &origin, Tracks &add) {
    for (auto &track : add) {
        bool contain = false;
        for (auto &existingTrack : origin) {
            if (existingTrack.operator>=(track)) {
                contain = true;
                break;
            }
        }
        if (!contain) {
            origin.push_back(std::move(track));
        }
    }
}
}  // namespace Hough