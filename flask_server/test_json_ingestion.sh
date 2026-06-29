#!/bin/bash

curl -X POST http://localhost:8080/api/scene \
    -H "Content-Type: application/json" \
    --data-binary "@test_data/scene_test.json"
sleep 3
curl -X POST http://localhost:8080/api/scene \
    -H "Content-Type: application/json" \
    --data-binary "@test_data/scene_clear.json"
sleep 3
curl -X POST http://localhost:8080/api/scene \
    -H "Content-Type: application/json" \
    --data-binary "@test_data/scene_broke.json"
sleep 3
curl -X POST http://localhost:8080/api/scene \
    -H "Content-Type: application/json" \
    --data-binary "@test_data/scene_test.json"
sleep 3
curl -X POST http://localhost:8080/api/scene \
    -H "Content-Type: application/json" \
    --data-binary "@test_data/scene_clear.json"
