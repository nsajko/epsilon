#include "wakeup.h"

#include "battery.h"
#include "usb.h"
#include "keyboard.h"

#include "regs/syscfg.h"
#include "regs/exti.h"

namespace Ion {
namespace WakeUp {
namespace Device {

void onChargingEvent() {
  Battery::Device::initGPIO();

  /* Warning: pins with the same number in different groups cannot be set as
   * source input for EXTI at the same time. Here, EXTICR1 register is filled
   * between position 12-15 (charging pin = 3) with
   * 0000 (ChargingGPIO = group A). */
  SYSCFG.EXTICR1()->setEXTI(Battery::Device::ChargingPin.pin(), Battery::Device::ChargingPin.group());

  EXTI.EMR()->set(Battery::Device::ChargingPin.pin(), true);

  /* We need to detect when the battery stops charging. We set the
   * wake up event on the rising edge. */
  EXTI.RTSR()->set(Battery::Device::ChargingPin.pin(), true);
}

void onUSBPlugging() {
  USB::Device::VbusPin.group().MODER()->setMode(USB::Device::VbusPin.pin(), GPIO::MODER::Mode::Input);
  USB::Device::VbusPin.group().PUPDR()->setPull(USB::Device::VbusPin.pin(), GPIO::PUPDR::Pull::Down);
  /* Here, EXTICR3 register is filled between position 4-7 (Vbus pin = 9) with
   * 0000 (Vbus GPIO = group A). */
  SYSCFG.EXTICR3()->setEXTI(USB::Device::VbusPin.pin(), USB::Device::VbusPin.group());

  EXTI.EMR()->set(USB::Device::VbusPin.pin(), true);
  EXTI.FTSR()->set(USB::Device::VbusPin.pin(), true);
  EXTI.RTSR()->set(USB::Device::VbusPin.pin(), true);
}


void onPowerKeyDown() {
  Keyboard::Key key = Keyboard::Key::B2;
  uint8_t rowPin = Keyboard::Device::RowPins[Keyboard::Device::rowForKey(key)];
  Keyboard::Device::RowGPIO.MODER()->setMode(rowPin, GPIO::MODER::Mode::Output);
  Keyboard::Device::RowGPIO.OTYPER()->setType(rowPin, GPIO::OTYPER::Type::OpenDrain);
  Keyboard::Device::RowGPIO.ODR()->set(rowPin, 0);

  uint8_t column = Keyboard::Device::columnForKey(key);
  uint8_t columnPin = Keyboard::Device::ColumnPins[column];

  Keyboard::Device::ColumnGPIO.MODER()->setMode(columnPin, GPIO::MODER::Mode::Input);
  Keyboard::Device::ColumnGPIO.PUPDR()->setPull(columnPin, GPIO::PUPDR::Pull::Up);

  /* Here, EXTICR1 register is filled between position 8-11 (column pin = 2) with
   * 0010 (ColumnGPIO = group C). */

  SYSCFG.EXTICR1()->setEXTI(columnPin, Keyboard::Device::ColumnGPIO);

  EXTI.EMR()->set(columnPin, true);

  /* When the key is pressed, it will go from 1 (because it's pulled up) to
   * zero (because it's connected to the open-drain output. In other words,
   * we're waiting for a falling edge. */
  EXTI.FTSR()->set(columnPin, true);
}

}
}
}