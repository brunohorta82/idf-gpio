#pragma once

#if __cpp_exceptions

#include "Exceptions.hpp"
#include "System.hpp"
using namespace System;
namespace Components
{
    struct GPIOException : public ESPException
    {
        GPIOException(esp_err_t error);
    };

    /**
     * Check if the numeric pin number is valid on the current hardware.
     */
    esp_err_t isValidPin(uint32_t pin_num) noexcept;

    /**
     * Check if the numeric value of a drive strength is valid on the current hardware.
     */
    esp_err_t isValidDriveStrengthPin(uint32_t strength) noexcept;

    /**
     * This is a "Strong Value Type" class for GPIO. The GPIO pin number is checked during construction according to
     * the hardware capabilities. This means that any GPIONumBase object is guaranteed to contain a valid GPIO number.
     * See also the template class \c StrongValue.
     */
    template <typename GPIONumFinalType>
    class GPIONumBase final : public StrongValueComparable<uint32_t>
    {
    public:
        /**
         * @brief Create a numerical pin number representation and make sure it's correct.
         *
         * @throw GPIOException if the number does not reflect a valid GPIO number on the current hardware.
         */
        explicit GPIONumBase(uint32_t pin) : StrongValueComparable<uint32_t>(pin)
        {
            esp_err_t pin_check_result = isValidPin(pin);
            if (pin_check_result != ESP_OK)
            {
                throw GPIOException(pin_check_result);
            }
        }

        using StrongValueComparable<uint32_t>::operator==;
        using StrongValueComparable<uint32_t>::operator!=;
    };

    /**
     * This is a TAG type whose sole purpose is to create a distinct type from GPIONumBase.
     */
    class GPIONumType;

    /**
     * A GPIO number type used for general GPIOs, in contrast to specific GPIO pins like e.g. SPI_SCLK.
     */
    using GPIONum = GPIONumBase<class GPIONumType>;

    /**
     * Level of an input GPIO.
     */
    enum class GPIOLevel
    {
        HIGH,
        LOW
    };

    /**
     * Represents a valid pull up configuration for GPIOs.
     * It is supposed to resemble an enum type, hence it has static creation methods and a private constructor.
     * This class is a "Strong Value Type", see also the template class \c StrongValue for more properties.
     */
    class GPIOPullMode final : public StrongValueComparable<uint32_t>
    {
    private:
        /**
         * Constructor is private since it should only be accessed by the static creation methods.
         *
         * @param pull_mode A valid numerical respresentation of the pull up configuration. Must be valid!
         */
        explicit GPIOPullMode(uint32_t pull_mode) : StrongValueComparable<uint32_t>(pull_mode) {}

    public:
        /**
         * Create a representation of a floating pin configuration.
         * For more information, check the driver and HAL files.
         */
        static GPIOPullMode FLOATING();

        /**
         * Create a representation of a pullup configuration.
         * For more information, check the driver and HAL files.
         */
        static GPIOPullMode PULLUP();

        /**
         * Create a representation of a pulldown configuration.
         * For more information, check the driver and HAL files.
         */
        static GPIOPullMode PULLDOWN();

        using StrongValueComparable<uint32_t>::operator==;
        using StrongValueComparable<uint32_t>::operator!=;
    };

    /**
     * @brief Represents a valid wakup interrupt type for GPIO inputs.
     *
     * This class is a "Strong Value Type", see also the template class \c StrongValue for more properties.
     * It is supposed to resemble an enum type, hence it has static creation methods and a private constructor.
     * For a detailed mapping of interrupt types to numeric values, please refer to the driver types and implementation.
     */
    class GPIOWakeupIntrType final : public StrongValueComparable<uint32_t>
    {
    private:
        /**
         * Constructor is private since it should only be accessed by the static creation methods.
         *
         * @param pull_mode A valid numerical respresentation of a possible interrupt level to wake up. Must be valid!
         */
        explicit GPIOWakeupIntrType(uint32_t interrupt_level) : StrongValueComparable<uint32_t>(interrupt_level) {}

    public:
        static GPIOWakeupIntrType LOW_LEVEL();
        static GPIOWakeupIntrType HIGH_LEVEL();
    };

    /**
     * Class representing a valid drive strength for GPIO outputs.
     * This class is a "Strong Value Type", see also the template class \c StrongValue for more properties.
     * For a detailed mapping for values to drive strengths, please refer to the datasheet of the chip you are using.
     * E.g. for ESP32, the values in general are the following:
     *  - WEAK:             5mA
     *  - STRONGER:        10mA
     *  - DEFAULT/MEDIUM:  20mA
     *  - STRONGEST:       40mA
     */
    class GPIODriveStrength final : public StrongValueComparable<uint32_t>
    {
    public:
        /**
         * @brief Create a drive strength representation and checks its validity.
         *
         * After construction, this class should have a guaranteed valid strength representation.
         *
         * @param strength the numeric value mapping for a particular strength. For possible ranges, look at the
         *                  static creation functions below.
         * @throws GPIOException if the supplied number is out of the hardware capable range.
         */
        explicit GPIODriveStrength(uint32_t strength) : StrongValueComparable<uint32_t>(strength)
        {
            esp_err_t strength_check_result = isValidDriveStrengthPin(strength);
            if (strength_check_result != ESP_OK)
            {
                throw GPIOException(strength_check_result);
            }
        }

        /**
         * Create a representation of the default drive strength.
         * For more information, check the datasheet and driver and HAL files.
         */
        static GPIODriveStrength DEFAULT();

        /**
         * Create a representation of the weak drive strength.
         * For more information, check the datasheet and driver and HAL files.
         */
        static GPIODriveStrength WEAK();

        /**
         * Create a representation of the less weak drive strength.
         * For more information, check the datasheet and driver and HAL files.
         */
        static GPIODriveStrength LESS_WEAK();

        /**
         * Create a representation of the medium drive strength.
         * For more information, check the datasheet and driver and HAL files.
         */
        static GPIODriveStrength MEDIUM();

        /**
         * Create a representation of the strong drive strength.
         */
        static GPIODriveStrength STRONGEST();

        using StrongValueComparable<uint32_t>::operator==;
        using StrongValueComparable<uint32_t>::operator!=;
    };

    /**
     * @brief Implementations commonly used functionality for all GPIO configurations.
     *
     * Some functionality is only for specific configurations (set and get drive strength) but is necessary here
     * to avoid complicating the inheritance hierarchy of the GPIO classes.
     * Child classes implementing any GPIO configuration (output, input, etc.) are meant to intherit from this class
     * and possibly make some of the functionality publicly available.
     */
    class GPIO
    {
    protected:
        /**
         * @brief Construct a GPIO.
         *
         * This constructor will only reset the GPIO but leaves the actual configuration (input, output, etc.) to
         * the sub class.
         *
         * @param num GPIO pin number of the GPIO to be configured.
         *
         * @throws GPIOException
         *              - if the underlying driver function fails
         */
        GPIO(GPIONum num);
        void holdEnable();
        void holdDisable();
        void setDriveStrength(GPIODriveStrength strength);
        GPIODriveStrength getDriveStrength();

        /**
         * @brief The number of the configured GPIO pin.
         */
        GPIONum gpio_num;
    };

    /**
     * @brief This class represents a GPIO which is configured as output.
     */
    class PinOutput : public GPIO
    {
    public:
        /**
         * @brief Construct and configure a GPIO as output.
         *
         * @param num GPIO pin number of the GPIO to be configured.
         *
         * @throws GPIOException
         *              - if the underlying driver function fails
         */
        PinOutput(GPIONum num);
        void setHigh();
        void setLow();

        using GPIO::getDriveStrength;
        using GPIO::setDriveStrength;
    };

    /**
     * @brief This class represents a GPIO which is configured as input.
     */
    class PinInput : public GPIO
    {
    public:
        /**
         * @brief Construct and configure a GPIO as input.
         *
         * @param num GPIO pin number of the GPIO to be configured.
         *
         * @throws GPIOException
         *              - if the underlying driver function fails
         */
        PinInput(GPIONum num);
        GPIOLevel getLevel() const noexcept;
        void setPullMode(GPIOPullMode mode);
        void wakeupEnable(GPIOWakeupIntrType interrupt_type);
        void wakeupDisable();
    };

    /**
     * @brief This class represents a GPIO which is configured as open drain output and input at the same time.
     *
     * This class facilitates bit-banging for single wire protocols.
     */
    class PinOutputInput : public PinInput
    {
    public:
        /**
         * @brief Construct and configure a GPIO as open drain output as well as input.
         *
         * @param num GPIO pin number of the GPIO to be configured.
         *
         * @throws GPIOException
         *              - if the underlying driver function fails
         */
        PinOutputInput(GPIONum num);
        void setFloating();
        void setLow();

        using GPIO::getDriveStrength;
        using GPIO::setDriveStrength;
    };

}

#endif
