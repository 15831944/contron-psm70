        echo add ip
        #ifconfig $2 add $1
	  ip -4 addr add $1 dev $2
        ifconfig | grep -i "$1" && echo success || echo fail
