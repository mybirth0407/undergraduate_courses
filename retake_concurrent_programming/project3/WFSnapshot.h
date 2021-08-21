//
//  WFSnapshot.h
//  hw3
//
//  Created by Yedarm Seong on 2017. 11. 3..
//  Copyright © 2017년 Yedarm Seong. All rights reserved.
//

#ifndef WFSnapshot_h
#define WFSnapshot_h

#include "StampedSnap.h"
#include <unistd.h>
#include <stdbool.h>

class WFSnapshot {
public:
    // atomic snapshot table
    StampedSnap *a_table;
    // number of thread
    int capacity;
    
    // empty construct
    WFSnapshot() {}
    
    // construct
    WFSnapshot(int capacity, long long init) {
        this->capacity = capacity;
        this->a_table = new StampedSnap[capacity];
        for (int i = 0; i < capacity; ++i) {
            this->a_table[i] = StampedSnap(init);
        }
    }
    
    // copy to register, not atomic
    StampedSnap* collect() {
        StampedSnap *copy = new StampedSnap[this->capacity];
        for (int j = 0; j < this->capacity; ++j) {
            copy[j] = this->a_table[j];
        }
        return copy;
    }
    
    // return atomic array snapshot
    long long* scan() {
        StampedSnap *old_copy = collect();
        StampedSnap *new_copy = NULL;
        // bool *moved = new bool[this->capacity];
        // for (int i = 0; i < this->capacity; ++i) {
        //     moved[i] = false;
        // }
        
        bool moved[this->capacity] = {false, };
        // old_copy = collect();
    
    // loop label, not update
    collect:
        while (true) {
            // need to memory free!
            if (new_copy != NULL) {
                for (int j = 0; j < this->capacity; ++j) {
                    new_copy->~StampedSnap();
                }
            }
            new_copy = collect();
            
            for (int j = 0; j < this->capacity; ++j) {
                if (old_copy[j].stamp != new_copy[j].stamp) {
                    // if thread update complete, thread move
                    if (moved[j]) {
                        // no longer needed
                        new_copy->~StampedSnap();
                        return old_copy[j].snap;
                    }
                    // not yet update
                    else {
                        moved[j] = true;
                        old_copy = new_copy;
                        goto collect;
                    }
                }
            }
            
            long long *result = new long long[this->capacity];
            for (int j = 0; j < this->capacity; ++j) {
                result[j] = new_copy[j].value;
            }
            old_copy->~StampedSnap();
            new_copy->~StampedSnap();
            return result;
        }
    }
    
    // value wrtie to register, for tid'th thread
    void update(long long value, int tid) {
        this->a_table[tid].snap = this->scan();
        StampedSnap old_value = this->a_table[tid];
        // new stamp!
        StampedSnap new_value = \
            StampedSnap(old_value.stamp + 1, value, this->a_table[tid].snap);
        // memory free!
        old_value.~StampedSnap();
        this->a_table[tid] = new_value;
    }
};
#endif /* WFSnapshot_h */