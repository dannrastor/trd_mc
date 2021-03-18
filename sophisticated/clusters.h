#include <iostream>
#include <vector>

enum class ClusterType {
    CENTER,
    OFF_CENTER_SIDE,
    OFF_CENTER_CORNER,
    SIDES,
    CORNERS,
    _3PX,
    _4PX,
    NOT_DETECTED
};

std::ostream& operator << (std::ostream& out, const ClusterType& type);

// shape is a 3*3 array with a central pixel being hit
ClusterType define_type(const std::vector<std::vector<double>>& shape);
