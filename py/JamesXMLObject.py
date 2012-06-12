# Copyright 2011 Ludvig Widman
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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

    @classmethod
    def fromNode(cls, node):
        obj = cls()
        obj.parse_node(node)
        return obj

    @classmethod
    def fromxml(cls, filename):
        obj = cls()
        obj.parsexml(filename)
        return obj

    @classmethod
    def fromString(cls, str):
        obj = cls()
        obj.parseString(str)
        return obj

    def parseString(self, str):
        dom = parseString(str)
        self.parsedom(dom)

    def parsexml(self, xml):
        dom = parse(xml)
        self.parsedom(dom)

    def parsedom(self, dom):
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

    def strToHex(self, string):
        import binascii
        return binascii.unhexlify(string)

def main():
    pass

if __name__ == "__main__":
    main()
