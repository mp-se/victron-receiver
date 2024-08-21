#!/usr/bin/env python
import asyncio
import signal
import websockets

async def readSerialWS():  
  host = "192.168.1.188" # Change to the ip adress of the device

  async with websockets.connect("ws://" + host + "/serialws") as websocket:
    
    async for message in websocket:
      message = message.strip("\n")
      print(f"Received: {message}")

asyncio.run(readSerialWS())
