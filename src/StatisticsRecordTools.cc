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

#include "StatisticsRecordTools.h"
namespace Fanjing {
StatisticsRecordTools* StatisticsRecordTools::ptr_singleton = NULL;
StatisticsRecordTools::gs_eofType StatisticsRecordTools::endl = NULL;
void StatisticsRecordTools::initialize() {
    // do nothing
}

StatisticsRecordTools::StatisticsRecordTools() {
    // do nothing
}

void StatisticsRecordTools::eof() {
    get() << this->endl;
}

void StatisticsRecordTools::outputAll(string name, string dir,
        std::fstream::openmode openmode) {
    output(name, dir, "*", openmode);
}

void StatisticsRecordTools::setDefaultDir(string dir) {
    m_default_dir_name = dir;
}

StatisticsRecordTools::~StatisticsRecordTools() {
    clean();
}

void StatisticsRecordTools::finish() {
    ostringstream name;
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    name.width(2);
    name << "result_" << timeinfo->tm_year << "_" << timeinfo->tm_mon << "_"
            << timeinfo->tm_mday << "_" << timeinfo->tm_hour << "_"
            << timeinfo->tm_min << "_" << timeinfo->tm_sec << ".txt";
    output(name.str());
}

StatisticsRecordTools& StatisticsRecordTools::operator <<(gs_eofType& e) {
    if(unitData.size() > 0){
        StatisticsRecordUnit* unit = new StatisticsRecordUnit(unitData.size());
        int i = 0;
        for(std::list<StatisticsRecordUnit::DataUnit>::iterator it =
                unitData.begin(); it != unitData.end(); it++){
            switch((*it).type){
                case StatisticsRecordUnit::UNIT_TYPE_INT:
                    unit->setData((*it).intData, i);
                    break;
                case StatisticsRecordUnit::UNIT_TYPE_UINT32:
                    unit->setData((*it).uint32Data, i);
                    break;
                case StatisticsRecordUnit::UNIT_TYPE_UINT64:
                    unit->setData((*it).uint64Data, i);
                    break;
                case StatisticsRecordUnit::UNIT_TYPE_DOUBLE:
                    unit->setData((*it).douData, i);
                    break;
                case StatisticsRecordUnit::UNIT_TYPE_STRING:
                    unit->setData((*it).strData, i);
                    break;
                default:
                    break;
            }
            i++;
        }
        GlobalStatisticsMap::iterator it;
        it = globalStatisticsMap.find(m_name);
        if(it == globalStatisticsMap.end()){
            GlobalStatisticsList* list = new GlobalStatisticsList();
            globalStatisticsMap[m_name] = list;
        }
        globalStatisticsMap[m_name]->push_back(unit);
        unitData.clear();
    }
    return *ptr_singleton;
}

StatisticsRecordTools& StatisticsRecordTools::operator <<(double num) {
    StatisticsRecordUnit::DataUnit unit;
    unit.type = StatisticsRecordUnit::UNIT_TYPE_DOUBLE;
    unit.douData = num;
    unitData.push_back(unit);
    return *ptr_singleton;
}

StatisticsRecordTools& StatisticsRecordTools::changeName(string name) {
    GlobalStatisticsMap::iterator it;
    m_name = name;
    it = globalStatisticsMap.find(m_name);
    if(it == globalStatisticsMap.end()){
        GlobalStatisticsList* list = new GlobalStatisticsList();
        globalStatisticsMap[m_name] = list;
    }
    unitData.clear();
    return *ptr_singleton;
}

StatisticsRecordTools& StatisticsRecordTools::operator <<(string str) {
    StatisticsRecordUnit::DataUnit unit;
    unit.type = StatisticsRecordUnit::UNIT_TYPE_STRING;
    unit.strData = str;
    unitData.push_back(unit);
    return *ptr_singleton;
}

StatisticsRecordTools& StatisticsRecordTools::operator <<(int num) {
    StatisticsRecordUnit::DataUnit unit;
    unit.type = StatisticsRecordUnit::UNIT_TYPE_INT;
    unit.intData = num;
    unitData.push_back(unit);
    return *ptr_singleton;
}

StatisticsRecordTools& StatisticsRecordTools::operator <<(unsigned int num) {
    StatisticsRecordUnit::DataUnit unit;
    unit.type = StatisticsRecordUnit::UNIT_TYPE_UINT32;
    unit.uint32Data = num;
    unitData.push_back(unit);
    return *ptr_singleton;
}

StatisticsRecordTools& StatisticsRecordTools::operator <<(uint64_t num) {
    StatisticsRecordUnit::DataUnit unit;
    unit.type = StatisticsRecordUnit::UNIT_TYPE_UINT64;
    unit.uint64Data = num;
    unitData.push_back(unit);
    return *ptr_singleton;
}

void StatisticsRecordTools::output(string name, string dir, string field,
        std::fstream::openmode openmode) {
    std::fstream fs;
    string path;
    if(dir != ""){
        path = dir + "\\" + name;
    }else{
        if(m_default_dir_name != ""){
            path = m_default_dir_name + "\\" + name;
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
    for(GlobalStatisticsMap::iterator it = globalStatisticsMap.begin();
            it != globalStatisticsMap.end(); it++){
        if(field == "*" || it->first == field){
            for(GlobalStatisticsList::iterator lit = it->second->begin();
                    lit != it->second->end(); lit++){
                fs << it->first << "," << (*lit)->toString() << std::endl;
            }
        }
    }
    fs.close();
}

StatisticsRecordTools* StatisticsRecordTools::request() {
    if(ptr_singleton == NULL){
        ptr_singleton = new StatisticsRecordTools();
    }
    return ptr_singleton;
}

void StatisticsRecordTools::release() {
    if(ptr_singleton != NULL){
        delete ptr_singleton;
        ptr_singleton = NULL;
    }
}

void StatisticsRecordTools::clean() {
    for(GlobalStatisticsMap::iterator it = globalStatisticsMap.begin();
            it != globalStatisticsMap.end(); it++){
        for(GlobalStatisticsList::iterator lit = it->second->begin();
                lit != it->second->end(); lit++){
            delete (*lit);
        }
        delete (it->second);
    }
    globalStatisticsMap.clear();
}

StatisticsRecordTools& StatisticsRecordTools::get() {
    return *ptr_singleton;
}

}  // namespace Fanjing

