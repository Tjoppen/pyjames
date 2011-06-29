/* 
 * File:   BuiltInClasses.h
 * Author: tjoppen
 *
 * Created on February 14, 2010, 6:48 PM
 */

#ifndef _BUILTINCLASSES_H
#define	_BUILTINCLASSES_H

#include "Class.h"

class BuiltInClass : public Class {
public:
    BuiltInClass(std::string name);
    virtual ~BuiltInClass();

    bool isBuiltIn() const;

    std::string generateAppender() const;
    virtual std::string generateElementSetter(std::string memberName, std::string nodeName, std::string tabs) const;
    virtual std::string generateAttributeSetter(std::string memberName, std::string attributeName, std::string tabs) const;
    std::string generateParser() const;
    virtual std::string generateMemberSetter(std::string memberName, std::string nodeName, std::string tabs) const;
    virtual std::string generateAttributeParser(std::string memberName, std::string attributeName, std::string tabs) const;
};

#define GENERATE_BUILTIN(name, xslName, classname)\
class name : public BuiltInClass {\
public:\
    name() : BuiltInClass(xslName) {}\
    name(std::string xslOverride) : BuiltInClass(xslOverride) {}\
    std::string getClassname() const {return classname;}

#define GENERATE_BUILTIN_ALIAS(name, base, override)\
class name : public base {\
public:\
    name() : base(override) {}

GENERATE_BUILTIN(ByteClass, "byte", "char")};
GENERATE_BUILTIN(UnsignedByteClass, "unsignedByte", "unsigned char")};
GENERATE_BUILTIN(ShortClass, "short", "short")};
GENERATE_BUILTIN(UnsignedShortClass, "unsignedShort", "unsigned short")};
GENERATE_BUILTIN(IntClass, "int", "int")};
GENERATE_BUILTIN(UnsignedIntClass, "unsignedInt", "unsigned int")};
GENERATE_BUILTIN(LongClass, "long", "long long")};
GENERATE_BUILTIN(UnsignedLongClass, "unsignedLong", "unsigned long long")};
GENERATE_BUILTIN(StringClass, "string", "std::string")
    std::string generateElementSetter(std::string memberName, std::string nodeName, std::string tabs) const {
        std::ostringstream oss;
       
        oss << tabs << "tmp = document.createElement(\"" << nodeName << "\")" << std::endl;
        oss << tabs << "tmpText = document.createTextNode(str(" << memberName << "))" << std::endl;
        oss << tabs << "tmp.appendChild(tmpText)" << std::endl;
        oss << tabs << "node.appendChild(tmp)" << std::endl;
    
        return oss.str();
    }

    std::string generateAttributeSetter(std::string memberName, std::string attributeName, std::string tabs) const {
        std::ostringstream oss;
    
        oss << tabs << "tmpAttr = document.createAttribute(\"" << memberName << "\")" << std::endl;
    	oss << tabs << "tmpAttr.value = str(" << attributeName << ")" << std::endl;
    	oss << tabs << "node.setAttributeNode(tmpAttr)" << std::endl;
    
        return oss.str();
    }

    std::string generateMemberSetter(std::string memberName, std::string nodeName, std::string tabs) const {
        std::ostringstream oss;

        oss << tabs << memberName << " = ";
        std::string type = getClassname();
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
        oss << std::endl;

        return oss.str();
    }

    std::string generateAttributeParser(std::string memberName, std::string attributeName, std::string tabs) const {
        return tabs + memberName + " = XercesString(" + attributeName + "->getValue());";
    }
};

GENERATE_BUILTIN(FloatClass, "float", "float")};
GENERATE_BUILTIN(DoubleClass, "double", "double")};

GENERATE_BUILTIN(BooleanClass, "boolean", "bool")
    std::string generateElementSetter(std::string memberName, std::string nodeName, std::string tabs) const {
        std::ostringstream oss;
        
        oss << tabs << "tmp = document.createElement(\"" << nodeName << "\")" << std::endl;
        if(getClassname() == "bool") {
            oss << tabs << "tmpText = document.createTextNode(str(" << memberName << ").lower())" << std::endl;
        } else {
            oss << tabs << "tmpText = document.createTextNode(str(" << memberName << "))" << std::endl;
        }
        oss << tabs << "tmp.appendChild(tmpText)" << std::endl;
        oss << tabs << "node.appendChild(tmp)" << std::endl;
    
        return oss.str();
    }

    std::string generateAttributeSetter(std::string memberName, std::string attributeName, std::string tabs) const {
        std::ostringstream oss;
    
        oss << tabs << "tmpAttr = document.createAttribute(\"" << memberName << "\")" << std::endl;
    	oss << tabs << "tmpAttr.value = str(" << attributeName << ")" << std::endl;
    	oss << tabs << "node.setAttributeNode(tmpAttr)" << std::endl;
    
        return oss.str();
    }

    std::string generateMemberSetter(std::string memberName, std::string nodeName, std::string tabs) const {
        return tabs + memberName + " = bool(node.firstChild.nodeValue)";
    }

    std::string generateAttributeParser(std::string memberName, std::string attributeName, std::string tabs) const {
        std::ostringstream oss;

        oss << tabs << "{" << std::endl;
        oss << tabs << "//TODO: Strip string prior to this?" << std::endl;
        oss << tabs << "XercesString " << tempWithPostfix << "(" << attributeName << "->getValue());" << std::endl;
        oss << tabs << memberName << " = " << tempWithPostfix << " == \"true\" || " << tempWithPostfix << " == \"1\";" << std::endl;
        oss << tabs << "}" << std::endl;

        return oss.str();
    }
};

GENERATE_BUILTIN(HexBinaryClass, "hexBinary", "james::HexBinary")};

//aliases
GENERATE_BUILTIN_ALIAS(IntegerClass, IntClass, "integer")};
GENERATE_BUILTIN_ALIAS(AnyURIClass, StringClass, "anyURI")};
GENERATE_BUILTIN_ALIAS(TimeClass, StringClass, "time")};
GENERATE_BUILTIN_ALIAS(DateClass, StringClass, "date")};
GENERATE_BUILTIN_ALIAS(DateTimeClass, StringClass, "dateTime")};
GENERATE_BUILTIN_ALIAS(LanguageClass, StringClass, "language")};

#endif	/* _BUILTINCLASSES_H */

