#include "mrl.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]){
    struct MRL_T mrl;
    MXPSQL_MRL_init_ss(&mrl, "> ", stdin);

    char* line = NULL;
    while(1){
        line = MXPSQL_MRL_readline(&mrl, 1, 1);
        if(line != NULL){
            printf("%s\n", line);
            if(strcmp(line, "exit") == 0){
                break;
            }
        }else{
            printf("bed");
            break;
        }
    }

    {
        /* printf("\n%s\n", MXPSQL_MRL_get_history_str(&mrl)); */ 
        printf("\nHistory:\n");
        int i;
        char** histr = MXPSQL_MRL_get_history_array(&mrl);
        for(i = 0; i < MXPSQL_MRL_MAX_HISTORY_LEN; i++){
            if(histr[i] != NULL){
                printf("%s\n", histr[i]);
            }
        }
    }

    MXPSQL_MRL_free(&mrl);

    return 0;
}