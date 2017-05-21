//
// Created by mcochrane on 20/05/17.
//

#ifndef WS_OST_DATETIME_H
#define WS_OST_DATETIME_H

class DateTime {
public:
    DateTime() : val(0) {};
    DateTime(int v) : val(v) {};

    DateTime( const DateTime &obj) : val(obj.val) {};

    bool operator==(const DateTime& rhs) const { return val == rhs.val; };

    int val;
};

#endif //WS_OST_DATETIME_H
