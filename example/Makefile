# This file is in the public domain.
all: generated/__init__.py

generated/__init__.py: ../build/pyjames
	mkdir -p generated
	../build/pyjames generated example.xsd

#       TODO: don't force the use to have to do the following
	touch generated/__init__.py
	cp ../py/JamesXMLObject.py generated

../build/pyjames: ../*.cpp ../*.h
	mkdir -p ../build && cd ../build && cmake .. && make -j4

test: generated/__init__.py
	python example.py < person.xml
