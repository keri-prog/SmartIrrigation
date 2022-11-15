import serial
import datetime
import sys
import numpy as np
import tensorflow as tf


COM = '/dev/ttyACM0'
BAUD = 9600
rain = False
SerialPort = serial.Serial(COM, BAUD, timeout=1)
SerialPort.flush()

model = tf.keras.models.load_model("../model/rainafall_detect.h5")

conditions = {
    "max_temp": None,
    "min_temp": None,
    "pressure9am": None,
    "humidity_9am": None,
    "temp_9am": None,
    "pressure_3pm": None,
    "humidity_3pm": None,
    "temp_3pm": None

}

while True:
    IncomingData = SerialPort.readline()
    if (IncomingData):

        break

while (True):

    time_now = datetime.datetime.now().time()
    model_input = [
        conditions["min_temp"],
        conditions["max_temp"],
        conditions["humidity_9am"],
        conditions["humidity_3pm"],
        conditions["pressure9am"],
        conditions["pressure_3pm"],
        conditions["temp_9am"],
        conditions["temp_3pm"]
    ]
    pred = model.predcit([model_input])[0][0]
    rain = True if pred > 0.5 else False

    try:
        if rain:
            OutgoingData = "rain"
            SerialPort.write(bytes(OutgoingData, 'utf-8'))
        else:
            OutgoingData = "no_rain"
            SerialPort.write(bytes(OutgoingData, 'utf-8'))
    except KeyboardInterrupt:
        print("Closing and exiting the program")
        SerialPort.close()
        sys.exit(0)

    IncomingData = SerialPort.readline()
    if IncomingData.in_waiting > 0:
        data = IncomingData.decode('utf-8').rstrip()
        data = list(map(float, data.split()))
        print(f"Temperature: {data[0]}\nPressure: {data[1]}\nHumidity: {data[2]}\n Water Level: {data[3]}\n Rain: {rain}")
        if time_now.hour == 0:
            conditions["max_temp"] = data[0]
            conditions["min_temp"] = data[0]
        elif time_now.hour == 9:
            conditions["temp_9am"] = data[0]
            conditions["pressure_9am"] = data[1]
            conditions["humidity_9am"] = data[2]
        elif time_now.hour == 15:
            conditions["temp_3pm"] = data[0]
            conditions["pressure_3pm"] = data[1]
            conditions["humidity_3pm"] = data[2]

        if data[0] > conditions["max_temp"]:
            conditions["max_temp"] = data[0]
        elif data[0] < conditions["min_temp"]:
            conditions["min_temp"] = data[0]
