tr -d '\0' < ./test1/bytes.txt > ./test1/bytes_cleaned.txt # нужна для запуска через мейк файл теста (исключает \0 и пересоздает файлы без них)
tr -d '\0' < ./test1/3.csv > ./test1/3_cleaned.csv

first_file="./test1/bytes_cleaned.txt"
second_file="./test1/3_cleaned.csv"


first_file="./test1/bytes_cleaned.txt"
second_file="./test1/3_cleaned.csv"

test_1=$(cat $first_file)
test_1_my=$(./s21_cat $first_file)

if [ "$test_1" = "$test_1_my" ]
then
    echo -e "default_test: \033[32mSUCCESS\033[0m"
else
    echo -e "default_test: \033[31mFAIL\033[0m"
fi

echo

for flags in "-n" "-b" "-s" "-e" "-v" "-t"
do
    test_2=$(cat $flags $first_file)
    test_2_my=$(./s21_cat $flags $first_file)
    
    if [ "$test_2" = "$test_2_my" ]
    then
        echo -e "flags_test ($flags): \033[32mSUCCESS\033[0m"
    else
        echo -e "flags_test ($flags): \033[31mFAIL\033[0m"
    fi
done

echo

test_3=$(cat $first_file $second_file)
test_3_my=$(./s21_cat $first_file $second_file)

if [ "$test_3" = "$test_3_my" ]
then
    echo -e "test_many_files: \033[32mSUCCESS\033[0m"
else
    echo -e "test_many_files: \033[31mFAIL\033[0m"
fi

echo

for flags in "-n" "-b" "-s" "-e" "-v" "-t"
do
    test_4=$(cat $flags $first_file $second_file)
    test_4_my=$(./s21_cat $flags $first_file $second_file)
    
    if [ "$test_4" = "$test_4_my" ]
    then
        echo -e "test_flags_many_files ($flags): \033[32mSUCCESS\033[0m"
    else
        echo -e "test_flags_many_files ($flags): \033[31mFAIL\033[0m"
    fi
    
done

rm -f s21_cat
rm -f ./test1/bytes_cleaned.txt ./test1/3_cleaned.csv
