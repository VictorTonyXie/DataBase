/*
 * common.h
 *
 *  Created on: Dec 23, 2015
 *      Author: thomas
 */

#ifndef COMMON_COMMON_H_
#define COMMON_COMMON_H_

enum AttrType {
    MyINT,
    FLOAT,
    STRING
};

enum AggeType {
    NONE,
    SUM,
    AVG,
    MIN,
    MAX
};

enum CompOp {
    EQ_OP,
    LT_OP,
    GT_OP,
    LE_OP,
    GE_OP,
    NE_OP,
    LIKE_OP,
    NULL_OP,
    NO_OP
};

//
// RID: Record id interface
//
class RID {
public:
	static const int NULL_PAGE = -1;
	static const int NULL_SLOT = -1;
	RID() : page(NULL_PAGE), slot(NULL_SLOT) {}     // Default constructor
	RID(int pageNum, int slotNum) : page(pageNum), slot(slotNum) {}
	~RID(){}                                        // Destructor

	int GetPageNum(int &pageNum) const          // Return page number
	{ pageNum = page; return 0; }
	int GetSlotNum(int &slotNum) const         // Return slot number
	{ slotNum = slot; return 0; }

	int Page() const          // Return page number
	{ return page; }
	int Slot() const          // Return slot number
	{ return slot; }
	void Copy(RID _rid){
	  int Page = 0;
	  int Slot = 0;
          _rid.GetPageNum(Page);
	  _rid.GetSlotNum(Slot);
	  this->page = Page;
	  this->slot = Slot;
	}
	bool operator==(const RID & rhs) const
	{
		int p;
		int s;
		rhs.GetPageNum(p);
		rhs.GetSlotNum(s);
		return (p == page && s == slot);
	}

private:
	int page;
	int slot;
};

inline ostream& operator <<(ostream & os, const RID& r)
{
	int p, s;
	r.GetPageNum(p);
	r.GetSlotNum(s);
	os << "[" << p << "," << s << "]";
	return os;
}


#endif /* COMMON_COMMON_H_ */
