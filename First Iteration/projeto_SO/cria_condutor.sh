#!/bin/bash
letras=$1":"$2":"$3":"$4":"$5":"$6":"$7":"$8":0:0:0.0"
i=0
if [ -f condutores.txt ]; then
	i=$( cat condutores.txt | cut -d ':' -f1 | grep $1 | wc -l)
fi
if [ $i -lt 1 ]; then
	if echo $letras >> condutores.txt ;then
		echo "O condutor foi gravado"
	else
		echo "Problemas em gravar o condutor"
	fi
else
	echo "Já existe o condutor com este numero"
fi
