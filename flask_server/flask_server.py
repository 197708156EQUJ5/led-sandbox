from flask import Flask, jsonify, request

from scene_publisher import ScenePublisher


class LedDisplayServer:
    def __init__(self, publisher: ScenePublisher) -> None:
        self._publisher = publisher
        self.app = Flask(__name__)

        self.app.add_url_rule(
            "/api/scene",
            view_func=self.publish_scene,
            methods=["POST"],
        )

        self.app.add_url_rule(
            "/api/clear",
            view_func=self.clear_display,
            methods=["POST"],
        )

    def publish_scene(self):
        scene = request.get_json(silent=True)

        if scene is None:
            return jsonify(
                {
                    "error": "Request body must contain JSON."
                }
            ), 400

        objects = scene.get("objects")

        if not isinstance(objects, list):
            return jsonify(
                {
                    "error": "JSON must contain an 'objects' array."
                }
            ), 400

        self._publisher.publish(scene)

        return jsonify(
            {
                "status": "published",
                "objectCount": len(objects),
            }
        ), 200

    def clear_display(self):
        self._publisher.clear()

        return jsonify(
            {
                "status": "cleared"
            }
        ), 200