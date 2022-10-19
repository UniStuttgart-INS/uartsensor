#pragma once

#include <cstddef>
#include <vector>
#include "uart/xplat/export.hpp"

namespace uart::xplat
{
/// \brief Interface for a simple port.
class proglib_DLLEXPORT IPort
{
    // Types //////////////////////////////////////////////////////////////////

  public:
    /// \brief Callback handler signature that can receive notification when
    /// new data has been received on the port.
    ///
    /// \param[in] userData Pointer to user data that was initially supplied
    ///     when the callback was registered with registerDataReceivedHandler.
    using DataReceivedHandler = void (*)(void* userData);

    // Public Methods /////////////////////////////////////////////////////////

    /// Default constructor
    IPort() = default;
    /// Destructor
    virtual ~IPort() = 0;
    /// Copy constructor
    IPort(const IPort&) = delete;
    /// Move constructor
    IPort(IPort&&) = delete;
    /// Copy assignment operator
    IPort& operator=(const IPort&) = delete;
    /// Move assignment operator
    IPort& operator=(IPort&&) = delete;

    /// \brief Opens the simple port.
    virtual void open() = 0;

    /// \brief Closes the simple port.
    virtual void close() = 0;

    /// \brief Indicates if the simple port is open.
    ///
    /// \return <c>true</c> if the serial port is open; otherwise <c>false</c>.
    virtual bool isOpen() = 0;

    /// \brief Writes out data to the simple port.
    ///
    /// \param[in] data The data array to write out.
    /// \param[in] length The length of the data array to write out.
    virtual void write(const char* data, size_t length) = 0;

    /// \brief Allows reading data from the simple port.
    ///
    /// \param[out] dataBuffer The data buffer to write the read data bytes to.
    ///     from the simple port.
    virtual void read(std::vector<unsigned char>& dataBuffer) = 0;

    /// \brief Registers a callback method for notification when new data is
    ///     received on the port.
    ///
    /// \param[in] userData Pointer to user data, which will be provided to the
    ///     callback method.
    /// \param[in] handler The callback method.
    virtual void registerDataReceivedHandler(void* userData, DataReceivedHandler handler) = 0;

    /// \brief Unregisters the registered callback method.
    virtual void unregisterDataReceivedHandler() = 0;
};

} // namespace uart::xplat
