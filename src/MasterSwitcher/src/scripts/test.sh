
file_name=$(basename "${BASH_SOURCE[0]}")

case $1 in
    add)
        echo add ip
        ifconfig $3 add $2
        ifconfig | grep -i "$2" && echo success || echo fail
        ;;
    del)
        echo delete ip
        ifconfig $3 del $2
        ifconfig | grep -i "$2" && echo fail || echo success
        ;;
    search)
        echo search ip
        ifconfig | grep -i "$2" && echo success || echo fail
        ;;
    *)
        echo usage
        ;;
esac
