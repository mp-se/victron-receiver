import logging
import json
import os, sys

logger = logging.getLogger("convert")

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)-15s %(name)-8s %(levelname)s: %(message)s",
)

args = len(sys.argv)
if args != 2:
  print("Usage: convert.py payload_file")
  exit(0)

with open(sys.argv[1], 'r') as file:
    data = json.load(file)
    print("VictronTestData testData[1] = [")
    print("\t{ \"" + data["name"] + "\", " + data["model"] + ", 0x00, { " + data["decrypted_data"] + " } }")
    print("];")

logger.info("Exit from payload converter")
