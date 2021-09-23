#!/bin/bash
i=1
n=$(cat condutores.txt | wc -l)
while [ $i -lt $(( $n+1 )) ] ; do
	pontos=$( head -n $i condutores.txt | tail -n 1 | cut -d ':' -f10)
	viagens=$( head -n $i condutores.txt | tail -n 1 | cut -d ':' -f9)
	if [ $viagens -gt 0 ]; then
		media=$(($pontos/$viagens))
		if [ $media -lt 5 ]; then
			head -n $i condutores.txt | tail -n 1 | cut -d ':' -f5 >> enviar
			head -n $i condutores.txt | tail -n 1 | cut -d ':' -f11 >> saldos
		fi
	fi
	i=$(( $i+1 ))
done
if [ -f enviar ]; then
	a=$( cat enviar | wc -l)
	echo "Deseja enviar email ao maus condutores?"
	read resposta
	if [ $resposta == sim ]; then
		k=1
		while [ $k -lt $(( $a+1 )) ] ; do
			echo "O seu saldo é $(head -n $k saldos | tail -n 1) se continuar assim vai ser excluido da plataforma!" | mail -s 'LYFTIUL' $(head -n $k enviar | tail -n 1)
			echo "Foi enviado o email para $(head -n $k enviar | tail -n 1 )"
			k=$(( $k+1 ))	
		done
		echo "Emails enviados com sucesso!"
	fi
	rm enviar
	rm saldos
else
	echo "Não existem condutores com rating abaixo de 5 valores!"
fi
