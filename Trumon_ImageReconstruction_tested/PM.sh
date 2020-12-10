#!/bin/bash

#script untuk restart aplikasi

date

var=$(ps axu | grep -v grep | grep "PrintEmulator 127.0.0.1 trumon 3306 prasimax 'pmx123'")
        if [ ! -z "$var" ];
                then echo "found";
        else
          while true
          do
		./PrintEmulator 127.0.0.1 trumon 3306 prasimax 'pmx123'|more
          done
fi

