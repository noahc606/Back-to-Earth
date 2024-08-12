#pragma once
#include "Defs.h"

typedef Defs::t_tripleI64 t_tripleI64;

class CollectionUtils {
public:
    template<typename T> static T* findInMap(std::map<t_tripleI64, T>& coll, t_tripleI64 triple) {
        auto citr = coll.find(triple);
        if( citr!=coll.end() )
            return &citr->second;
        return nullptr;
    }

    template<typename T> static T* findInMap(std::map<t_tripleI64, T*>& coll, t_tripleI64 triple) {
        auto citr = coll.find(triple);
        if( citr!=coll.end() )
            return citr->second;
        return nullptr;
    }

    static bool findInMap(std::map<t_tripleI64, bool>& coll, t_tripleI64 triple) {
        auto citr = coll.find(triple);
        if( citr!=coll.end() )
            return &citr->second;
        return false;
    }

    template<typename T> static void insertInMap(std::map<t_tripleI64, T>& coll, std::pair<t_tripleI64, T> pair) {
        auto citr = coll.find(pair.first);
        if( citr==coll.end() ) {
            coll.insert(pair);
        }
    }

};