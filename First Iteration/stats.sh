#!/bin/sh
#num condutores e passsageiros
numCondutores=$( cat condutores.txt | wc -l )
numPassageiros=$( cat passageiros.txt | wc -l)
echo "Número de condutores: $numCondutores. Número de Passageiros: $numPassageiros"

#Saldo total
saldoTotal=0
i=1
while [ $i -lt $(($numCondutores+1)) ]; do
	linhaCondutor=$(head -n $i condutores.txt | tail -n 1)
	saldoCondutor=$( echo $linhaCondutor | cut -d ':' -f11)
	saldoTotal=$( echo $saldoTotal+$saldoCondutor | bc )
	i=$(($i+1))
done
echo
echo "SaldoTotal: $saldoTotal"
#top 5 condutores
touch topFive.txt
i=1
while [ $i -lt $(($numCondutores+1)) ]; do
	linhaCondutor=$(head -n $i condutores.txt | tail -n 1)
	nomeCondutor=$( echo $linhaCondutor | cut -d ':' -f2)
	pontosCondutor=$( echo $linhaCondutor | cut -d ':'  -f10)
	echo "$pontosCondutor $nomeCondutor" >> topFive.txt
	i=$(($i+1))
done
cat topFive.txt | sort -r  > realtopFive.txt
rm topFive.txt
i=1
numLines=$( cat realtopFive.txt | wc -l )
echo
echo "TOP 5 condutores:"
while [ $i -lt $(($numLines+1)) ] && [ $i -lt 6 ]; do
	linhaShow=$( head -n $i realtopFive.txt | tail -n 1)
	echo "$linhaShow"
	i=$(( $i+1 ))
done
rm realtopFive.txt

#top 3 condutores com mais rating
touch topthree.txt
i=1
while [ $i -lt $(($numCondutores+1)) ]; do
	linhaCondutor=$( head -n $i condutores.txt | tail -n 1)
	nomeCondutor=$( echo $linhaCondutor | cut -d ':' -f2)
	pontosCondutor=$( echo $linhaCondutor | cut -d ':' -f10)
	numViagensCondutor=$( echo $linhaCondutor | cut -d ':' -f9)
	if [ $numViagensCondutor -gt 0 ]; then
		ratio=$(($pontosCondutor / $numViagensCondutor))
		echo "$ratio $nomeCondutor" >> topthree.txt
	else
		echo "0 $nomeCondutor" >> topthree.txt
	fi
	i=$(($i+1))
done
cat topthree.txt | sort -k1 -r -n > realtopThree.txt
rm topthree.txt
i=1
numLines=$( cat realtopThree.txt | wc -l)
echo
echo "TOP 3 com maior rating:"
while [ $i -lt $(($numLines+1)) ] && [ $i -lt 4 ]; do
	linhaShow=$( head -n $i realtopThree.txt | tail -n 1 )
	echo "$linhaShow"
	i=$(($i+1))
done
rm realtopThree.txt

#estaticas por curso
LEI=0
IGE=0
ETI=0
OUTROS=0
i=1
numLines=$(cat condutores.txt | wc -l) 
while [ $i -lt $(($numLines+1)) ]; do
	linhaCondutor=$( head -n $i condutores.txt | tail -n 1 )
	curso=$( echo $linhaCondutor | cut -d ':' -f3)
	case $curso in
		EIB*|LEI*) LEI=$(($LEI+1));;
		ETB*|ETI*) ETI=$(($ETI+1));;
		IB*|IGE*) IGE=$(($IGE+1));;
		*) OUTROS=$(($OUTROS+1));;
	esac
	i=$(($i+1))
done
echo
echo "Condutores por cursos:"
echo "LEI: $LEI , IGE: $IGE , ETI: $ETI, OUTROS:$OUTROS"
