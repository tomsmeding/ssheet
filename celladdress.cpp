#include "celladdress.h"
#include "util.h"

using namespace std;

CellAddress::CellAddress(unsigned int row,unsigned int column) noexcept
	:row(row),column(column){}

Maybe<CellAddress> CellAddress::fromRepresentation(string repr) noexcept {
	const int sz=repr.size();
	int i;
	int row=0,column=0;
	for(i=0;i<sz;i++){
		if(repr[i]<'A'||repr[i]>'Z')break;
		column=26*column+repr[i]-'A'+1;
	}
	if(i==0||i==sz){ //other things before letters, or no digits
		return Nothing();
	}
	column--;
	for(;i<sz;i++){
		if(repr[i]<'0'||repr[i]>'9'){ //other things after the digits
			return Nothing();
		}
		row=10*row+repr[i]-'0';
	}
	row--;
	return CellAddress(row,column);
}

string CellAddress::toRepresentation() const noexcept {
	return columnLabel(column)+to_string(row+1);
}

CellAddress CellAddress::deserialise(istream &in){
	unsigned int row=readUInt32LE(in);
	unsigned int column=readUInt32LE(in);
	return CellAddress(row,column);
}

void CellAddress::serialise(ostream &os) const {
	writeUInt32LE(os,row);
	writeUInt32LE(os,column);
}


bool operator==(const CellAddress &a,const CellAddress &b) noexcept {
	return a.row==b.row&&a.column==b.column;
}


namespace std{
	bool less<CellAddress>::operator()(const CellAddress &a,const CellAddress &b) const noexcept {
		return a.row<b.row||(a.row==b.row&&a.column<b.column);
	}

	size_t hash<CellAddress>::operator()(const CellAddress &a) const noexcept {
		return ((size_t)a.row<<32)|a.column;
	}
}



CellRange::CellRange(CellAddress from,CellAddress to) noexcept
	:from(from),to(to){}

Maybe<CellRange> CellRange::fromRepresentation(string repr) noexcept {
	const size_t idx=repr.find(':');
	if(idx==string::npos){
		return Nothing();
	}
	Maybe<CellAddress> mfrom=CellAddress::fromRepresentation(repr.substr(0,idx));
	if(mfrom.isNothing()){
		return Nothing();
	}
	Maybe<CellAddress> mto=CellAddress::fromRepresentation(repr.substr(idx+1));
	if(mto.isNothing()){
		return Nothing();
	}
	CellAddress from=mfrom.fromJust(),to=mto.fromJust();
	if(from.column>to.column){
		swap(from.column,to.column);
	}
	if(from.row>to.row){
		swap(from.row,to.row);
	}
	return CellRange(from,to);
}

string CellRange::toRepresentation() const noexcept {
	return from.toRepresentation()+":"+to.toRepresentation();
}

unsigned int CellRange::size() const noexcept {
	return (to.column-from.column+1)*(to.row-from.row+1);
}
