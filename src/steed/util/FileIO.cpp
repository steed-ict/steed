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
 * @file FileIOInOS.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for FileIOInOS:
 *   Use OS 2 buffer content via system call read and write. 
 */

#include "FileIO.h" 

namespace steed {


void FileIO::uninit(void)
{
    // close file if needed
    if ((m_file_hand != nullptr) && (m_file_hand->close() < 0))
    {   DebugInfo::printStackAndExit();   } 

    delete m_file_hand;
    m_file_hand = nullptr, m_file_name.clear();  
    m_file_size = 0, m_offset = 0, m_type = invalid;
} // dtor 



void FileIO::output2Debug(void) 
{
    puts("\n");
    printf("FileIO [file info]: off[%lu] size[%lu] path[%s]\n",
        m_offset, m_file_size, m_file_name.c_str());
} // output2Debug





int FileIOViaOS::init(IOType t, const string &n, int flags, mode_t mt)
{
    if (m_type != invalid)
    {
        printf("FileIOViaOS: init write failed!\n");
        DebugInfo::printStackAndExit(); 
        return -1;
    } // if-else

    m_type = t;
    m_file_name.assign(n);
    m_file_hand = new FileHandlerViaOS(); 

    int ret_val = 0;
    const char *fn = m_file_name.c_str();
    int fd = m_file_hand->open(fn, flags, mt);
    if (fd < 0)
    {
//        printf("FileIOViaOS: init [%s] failed! Got errno [%d].\n", fn, errno);
//        DebugInfo::printStackAndExit(); 
        ret_val = -1; 
    } // if 

    m_file_size = FileHandler::getFileSize(fn);
    return ret_val;
} // init

} // namespace steed 
