#include "rooms.h"

string DeluxeRoom::getDescription() const {
    string d = "Делюкс, " +
               to_string(capacity) + " місць";
    if (hasBalcony) d += ", з балконом";
    return d;
}

string Suite::getDescription() const {
    string d = "Люкс, " +
               to_string(capacity) + " місць, " +
               to_string(roomCount) + " кімнати";
    if (hasJacuzzi) d += ", джакузі";
    return d;
}