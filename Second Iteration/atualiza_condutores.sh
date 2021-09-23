#!/bin/bash
if [ -f viagens.txt ]; then
	touch  atualizar.txt
	n=$( cat viagens.txt | wc -l)
	i=1
	l=$( cat condutores.txt | wc -l )
	while [ $i -lt $(($l+1)) ]; do
		linhaCondutor=$(head -n $i condutores.txt | tail -n 1)
		numeroCondutor=$( echo $linhaCondutor | cut -d':' -f1)
		laux=1
        cat viagens.txt | awk -v numeroCondutor="$numeroCondutor" -F':' '$2==numeroCondutor' > temp.txt
        numLinhasViagensCondutor=$(cat temp.txt | wc -l)
        oldViagensCondutor=$( echo $linhaCondutor | cut -d ':' -f9)
        oldPontosCondutor=$( echo $linhaCondutor | cut -d ':' -f10)
        oldSaldoCondutor=$( echo $linhaCondutor | cut -d ':' -f11)
        numViagensCondutor=0
        numPontosCondutor=0
        numSaldoCondutor=0
		   while [ $laux -lt $(($numLinhasViagensCondutor+1)) ]; do 
			   linhaViagem=$( head -n $laux temp.txt | tail -n 1)
               numViagensCondutor=$(($numViagensCondutor+1))
               pViagem=$( echo $linhaViagem | cut -d ':' -f4)
			   numPontosCondutor=$(($numPontosCondutor+$pViagem))
               sViagem=$( echo $linhaViagem | cut -d ':' -f5)
               numSaldoCondutor=$(echo $numSaldoCondutor+$sViagem | bc)
               laux=$(($laux+1 ))
           done
        newViagensCondutor=$(($oldViagensCondutor+$numViagensCondutor))
        newPontosCondutor=$(($oldPontosCondutor+$numPontosCondutor))
        newSaldoCondutor=$(echo $oldSaldoCondutor+$numSaldoCondutor | bc)
		nomeCondutor=$( echo $linhaCondutor | cut -d ':' -f2)
        curso=$( echo $linhaCondutor | cut -d ':' -f3)
	    numTelemovel=$( echo $linhaCondutor | cut -d ':' -f4)
	    email=$( echo $linhaCondutor | cut -d ':' -f5)
	    veiculo=$( echo $linhaCondutor | cut -d ':' -f6)
	    marca=$( echo $linhaCondutor | cut -d ':' -f7)
	    matricula=$( echo $linhaCondutor | cut -d ':' -f8)
	     echo "$numeroCondutor":"$nomeCondutor":"$curso":"$numTelemovel":"$email":"$veiculo":"$marca":"$matricula":"$newViagensCondutor":"$newPontosCondutor":"$newSaldoCondutor" >> atualizar.txt
		i=$(($i+1))
	done
    rm temp.txt
	cp atualizar.txt condutores.txt
	rm atualizar.txt
	rm viagens.txt
    echo "Os dados de todos os condutores foram atualizados com sucesso!"
else
	echo "Não existe nada para atualizar"
fi
