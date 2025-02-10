import logging

# Bitreader from https://github.com/keshavdv/victron-ble/blob/main/victron_ble/devices/base.py
class BitReader:
    def __init__(self, data: bytes):
        self._data = data
        self._index = 0

    def read_bit(self) -> int:
        bit = (self._data[self._index >> 3] >> (self._index & 7)) & 1
        self._index += 1
        return bit

    def read_unsigned_int(self, num_bits: int) -> int:
        value = 0
        for position in range(0, num_bits):
            value |= self.read_bit() << position
        return value

    def read_signed_int(self, num_bits: int) -> int:
        return BitReader.to_signed_int(self.read_unsigned_int(num_bits), num_bits)

    @staticmethod
    def to_signed_int(value: int, num_bits: int) -> int:
        return value - (1 << num_bits) if value & (1 << (num_bits - 1)) else value



logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)-15s %(name)-8s %(levelname)s: %(message)s",
)

# Reading data related to: Solar Charger
# -------------------------------------------------------------------------------------------

input = "0x06,0x00,0x30,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x98,0x83,0xCB,0x3F,0xF5,0xA3,0x00,0x00,0x00"
data = bytes.fromhex(input.replace("0x","").replace(",",""))
reader = BitReader(data)

# This is the content of the solar charger profile,
charge_state = reader.read_unsigned_int(8)
charger_error = reader.read_unsigned_int(8)
# Battery voltage reading in 0.01V increments
battery_voltage = reader.read_signed_int(16) / 100
# Battery charging Current reading in 0.1A increments
battery_charging_current = reader.read_signed_int(16) / 10
# Todays solar power yield in 10Wh increments
yield_today = reader.read_unsigned_int(16) / 10
# Current power from solar in 1W increments
solar_power = reader.read_unsigned_int(16)
# External device load in 0.1A increments
external_device_load = reader.read_unsigned_int(9) /10

print("State:", charge_state)
print("Error:", charger_error)
print("Batt V:", battery_voltage)
print("Batt A:", battery_charging_current)
print("Yield:", yield_today)
print("PV:", solar_power)
print("Load:", external_device_load)

# Reading data related to: AC Charger
# -------------------------------------------------------------------------------------------

# input = "0x06,0x00,0x60,0xA5,0x05,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xA6,0xFF,0xFF,0xFF,0x3F,0x6B,0xDF,0x00,0x00,0x00"
# data = bytes.fromhex(input.replace("0x","").replace(",",""))
# reader = BitReader(data)

# device_state = reader.read_unsigned_int(8)
# charger_error = reader.read_unsigned_int(8)
# battery_voltage1 = reader.read_unsigned_int(13)
# battery_current1 = reader.read_unsigned_int(11)
# battery_voltage2 = reader.read_unsigned_int(13)
# battery_current2 = reader.read_unsigned_int(11)
# battery_voltage3 = reader.read_unsigned_int(13)
# battery_current3 = reader.read_unsigned_int(11)
# temp = reader.read_unsigned_int(7)
# current = reader.read_unsigned_int(9)

# print("State", device_state)
# print("Error", charger_error)
# print("Batt1 (V)", battery_voltage1/100, hex(battery_voltage1))
# print("Batt1 (A)", battery_current1/10, hex(battery_current1))
# print("Batt2 (V)", battery_voltage2/100, hex(battery_voltage2))
# print("Batt2 (A)", battery_current2/10, hex(battery_current2))
# print("Batt3 (V)", battery_voltage3/100, hex(battery_voltage2))
# print("Batt3 (A)", battery_current3/10, hex(battery_current3))
# print("Temp", temp-40)
# print("Total (A)", current/10)

# Reading data related to: Battery Monitor
# -------------------------------------------------------------------------------------------

# input = "0xFF,0xFF,0x8C,0x05,0x00,0x00,0x1C,0x05,0x00,0x00,0x00,0x0B,0x00,0x40,0xFE,0x00,0x3B,0xE7,0x00,0x00,0x00"
# data = bytes.fromhex(input.replace("0x","").replace(",",""))
# reader = BitReader(data)

# ttg = reader.read_unsigned_int(16)
# battery_voltage = reader.read_signed_int(16)
# alarm = reader.read_unsigned_int(16)
# variant = reader.read_unsigned_int(16)
# variant_type = reader.read_unsigned_int(2)
# battery_current = reader.read_unsigned_int(22)
# consumed = reader.read_unsigned_int(20)
# soc = reader.read_unsigned_int(10)

# print("TTG", ttg)
# print("Batt (V)", battery_voltage/100, hex(battery_voltage))
# print("Alarm", hex(alarm))
# print("Variant", variant/100, hex(variant))
# print("VariantType", variant_type)
# print("Batt (A)", battery_current/1000, hex(battery_current))
# print("Consumed", consumed/10, hex(consumed))
# print("SOC", soc/10, hex(soc))


