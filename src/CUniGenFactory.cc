//
// Created by _amp_ on 8/3/24.
//

#include "CUniGenFactory.hh"

cola::VFilter *CUniGenFactory::create(const std::map<std::string, std::string> & paramMap) {
    return new CUniGenWriter(paramMap.at("fname"));
}