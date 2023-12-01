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
 * @file ColumnItem.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for ColumnItem 
 */

#pragma once 

#include <stdio.h>
#include <stdint.h>

#include "Config.h"
#include "DataType.h"


namespace steed {

extern Config g_config;

class ColumnItem {
private: 
    const void *m_bin {nullptr}; /**< bin value cotent */
    uint32_t    m_rep {0};       /**< repetition value */
    uint32_t    m_def {0};       /**< definition value */
    uint32_t    m_nrep{0};       /**< next repetition  */

public:
    ~ColumnItem(void) = default;
    ColumnItem (const ColumnItem &org) = default;
    ColumnItem (uint32_t rep = 0, uint32_t def = 0, const void *bin = nullptr,
        uint32_t nrep = 0);

public: 
    const void *getBin    (void)  { return m_bin;  }
    uint32_t    getRep    (void)  { return m_rep;  }
    uint32_t    getDef    (void)  { return m_def;  }
    uint32_t    getNextRep(void)  { return m_nrep; }

public: 
    void reset(void);
    void set  (ColumnItem &ci);
    void set  (uint32_t rep, uint32_t def, const void *bin, uint32_t nrep);
    void setBinVal (const void *bin) { m_bin  =  bin; }
    void setRepVal (uint32_t    rep) { m_rep  =  rep; }
    void setDefVal (uint32_t    def) { m_def  =  def; }
    void setNextRep(uint32_t   nrep) { m_nrep = nrep; }
  
public:
    void output2debug(void);
    void output2debug(uint32_t max_def, DataType *dt);
}; // ColumnItem



inline ColumnItem::
ColumnItem(uint32_t rep, uint32_t def, const void *bin, uint32_t nrep):
    m_bin(bin), m_rep(rep), m_def(def), m_nrep(nrep)
{} 

inline
void ColumnItem::reset(void)
{   m_rep = 0, m_def = 0, m_bin = nullptr, m_nrep = 0; }

inline
void ColumnItem::set(ColumnItem &ci)
{   m_rep = ci.m_rep, m_def = ci.m_def, m_bin = ci.m_bin, m_nrep = ci.m_nrep;  }

inline
void ColumnItem::set(uint32_t rep, uint32_t def, const void *bin, uint32_t nrep)
{   m_rep = rep, m_def = def, m_bin = bin, m_nrep = nrep;   }

inline
void ColumnItem::output2debug(void)
{
    printf("Col Item: [%d,%d,<%p>] nrep [%d]\n", m_rep, m_def, m_bin, m_nrep);
}

inline
void ColumnItem::output2debug(uint32_t max_def, DataType *dt)
{
    uint32_t blen = g_config.m_max_bin_val_len; 
    char buf[blen];
    if (m_def < max_def)
    {   memcpy(buf, "null", sizeof("null"));   }
    else if (dt->transBin2Txt(m_bin, buf, blen) < 0)
    {   DebugInfo::printStackAndExit();  }

    printf("<%u:%u:%u:%s>", m_rep, m_def, m_nrep, buf);
} // output2debug


} // namespace steed
