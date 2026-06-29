import json
import time
import zmq

context = zmq.Context()
socket = context.socket(zmq.PUSH)
socket.connect("ipc:///tmp/led-display.sock")

time.sleep(0.1)

scene = {
    "objects": [
        {
            "type": "circle",
            "position": {"x": 16, "y": 5},
            "radius": 4,
            "color": "#00ff00",
            "fill": True,
        }
    ]
}

socket.send_string(json.dumps(scene))
print("{}".format(scene))
time.sleep(0.1)

socket.close()
context.term()