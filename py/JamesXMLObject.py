from xml.dom.minidom import *

class JamesXMLObject:
    def __init__(self):
        doc = None

    def toxml(self):
        impl = getDOMImplementation()
        newdoc = impl.createDocument(self.get_namespace(), self.get_name(), None)
        root_node = newdoc.documentElement
        tmpAttr = newdoc.createAttribute("xmlns")
        tmpAttr.value = self.get_namespace()
        root_node.setAttributeNode(tmpAttr)

        self.append_children(root_node, newdoc)
        return newdoc.toxml("UTF-8")

    def parsexml(self, xml):
        dom = parse(xml)
        node = dom.documentElement
        if node.tagName == self.get_name():
            self.parse_node(node)

    def append_children(self, node, document):
        pass

    def parse_node(self, node):
        pass

    def get_name(self):
        pass

    def get_namespace(self):
        pass
    
    def strToBool(self, string):
        if(string.lower() == "true" or string == "1"):
            return True
        return False



def main():
    pass

if __name__ == "__main__":
    main()
