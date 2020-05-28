#!/bin/bash

# diseasesFile $1
# countriesFile $2
# input_dir $3
# numFilesPerDirectory $4
# numRecordsPerFile $5

if [ "$#" -ne 5 ]; then
    echo "You must enter exactly 5 arguments to run the script"
    exit 1
fi

input="countriesFile.txt"

while IFS= read -r line
do
    fileCounter=0
    mkdir -p $3/$line
    echo "TATAS"
    while [ $fileCounter -lt $4 ]
    do
        #generate random day and turn it in DD form if needed
        day=$((1 + RANDOM % 29))
        if [ "$day" -le 9 ]; then
            day=0$day
        fi
        #generate random month and turn it in MM form if needed
        month=$((1 + RANDOM % 11))
        if [ "$month" -le 9 ]; then
            month=0$month
        fi
        #generate random date in range [1900,2020]
        date=$day-$month-$((RANDOM % 120 + 1900))
        
        entryCounter=0
        while [ $entryCounter -lt $5 ]
        do
            id=2
            state="EXIT"
            ran=$((RANDOM % 10))
            # 60% probability of ENTER
            if [ "$ran" -le 6 ]; then
                state="ENTER"
            fi

            #create the entry
            entry=$id" "$state" "$(head /dev/urandom | tr -dc A-Za-z | head -c 13)" "$(head /dev/urandom | tr -dc A-Za-z | head -c 13)" "$(shuf -n 1 $input)" "$((RANDOM % 120 ))
            echo $entry >> $3/$line/$date
            ((entryCounter++))
        done
        ((fileCounter++))
    done
done < "$input"