import json
import zmq


class ScenePublisher:
    def __init__(self, endpoint: str) -> None:
        self._context = zmq.Context()
        self._socket = self._context.socket(zmq.PUSH)
        self._socket.connect(endpoint)

    def publish(self, scene: dict) -> None:
        self._socket.send_string(json.dumps(scene))

    def clear(self) -> None:
        self.publish(
            {
                "objects": []
            }
        )

    def close(self) -> None:
        self._socket.close()
        self._context.term()