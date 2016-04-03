#include "spreadsheet.h"
#include "cell.h"
#include "cellvalue.h"
#include "celladdress.h"
#include "util.h"
#include <vector>
#include <string>
#include <utility>

using namespace std;


Cell::Cell(CellValue *value,CellAddress address) noexcept
	:value(value),address(address){}

Cell::Cell(CellAddress address) noexcept
	:value(new CellValueBasic<string>("")),address(address){}

Cell::Cell(string editString,CellAddress address) noexcept
	:value(nullptr),address(address){
	setEditString(editString);
}

Cell::~Cell() noexcept {}

Cell Cell::makeErrorCell(string errString,string editString,CellAddress address) noexcept {
	return Cell(new CellValueError(errString,editString),address);
}

void Cell::setError(string errString) noexcept {
	CellValue *newvalue;
	if(value){
		newvalue=new CellValueError(errString,value->getEditString());
		delete value;
	} else {
		newvalue=new CellValueError(errString,"");
	}
	value=newvalue;
}

const set<CellAddress>& Cell::getReverseDependencies() const noexcept {
	return revdeps;
}

bool Cell::addReverseDependency(CellAddress addr) noexcept {
	return revdeps.insert(addr).second;
}

void Cell::addReverseDependencies(const set<CellAddress> &addrs) noexcept {
	for(const CellAddress &addr : addrs){
		revdeps.insert(addr);
	}
}

bool Cell::removeReverseDependency(CellAddress addr) noexcept {
	auto it=revdeps.find(addr);
	if(it==revdeps.end())return false;
	revdeps.erase(it);
	return true;
}

void Cell::setEditString(string s) noexcept {
	if(value)delete value;
	value=CellValue::cellValueFromString(s);
}

string Cell::getDisplayString() const noexcept {
	return value->getDisplayString();
}

string Cell::getEditString() const noexcept {
	return value->getEditString();
}

void Cell::update(const CellArray &cells) noexcept {
	if(value->update(cells)){
		CellValue *newvalue=CellValue::cellValueFromString(value->getEditString());
		delete value;
		value=newvalue;
		value->update(cells);
	}
}

vector<CellAddress> Cell::getDependencies() const noexcept {
	return value->getDependencies();
}

/*
Serialisation format:
First the number of reverse dependencies, then a list of them;
followed by the edit string.
*/
void Cell::serialise(ostream &os) const {
	writeUInt32LE(os,revdeps.size());
	for(const CellAddress &revdepaddr : revdeps){
		revdepaddr.serialise(os);
	}
	const string &s=value->getEditString();
	writeUInt32LE(os,s.size());
	os<<s;
}

void Cell::deserialise(istream &in){
	unsigned int nrevdeps=readUInt32LE(in);
	if(in.fail())return; //random allocation prevention
	revdeps.clear();
	unsigned int i;
	for(i=0;i<nrevdeps;i++){
		CellAddress ca=CellAddress::deserialise(in);
		revdeps.insert(ca);
		// revdeps.insert(CellAddress::deserialise(in));
	}
	unsigned int len=readUInt32LE(in);
	if(in.fail())return;
	string s;
	s.resize(len);
	in.read(&s.front(),len);
	setEditString(s);
}
