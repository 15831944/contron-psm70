        echo delete ip
        #ifconfig $2 del $1
	  ip -4 addr del $1 dev $2		
        ifconfig | grep -i "$1" && echo fail || echo success
