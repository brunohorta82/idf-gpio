#if __cpp_exceptions

#include <array>
#include "driver/gpio.h"
#include "Gpio.hpp"
using namespace System;
namespace Components
{

#define GPIO_CHECK_THROW(err) CHECK_THROW_SPECIFIC((err), GPIOException)

    namespace
    {
#if CONFIG_IDF_TARGET_LINUX
        constexpr std::array<uint32_t, 1> INVALID_GPIOS = {24};
#elif CONFIG_IDF_TARGET_ESP32
        constexpr std::array<uint32_t, 1> INVALID_GPIOS = {24};
#elif CONFIG_IDF_TARGET_ESP32S2
        constexpr std::array<uint32_t, 4> INVALID_GPIOS = {22, 23, 24, 25};
#elif CONFIG_IDF_TARGET_ESP32S3
        constexpr std::array<uint32_t, 4> INVALID_GPIOS = {22, 23, 24, 25};
#elif CONFIG_IDF_TARGET_ESP32C3
        constexpr std::array<uint32_t, 0> INVALID_GPIOS = {};
#elif CONFIG_IDF_TARGET_ESP32C2
        constexpr std::array<uint32_t, 0> INVALID_GPIOS = {};
#else
#error "No GPIOs defined for the current target"
#endif

    }

    GPIOException::GPIOException(esp_err_t error) : ESPException(error) {}

    esp_err_t isValidPin(uint32_t pin_num) noexcept
    {
        if (pin_num >= GPIO_NUM_MAX)
        {
            return ESP_ERR_INVALID_ARG;
        }

        for (auto num : INVALID_GPIOS)
        {
            if (pin_num == num)
            {
                return ESP_ERR_INVALID_ARG;
            }
        }

        return ESP_OK;
    }

    esp_err_t isValidDriveStrengthPin(uint32_t strength) noexcept
    {
        if (strength >= GPIO_DRIVE_CAP_MAX)
        {
            return ESP_ERR_INVALID_ARG;
        }

        return ESP_OK;
    }

    GPIOPullMode GPIOPullMode::FLOATING()
    {
        return GPIOPullMode(GPIO_FLOATING);
    }

    GPIOPullMode GPIOPullMode::PULLUP()
    {
        return GPIOPullMode(GPIO_PULLUP_ONLY);
    }

    GPIOPullMode GPIOPullMode::PULLDOWN()
    {
        return GPIOPullMode(GPIO_PULLDOWN_ONLY);
    }

    GPIOWakeupIntrType GPIOWakeupIntrType::LOW_LEVEL()
    {
        return GPIOWakeupIntrType(GPIO_INTR_LOW_LEVEL);
    }

    GPIOWakeupIntrType GPIOWakeupIntrType::HIGH_LEVEL()
    {
        return GPIOWakeupIntrType(GPIO_INTR_HIGH_LEVEL);
    }

    GPIODriveStrength GPIODriveStrength::DEFAULT()
    {
        return MEDIUM();
    }

    GPIODriveStrength GPIODriveStrength::WEAK()
    {
        return GPIODriveStrength(GPIO_DRIVE_CAP_0);
    }

    GPIODriveStrength GPIODriveStrength::LESS_WEAK()
    {
        return GPIODriveStrength(GPIO_DRIVE_CAP_1);
    }

    GPIODriveStrength GPIODriveStrength::MEDIUM()
    {
        return GPIODriveStrength(GPIO_DRIVE_CAP_2);
    }

    GPIODriveStrength GPIODriveStrength::STRONGEST()
    {
        return GPIODriveStrength(GPIO_DRIVE_CAP_3);
    }

    GPIO::GPIO(GPIONum num) : gpio_num(num)
    {
        GPIO_CHECK_THROW(gpio_reset_pin(gpio_num.get_value<gpio_num_t>()));
    }

    void GPIO::holdEnable()
    {
        GPIO_CHECK_THROW(gpio_hold_en(gpio_num.get_value<gpio_num_t>()));
    }

    void GPIO::holdDisable()
    {
        GPIO_CHECK_THROW(gpio_hold_dis(gpio_num.get_value<gpio_num_t>()));
    }

    void GPIO::setDriveStrength(GPIODriveStrength strength)
    {
        GPIO_CHECK_THROW(gpio_set_drive_capability(gpio_num.get_value<gpio_num_t>(),
                                                   strength.get_value<gpio_drive_cap_t>()));
    }

    PinOutput::PinOutput(GPIONum num) : GPIO(num)
    {
        GPIO_CHECK_THROW(gpio_set_direction(gpio_num.get_value<gpio_num_t>(), GPIO_MODE_OUTPUT));
    }

    void PinOutput::setHigh()
    {
        GPIO_CHECK_THROW(gpio_set_level(gpio_num.get_value<gpio_num_t>(), 1));
    }

    void PinOutput::setLow()
    {
        GPIO_CHECK_THROW(gpio_set_level(gpio_num.get_value<gpio_num_t>(), 0));
    }

    GPIODriveStrength GPIO::getDriveStrength()
    {
        gpio_drive_cap_t strength;
        GPIO_CHECK_THROW(gpio_get_drive_capability(gpio_num.get_value<gpio_num_t>(), &strength));
        return GPIODriveStrength(static_cast<uint32_t>(strength));
    }

    PinInput::PinInput(GPIONum num) : GPIO(num)
    {
        GPIO_CHECK_THROW(gpio_set_direction(gpio_num.get_value<gpio_num_t>(), GPIO_MODE_INPUT));
    }

    GPIOLevel PinInput::getLevel() const noexcept
    {
        int level = gpio_get_level(gpio_num.get_value<gpio_num_t>());
        if (level)
        {
            return GPIOLevel::HIGH;
        }
        else
        {
            return GPIOLevel::LOW;
        }
    }

    void PinInput::setPullMode(GPIOPullMode mode)
    {
        GPIO_CHECK_THROW(gpio_set_pull_mode(gpio_num.get_value<gpio_num_t>(),
                                            mode.get_value<gpio_pull_mode_t>()));
    }

    void PinInput::wakeupEnable(GPIOWakeupIntrType interrupt_type)
    {
        GPIO_CHECK_THROW(gpio_wakeup_enable(gpio_num.get_value<gpio_num_t>(),
                                            interrupt_type.get_value<gpio_int_type_t>()));
    }

    void PinInput::wakeupDisable()
    {
        GPIO_CHECK_THROW(gpio_wakeup_disable(gpio_num.get_value<gpio_num_t>()));
    }

    PinOutputInput::PinOutputInput(GPIONum num) : PinInput(num)
    {
        GPIO_CHECK_THROW(gpio_set_direction(gpio_num.get_value<gpio_num_t>(), GPIO_MODE_INPUT_OUTPUT_OD));
    }

    void PinOutputInput::setFloating()
    {
        GPIO_CHECK_THROW(gpio_set_level(gpio_num.get_value<gpio_num_t>(), 1));
    }

    void PinOutputInput::setLow()
    {
        GPIO_CHECK_THROW(gpio_set_level(gpio_num.get_value<gpio_num_t>(), 0));
    }

}

#endif
