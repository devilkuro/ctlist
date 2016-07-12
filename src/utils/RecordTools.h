//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __FANJING_STATISTICSRECORDTOOLS_H_
#define __FANJING_STATISTICSRECORDTOOLS_H_

#include <map>
#include <list>
#include <string>
#include <stdarg.h>
#include "ctime"
#include <fstream>
#include <iostream>
#include <sstream>

//#include "RecordUnit.h"
//#include "cmessage.h"
using std::map;
using std::string;
namespace Fanjing {
/**
 *
 * usage:
 *  gs.changeName(name[,title])<<value0<<value1<<...<<valueN<<gs.endl;
 *  or
 *  gs.changeName(name[,title]);
 *  gs.get()<<value0;
 *  gs.get()<<value1;
 *  ...
 *  gs.get()<<valueN;
 *  gs.get()<<gs.endl;
 *
 *  out:
 *  name:title
 *  value0,value1,value2,...
 *  value0,value1,value2,...
 *  value0,value1,value2,...
 */

class RecordTools {
protected:
    class RecordUnit {
    public:
        RecordUnit(int size);
        virtual ~RecordUnit();
    public:
        enum UnitType {
            UNIT_TYPE_ERROR = -1,
            UNIT_TYPE_INT = 0,
            UNIT_TYPE_UINT32,
            UNIT_TYPE_UINT64,
            UNIT_TYPE_DOUBLE,
            UNIT_TYPE_STRING
        };
        struct DataUnit {
            UnitType type;
            union {
                int intData;
                double douData;
                uint32_t uint32Data;
                uint64_t uint64Data;
            };
            string strData;
            string toString();
        };
    public:
        int getDataType(int index);
        void setData(double data, int index);
        void setData(int data, int index);
        void setData(uint32_t data, int index);
        void setData(uint64_t data, int index);
        void setData(string data, int index);
        int getSize() const;
        string toString();

    private:
        int size;       // read only, it must be set at initialize phase.
        DataUnit *data;   // statistics data
    };

    typedef std::list<RecordUnit*> RecordUnitList;
    typedef std::map<string, RecordUnitList*> RecordListMap;
public:
    typedef void* gs_eofType;
public:
    bool recordWhenTerminate;

    static RecordTools * request();
    static void release();

    RecordTools& operator<<(gs_eofType& e);
    RecordTools& operator<<(double num);
    RecordTools& operator<<(int num);
    RecordTools& operator<<(unsigned int num);
    RecordTools& operator<<(uint64_t num);
    RecordTools& operator<<(string str);

    RecordTools& changeName(string name, string title = "");
    RecordTools& get();

    void setDefaultDir(string dir);
    void eof();
    void clean();
    void output(string name, string dir = "", string field = "",
            std::fstream::openmode openmode = std::fstream::out);
    void outputAll(string name, string dir = "",
            std::fstream::openmode openmode = std::fstream::out);
    void outputSeparate(string name, string dir = "", string field = "",
            std::fstream::openmode openmode = std::fstream::out);
    virtual void initialize();
    virtual void finish();

    static gs_eofType endl;
    std::map<string, double> dblMap;
    //std::map<string, cMessage*> msgMap;
protected:
    RecordListMap m_recordListMap;
    std::map<string, string> m_titleMap;
    std::list<RecordUnit::DataUnit> m_unitData;
    string m_name;
    string m_default_dir_name;
private:
    static RecordTools* ptr_singleton;
    RecordTools();
    virtual ~RecordTools();

    string getFileName(string name);
    string getSuffix(string name);
    string getTitleFromName(string name);
};

}  // namespace Fanjing
#endif
