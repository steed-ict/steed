/*
 * Copyright 2023 Zhiyi Wang
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



/**
 * @file DebugInfo.cpp
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    a simple DebugInfo system
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <cxxabi.h>
#include <execinfo.h>
#include "DebugInfo.h"

#define STACK_BUF_CAP  128
#define OUTPUT_BUF_CAP 1024 


namespace steed {
namespace DebugInfo {


void printTime(void)
{
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime (&rawtime);

    char tmpbuf[OUTPUT_BUF_CAP];
    strftime(tmpbuf, OUTPUT_BUF_CAP, "[%Y-%m-%d %H:%M:%S]", timeinfo);
    printf("%s ", tmpbuf);
} // printTime





void printStackAndExit(void)
{
    printStack();
    exit(EXIT_FAILURE);
} // printStackAndExit



void printStack(void)
{
    printTime();
    printf("STEED INFO: current stack: \n");

    int stack_size = 0;
    void *stack_buf[STACK_BUF_CAP]; 
    char **proc_names; // procedure names for each stack level

    // get void*'s for all entries on the stack
    stack_size = backtrace(stack_buf, STACK_BUF_CAP);
    printf("Got %d addresses from [%s]:[%d]\n", stack_size, __FILE__, __LINE__);

    /* The call backtrace_symbols_fd(stack_buf, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */
    proc_names = backtrace_symbols(stack_buf, stack_size);
    if (proc_names == NULL)
    {
        perror("[STEED INFO]: backtrace_symbols failed!\n");
        exit(EXIT_FAILURE);
    } // if

    puts("--------------------------------------------------------------------------------");
    for (uint32_t j = 0; j < uint32_t(stack_size); j++)
    {
        printFuncName(j, proc_names[j]);
    } // for
    puts("--------------------------------------------------------------------------------");
    puts("");

    free(proc_names);
} // printStack



void printFuncName(uint32_t slvl, const char* fname)
{
    int status = 0;
    char out_buf[OUTPUT_BUF_CAP] = {0};

    if (1 == sscanf(fname, "%*[^(]%*[^_]%255[^)+]", out_buf))
    {
        char* func_name = abi::__cxa_demangle(out_buf, nullptr, nullptr, &status);
        if (func_name != nullptr)
        {
            printf("%3d: [%s]\n", slvl, func_name);
            free(func_name); 
        } // if
    } //if 
} // printFuncName


} // namespace DebugInfo
} // namespace steed
