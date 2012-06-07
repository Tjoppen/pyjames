# This file is in the public domain.
import sys
from xml.dom.minidom import *
from generated.PersonDocument import *
from generated.PersonListDocument import *

def main():
    #unmarshal personIn from stdin
    personIn = PersonDocument.fromxml(sys.stdin)

    #create a list containing personIn and some other person
    list = PersonListDocument()
    list.person.append(personIn)
    list.person.append(PersonType("Some Otherguy", "Somewhere 999", 1985))

    #finally, marshal the list to stdout
    print list.toxml()

if __name__ == "__main__":
    main()
