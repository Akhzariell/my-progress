#!/bin/bash
grep="grep "
s21_grep="./s21_grep"
SUCCESS=0
FAIL=0
COUNTER=0

RED="\033[31m"
GREEN="\033[32m"
BLUE="\033[36m"
ENDCOLOR="\033[0m"

grep_c="s21_grep.c"

testing(){
    ./s21_grep $var $grep_c> test_s21_grep.log 
    grep $var $grep_c > test_grep.log
    if cmp -s test_s21_grep.log test_grep.log
    then
    (( SUCCESS++ ))
    (( COUNTER++ ))

    echo "${GREEN}№$COUNTER Success${ENDCOLOR} $s21_grep $var $grep_c"
    else
    (( FAIL++ ))
    (( COUNTER++ ))

    echo "${RED}№$COUNTER Fail${ENDCOLOR} $s21_grep $var $grep_c"
    fi
    rm -f ./test_s21_grep.log ./test_grep.log
}


echo "${BLUE}No flag${ENDCOLOR}"
for var in 0 A int
do  
    testing "$var"
done
echo "\n"

echo "${BLUE}1 flag, 1 file, except e, f, s${ENDCOLOR}"
for var1 in v c l i h o n 
do  
    var="0 -$var1"
    testing "$var"
done
echo "\n"

echo "${BLUE}2 flags, 1 file, except e, f, s${ENDCOLOR}"
for var1 in v c l i h o n
do  
    for var2 in v c l i h o n
    do  
        if [ $var1 != $var2 ]
        then
            var="0 -$var1 -$var2"
            testing "$var"
        fi
    done
done
echo "\n"

echo "${BLUE}1 flag, 2 files, except e, f, s${ENDCOLOR}"
for var1 in v c l i h o n
do  
    var="0 -$var1 s21_grep.c"
    testing "$var s21_grep.c"
done
echo "\n"

echo "${BLUE}2 flag, 3 files, except e, f, s${ENDCOLOR}"
for var1 in v c l i h o n
do  
    for var2 in v c l i h o n
    do  
        if [ $var1 != $var2 ]
        then
            var="0 -$var1 s21_grep.c -$var2 s21_grep.c"
            testing "$var"
        fi
    done
done
echo "\n"

echo "${BLUE}Only s flag${ENDCOLOR}"
for var1 in s21_grep.c grep.h gre.b grep.tlsdf s21 
do  
    var="A -s $var1"
    testing "$var"
done
echo "\n"

echo "${BLUE}Only e flag${ENDCOLOR}"
for var1 in " '[0-9]'" " '[a-z]'" "'a'" " '[0-9].*'"
do  
    var="-e$var1"
    testing "$var"
done
echo "\n"

echo "${BLUE}Only f flag${ENDCOLOR}"
for var1 in "1.txt" "2.txt"
do  
    var="-f $var1"
    testing "$var"
done
echo "\n"

echo "${BLUE}2 f flags${ENDCOLOR}"
for var1 in "1.txt" "2.txt"
do
    for var2 in "3.txt" "4.txt"    
    do  
        var="-f $var1 -f $var2"
        testing "$var"
    done
done
echo "\n"

echo "${BLUE}2 e flags${ENDCOLOR}"
for var1 in " '[0-9]'" " '[a-z]'" "'a'" " '[0-9].*'"
do
    for var2 in " 1" " '[a-x]'" "'vo'" "'.*ap.*'"    
    do  
        var="-e$var1 -e$var2"
        testing "$var"
    done
done
echo "\n"

echo "${BLUE}Multiflag${ENDCOLOR}"
for var1 in iv ivn ilc ivcls hnv nie 
do  
    var="-$var1 Int"
    testing "$var"
done

echo "${GREEN}SUCCESS $SUCCESS ${ENDCOLOR} | ${RED} FAILED $FAIL${ENDCOLOR}";
