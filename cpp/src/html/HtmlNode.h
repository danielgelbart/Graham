#ifndef HTML_NODE_H
#define HTML_NODE_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <iosfwd>

// generic data in the DOM tree
class HtmlData {
public:
    virtual ~HtmlData() {}

    // returns all the plain text in the subtree
    virtual std::string content() const = 0;

    // returns all the plain text in the subtree, with content in tags marked 
    // with < xxx >
    virtual std::string contentDelineated() const = 0;

    // returns all the plain text in the subtree, skipping set of tags
    virtual std::string contentWithoutTags(const std::set<std::string>& tags) const = 0;
    
    // all content from a specific tag
    virtual std::string contentFromTag(const std::string& tag) const = 0;
    
    // content from a certain tag in a certain param
    virtual std::string contentFromTagParam(const std::string& tag, 
					    const std::string& param) const = 0;

    // prints entire subtree nicely indented
    virtual void nicePrint(std::ostream& os, const size_t indent) const = 0;

    // returns type of derived class
    virtual bool isNode() const = 0;
};

// plain text in the DOM tree
class HtmlText :public HtmlData {
public:
    HtmlText(const std::string& content) : _content(content) {}
    std::string content() const { return _content; }
    std::string contentDelineated() const { return _content ; }
    std::string contentWithoutTags(const std::set<std::string>& tags) const { 
	return _content; 
    }
    std::string contentFromTagParam(const std::string& tag, 
				    const std::string& param) const {
	return std::string();
    }
    std::string contentFromTag(const std::string& tag) const {
        return std::string();
    }

    void nicePrint(std::ostream& os, const size_t indent) const;

private:
    bool isNode() const { return false; }

private:
    std::string   _content;
};

// node in the DOM tree
class HtmlNode : public HtmlData {
public:
    HtmlNode(HtmlNode* parent) : _parent(parent) {}
    HtmlNode(HtmlNode* parent, const std::string& tag) 
	: _parent(parent), _tag(tag) 
	{}
    ~HtmlNode();

    //accessors
    HtmlNode* parent() const { return _parent; }
    const std::string& tag() const { return _tag; }
    std::string content() const;
    std::string contentDelineated() const;
    std::string contentWithoutTags(const std::set<std::string>& tags) const;
    std::string contentFromTag(const std::string& tags) const;
    std::string contentFromTagParam(const std::string& tag, 
				    const std::string& param) const;

    std::string operator[](const std::string& param) const;

    //mutators
    void insertParamVal(const std::string& param, const std::string& val);
    void addContentChild(const std::string& content);
    HtmlNode* addTagChild(const std::string& tag);

    bool hasChildTag(const std::string& tag) const;
    
    typedef std::vector<HtmlNode*> NodeCollection;
    typedef std::vector<const HtmlNode*> ConstNodeCollection;

    void allTagsRecur(const std::string& tag, ConstNodeCollection& ret) const;
    void allTagsRecur(const std::string& tag, NodeCollection& ret);
    
    void nicePrint(std::ostream& os, const size_t indent) const;

    template<class NodeClass, class CollectionClass>
    friend 
    void 
    genericAllTagsRecur(NodeClass* n, 
                        const std::string& tag, CollectionClass& ret);

private:
    //no copying allowed - these functions are not implemented
    HtmlNode(const HtmlNode& other);
    HtmlNode(HtmlNode& other);
    HtmlNode operator = (const HtmlNode& other);

    bool isNode() const { return true;}
    

private:
    HtmlNode*                                     _parent;
    std::string                                   _tag;

    // holds tag attributes and values
    typedef std::map<std::string, std::string>    ParamMap;
    ParamMap                                      _param2Val;

    typedef std::vector<HtmlData*>                Children;
    Children                                      _children;


};


#endif //HTML_NODE_H

