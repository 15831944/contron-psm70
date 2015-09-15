
file_name=$(basename "${BASH_SOURCE[0]}")
dir_name="$(dirname "${BASH_SOURCE[0]}")"
#echo $dir_name
file_dir="$( cd "$dir_name" && pwd)"
echo test--------------------
echo $file_name

case $1 in
    add)
      sh "$file_dir/ip_add.sh" $2 $3
        ;;
    del)
      sh "$file_dir/ip_del.sh" $2 $3
        ;;
    search)
      sh "$file_dir/ip_search.sh" $2
        ;;
    *)
        echo usage
        ;;
esac

