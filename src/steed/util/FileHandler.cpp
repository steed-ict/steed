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
 * @file FileHandler.cpp
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   FileHandler implementation
 */

#include <stdio.h>
#include <stdint.h>
#include "FileHandler.h"

namespace steed {


int FileHandler::remove(const char *p)
{
    int ret = ::remove(p);
    if (ret < 0)
    {
        printf("FileHandler: remove file[%s] failed!\n", p);
        DebugInfo::printStackAndExit(); 
    } // if
    return ret;
} // remove





int64_t FileHandlerViaOS::read(uint64_t offset, void *buffer, uint64_t bufsize)
{
    if (uint64_t(-1) == this->seek2offset(offset))
    {
        printf("FileHandlerViaOS: read calls seek2offset to [%lu] failed!\n", offset);
        DebugInfo::printStackAndExit(); 
        return -1;
    } // if 

    int64_t  rd_done = 0;
    uint64_t rd_size = bufsize;
    char    *rd_pos  = (char*)buffer; 
    while   (rd_size > 0)  // avoid read() interrupted by a signal
    {
        int64_t rd_num = ::read(m_file_desc, rd_pos, rd_size);
        if (rd_num > 0)       // success  
        {
            rd_pos += rd_num, rd_done += rd_num, rd_size -= rd_num;
        }
        else if (rd_num == 0) // EOF
        {   break;   }
        else if (rd_num < 0)  // error
        {
            printf("FileHandlerViaOS: read file[%d] got errno[%d]!\n", m_file_desc, errno);
            DebugInfo::printStackAndExit(); 
            return -1;
        }
    } // while 

    m_file_off += rd_done;  

    return rd_done; 
} // read



int64_t FileHandlerViaOS::write(uint64_t offset, const void *buffer, uint64_t bufsize)
{
    if (uint64_t(-1) == this->seek2offset(offset))
    {
        printf("FileHandlerViaOS: write calls seek2offset to [%lu] failed!\n", offset);
        DebugInfo::printStackAndExit(); 
        return -1;
    } // if ;

    const char *wt_pos = (const char*)buffer;
    int64_t  wt_done = 0;
    uint64_t wt_size = bufsize;
    while   (wt_size > 0)  // avoid write() interrupted by a signal
    {
        int64_t wt_num = ::write(m_file_desc, wt_pos, wt_size);
        if (wt_num > 0)
        {
            wt_pos += wt_num, wt_done += wt_num, wt_size -= wt_num;
        } 
        else
        {
            printf("FileHandlerViaOS: write file[%d] using len[%lu]@[%p] got errno[%d]!\n", 
                m_file_desc, wt_size, wt_pos, errno);
            DebugInfo::printStackAndExit(); 
            return -1;
        }
    } // while

    m_file_off += wt_done; 

    return wt_done;
} // write 

} // namespace steed 
