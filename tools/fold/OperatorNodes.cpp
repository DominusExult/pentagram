/*
 *	OperatorNodes.cpp -
 *
 *  Copyright (C) 2002 The Pentagram Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "OperatorNodes.h"
#include "Folder.h"

/****************************************************************************
	UniOperatorNode
 ****************************************************************************/

void UniOperatorNode::print_unk(Console &o, const uint32 isize) const
{
	assert(rtype().type()!=Type::T_INVALID);
	switch(mtype)
	{
		case NOT:
			assert(node!=0);
			o.Printf("not ");
			node->print_unk(o, isize);
			break;
		default: assert(false); // can't happen
	}
}

void UniOperatorNode::print_asm(Console &o) const
{
	assert(rtype().type()!=Type::T_INVALID);
	switch(mtype)
	{
		case NOT:
			assert(node!=0);
			node->print_asm(o);
			o.Putchar('\n');
			Node::print_asm(o);
			o.Printf("not");
			break;
		default: assert(false); // can't happen
	}
}

void UniOperatorNode::print_bin(OBufferDataSource &o) const
{
	assert(rtype().type()!=Type::T_INVALID);
	switch(mtype)
	{
		case NOT:
			assert(node!=0);
			node->print_bin(o);
			o.write1(0x30);
			break;
		default: assert(false); // can't happen
	}
}

bool UniOperatorNode::fold(Unit */*unit*/, std::deque<Node *> &nodes)
{
	switch(mtype)
	{
		case NOT:
			assert(nodes.back()->rtype()==Type::T_WORD || nodes.back()->rtype()==Type::T_BYTE);
			grab_n(nodes);
			break;
		default: assert(false);
	}
	return true;
};

/*void UniOperatorNode::fold(const uint32 end)
{
	assert(foldops[end].deleted==false);

	_opcode = foldops[end].op(); // store the op locally
	_offset = foldops[end].offset; // store the offset locally

	switch(_opcode)
	{
		case 0x3B: mtype=BITNOT;	rtype=Type::T_WORD; break;
		default: assert(false); // can't happen
	}

	sint32 tempsize = 2;
	grab_n(tempsize, end);
	assert(tempsize==0);

	// debugging
	assert(node!=0);
}*/

/*void UniOperatorNode::print() const
{
	assert(node!=0);

	printf("(%s) (", rtype.name());

	switch(mtype)
	{
		case BITNOT:	printf("~");		break;
		default:	assert(false); // can't happen
	}

	node->print();
	printf(")");
}*/

/****************************************************************************
	BinOperatorNode
 ****************************************************************************/

void BinOperatorNode::print_unk(Console &o, const uint32 isize) const
{
	assert(rtype().type()!=Type::T_INVALID);
	assert(lnode!=0);
	assert(rnode!=0);
	
	lnode->print_unk(o, isize);
	Node::print_linenum_unk(o, isize);
	switch(mtype)
	{
		case M_CMP: o.Printf(" == "); break;
		case M_GT:  o.Printf(" > ");  break;
		case M_OR:  o.Printf(" or "); break;
		default: assert(false); // can't happen
	}
	rnode->print_unk(o, isize);
}

void BinOperatorNode::print_asm(Console &o) const
{
	assert(rtype().type()!=Type::T_INVALID);
	assert(lnode!=0);
	assert(rnode!=0);
	
	Node::print_linenum_asm(o);
	lnode->print_asm(o);
	o.Putchar('\n');
	rnode->print_asm(o);
	o.Putchar('\n');
	Node::print_asm(o);
	switch(mtype)
	{
		case M_CMP: o.Printf("cmp"); break;
		case M_GT:  o.Printf("gt");  break;
		case M_OR:  o.Printf("or");  break;
		default: assert(false); // can't happen
	}
}

void BinOperatorNode::print_bin(OBufferDataSource &o) const
{
	assert(rtype().type()!=Type::T_INVALID);
	assert(lnode!=0);
	assert(rnode!=0);
	
	Node::print_linenum_bin(o);
	lnode->print_bin(o);
	rnode->print_bin(o);
	switch(mtype)
	{
		case M_CMP: o.write1(0x24); break;
		case M_GT:  o.write1(0x2C); break;
		case M_OR:  o.write1(0x34); break;
		default: assert(false); // can't happen
	}
}

bool BinOperatorNode::fold(Unit *unit, std::deque<Node *> &nodes)
{
	switch(mtype)
	{
		case M_CMP:
		case M_GT:
		case M_OR:
			assert(nodes.back()->rtype()==Type::T_WORD || nodes.back()->rtype()==Type::T_BYTE);
			grab_r(nodes);
			//fold_linenum(nodes);
			assert(nodes.back()->rtype()==Type::T_WORD || nodes.back()->rtype()==Type::T_BYTE);
			grab_l(nodes);
			fold_linenum(nodes);
			rtype(Type::T_WORD);
			break;
		default: assert(print_assert(this, unit));
	}
	return true;
};
