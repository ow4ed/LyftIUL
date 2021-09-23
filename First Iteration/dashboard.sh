#!/bin/bash
if [ -f ~/public_html/stats.html ]; then
	rm ~/public_html/stats.html
fi
touch ~/public_html/stats.html
{
echo "<html>"
echo "<head>"
echo "	<meta charset="UTF-8">"
echo "	<title>Estatisticas LyftIUL</title>"
echo "</head>" 
echo "<body>" 
echo "<h1>Condutores</h1>"
if [ -f maus.txt ]; then #caso ocorra erro em apagar o ficheiro antes
 rm maus.txt
fi

i=1

numCondutores=$(cat ~/projeto/condutores.txt | wc -l)

while [ $i -lt $(( $numCondutores+1 )) ]; do
	linhaCondutor="$(head -n $i ~/projeto/condutores.txt | tail -n 1)"
	nome="$( echo $linhaCondutor | cut -d ':' -f2)"
	num="$( echo $linhaCondutor | cut -d ':' -f1)"
	viagens="$( echo $linhaCondutor | cut -d ':' -f9)"
	saldo="$( echo $linhaCondutor | cut -d ':' -f11)"
	pontos="$( echo $linhaCondutor | cut -d ':' -f10 )"
	rating=0
	if [ $viagens -gt 0 ]; then
		rating="$(( $pontos / $viagens ))"
	fi
	if [ $rating -lt 5 ]; then
		echo "$num, "$nome", viagens: $viagens, pontos: $pontos, rating: $rating, saldo: $saldo" >> maus.txt
	fi	
	echo "$num, "$nome", viagens: "$viagens", pontos: "$pontos", rating: "$rating", saldo: "$saldo"<br>"
	i=$(( $i+1 ))
done

echo "<h1>Passageiros</h1>"

i=1
numPassageiros=$(cat ~/projeto/passageiros.txt | wc -l)
while [ $i -lt $(($numPassageiros + 1)) ]; do
	linhaPassageiro=$(head -n $i ~/projeto/passageiros.txt | tail -n 1)
	num=$( echo $linhaPassageiro | cut -d ':' -f1)
	nome=$( echo $linhaPassageiro | cut -d ':' -f2)
	curso=$( echo $linhaPassageiro | cut -d ':' -f3)
	echo "$num, $nome, $curso<br>"
	i=$(($i+1))
done

echo "<h1>Maus Condutores</h1>"

i=1
numMaus=$( cat maus.txt | wc -l) 

while [ $i -lt $(($numMaus+1)) ]; do
	linhaMau=$( head -n $i maus.txt | tail -n 1)
	echo "$linhaMau<br>"
	i=$(($i+1))
done

rm maus.txt

echo "</body>"
echo "</html>"
} >> ~/public_html/stats.html

