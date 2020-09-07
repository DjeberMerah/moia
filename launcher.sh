#!/bin/bash

set -e

export LD_LIBRARY_PATH=/applis/sicstus-4.3.3/lib/

cd Communication/

make joueur -f Makefile

cd ..

cd srcMOIA/

javac -cp /applis/sicstus-4.3.3/lib/sicstus-4.3.3/bin/jasper.jar *.java
jar cvfm Engine.jar otherMF/META-INF/MANIFEST.MF *.class
nohup java -jar Engine.jar $1 &

sleep 1

cd ../

./Communication/joueur $2 $3 $1

fg
