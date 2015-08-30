ping -c $2 $1 | grep -i "ttl=" && echo success || echo fail
