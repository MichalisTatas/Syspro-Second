#include "../include/queries.h"

int queriesHandler(workerInfoPtr workersList,const char* querie)
{
    wordexp_t p;
    wordexp(querie, &p, 0);

    if (!strcmp(p.we_wordv[0], "/listCountries")) {
        printf("/listCountries\n");
        if (p.we_wordc != 1) {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
    }
    else if (!strcmp(p.we_wordv[0], "/diseaseFrequency")) {
        printf("/diseaseFrequency\n");
        // if (p.we_wordc == 4) {
        //     //call function
        // }
        // else if (p.we_wordc == 5) {
        //     //call selectworker
        //     //callfunction
        // }
        // else {
            // printf("number of arguments is not right! \n");
            // wordfree(&p);
            // return -1;
        // }
    }
    else if (!strcmp(p.we_wordv[0], "/topk-AgeRanges")) {
        printf("/topk-AgeRanges\n");
        // if (p.we_wordc == 6) {
        //     //call function
        // }
        // else {
        //     printf("number of arguments is not right! \n");
        //     wordfree(&p);
        //     return -1;
        // }
    }
    else if (!strcmp(p.we_wordv[0], "/searchPatientRecord")) {
        printf("/searchPatientRecord\n");
        // if (p.we_wordc == 2) {
        //     // call function
        // }
        // else {
        //     printf("number of arguments is not right! \n");
        //     wordfree(&p);
        //     return -1;
        // }
    }
    else if (!strcmp(p.we_wordv[0], "/numPatientAdmissions")) {
        printf("/numPatientAdmissions\n");
        // if (p.we_wordc == 4) {
        //     // call funtion
        // }
        // else if (p.we_wordc == 5) {
        //     //call selectWorker
        //     // call function
        // }
        // else {
        //     printf("number of arguments is not right! \n");
        //     wordfree(&p);
        //     return -1;
        // }
    }
    else if (!strcmp(p.we_wordv[0], "/numPatientDischarges")) {
        printf("/numPatientDischarges\n");
        // if (p.we_wordc == 4) {
        //     //call function
        // }
        // else if (p.we_wordc == 5) {
        //     //call selectWorker
        //     //call function
        // }
        // else {
        //     printf("number of arguments is not right! \n");
        //     wordfree(&p);
        //     return -1;
        // }
    }
    else if (!strcmp(p.we_wordv[0], "/exit")) {
        printf("/exit\n");
        if (p.we_wordc != 1) {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
        wordfree(&p);
        return -1;
    }

    wordfree(&p);
    return 0;
}

int selectWorker(workerInfoPtr workersList, char* country) // returns readDesc of worker which handles the country
{
    workerInfoPtr iterator = workersList;
    countryPtr countryIterator;
    while (iterator != NULL) {
        countryIterator = iterator->countriesList;
        while (countryIterator != NULL) {
            if (!strcmp(countryIterator->name, country)) {
                return iterator->read;
            }
            countryIterator = countryIterator->next;
        }
        iterator = iterator->next;
    }
    return -1;
}