
        echo search ip
        ifconfig | grep -i "$1" && echo success || echo fail
