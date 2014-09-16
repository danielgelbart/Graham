#include <iostream>
#include "Utils.hpp"
#include "HtmlNode.h"
#include "HtmlParserObj.h"

using namespace std;


HtmlNode::~HtmlNode()
{
    deleteRange(_children.begin(), _children.end());    
}


void 
HtmlNode::addContentChild(const string& content)
{
    HtmlData* child = new HtmlText(trimSpaces(content));
    _children.push_back(child);
}

HtmlNode* 
HtmlNode::addTagChild(const std::string& tag)
{
    HtmlNode* child = new HtmlNode(this, tag);
    _children.push_back(child);
    return child;
}

string 
HtmlNode::content() const
{
    string ret;
    for (auto it = _children.begin(); it != _children.end(); ++it)
	ret += (*it)->content();
    
    return ret;
}
string 
HtmlNode::contentDelineated() const
{
    string ret;
    for (auto it = _children.begin(); it != _children.end(); ++it)
	ret += (*it)->contentDelineated();
    
    if (ret.length() > 0)
	ret = "<" + ret + ">";
    return ret;
}

string 
HtmlNode::contentFromTag(const string& tag) const
{
    string ret;

    HtmlNode::ConstNodeCollection nodes;
    allTagsRecur(tag, nodes);
    for (auto it = nodes.begin(); it != nodes.end(); ++it)
        ret += "<" + trimSpaces((*it)->content()) + ">";
    
    return ret;
}

string 
HtmlNode::contentWithoutTags(const set<string>& tags) const
{
    if (tags.count(_tag) != 0)
        return "";
    
    string ret;
    for (auto it = _children.begin(); it != _children.end(); ++it)
        ret += (*it)->contentWithoutTags(tags);
    
    if (ret.length() > 0){
        if (_tag == "b" || _tag == "strong" || _tag == "em" || _tag == "i" )
            ret = " " + ret + " ";
        else
            ret = "<" + ret + ">";
    }
    return ret;
}

string
HtmlNode::contentFromTagParam(const string& tag, 
			      const string& param) const
{
    string ret;
    for (auto it = _children.begin(); it != _children.end(); ++it)
	ret += (*it)->contentFromTagParam(tag, param);
    
    if (_tag == tag){
	auto it = _param2Val.find(param);
	if (it != _param2Val.end())
	    ret += string("<") + it->second + string(">");
    }
    return ret;
}


string 
HtmlNode::operator[](const string& param) const
{
    auto it = _param2Val.find(param);
    if (it == _param2Val.end())
	return string();
    else
	return it->second;
	
}

void 
HtmlNode::insertParamVal(const std::string& param, const std::string& val)
{
    _param2Val[param] = val;
}

void 
HtmlText::nicePrint(ostream& os, const size_t indent) const
{
    if (_content.size())
	os << string(indent, ' ') << _content << endl;
}

void 
HtmlNode::nicePrint(ostream& os, const size_t indent) const
{
    if (_tag.size()){
	os << string(indent, ' ') << '<' << _tag;
	for (auto it  = _param2Val.begin(); it != _param2Val.end(); ++it)
	    os << " " << it->first << "=\"" << it->second << '"';
	
	if (HtmlParser::isStandAlone(_tag))
	    os << "/>" << endl;
	else
	    os << '>' << endl;
    }
    if (HtmlParser::isStandAlone(_tag))
	return;
    
    for (auto it = _children.begin(); it != _children.end(); ++it)
        (*it)->nicePrint(os, indent + 4);
    
    if (_tag.size())
	os << string(indent, ' ') << "</" << _tag << '>' << endl;
}

template<class NodeClass, class CollectionClass>
void 
genericAllTagsRecur(NodeClass* n, const string& tag, CollectionClass& ret)
{
    if (n->_tag == tag)
	ret.push_back(n);
    for (auto it = n->_children.begin(); it != n->_children.end(); ++it)
	if ((*it)->isNode()){
            NodeClass* child = dynamic_cast<NodeClass*>(*it);
	    genericAllTagsRecur(child, tag, ret);
        }    
}

void 
HtmlNode::allTagsRecur(const string& tag, NodeCollection& ret)
{
    genericAllTagsRecur(this, tag, ret);
}

void 
HtmlNode::allTagsRecur(const string& tag, ConstNodeCollection& ret) const
{
    genericAllTagsRecur(this, tag, ret);
}


bool 
HtmlNode::hasChildTag(const std::string& tag) const
{
    if (_tag == tag)
	return true;
    for (auto it = _children.begin(); it != _children.end(); ++it)
	if ((*it)->isNode())
	    if ((dynamic_cast<HtmlNode*>(*it))->hasChildTag(tag))
		return true;
    
    return false;
}
