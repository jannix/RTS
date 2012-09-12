#!/bin/bash

if [[ -n "$1" ]]
then	if [[ "$1" == "-h" || "$1" == "-help" ]]
	then	echo "synthax is: $0 RIGHT USER [FOLDER]
default folder is current
rights are:
\"all\" or \"admin\" for \"rlidwka\"
\"write\" for \"rlidw\"
\"read\" for \"rl\"
\"list\" for \"l\"
\"no\" or \"none\" for nothing"
	elif [[ -n "$2" ]]
	then	if [[ -n "$3" ]]
		then	$DIR=$3;
		else	DIR=./
		fi
		if [[ $1 == 'all' || $1 == 'admin' ]]
		then	fs setacl -dir $DIR -acl $2 rlidwka
		elif [[ $1 == 'write' ]]
		then	fs setacl -dir $DIR -acl $2 rlidw
		elif [[ $1 == 'read' ]]
		then	fs setacl -dir $DIR -acl $2 rl
		elif [[ $1 == 'list' ]]
		then	fs setacl -dir $DIR -acl $2 l
		elif [[ $1 == 'no' || $1 == 'none' ]]
		then	fs setacl -dir $DIR -acl $2 none
		else	echo "unvalid right argument"
			exit 0;
		fi
		if [[ $? == 0 ]]
		then	echo "$2 now have $1 right on $DIR"
		fi
	fi
else	echo "SYNTHAX ERROR
Correct synthax is: $0 RIGHT USER [FOLDER]
For more help, use -h or -help option";
fi
