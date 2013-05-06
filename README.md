[Description in English](#desc_eng)

# Описание

Данный репозиторий содержит схему и исходные коды прошивки микроконтроллера Atmel ATTINY25/45/85 для простого устройства
управления скоростью вращения 12/24В вентилятора в зависимости от температуры охлаждаемого им объекта.

# Лицензия

[Creative Commons СС0 1.0](http://creativecommons.org/publicdomain/zero/1.0/legalcode.txt).

# Аппаратная часть

Схема электрическая принципиальная аппаратной части в формате [Eagle PCB](http://www.cadsoftusa.com/eagle-pcb-design-software/)
находится в директории Schematic.

Для измерения температуры используется стандартный 10 кОм терморезистор TR1, включенный в верхнее плечо резисторного делителя напряжения.

Скорость вращения вентилятора регулируется путем ШИМ управляющего напряжения на затворе MOSFET-транзистора Q1.

Измерение температуры и коррекция скорости вращения вентилятора производятся микроконтроллером IC2 (Atmel ATTINY25/45/85).

# Программная часть

Управляющая программа написана на языке C и использует менее 700 байт FLASH-памяти микроконтроллера.

Измерение температуры производится путем аналого-цифрового преобразования напряжения с терморезистора
и последующего его преобразованием в температуру с помощью таблицы соответствия.

При температуре, меньшей чем `MOTOR_START_TEMP`, напряжение на вентилятор не подается.

При достижении указанной температуры напряжение на вентиляторе изменяется линейно путем ШИМ в диапазоне заполнения
импульсов от `PWM_MIN` до `PWM_MAX` (при температуре `MOTOR_FULL_TEMP` и выше).

Для измерения температуры и управления коэффициентом заполнения импульсов ШИМ используется управление по прерыванию.
Используется тактирование таймера от имеющегося в составе МК умножителя частоты с ФАПЧ, частота ШИМ при этом
составляет 62.5 кГц при тактировании от встроенного RC-генератора (8 МГц), что обеспечивает отсутствие
модуляционного шума от электродвигателя вентилятора в звуковом диапазоне.

В случае, если напряжение с делителя при измерении составит менее `ADC_THR_DISC`, управляющая программа считает, что произошел
обрыв в цепи терморезистора. При этом принудительно выставляется максимальная скорость вращения вентилятора.

## Компиляция

Для сборки используется [Eclipse IDE](http://www.eclipse.org/) версии не ниже 3.7 с установленным плагином
[AVR Eclipse](http://avr-eclipse.sourceforge.net/wiki/index.php/The_AVR_Eclipse_Plugin).

## Прошивка

При прошивке необходимо убедиться, что выбран внутренний RC-генератор (8 МГц), а также то, что
fuse-бит `CKDIV8` установлен в значение **NO** (тактирование без использования предварительного делителя на 8).

# Контакты

Вопросы и пожелания направляйте по адресу: <v.antonovich@gmail.com>.

---

# Description<a id="desc_eng"></a>

This repository contains schematic and MCU firmware source code for simple temperature-driven 12/24V fan speed controller.

# License

[Creative Commons СС0 1.0](http://creativecommons.org/publicdomain/zero/1.0/legalcode.txt).

# Hardware

Schematic in [Eagle PCB](http://www.cadsoftusa.com/eagle-pcb-design-software/) format can be found
in project Schematic directory.

Schematic uses 10K NTC thermistor TR1 in 1:1 resistor divider connected to ADC input of MCU IC2
(Atmel ATTINY25/45/85).

Fan speed controlled by PWM applied to gate of MOSFET Q1.

# Software

Control software is written in C language and uses about 700 bytes of MCU flash memory.

Divider output voltage measured by ADC is converted to temperature using lookup table.

Motor is stopped if measured temperature is below `MOTOR_START_TEMP`, otherwise is
driven using PWM duty cycle in range from `PWM_MIN` to `PWM_MAX` when `MOTOR_FULL_TEMP`
temperature is reached.

Both temperature measuring and PWM output control are interrupt-driven. Timer 1 used for PWM
is clocked from internal PLL frequency multiplier, so PWM frequency is about 62.5 kHz for
noise-free motor control.

Software do checks for thermistor breakage. In case if ADC measurement result is
less than `ADC_THR_DISC`, fan speed will be set at maximum value as failsafe action.

## Compilation

[Eclipse IDE](http://www.eclipse.org/) version 3.7 or higher is used with
[AVR Eclipse](http://avr-eclipse.sourceforge.net/wiki/index.php/The_AVR_Eclipse_Plugin)
plugin installed.

## Firmware uploading

Make sure fuses are configured for clocking from internal 8 MHz RC-oscillator with
prescaler disabled (`CKDIV8` fuse should be set to **NO**).

## Contacts

Feel free to send all your questions and suggestions to e-mail <v.antonovich@gmail.com>.