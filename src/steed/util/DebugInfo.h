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
 * @file DebugInfo.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    A simple DebugInfo module
 */

#pragma once 

#include <stdint.h>

namespace steed {
namespace DebugInfo {


/**
 * print current time in format [YYYY-MM-DD HH:MM:SS]
 */
void printTime(void);


/**
 * print current call stack and exit
 */
void printStackAndExit(void);

/**
 * print current call stack
 */
void printStack(void);

/**
 * print current func name
 * @param slvl stack level
 * @param fname func name
 */
void printFuncName(uint32_t slvl, const char* fname);


} // namespace DebugInfo
} // namespace steed 
