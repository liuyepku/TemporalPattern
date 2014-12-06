
clean:
	rm -rf train.ddl ?.ddl

#### IRIS-1NN

train.ddl: 
	../../src/sample "data:f<iris.ddl>x.ddl data/label:c data/value:c args/subset=false args/count=15"
	../../src/group "data<x.ddl>y.ddl data/label</included data/class:c</label data/value:c"
	../../src/copy "data(0,*):f<y.ddl>test.ddl data/orig:c</class"
	../../src/copy "data(136,*):f<y.ddl>train.ddl data/class:c"

# "unclassified/orig:c"
result.ddl: train.ddl
	../../src/nnc "classified<train.ddl/data unclassified:f<test.ddl/data>result.ddl"
	../../src/cmat "data<result.ddl data/x</class data/y<test.ddl/orig"


