/* 
 * File:   Class.cpp
 * Author: tjoppen
 * 
 * Created on February 14, 2010, 4:20 PM
 */

#include "Class.h"
#include <sstream>
#include <stdexcept>
#include <iostream>

using namespace std;

extern bool verbose;

const string variablePostfix = "_james";

const string nodeWithPostfix = "node" + variablePostfix;
const string tempWithPostfix = "temp" + variablePostfix;
const string convertedWithPostfix = "converted" + variablePostfix;
const string ssWithPostfix = "ss" + variablePostfix;

const string t = "    "; // Python indentation step (four spaces)

Class::Class(FullName name, ClassType type) : name(name), type(type), 
        isDocument(false), base(NULL)  {
}

Class::Class(FullName name, ClassType type, FullName baseType) : name(name),
        type(type), isDocument(false), baseType(baseType), base(NULL) {
}

Class::~Class() {
}

bool Class::isSimple() const {
    return type == SIMPLE_TYPE;
}

bool Class::isBuiltIn() const {
    return false;
}

bool Class::hasBase() const {
    return baseType.second.length() > 0;
}

void Class::addConstructor(const Constructor& constructor) {
    //first make sure an identical constructor doesn't already exist
    for(list<Constructor>::const_iterator it = constructors.begin(); it != constructors.end(); it++)
        if(it->hasSameSignature(constructor))
            return;

    constructors.push_back(constructor);
}

void Class::doPostResolveInit() {
    //figure out which constructors we need
    addConstructor(Constructor(this, true,  true));

    if(constructors.size() == 0)
        throw runtime_error("No constructors in class " + getClassname());
}

std::list<Class::Member>::iterator Class::findMember(std::string name) {
    for(std::list<Member>::iterator it = members.begin(); it != members.end(); it++)
        if(it->name == name)
            return it;

    return members.end();
}

void Class::addMember(Member memberInfo) {
    if(findMember(memberInfo.name) != members.end())
        throw runtime_error("Member " + memberInfo.name + " defined more than once in " + this->name.second);

    if(verbose) cerr << this->name.second << " got " << memberInfo.type.first << ":" << memberInfo.type.second << " " << memberInfo.name << ". Occurance: ";

    if(memberInfo.maxOccurs == UNBOUNDED) {
        if(verbose) cerr << "at least " << memberInfo.minOccurs;
    } else if(memberInfo.minOccurs == memberInfo.maxOccurs) {
        if(verbose) cerr << "exactly " << memberInfo.minOccurs;
    } else {
        if(verbose) cerr << "between " << memberInfo.minOccurs << "-" << memberInfo.maxOccurs;
    }

    if(verbose) cerr << endl;

    members.push_back(memberInfo);
}

/**
 * Default implementation of generateAppender()
 */
string Class::generateAppender() const {
    ostringstream oss;

	oss << t << endl;
    oss << t << "def append_children(self, node, document):" << endl;
    if(base) {
        if(base->isSimple()) {
            //simpleContent
		    oss << t << t << "tmpText = document.createTextNode(str(self.content))" << endl;
		    oss << t << t << "node.appendChild(tmpText)" << endl;
        } else {
            //call base appender
            oss << t << t << base->getClassname() << ".append_children(self, node, document);" << endl;
        }
    } 
    for(std::list<Member>::const_iterator it = members.begin(); it != members.end(); it++) {
        string name = it->name;
        string setterName = it->name;
        string nodeName = name + "Node";
        string et = t+t; // Extra tabs
        string subName = "self." + name;

        if(it != members.begin())
            oss << endl;

		if(it->isOptional()) {
            oss << et << "if self." << name << " != None:" << endl;
            et = et + t;
        }
        if(it->isArray()) {
            oss << et << "if len(self." << name << ") > 0:" << endl;
            et = et + t;
			oss << et << "for value in self." << name << ":" << endl;
            subName = "value";
            et = et + t;
        } 
       	
        if(it->isAttribute) {
    		//attribute
            oss << it->cl->generateAttributeSetter(name, subName, et);
		} else {
            //element
            oss << it->cl->generateElementSetter(subName, name, et);
        }
    }

    return oss.str();
}

string Class::generateElementSetter(string memberName, string nodeName, string tabs) const {
    ostringstream oss;
    if(isSimple() && base)
        return base->generateElementSetter(memberName, nodeName, tabs);

    oss << tabs << "tmp = document.createElement(\"" << nodeName << "\")" << endl;
	oss << tabs << "node.appendChild(tmp)" << endl;
    oss << tabs << memberName << ".append_children(tmp, document)" << endl;
    return oss.str();
}

string Class::generateAttributeSetter(string memberName, string attributeName, string tabs) const {
    if(isSimple() && base)
        return base->generateAttributeSetter(memberName, attributeName, tabs);

    throw runtime_error("Tried to generateAttributeSetter() for a non-simple Class");
}

string Class::generateParser() const {
    ostringstream oss;
    string childName = "child" + variablePostfix;
    string nameName = "name" + variablePostfix;

	oss << t << endl;
    oss << t << "def parse_node(self, node):" << endl;

    if(base) {
        if(base->isSimple()) {
            //simpleContent
            oss << base->generateMemberSetter("self.content", "content", t+t) << endl;
        } else {
            oss << t << t << base->getClassname() << ".parse_node(self, node)" << endl;
        }
    }
	
	oss << endl;
    oss << t << t << "for childNode in node.childNodes:" << endl;
    oss << t << t << t << "if childNode.localName == None:" << endl;
    oss << t << t << t << t << "continue" << endl;
    oss << endl;

    //TODO: replace this with a map<pair<string, DOMNode::ElementType>, void(*)(DOMNode*)> thing?
    //in other words, lookin up parsing function pointers in a map should be faster then all these string comparisons
    bool first = true;

    for(std::list<Member>::const_iterator it = members.begin(); it != members.end(); it++) {
        if(!it->isAttribute) {
            if(first)
                first = false;
            else
                oss << endl;

            oss << t << t << t << "if childNode.localName == \"" << it->name << "\" and childNode.nodeType == Node.ELEMENT_NODE:" << endl;
            
            oss << t << t << t << t << "self." << it->name;
			if(it->isArray()) {
				oss << ".append(";
			} else {
				oss << " = ";
			}
            string type = it->type.second;
            if(type == "int" || type == "short" || type == "unsignedShort" || type == "unsignedInt" || type == "byte" || type == "unsignedByte") {
				oss << "int(childNode.firstChild.nodeValue)";
            } else if(type == "long" || type == "unsignedLong") {
				oss << "long(childNode.firstChild.nodeValue)";
            } else if(type == "float" || type == "double") {
				oss << "float(childNode.firstChild.nodeValue)";
            } else if(type == "boolean") {
            	oss << "self.strToBool(childNode.firstChild.nodeValue)";
			} else if(type == "string" || type == "anyURI") {
				oss << "str(childNode.firstChild.nodeValue)";
            } else {
                oss << type << ".fromNode(childNode)";
            }
			
			if(it->isArray()) {
				oss << ")" << endl;
			} else {
				oss << endl;
			}
        }
    }

    //attributes
    for(std::list<Member>::const_iterator it = members.begin(); it != members.end(); it++) {
        if(it->isAttribute) {
			oss << t << t << endl;
			oss << t << t << "if node.hasAttribute(\"" << it->name << "\"):" << endl;
			oss << t << t << t << "self." << it->name << " = ";

            string type = it->type.second;
            if(type == "int" || type == "short" || type == "unsignedShort" || type == "unsignedInt" || type == "byte" || type == "unsignedByte") {
				oss << "int(";
            } else if(type == "long" || type == "unsignedLong") {
				oss << "long(";
            } else if(type == "float" || type == "double") {
				oss << "float(";
            } else  {
				oss << "str(";
            } 
			oss << "node.getAttribute(\"" << it->name << "\"))" << endl;
        }
    }

    return oss.str();
}

string Class::generateMemberSetter(string memberName, string nodeName, string tabs) const {
    if(isSimple() && base)
        return base->generateMemberSetter(memberName, nodeName, tabs);

    ostringstream oss;

    oss << tabs << memberName << ".parse_node(node);" << endl;

    return oss.str();
}

string Class::generateAttributeParser(string memberName, string attributeName, string tabs) const {
    if(isSimple() && base)
        return base->generateAttributeParser(memberName, attributeName, tabs);

    throw runtime_error("Tried to generateAttributeParser() for a non-simple Class");
}

string Class::getClassname() const {
    return name.second;
}

string Class::getBaseHeader() const {
    if(base->isSimple())
        return base->getBaseHeader();

    return "\"" + base->getClassname() + ".h\"";
}

bool Class::hasHeader() const {
    return true;
}

void Class::writeImplementation(ostream& os) const {
    ClassName className = name.second;
	
	os << "from JamesXMLObject import *" << endl;

	
	// Create class with inheritance
    if(isDocument) {
		os << "from " << base->getClassname() << " import *" << endl << endl;
    	os << "class " << className << "(" << base->getClassname() << ", JamesXMLObject):";
    } else if(base && !base->isSimple()) {
		os << "from " << base->getClassname() << " import *" << endl << endl;
    	os << "class " << className << "(" << base->getClassname() << "):";
    } else {
    	os << endl << "class " << className << "(JamesXMLObject):";
	}
	
    //not much implementation needed for simple types
    if(isSimple()) {
	    os << t << endl;
        os << t << "def __init__(self, content = None):" << endl;
        os << t << t << "self.content = content" << endl;
	    os << t << endl;
        os << t << "def __str__(self):" << endl;
        os << t << t << "return str(self.content)" << endl;
		
    } else {
		// Create constructor
		os << t << endl;
    	constructors.back().writeBody(os);
	
	}
	//Factory methods
	os << t << endl;
	os << t << "@classmethod" << endl;
	os << t << "def fromNode(cls, node):" << endl;
	os << t << t << "obj = cls()" << endl;
	os << t << t << "obj.parse_node(node)" << endl;
	os << t << t << "return obj" << endl;

	os << t << endl;
	os << t << "@classmethod" << endl;
	os << t << "def fromXML(cls, filename):" << endl;
	os << t << t << "obj = cls()" << endl;
	os << t << t << "obj.parsexml(filename)" << endl;
	os << t << t << "return obj" << endl;

    //get_name()
	os << t << endl;
    os << t << "def " << "get_name(self):" << endl;
    os << t << t <<"return \"" << className << "\"" << endl;

    //get_namespace()
	os << t << endl;
    os << t << "def " << "get_namespace(self):" << endl;
    os << t << t <<"return \"" << name.first << "\"" << endl;

	//append_children()
    os << generateAppender();

	//parse_node()
    os << generateParser() << endl;

    set<string> classesToInclude = getIncludedClasses();
    for(set<string>::const_iterator it = classesToInclude.begin(); it != classesToInclude.end(); it++)
    	os << "from " << *it << " import *" << endl;

    set<string> classesToPrototype = getPrototypeClasses();
    for(set<string>::const_iterator it = classesToPrototype.begin(); it != classesToPrototype.end(); it++)
    	if(*it != className)
			os << "from " << *it << " import *" << endl;
}

set<string> Class::getIncludedClasses() const {
    set<string> classesToInclude;

    //return classes of any simple non-builtin elements and any required non-simple elements
    for(list<Member>::const_iterator it = members.begin(); it != members.end(); it++)
        if((!it->cl->isBuiltIn() && it->cl->isSimple()) || (it->isRequired() && !it->cl->isSimple()))
            classesToInclude.insert(it->cl->getClassname());

    return classesToInclude;
}

set<string> Class::getPrototypeClasses() const {
    set<string> classesToInclude = getIncludedClasses();
    set<string> classesToPrototype;

    //return the classes of any non-simple non-required elements
    for(list<Member>::const_iterator it = members.begin(); it != members.end(); it++)
        if(classesToInclude.find(it->cl->getClassname()) == classesToInclude.end() && !it->cl->isSimple() && !it->isRequired())
            classesToPrototype.insert(it->cl->getClassname());

    return classesToPrototype;
}

void Class::writeHeader(ostream& os) const {
    ClassName className = name.second;

    os << "#ifndef _" << className << "_H" << endl;
    os << "#define _" << className << "_H" << endl;

    os << "#include <vector>" << endl;

    if(isDocument)
        os << "#include <istream>" << endl;

    os << "#include <xercesc/util/XercesDefs.hpp>" << endl;
    os << "XERCES_CPP_NAMESPACE_BEGIN class DOMElement; XERCES_CPP_NAMESPACE_END" << endl;
    os << "#include <libjames/HexBinary.h>" << endl;
    os << "#include <libjames/optional.h>" << endl;
    
    //simple types only need a typedef
    if(isSimple()) {
        os << "typedef " << base->getClassname() << " " << name.second << ";" << endl;
    } else {
        if(base && base->hasHeader())
            os << "#include " << getBaseHeader() << endl;
        
        if(!base || base->isSimple())
            os << "#include <libjames/XMLObject.h>" << endl;

        if(isDocument)
            os << "#include <libjames/XMLDocument.h>" << endl;

        //include member classes that we can't prototype
        set<string> classesToInclude = getIncludedClasses();

        for(set<string>::const_iterator it = classesToInclude.begin(); it != classesToInclude.end(); it++)
            os << "#include \"" << *it << ".h\"" << endl;

        os << endl;

        set<string> classesToPrototype = getPrototypeClasses();

        //member class prototypes, but only for classes that we haven't already included
        for(set<string>::const_iterator it = classesToPrototype.begin(); it != classesToPrototype.end(); it++)
            os << "class " << *it << ";" << endl;

        if(classesToPrototype.size() > 0)
            os << endl;

        if(isDocument)
            os << "class " << className << " : public " << base->getClassname() << ", public james::XMLDocument";
        else if(base && !base->isSimple())
            os << "class " << className << " : public " << base->getClassname();
        else
            os << "class " << className << " : public james::XMLObject";
        
        os << " {" << endl;
        os << "public:" << endl;

        //constructors
        for(list<Constructor>::const_iterator it = constructors.begin(); it != constructors.end(); it++) {
            os << "\t";
            it->writePrototype(os, true);
            os << endl;
        }

        //prototypes
        //add constructor for unmarshalling this document from an istream of string
        os << "\t" << className << "(std::istream& is);" << endl;
        os << endl;

        //factory method for unmarshalling std::string
        //we can't use a constructor since that would conflict with the required
        //element constructor for a type that only has one string element
        os << "\tstatic " << className <<  " fromString(const std::string& str);" << endl;
        os << endl;

        //string cast operator
        os << "\toperator std::string () const;" << endl;

        //getName()
        os << "\tstd::string getName() const;" << endl;

        //getNamespace()
        os << "\tstd::string getNamespace() const;" << endl;
        
        os << "\tvoid appendChildren(xercesc::DOMElement *node) const;" << endl;
        os << "\tvoid parseNode(xercesc::DOMElement *node);" << endl;
        os << endl;

        //simpleContent
        if(base && base->isSimple())
            os << "\t" << base->getClassname() << " content;" << endl;

        //members
        for(list<Member>::const_iterator it = members.begin(); it != members.end(); it++) {
            os << "\t";

            if(it->isOptional())
                os << "james::optional<";
            else if(it->isArray())
                os << "std::vector<";

            os << it->cl->getClassname();

            if(it->isOptional() || it->isArray())
                os << " >";

            os << " " << it->name << ";" << endl;
        }

        os << "};" << endl;
        os << endl;

        //include classes that we prototyped earlier
        for(set<string>::const_iterator it = classesToPrototype.begin(); it != classesToPrototype.end(); it++)
            os << "#include \"" << *it << ".h\"" << endl;

        if(classesToPrototype.size() > 0)
            os << endl;
    }
    
    os << "#endif //_" << className << "_H" << endl;
}

bool Class::shouldUseConstReferences() const {
    return true;
}

bool Class::Member::isArray() const {
    return maxOccurs > 1 || maxOccurs == UNBOUNDED;
}

bool Class::Member::isOptional() const {
    return minOccurs == 0 && maxOccurs == 1;
}

bool Class::Member::isRequired() const {
    return !isArray() && !isOptional();
}

std::list<Class::Member> Class::getElements(bool includeBase, bool vectors, bool optionals) const {
    std::list<Member> ret;

    if(includeBase && base)
        ret = base->getElements(true, vectors, optionals);

    //regard the contents of a complexType with simpleContents as a required
    //element named "content" since we already have that as an element
    if(base && base->isSimple()) {
        Member contentMember;
        contentMember.name = "content";
        contentMember.cl = base;
        contentMember.minOccurs = contentMember.maxOccurs = 1;

        ret.push_back(contentMember);
    }

    for(std::list<Member>::const_iterator it = members.begin(); it != members.end(); it++)
        if(it->isRequired() || (it->isArray() && vectors) || (it->isOptional() && optionals))
            ret.push_back(*it);

    return ret;
}

Class::Constructor::Constructor(Class *cl) : cl(cl) {
}

Class::Constructor::Constructor(Class *cl, bool vectors, bool optionals) :
        cl(cl) {
    if(cl->base)
        baseArgs = cl->base->getElements(true, vectors, optionals);

    ourArgs = cl->getElements(false, vectors, optionals);
}

list<Class::Member> Class::Constructor::getAllArguments() const {
    list<Class::Member> ret = baseArgs;

    ret.insert(ret.end(), ourArgs.begin(), ourArgs.end());

    return ret;
}

bool Class::Constructor::hasSameSignature(const Constructor& other) const {
    list<Member> a = getAllArguments();
    list<Member> b = other.getAllArguments();

    if(a.size() != b.size())
        return false;

    list<Member>::iterator ita = a.begin(), itb = b.begin();

    //return false if the arguments in any position are of different types or
    //if one is an array but the other isn't
    for(; ita != a.end(); ita++, itb++)
        if(ita->cl->getClassname() != itb->cl->getClassname() || ita->isArray() != itb->isArray())
            return false;

    return true;
}

void Class::Constructor::writePrototype(ostream &os, bool withSemicolon) const {
    list<Member> all = getAllArguments();

    os << t << "def __init__" << "(self, ";

    for(list<Member>::const_iterator it = all.begin(); it != all.end(); it++) {
        if(it != all.begin())
            os << ", ";
	    
		os << it->name << " = None";
    }

    os << "):" << endl;
}

void Class::Constructor::writeBody(ostream &os) const {
    list<Member> all = getAllArguments();
    writePrototype(os, false);
	
    for(list<Member>::const_iterator it = all.begin(); it != all.end(); it++) {
	   	os << t << t << "self." << it->name << " = " << it->name << endl;
		if(it->isArray()) {
			os << t << t << "if self." << it->name << " == None:" << endl;
			os << t << t << t << "self." << it->name << " = []" << endl;
		}
    }
}
