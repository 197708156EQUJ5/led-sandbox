#!/bin/bash
url=$1
#curl -X POST "http://$1:8080/api/scene" \
#    -H "Content-Type: application/json" \
#    --data-binary "@test_data/scene_f1_open.json"
#sleep 3
#curl -X POST "http://$1:8080/api/scene" \
#    -H "Content-Type: application/json" \
#    --data-binary "@test_data/scene_clear.json"
#sleep 3
#for ((i = 0; i <= 5; i++))
#do
#    curl -X POST "http://$1:8080/api/scene" \
#        -H "Content-Type: application/json" \
#        --data-binary "@test_data/scene_f1_lights_out_light$i.json"
#    sleep 1 
#done
#curl -X POST "http://$1:8080/api/scene" \
#    -H "Content-Type: application/json" \
#    --data-binary "@test_data/scene_f1_lights_out_light0.json"
#sleep 1 
#curl -X POST "http://$1:8080/api/scene" \
#    -H "Content-Type: application/json" \
#    --data-binary "@test_data/scene_clear.json"
curl -X POST "http://$1:8080/api/scene" \
    -H "Content-Type: application/json" \
    --data-binary "@test_data/scene_f1_lap1.json"
sleep 7
curl -X POST "http://$1:8080/api/scene" \
    -H "Content-Type: application/json" \
    --data-binary "@test_data/scene_clear.json"
