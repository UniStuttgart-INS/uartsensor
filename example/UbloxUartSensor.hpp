// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// @file UbloxUartSensor.hpp
/// @brief Class to read out Ublox Sensors
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2020-07-22

#pragma once

#include <memory>

#include "uart/sensors/sensors.hpp"

class UbloxUartSensor
{
  public:
    /// @brief Constructor
    /// @param[in] name Name of the Parent Node
    explicit UbloxUartSensor(std::string name);

    /// @brief Default constructor
    UbloxUartSensor() = default;
    /// @brief Destructor
    ~UbloxUartSensor() = default;
    /// @brief Copy constructor
    UbloxUartSensor(const UbloxUartSensor&) = delete;
    /// @brief Move constructor
    UbloxUartSensor(UbloxUartSensor&&) = delete;
    /// @brief Copy assignment operator
    UbloxUartSensor& operator=(const UbloxUartSensor&) = delete;
    /// @brief Move assignment operator
    UbloxUartSensor& operator=(UbloxUartSensor&&) = delete;
    /// @brief Arrow operator overload
    uart::sensors::UartSensor* operator->() { return &sensor; };

    /// @brief Collects data bytes and searches for packages inside of them
    /// @param[in] dataByte The next data byte
    /// @return nullptr if no packet found yet, otherwise a pointer to the packet
    std::unique_ptr<uart::protocol::Packet> findPacket(uint8_t dataByte);

    static constexpr uint8_t BinarySyncChar1 = 0xB5; // µ
    static constexpr uint8_t BinarySyncChar2 = 0x62; // b
    static constexpr uint8_t AsciiStartChar = '$';

  private:
    /// Name of the Parent Node
    const std::string name;

    uart::sensors::UartSensor sensor{ endianness,
                                      packetFinderFunction,
                                      this,
                                      packetTypeFunction,
                                      checksumFunction,
                                      isErrorFunction,
                                      isResponseFunction,
                                      packetHeaderLength };

    static void packetFinderFunction(const std::vector<uint8_t>& data,
                                     const uart::xplat::TimeStamp& timestamp,
                                     uart::sensors::UartSensor::ValidPacketFoundHandler dispatchPacket, void* dispatchPacketUserData,
                                     void* userData);

    static uart::protocol::Packet::Type packetTypeFunction(const uart::protocol::Packet& packet);

    static bool checksumFunction(const uart::protocol::Packet& packet);

    static bool isErrorFunction(const uart::protocol::Packet& packet);

    static bool isResponseFunction(const uart::protocol::Packet& packet);

    static constexpr uart::Endianness endianness = uart::Endianness::ENDIAN_LITTLE;
    static constexpr size_t packetHeaderLength = 2;
    static constexpr uint8_t AsciiEndChar1 = '\r';
    static constexpr uint8_t AsciiEndChar2 = '\n';
    static constexpr uint8_t AsciiEscapeChar = '\0';

    bool currentlyBuildingAsciiPacket{ false };
    bool currentlyBuildingBinaryPacket{ false };

    bool asciiEndChar1Found{ false };
    bool binarySyncChar2Found{ false };
    bool binaryMsgClassFound{ false };
    bool binaryMsgIdFound{ false };
    bool binaryPayloadLength1Found{ false };
    bool binaryPayloadLength2Found{ false };

    uint8_t binaryMsgClass{ 0 };
    uint8_t binaryMsgId{ 0 };
    uint16_t binaryPayloadLength{ 0 };

    std::vector<uint8_t> _buffer;

    /// Used for correlating raw data with where the packet was found for the end user.
    size_t runningDataIndex{ 0 };
    size_t numOfBytesRemainingForCompletePacket{ 0 };

    void resetTracking();
};