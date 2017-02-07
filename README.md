#**cairoMap**
This project is written in C++,given any position in map,it can show all the furthest place available in some distance.
#**result**:
##*All Roads  to Shanghai's People Square*
![Alt text](/results/shanghai5000.png?raw=true "Shanghai")
##*Whole City View*
![Alt text](/results/shanghai.png?raw=true "Shanghai")
#**Background**:
I made this project ,because i was fascinated by the project "dijkstra-cartography" https://github.com/ibaaj/dijkstra-cartography.
>The project is written in nodejs,I downloaded the source code of the project,initially baffled by many build errors,so i have to learn nodejs ,javascript,cairo to make the project successfully run.
There is huge computation involved in such project,nodejs is too inefficient in dealing with much computation,it will took several days for a computer to finish,i was never making that far,because my computer won't tolerate continuously working that long time .So I finanly decided to write a c++ project,in order to get the fascinated images .

#**recipe**:
-[OSM FILE](http://download.geofabrik.de/asia/china.html).

-Extracted shanghai.osm with [osmconvert](http://wiki.openstreetmap.org/wiki/Osmconvert), and polygon file.To get polygon file please refer [polygon](https://github.com/JamesChevalier/cities).

`osmconvert china-latest.osm.pbf -B=Shanghai.poly -o=shanghai.osm`
