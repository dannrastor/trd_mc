#include "clusters.h"

using namespace std;

std::ostream& operator << (std::ostream& out, const ClusterType& type) {
    if (type == ClusterType::CENTER) {
        out << "1PX CENTER";
    } else if (type == ClusterType::OFF_CENTER_SIDE) {
        out << "1PX OFF_CENTER_SIDE";
    } else if (type == ClusterType::OFF_CENTER_CORNER) {
        out << "1PX OFF_CENTER_CORNER";
    } else if (type == ClusterType::SIDES) {
        out << "2PX SIDES";
    } else if (type == ClusterType::CORNERS) {
        out << "2PX CORNERS";
    } else if (type == ClusterType::_3PX) {
        out << "3PX";
    } else if (type == ClusterType::_4PX) {
        out << "4PX";
    } else {
        out << "NOT_DETECTED";
    }

    return out;
}

ClusterType define_type(const map<pair<int, int>, double>& shape) {


    switch(shape.size()) {
        case 0:
            return ClusterType::NOT_DETECTED;
            break;
        case 1:
            if (shape.count({0, 0})) {
                return ClusterType::CENTER;
            } else if (shape.count({-1, -1}) || shape.count({-1, 1}) || shape.count({1, -1}) || shape.count({1, 1})) {
                return ClusterType::OFF_CENTER_CORNER;
            } else {
                return ClusterType::OFF_CENTER_SIDE;
            }
            break;
        case 2:
            if (shape.count({-1, -1}) || shape.count({-1, 1}) || shape.count({1, -1}) || shape.count({1, 1})) {
                return ClusterType::CORNERS;
            } else {
                return ClusterType::SIDES;
            }
            break;
        case 3:
            return ClusterType::_3PX;
            break;
        case 4:
            return ClusterType::_4PX;
            break;
    }
}
