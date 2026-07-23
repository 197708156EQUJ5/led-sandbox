#!/bin/bash
url=$1
curl -X POST "http://$1:8080/api/scene" \
    -H "Content-Type: application/json" \
    --data-binary "@test_data/scene_test.json"
#sleep 3
#curl -X POST "http://$1:8080/api/scene" \
#    -H "Content-Type: application/json" \
#    --data-binary "@test_data/scene_clear.json"
#sleep 3
#curl -X POST "http://$1:8080/api/scene" \
#    -H "Content-Type: application/json" \
#    --data-binary "@test_data/scene_broke.json"
#sleep 3
#curl -X POST "http://$1:8080/api/scene" \
#    -H "Content-Type: application/json" \
#    --data-binary "@test_data/scene_test.json"
#sleep 3
#curl -X POST "http://$1:8080/api/scene" \
#    -H "Content-Type: application/json" \
#    --data-binary "@test_data/scene_clear.json"
