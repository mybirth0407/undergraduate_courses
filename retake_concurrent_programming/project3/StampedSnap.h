//
//  StampedSnap.h
//  hw3
//
//  Created by Yedarm Seong on 2017. 11. 3..
//  Copyright © 2017년 Yedarm Seong. All rights reserved.
//

#ifndef StampedSnap_h
#define StampedSnap_h

class StampedSnap {
public:
    // time stamp
    long long stamp;
    long long value;
    // recent latest snapshot
    long long *snap;
    
    // construct
    StampedSnap() {}
    
    // init value construct
    StampedSnap(long long value) {
        this->stamp = 0;
        this->value = value;
        this->snap = NULL;
    }
    
    // full argument construct
    StampedSnap(long long stamp, long long value, long long *snap) {
        this->stamp = stamp;
        this->value = value;
        this->snap = snap;
    }
};
#endif /* StampedSnap_h */
