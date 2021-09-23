#!/bin/bash
if cat /etc/passwd | awk -F '[:]' '{print $1, $5, $1"@iscte-iul.pt"}' | grep '^a[0-9]' | sed 's/a//' | sed 's/,,,//' | sed -r 's/ +/:/' | sed 's/\(.*\) /\1:::/' | sed 's/$/:/' > passageiros.txt ; then
	echo "Os passageiros foram criados"
else
	echo "Houve problemas em criar os passageiros"
fi
