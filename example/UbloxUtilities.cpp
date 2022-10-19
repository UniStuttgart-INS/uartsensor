// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "UbloxUtilities.hpp"

std::pair<uint8_t, uint8_t> checksumUBX(const std::vector<uint8_t>& data)
{
    uint8_t cka = 0;
    uint8_t ckb = 0;

    for (size_t i = 2; i < data.size() - 2; i++)
    {
        cka += data.at(i);
        ckb += cka;
    }
    return std::make_pair(cka, ckb);
}

uint8_t checksumNMEA(const std::vector<uint8_t>& data)
{
    uint8_t calcChecksum = 0;
    for (size_t i = 1; i < data.size() - 5; i++)
    {
        calcChecksum ^= data.at(i);
    }
    return calcChecksum;
}