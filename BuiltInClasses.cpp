/* 
 * File:   BuiltInClasses.cpp
 * Author: tjoppen
 * 
 * Created on February 14, 2010, 6:48 PM
 */

#include <stdexcept>
#include <sstream>
#include "BuiltInClasses.h"
#include "main.h"

using namespace std;

BuiltInClass::BuiltInClass(string name) : Class(FullName(XSL, name), Class::SIMPLE_TYPE) {
}

BuiltInClass::~BuiltInClass() {
}

bool BuiltInClass::isBuiltIn() const {
    return true;
}

string BuiltInClass::generateAppender() const {
    throw runtime_error("generateAppender() called in BuiltInClass");
}

string BuiltInClass::generateElementSetter(string memberName, string nodeName, string tabs) const {
    ostringstream oss;
    oss << tabs << "tmp = document.createElement(\"" << nodeName << "\")" << endl;
    oss << tabs << "tmpText = document.createTextNode(str(" << memberName << "))" << endl;
    oss << tabs << "tmp.appendChild(tmpText)" << endl;
    oss << tabs << "node.appendChild(tmp)" << endl;

    return oss.str();
}

string BuiltInClass::generateAttributeSetter(string memberName, string attributeName, string tabs) const {
    ostringstream oss;

    oss << tabs << "tmpAttr = document.createAttribute(\"" << memberName << "\")" << endl;
	oss << tabs << "tmpAttr.value = str(" << attributeName << ")" << endl;
	oss << tabs << "node.setAttributeNode(tmpAttr)" << endl;

    return oss.str();
}

string BuiltInClass::generateParser() const {
    throw runtime_error("generateParser() called in BuiltInClass");
}

string BuiltInClass::generateMemberSetter(string memberName, string nodeName, string tabs) const {
    ostringstream oss;
    
    oss << tabs << memberName << " = ";
    string type = getClassname();
    if(type == "int" || type == "short" || type == "unsignedShort" || type == "unsignedInt" || type == "byte" || type == "unsignedByte") {
    	oss << "int(node.firstChild.nodeValue)";
    } else if(type == "long" || type == "unsignedLong") {
    	oss << "long(node.firstChild.nodeValue)";
    } else if(type == "float" || type == "double") {
    	oss << "float(node.firstChild.nodeValue)";
    } else if(type == "string") {
       	oss << "node.firstChild.nodeValue";
    } else {
    	oss << "str(node.firstChild.nodeValue)";
    } 

    return oss.str();
}

string BuiltInClass::generateAttributeParser(string memberName, string attributeName, string tabs) const {
    ostringstream oss;

    oss << tabs << "{" << endl;
    oss << tabs << "\tstringstream " << ssWithPostfix << ";" << endl;
    oss << tabs << "\t" << ssWithPostfix << " << XercesString(" << attributeName << "->getValue());" << endl;
    oss << tabs << "\t" << ssWithPostfix << " >> " << memberName << ";" << endl;
    oss << tabs << "}" << endl;

    return oss.str();
}
