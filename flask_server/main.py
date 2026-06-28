from flask_server import LedDisplayServer
from scene_publisher import ScenePublisher


class Main:
    def __init__(self) -> None:
        endpoint = "ipc:///tmp/led-display.sock"

        self._publisher = ScenePublisher(endpoint)
        self._server = LedDisplayServer(self._publisher)

    def run(self) -> None:
        try:
            self._server.app.run(
                host="0.0.0.0",
                port=8080,
                debug=False,
                threaded=False,
            )
        finally:
            self._publisher.close()


if __name__ == "__main__":
    app = Main()
    app.run()