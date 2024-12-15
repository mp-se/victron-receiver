import logging
import json
# import time
# import os
# from uuid import UUID
from construct import Array, Byte, Const, Int8sl, Int16ub, Int16sb, Int32ub, Int16sl, Int16ul, Int32ul, Int8ul, Struct, GreedyBytes
# from construct.core import ConstError


# ibeacon_format = Struct(
#     "type_length" / Const(b"\x02\x15"),
#     "uuid" / Array(16, Byte),
#     "major" / Int16ub,
#     "minor" / Int16ub,
#     "power" / Int8sl,
# )

# eddystone_format = Struct(
#     "type_length" / Const(b"\x20\x00"),
#     "battery" / Int16ub,
#     "temp" / Int16ub,
#     "gravity" / Int16ub,
#     "angle" / Int16ub,
#     "chipid" / Int32ub,
# )

# Generic structure
generic = Struct(
     "byte" / Array(21, Byte),
)

logger = logging.getLogger("parse")

def parse_ac_charger(fName):

    ac_charger = Struct(
        "state" / Int8ul, # Correct
        "error" / Int8ul, # Assumed correct
        "batteryVoltage1" / Int16ul, # Correct (Need filter 0x1fff)
        
        "A" / Int16ul, # Not used ?
        "B" / Int16ul,
        "C" / Int16ul,
        "D" / Int16ul,
        "E" / Int16ul,
       
        # "E2" / Int8ul,

        "F" / Int16ul, # Temp ?
        "G" / Int16ul, # Current ?
        "H" / Int16ul,
        GreedyBytes
    )

    with open(fName, 'r') as file:
        data = json.load(file)

    s = bytes.fromhex(data["decrypted_data"].replace("0x","").replace(",",""))

    d = ac_charger.parse(s)
    # logger.info(f"AC - State: {d['state']} ")
    # logger.info(f"AC - Error {d['error']} ")

    d['batteryVoltage1'] = d['batteryVoltage1']&0x1FFF
    # d['batteryVoltage2'] = d['batteryVoltage2']&0x1FFF
    # d['batteryVoltage3'] = d['batteryVoltage3']&0x1FFF

    #d['batteryCurrent1'] = d['batteryCurrent1']&0x7FF
    # d['batteryCurrent2'] = d['batteryCurrent2']&0x7FF
    # d['batteryCurrent3'] = d['batteryCurrent3']&0x7FF

    # d['temperature'] = d['temperature']&0x7F
    # d['acCurrent'] = d['acCurrent']&0x1FF

    d['A'] = d['A']&0x1FF
    # d['G'] = d['G']&0x1FF
    # d['H'] = d['H']&0x1FF

    logger.info(f"AC - BattVoltage1 { 0 if d['batteryVoltage1'] == 0X1FFF else d['batteryVoltage1'] / 100 } ({hex(d['batteryVoltage1'])})")

    logger.info(f"AC - A {d['A']/10} ({hex(d['A'])})")
    # logger.info(f"AC - B ({hex(d['B'])})")
    # logger.info(f"AC - C ({hex(d['C'])})")
    # logger.info(f"AC - D ({hex(d['D'])})")
    # logger.info(f"AC - E ({hex(d['E'])})")

    # logger.info(f"AC - F {d['F']/10} ({hex(d['F'])})")
    # logger.info(f"AC - G {d['G']/10} ({hex(d['G'])})")
    # logger.info(f"AC - H {d['H']/10} ({hex(d['H'])})")

    # logger.info(f"AC - BattCurrent1 { 0 if d['batteryCurrent1'] == 0x7FF else d['batteryCurrent1'] / 10 } ({hex(d['batteryCurrent1'])})")
    # logger.info(f"AC - BattVoltage2 { 0 if d['batteryVoltage2'] == 0X1FFF else d['batteryVoltage2'] / 100 } ({hex(d['batteryVoltage2'])})")
    # logger.info(f"AC - BattCurrent2 { 0 if d['batteryCurrent2'] == 0x7FF else d['batteryCurrent2'] / 10 } ({hex(d['batteryCurrent2'])})")
    # logger.info(f"AC - BattVoltage3 { 0 if d['batteryVoltage3'] == 0X1FFF else d['batteryVoltage3'] / 100 } ({hex(d['batteryVoltage3'])})")
    # logger.info(f"AC - BattCurrent3 { 0 if d['batteryCurrent3'] == 0x7FF else d['batteryCurrent3'] / 10 } ({hex(d['batteryCurrent3'])})")

    # logger.info(f"AC - Temperature { 0 if d['temperature'] == 0x7F else d['temperature'] - 40 } ({hex(d['temperature'])})")
    # logger.info(f"AC - AC Current { 0 if d['acCurrent'] == 0x1FF else d['acCurrent'] / 10 } ({hex(d['acCurrent'])})")


def parse_dcdc_charger(fName):
    dcdc_charger = Struct(
        "state" / Int8ul,
        "error" / Int8ul,
        "inputVoltage" / Int16ul,
        "outputVoltage" / Int16sl,
        "offReason" / Int32ul,
        GreedyBytes
    )

    with open(fName, 'r') as file:
        data = json.load(file)

    s = bytes.fromhex(data["decrypted_data"].replace("0x","").replace(",",""))

    d = dcdc_charger.parse(s)
    logger.info(f"DCDC - State: {d['state']} ")
    logger.info(f"DCDC - Error {d['error']} ")
    logger.info(f"DCDC - InVoltage {d['inputVoltage'] / 100 } ")
    logger.info(f"DCDC - OutVoltage { 0 if d['outputVoltage'] == 0x7fff else d['outputVoltage'] / 100 } ")
    logger.info(f"DCDC - OffReason {d['offReason']} ")


def parse_smartmon(fName):
    smartmon = Struct(
        "unused" / Int16ul,
        "batteryVoltage" / Int16sl,
        "alarm" / Int16ul,
        "temperature" / Int16ul,
        GreedyBytes
    )

    with open(fName, 'r') as file:
        data = json.load(file)

    s = bytes.fromhex(data["decrypted_data"].replace("0x","").replace(",",""))

    d = smartmon.parse(s)
    logger.info(f"MON - alarm: {d['alarm']} ")
    logger.info(f"MON - BattVoltage {d['batteryVoltage'] / 100 } ")
    logger.info(f"MON - temperature {d['temperature'] / 100 - 273.15} ")


def parse_dump(fName):
    dump = Struct(
        "1" / Int8ul,
        "2" / Int8ul,
        "3" / Int16ul,

        "4" / Int16ul,
        "5" / Int16ul,
        "6" / Int16ul,
        "7" / Int16ul,
        "8" / Int16ul,
        "9" / Int16ul,
        "A" / Int16ul,
        "B" / Int16ul,
        GreedyBytes
    )

    with open(fName, 'r') as file:
        data = json.load(file)

    s = bytes.fromhex(data["decrypted_data"].replace("0x","").replace(",",""))

    d = dump.parse(s)
    # logger.info(f"1 {d['1']} {hex(d['1'])}")
    # logger.info(f"2 {d['2']} {hex(d['2'])}")
    # logger.info(f"3 {d['3']} {hex(d['3']&0x1fff)}")
    logger.info(f"4 {d['4']} {hex(d['4'])}")
    logger.info(f"5 {d['5']} {hex(d['5'])}")
    logger.info(f"6 {d['6']} {hex(d['6'])}")
    logger.info(f"7 {d['7']} {hex(d['7'])}")
    logger.info(f"8 {d['8']} {hex(d['8']&0x1fff)} {d['8']&0x1ff} {hex(d['8']&0x1ff)}")
    logger.info(f"9 {d['9']} {hex(d['9']&0x1fff)} {d['9']&0x1ff} {hex(d['9']&0x1ff)}")
    logger.info(f"A {d['A']} {hex(d['A']&0x1fff)} {d['A']&0x1ff} {hex(d['A']&0x1ff)}")
    # logger.info(f"B {d['B']} {hex(d['B']&0x1fff)} {d['B']&0x1ff} {hex(d['B']&0x1ff)}")


def main():
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)-15s %(name)-8s %(levelname)s: %(message)s",
    )

    # parse_dcdc_charger('dcdc_charger.json')
    # parse_ac_charger('ac_charger.json')
    # parse_ac_charger('ac_charger1.json')
    # parse_ac_charger('ac_charger2.json')
    # parse_ac_charger('ac_charger3.json')
    # # parse_smartmon('smart_monitor.json')
 
    # parse_dump('ac_charger.json')
    # parse_dump('ac_charger1.json')
    # parse_dump('ac_charger2.json')
    # parse_dump('ac_charger3.json')

    # "decrypted_data": "0x06,0x00,0x2C,0x05,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x86,0xCA,0x00,0x00,0x00",
    # "decrypted_dat1": "0x04,0x00,0xA0,0xA5,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x03,0x06,0x00,0x73,0x00,0x00,0x00,0x00",
    # "decrypted_dat2": "0x04,0x00,0xA0,0x65,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x07,0x06,0x00,0xF4,0x01,0x00,0x00,0x00",
    # "decrypted_dat3": "0x05,0x00,0x65,0x05,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x07,0x06,0x00,0x47,0x07,0x00,0x00,0x00",
    # "decrypted_dat4": "0x04,0x00,0xA0,0x25,0x04,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0x7C,0x00,0x00,0x00,0x00",
    #                    0x10,0x02,0x89,0xa3,0x02,0xb0,0x40,0xaf,0x92,0x5d,0x09,0xa4,0xd8,0x9a,0xa01,0x28,0xbd,0xef4,0x8c,0x62,0x98,0xa9

    #                    

    a = 0x747 # 0x0073 # 0xca86

    print(f"{a&0x1ff} {a&0x7ff}")

main()
logger.info("Exit from data parser")
