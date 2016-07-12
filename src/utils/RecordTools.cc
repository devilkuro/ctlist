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

#include "RecordTools.h"

#include "StringHelper.h"
namespace Fanjing {

RecordTools* RecordTools::ptr_singleton = NULL;
RecordTools::gs_eofType RecordTools::endl = NULL;
RecordTools::RecordUnit::RecordUnit(int size) {
    this->size = size;
    data = new DataUnit[size];
}

RecordTools::RecordUnit::~RecordUnit() {
    size = 0;
    if(data != NULL){
        delete[] data;
        data = NULL;
    }
}

void RecordTools::RecordUnit::setData(double data, int index) {
    if(index >= 0 && index < size){
        this->data[index].type = UNIT_TYPE_DOUBLE;
        this->data[index].douData = data;
    }
}

int RecordTools::RecordUnit::getSize() const {
    return size;
}

void RecordTools::RecordUnit::setData(int data, int index) {
    if(index >= 0 && index < size){
        this->data[index].type = UNIT_TYPE_INT;
        this->data[index].intData = data;
    }
}

void RecordTools::RecordUnit::setData(uint32_t data, int index) {
    if(index >= 0 && index < size){
        this->data[index].type = UNIT_TYPE_UINT32;
        this->data[index].uint32Data = data;
    }
}

void RecordTools::RecordUnit::setData(uint64_t data, int index) {
    if(index >= 0 && index < size){
        this->data[index].type = UNIT_TYPE_UINT64;
        this->data[index].uint64Data = data;
    }
}

void RecordTools::RecordUnit::setData(string data, int index) {
    if(index >= 0 && index < size){
        this->data[index].type = UNIT_TYPE_STRING;
        this->data[index].strData = data;
    }
}

int RecordTools::RecordUnit::getDataType(int index) {
    if(index >= 0 && index < size){
        return data[index].type;
    }else{
        return UNIT_TYPE_ERROR;
    }
}

string RecordTools::RecordUnit::toString() {
    stringstream ss;
    for(int i = 0; i < size; i++){
        if(i != 0)
            ss << "\t";
        ss << data[i].toString();
    }
    return ss.str();
}

string RecordTools::RecordUnit::DataUnit::toString() {
    stringstream ss;
    switch(type){
        case UNIT_TYPE_INT:
            ss << intData;
            break;
        case UNIT_TYPE_UINT32:
            ss << uint32Data;
            break;
        case UNIT_TYPE_UINT64:
            ss << uint64Data;
            break;
        case UNIT_TYPE_DOUBLE:
            ss << douData;
            break;
        case UNIT_TYPE_STRING:
            ss << strData;
            break;
        default:
            break;
    }
    string str = ss.str();
    ss.str(std::string());
    ss.clear();
    return str;
}
void RecordTools::initialize() {
    // do nothing
}

RecordTools::RecordTools() {
    recordWhenTerminate = false;
    // do nothing
}

void RecordTools::eof() {
    get() << this->endl;
}

void RecordTools::outputAll(string name, string dir,
        std::fstream::openmode openmode) {
    output(name, dir, "", openmode);
}

void RecordTools::setDefaultDir(string dir) {
    m_default_dir_name = dir;
    recordWhenTerminate = true;
}

RecordTools::~RecordTools() {
    if(recordWhenTerminate){
        outputSeparate("default.txt", "./results");
    }
    clean();
}

void RecordTools::finish() {
    ostringstream name;
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    name.width(2);
    name << "./results/" << "result_" << timeinfo->tm_year << "_"
            << timeinfo->tm_mon << "_" << timeinfo->tm_mday << "_"
            << timeinfo->tm_hour << "_" << timeinfo->tm_min << "_"
            << timeinfo->tm_sec << ".txt";
    output(name.str());
}

RecordTools& RecordTools::operator <<(gs_eofType& e) {
    if(m_unitData.size() > 0){
        RecordUnit* unit = new RecordUnit(m_unitData.size());
        int i = 0;
        for(std::list<RecordUnit::DataUnit>::iterator it = m_unitData.begin();
                it != m_unitData.end(); it++){
            switch((*it).type){
                case RecordUnit::UNIT_TYPE_INT:
                    unit->setData((*it).intData, i);
                    break;
                case RecordUnit::UNIT_TYPE_UINT32:
                    unit->setData((*it).uint32Data, i);
                    break;
                case RecordUnit::UNIT_TYPE_UINT64:
                    unit->setData((*it).uint64Data, i);
                    break;
                case RecordUnit::UNIT_TYPE_DOUBLE:
                    unit->setData((*it).douData, i);
                    break;
                case RecordUnit::UNIT_TYPE_STRING:
                    unit->setData((*it).strData, i);
                    break;
                default:
                    break;
            }
            i++;
        }
        RecordListMap::iterator it;
        it = m_recordListMap.find(m_name);
        if(it == m_recordListMap.end()){
            RecordUnitList* list = new RecordUnitList();
            m_recordListMap[m_name] = list;
        }
        m_recordListMap[m_name]->push_back(unit);
        m_unitData.clear();
    }
    return *ptr_singleton;
}

RecordTools& RecordTools::operator <<(double num) {
    RecordUnit::DataUnit unit;
    unit.type = RecordUnit::UNIT_TYPE_DOUBLE;
    unit.douData = num;
    m_unitData.push_back(unit);
    return *ptr_singleton;
}

RecordTools& RecordTools::changeName(string name, string title) {
    RecordListMap::iterator it;
    m_name = name;
    it = m_recordListMap.find(m_name);
    if(it == m_recordListMap.end()){
        RecordUnitList* list = new RecordUnitList();
        m_recordListMap[m_name] = list;
        m_titleMap[m_name] = title;
        std::cout << "StatisticsRecordTools::addTitle:" << m_name << std::endl;
    }
    m_unitData.clear();
    return *ptr_singleton;
}

RecordTools& RecordTools::operator <<(string str) {
    RecordUnit::DataUnit unit;
    unit.type = RecordUnit::UNIT_TYPE_STRING;
    unit.strData = str;
    m_unitData.push_back(unit);
    return *ptr_singleton;
}

RecordTools& RecordTools::operator <<(int num) {
    RecordUnit::DataUnit unit;
    unit.type = RecordUnit::UNIT_TYPE_INT;
    unit.intData = num;
    m_unitData.push_back(unit);
    return *ptr_singleton;
}

RecordTools& RecordTools::operator <<(unsigned int num) {
    RecordUnit::DataUnit unit;
    unit.type = RecordUnit::UNIT_TYPE_UINT32;
    unit.uint32Data = num;
    m_unitData.push_back(unit);
    return *ptr_singleton;
}

RecordTools& RecordTools::operator <<(uint64_t num) {
    RecordUnit::DataUnit unit;
    unit.type = RecordUnit::UNIT_TYPE_UINT64;
    unit.uint64Data = num;
    m_unitData.push_back(unit);
    return *ptr_singleton;
}

void RecordTools::output(string name, string dir, string field,
        std::fstream::openmode openmode) {
    std::fstream fs;
    string path;
    if(dir != ""){
        path = dir + "/" + name;
    }else{
        if(m_default_dir_name != ""){
            path = m_default_dir_name + "/" + name;
        }else{
            path = name;
        }
    }
    fs.open(path.c_str(), std::fstream::out | openmode);
    if(!fs.good()){
        std::cout << "error:" << std::endl;
        std::cout << "eof()\t" << fs.eof() << std::endl;
        std::cout << "fail()\t" << fs.fail() << std::endl;
        std::cout << "bad()\t" << fs.bad() << std::endl;
    }
    for(RecordListMap::iterator it = m_recordListMap.begin();
            it != m_recordListMap.end(); it++){
        if(field == "" || it->first == field){
            for(RecordUnitList::iterator lit = it->second->begin();
                    lit != it->second->end(); lit++){
                fs
                        << it->first
                                + (m_titleMap[it->first] == "" ? "" :
                                        (":" + m_titleMap[it->first])) << "\t"
                        << (*lit)->toString() << std::endl;
            }
        }
    }
    fs.close();
}

void RecordTools::outputSeparate(string name, string dir, string field,
        std::fstream::openmode openmode) {
    for(RecordListMap::iterator it = m_recordListMap.begin();
            it != m_recordListMap.end(); it++){
        if(field == "" || it->first == field){
            std::fstream fs;
            string path;
            string sname = getFileName(name) + "-"
                    + StringHelper::convertStrToFileName(it->first)
                    + getSuffix(name);
            if(dir != ""){
                path = dir + "/" + sname;
            }else{
                if(m_default_dir_name != ""){
                    path = m_default_dir_name + "/" + sname;
                }else{
                    path = sname;
                }
            }
            fs.open(path.c_str(), std::fstream::out | openmode);
            if(!fs.good()){
                std::cout << "error:" << std::endl;
                std::cout << "eof()\t" << fs.eof() << std::endl;
                std::cout << "fail()\t" << fs.fail() << std::endl;
                std::cout << "bad()\t" << fs.bad() << std::endl;
            }
            // print title

            if(m_titleMap[it->first] == ""){
                fs << getTitleFromName(it->first) << std::endl;
            }else{
                fs << m_titleMap[it->first] << std::endl;
            }
            // print each entry
            for(RecordUnitList::iterator lit = it->second->begin();
                    lit != it->second->end(); lit++){
                fs << (*lit)->toString() << std::endl;
            }
        }
    }
}
string RecordTools::getTitleFromName(string name) {
    unsigned int n_first = name.find_first_of(':');
    unsigned int n_last = name.find_last_of(':');
    if(n_first != name.npos && n_first == n_last
            && n_first < name.length() - 1){
        return name.substr(n_first + 1);
    }
    return "";
}

string RecordTools::getFileName(string name) {
    unsigned int pos = name.find_last_of('.');
    if(pos != name.npos && pos < name.length()){
        if(pos != 0){
            return name.substr(0, pos);
        }else{
            return "default";
        }
    }else{
        return name;
    }
}

string RecordTools::getSuffix(string name) {
    unsigned int pos = name.find_last_of('.');
    if(pos != name.npos && pos < name.length()){
        if(pos != 0){
            return name.substr(pos);
        }else{
            return ".txt";
        }
    }else{
        return "";
    }
}
RecordTools* RecordTools::request() {
    if(ptr_singleton == NULL){
        ptr_singleton = new RecordTools();
    }
    return ptr_singleton;
}

void RecordTools::release() {
    if(ptr_singleton != NULL){
        delete ptr_singleton;
        ptr_singleton = NULL;
    }
}

void RecordTools::clean() {
    for(RecordListMap::iterator it = m_recordListMap.begin();
            it != m_recordListMap.end(); it++){
        for(RecordUnitList::iterator lit = it->second->begin();
                lit != it->second->end(); lit++){
            delete (*lit);
        }
        delete (it->second);
    }
    m_recordListMap.clear();
    m_titleMap.clear();
}

RecordTools& RecordTools::get() {
    return *ptr_singleton;
}

}  // namespace Fanjing
