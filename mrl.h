#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* windows or posix */
#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#define isatty _isatty
#ifndef STDIN_FILENO
	#define STDIN_FILENO _fileno(stdin)
#endif
#define fileno _fileno
#else
#include <unistd.h>
#endif

#ifndef MXPSQL_MRL_H
/**
 * @brief Include guard
 * @details Yes, include guard so no double definition
 */
#define MXPSQL_MRL_H

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @file mrl.h
 * @brief MRL, MXPSQL ReadLine
 * @details A very simple readline implementation.
 * GPL compatible, but not licensed under the GPL.
 * Only prompt and history are supported.
 * Written in ANSI C.
 * @copyright MIT License
 * 
 * Copyright (c) 2022 MXPSQL
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */

#ifndef MXPSQL_MRL_MAX_HISTORY_LEN
/**
 * @brief The maximum entry count for the history
 * @details Define this macro before inclusion if you need more entries
 * 
 */
#define MXPSQL_MRL_MAX_HISTORY_LEN 10
#endif
#ifndef MXPSQL_MRL_MAX_STR_LEN
/**
 * @brief The maximum characters that can be read from the terminal
 * @details Define this macro before inclusion if you need more space
 */
#define MXPSQL_MRL_MAX_STR_LEN sizeof(char*)*100000*1024
#endif

/* ok or not */
#ifndef MXPSQL_MRL_ERR_OK
/**
 * @brief Everything is ok
 * @details You should check with an if to see if the return value is MXPSQL_MRL_ERR_OK, if not, something is wrong
 * This applies to all functions with an int return value
 * 
 */
#define MXPSQL_MRL_ERR_OK 0
#endif
#ifndef MXPSQL_ERR_NOT_OK
/**
 * @brief Everything is not ok
 * @details It's not ok here
 */
#define MXPSQL_MRL_ERR_NOT_OK -1
#endif

/**
 * @brief The state handle struct
 * @details This is the state handle structure, 
 * it holds the current cursor position, the prompt and the history, 
 * you may not want to edit mose of these directly.
 * 
 */
struct MRL_T{
    /* main readline */
    /**
     * @brief The prompt variable
     * @details the prompt variable is used to display the prompt
     */
    char *prompt;
    /**
     * @brief The file to scan from
     * @details yeah, it is the only purpose
     */
    FILE* scanner_source;
    /**
     * @brief The cursor position
     * @details Used once the edit mode is fully ready for production, it stores the cursor position
     */
    int cursor_pos;

    /* history */
    /**
     * @brief The history list
     * @details Store all the history
     */
    char* history[MXPSQL_MRL_MAX_HISTORY_LEN];
    /**
     * @brief The history position
     * @details The current history position
     */
    int history_pos;
};

/* memory management */
int MXPSQL_MRL_init(struct MRL_T *mrl, char *prompt);
int MXPSQL_MRL_init_ss(struct MRL_T *mrl, char* prompt, FILE* scanner_source);
void MXPSQL_MRL_free(struct MRL_T *mrl);

/* boring setters and getters */
void MXPSQL_MRL_set_prompt(struct MRL_T *mrl, char *prompt);
char* MXPSQL_MRL_get_prompt(struct MRL_T *mrl);
void MXPSQL_MRL_set_scannerSource(struct MRL_T *mrl, FILE* scanner_source);
FILE* MXPSQL_MRL_get_scannerSource(struct MRL_T *mrl);

/* history api */
int MXPSQL_MRL_add_history(struct MRL_T *mrl, char *line);
void MXPSQL_MRL_set_history_array(struct MRL_T *mrl, char **history);
int MXPSQL_MRL_set_history_file(struct MRL_T *mrl, FILE* fp);
void MXPSQL_MRL_set_history_str(struct MRL_T *mrl, char* history);
char** MXPSQL_MRL_get_history_array(struct MRL_T *mrl);
int MXPSQL_MRL_save_history_file(struct MRL_T *mrl, FILE* fp);
char* MXPSQL_MRL_get_history_str(struct MRL_T *mrl);

char* MXPSQL_MRL_readline(struct MRL_T *mrl, int add_to_history, int basic_only);
/* for now these two are the same, but soon, the edit version will be more advanced
    also the block under this comment is internal
 */
char* MXPSQL_MRL_readline_basic(struct MRL_T *mrl);
char* MXPSQL_MRL_readline_edit(struct MRL_T *mrl);
void MXPSQL_MRL_readline_refreshLine(struct MRL_T *mrl, char* line);
void MXPSQL_MRL_readline_move_cursor_left(struct MRL_T *mrl, char* line);
void MXPSQL_MRL_readline_move_cursor_right(struct MRL_T *mrl, char* line);

/* Extra utils i guess */
int MXPSQL_MRL_isSupportedTerm(void);
int MXPSQL_MRL_clsTerm(void);

/**
 * @brief Initialize the state handle with stdin as the source
 * 
 * @param mrl the handle
 * @param prompt the prompt
 * @return int Ok, or not
 */
int MXPSQL_MRL_init(struct MRL_T *mrl, char *prompt){
    mrl->prompt = (char*)malloc(strlen(prompt) + 1);
    if(!mrl->prompt){
        return MXPSQL_MRL_ERR_NOT_OK;
    }
    strcpy(mrl->prompt, prompt);

    int i;
    for(i = 0; i < MXPSQL_MRL_MAX_HISTORY_LEN; i++){
        mrl->history[i] = NULL;
    }

    mrl->scanner_source = stdin;

    mrl->history_pos = 0;
    
    return MXPSQL_MRL_ERR_OK;
}

/**
 * @brief Intialize the state handle like the default init function, but you can choose the source
 * @a MXPSQL_MRL_init
 * 
 * @param mrl the state handle to initialize
 * @param prompt the prompt
 * @param scanner_source the source of input
 * @return int Good or bad?
 */
int MXPSQL_MRL_init_ss(struct MRL_T *mrl, char* prompt, FILE* scanner_source){
    if(MXPSQL_MRL_init(mrl, prompt) != MXPSQL_MRL_ERR_OK){
        return MXPSQL_MRL_ERR_NOT_OK;
    }

    if(scanner_source == NULL){
        return MXPSQL_MRL_ERR_NOT_OK;
    }

    mrl->scanner_source = scanner_source;

    return MXPSQL_MRL_ERR_OK;
}

/**
 * @brief Free the state
 * 
 * @param mrl the handle
 */
void MXPSQL_MRL_free(struct MRL_T *mrl){
    free(mrl->prompt);

    mrl->prompt = NULL;

    int i;
    for(i=0;i<MXPSQL_MRL_MAX_HISTORY_LEN;i++){
        if(mrl->history[i] != NULL){
            free(mrl->history[i]);
            mrl->history[i] = NULL;
        }
    }
}


/**
 * @brief Set the prompt for a state handle
 * 
 * @param mrl the handle
 * @param prompt the prompt
 */
void MXPSQL_MRL_set_prompt(struct MRL_T *mrl, char *prompt){
    free(mrl->prompt);
    mrl->prompt = (char*)malloc(strlen(prompt) + 1);
    strcpy(mrl->prompt, prompt);
}

/**
 * @brief Get the prompt
 * 
 * @param mrl the state handle
 * @return char* the prompt that is set for the handle
 */
char* MXPSQL_MRL_get_prompt(struct MRL_T *mrl){
    return mrl->prompt;
}

/**
 * @brief Set the source of input
 * 
 * @param mrl the state handle
 * @param scanner_source the source
 */
void MXPSQL_MRL_set_scannerSource(struct MRL_T *mrl, FILE* scanner_source){
    mrl->scanner_source = scanner_source;
}

/**
 * @brief Get the source of input
 * 
 * @param mrl the state handle
 * @return FILE* the source
 */
FILE* MXPSQL_MRL_get_scannerSource(struct MRL_T *mrl){
    return mrl->scanner_source;
}



/**
 * @brief Add a line to a state handle's history
 * 
 * @param mrl the handle
 * @param line the line for a history
 * @return int Good?
 */
int MXPSQL_MRL_add_history(struct MRL_T *mrl, char *line){
    char* linecpio = NULL;

    if(line == NULL){
        return MXPSQL_MRL_ERR_NOT_OK;
    }

    if(mrl->history == NULL){
        return MXPSQL_MRL_ERR_NOT_OK;
    }

    {
        int i;
        for(i = 0; i < MXPSQL_MRL_MAX_HISTORY_LEN; i++){
            if(mrl->history[i] != NULL){
                if(strcmp(mrl->history[i], line) == 0){
                    return MXPSQL_MRL_ERR_NOT_OK;
                }
            }
        }
    }

    linecpio = (char*)malloc(strlen(line) + 1);
    strcpy(linecpio, line);
    int currentused = 0;
    for(currentused = 0; currentused < MXPSQL_MRL_MAX_HISTORY_LEN; currentused++){
        if(mrl->history[currentused] == NULL){
            break;
        }
    }
    if(!linecpio) return MXPSQL_MRL_ERR_NOT_OK;
    if(currentused == MXPSQL_MRL_MAX_HISTORY_LEN){
        if(mrl->history != NULL){
            free(mrl->history[0]);
            memmove(mrl->history, mrl->history+1, sizeof(char*)*(MXPSQL_MRL_MAX_HISTORY_LEN-1));
            mrl->history[MXPSQL_MRL_MAX_HISTORY_LEN-1] = linecpio;
        }
    }
    else{
        mrl->history[currentused] = linecpio;
    }

    return MXPSQL_MRL_ERR_OK;    
}

/**
 * @brief Set the history from an array
 * 
 * @param mrl the state handle
 * @param history the array of history
 */
void MXPSQL_MRL_set_history_array(struct MRL_T *mrl, char** history){
    int i;
    for(i = 0; i < MXPSQL_MRL_MAX_HISTORY_LEN; i++){
        if(mrl->history[i] != NULL){
            free(mrl->history[i]);
            mrl->history[i] = NULL;
        }
    }

    for(i = 0; i < MXPSQL_MRL_MAX_HISTORY_LEN; i++){
        if(history[i] == NULL){
            mrl->history[i] = (char*)malloc(strlen(history[i]) + 1);
            strcpy(mrl->history[i], history[i]);
        }
    }
}

/**
 * @brief Load the history from a file
 * @warning You need to close the file manually
 * 
 * @param mrl the state handle
 * @param fp the file handle
 * @return int good?
 */
int MXPSQL_MRL_set_history_file(struct MRL_T *mrl, FILE* fp){
    /* separate history by newline */
    char* line = NULL;
    if(!fp){
        return MXPSQL_MRL_ERR_NOT_OK;
    }

    while(fgets(line, MXPSQL_MRL_MAX_STR_LEN, fp) != NULL){
        if(line[strlen(line)-1] == '\n'){
            line[strlen(line)-1] = '\0';
        }
        MXPSQL_MRL_add_history(mrl, line);
    }

    return MXPSQL_MRL_ERR_OK;
}

/**
 * @brief Set the history from a string
 * 
 * @param mrl the handle
 * @param history the string of history
 */
void MXPSQL_MRL_set_history_str(struct MRL_T *mrl, char* history){
    /* newline separated, strtok */
    char* line = NULL;
    if(!history){
        return;
    }

    line = strtok(history, "\n");
    while(line != NULL){
        MXPSQL_MRL_add_history(mrl, line);
        line = strtok(NULL, "\n");
    }
}

/**
 * @brief Get the history as an array
 * 
 * @param mrl the handle
 * @return char** the array
 */
char** MXPSQL_MRL_get_history_array(struct MRL_T *mrl){
    return mrl->history;
}

/**
 * @brief Save the history to a file
 * @warning You need to close the file manually
 * 
 * @param mrl the handle
 * @param fp the file handle to write to
 * @return int good?
 */
int MXPSQL_MRL_save_history_file(struct MRL_T *mrl, FILE* fp){
    if(!fp){
        return MXPSQL_MRL_ERR_NOT_OK;
    }

    int i;
    for(i = 0; i < MXPSQL_MRL_MAX_HISTORY_LEN; i++){
        if(mrl->history[i] != NULL){
            fprintf(fp, "%s\n", mrl->history[i]);
        }
    }
    return MXPSQL_MRL_ERR_OK;
}

/**
 * @brief Get the history as a string
 * 
 * @param mrl the state handle
 * @return char* the string
 */
char* MXPSQL_MRL_get_history_str(struct MRL_T *mrl){
    char* stri = (char*) malloc(sizeof(char)*MXPSQL_MRL_MAX_HISTORY_LEN*MXPSQL_MRL_MAX_STR_LEN);
    int i;
    for(i = 0; i < MXPSQL_MRL_MAX_HISTORY_LEN; i++){
        if(mrl->history[i] != NULL){
            sprintf(stri, "%s\n%s", stri, mrl->history[i]);
        }
    }
}



/**
 * @brief Read a line
 * @details Read a line from a source, and return it. There are 2 modes: basic and editor mode.
 * Basic mode is used if forced, the source is not a tty or the terminal is not supported.
 * The editor mode or the advanced mode is currently the same with Basic mode, but soon, will support editing and history.
 * 
 * @param mrl The state handle
 * @param add_to_history add to history or not?
 * @param basic_only force basic mode?
 * @return char* the string
 */
char* MXPSQL_MRL_readline(struct MRL_T *mrl, int add_to_history, int basic_only){
    char* line = NULL;
    printf("%s", mrl->prompt);
    line = (char*)malloc(MXPSQL_MRL_MAX_STR_LEN);
    mrl->cursor_pos = 0;
    mrl->history_pos = 0;

    if(basic_only || !isatty(fileno(mrl->scanner_source)) || MXPSQL_MRL_isSupportedTerm() != MXPSQL_MRL_ERR_OK){
        char* l = MXPSQL_MRL_readline_basic(mrl);
        line = (char*)realloc(line, strlen(l) + 1);
        strcpy(line, l);
    }
    else{
        char* l = MXPSQL_MRL_readline_edit(mrl);
        line = (char*)realloc(line, strlen(l) + 1);
        strcpy(line, l);
    }

    if(add_to_history){
        MXPSQL_MRL_add_history(mrl, line);
    }

    return line;
}

/**
 * @brief Basic readline
 * 
 * @return char* the string
 */
char* MXPSQL_MRL_readline_basic(struct MRL_T *mrl){
    char *line = NULL;
    size_t len = 0, maxlen = 0;

    while(1) {
        if (len == maxlen) {
            if (maxlen == 0) maxlen = 16;
            maxlen *= 2;
            char *oldval = line;
            line = (char*) realloc(line,maxlen);
            if (line == NULL) {
                if (oldval) free(oldval);
                line = NULL;
                break;
            }
        }
        int c = fgetc(mrl->scanner_source);
        if (c == EOF || c == '\n') {
            if (c == EOF && len == 0) {
                free(line);
                line = NULL;
                break;
            } else {
                line[len] = '\0';
                break;
            }
        } else {
            line[len] = c;
            len++;
        }
    }
       

    return line;
}

/**
 * @brief An advanced mode of the readline, with history traversal and line editing (no multiline)
 * 
 * @return char* the string
 */
char* MXPSQL_MRL_readline_edit(struct MRL_T *mrl){
    char *line = NULL;
    size_t len = 0, maxlen = 0;

    while(1) {
        if (len == maxlen) {
            if (maxlen == 0) maxlen = 16;
            maxlen *= 2;
            char *oldval = line;
            line = (char*) realloc(line,maxlen);
            if (line == NULL) {
                if (oldval) free(oldval);
                line = NULL;
                break;
            }
        }
        int c = fgetc(mrl->scanner_source);
        printf("%c", c);
        if (c == EOF || c == '\n') {
            if (c == EOF && len == 0) {
                free(line);
                line = NULL;
                break;
            } else {
                line[len] = '\0';
                break;
            }
        } else if (c == '\033') {
            c = fgetc(mrl->scanner_source);
            printf("yeh");
            if (c == '[') {
                c = fgetc(mrl->scanner_source);
                if (c == 'A') {
                    if(mrl->history_pos > 0){
                        mrl->history_pos--;
                        strcpy(line, mrl->history[mrl->history_pos]);
                        len = strlen(line);
                    }
                }
                else if (c == 'B') {
                    if(mrl->history_pos < MXPSQL_MRL_MAX_HISTORY_LEN - 1){
                        mrl->history_pos++;
                        strcpy(line, mrl->history[mrl->history_pos]);
                        len = strlen(line);
                    }
                }
                else if (c == 'C') {
                    MXPSQL_MRL_readline_move_cursor_right(mrl, line);
                }
                else if (c == 'D') {
                    MXPSQL_MRL_readline_move_cursor_left(mrl, line);
                }
            }
            
        } else {
            line[len] = c;
            len++;
        }
    }
       

    return line;
}

/**
 * @brief Refresh the line
 * @warning This function is for internal use
 * 
 * @param mrl the state handle
 * @param line the line
 */
void MXPSQL_MRL_readline_refreshLine(struct MRL_T *mrl, char* line){
    printf("\033[2K");
    printf("\033[%dC", mrl->cursor_pos);
    printf("%s", line);
    printf("\033[%dC", mrl->cursor_pos);
}

/**
 * @brief Move cursor right
 * @warning This function is for internal use
 * 
 * @param mrl State handle
 * @param line the line
 */
void MXPSQL_MRL_readline_move_cursor_right(struct MRL_T *mrl, char* line){
    if(mrl->cursor_pos < strlen(line)){
        mrl->cursor_pos++;
        MXPSQL_MRL_readline_refreshLine(mrl, line);
    }
}

/**
 * @brief Move cursor left
 * @warning This function is for internal use
 * 
 * @param mrl the state handle
 * @param line the line
 */
void MXPSQL_MRL_readline_move_cursor_left(struct MRL_T *mrl, char* line){
    if(mrl->cursor_pos > 0){
        mrl->cursor_pos--;
        MXPSQL_MRL_readline_refreshLine(mrl, line);
    }
}


/**
 * @brief Is the terminal supported?
 * 
 * @return int Yes! or a No.
 */
int MXPSQL_MRL_isSupportedTerm(void) {
    char *term = getenv("TERM");
    int j;

    if (term == NULL) return MXPSQL_MRL_ERR_OK;
    char *unsupported_term[] = {"dumb","cons25","emacs",NULL};
    for (j = 0; unsupported_term[j]; j++)
        if (!strcasecmp(term,unsupported_term[j])) return MXPSQL_MRL_ERR_NOT_OK;
    return MXPSQL_MRL_ERR_OK;
}

/**
 * @brief Clear the terminal, with commands and escape sequences
 * 
 * @return int Good or bad?
 */
int MXPSQL_MRL_clsTerm(void) {
    if(system(NULL) == -1) {
        printf("\033[2J");
        return MXPSQL_MRL_ERR_NOT_OK;
    }

    if(system("clear") == -1) {
        if(system("cls") == -1) {
            printf("\033[2J");
        }
    }
    return MXPSQL_MRL_ERR_OK;
}

#ifdef __cplusplus
}
#endif

#endif