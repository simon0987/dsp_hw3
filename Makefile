SRIPATH ?= /Users/kezhebang/Desktop/107-1課程/數位語音概論/srilm-1.5.10
MACHINE_TYPE ?= macosx
LM ?= bigram.lm

CXX = g++
CXXFLAG = -O3 -I$(SRIPATH)/include -w --std=c++11
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
MAP_IN = Big5-ZhuYin.map
MAP_OUT = ZhuYin-Big5.map 
RM_FILES =  $(OBJ) $(TARGET)
.PHONY: all clean
TEST1DIR=result1
TESTDATA=testdata
ANSDIR=result2


all: $(TARGET)

$(TARGET):$(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o:%.cpp
	$(CXX) $(CXXFLAG) -c $<

run:
	[ -d $(ANSDIR) ] || mkdir -p $(ANSDIR);
	@for i in $(shell seq 1 10) ; do \
		echo "Running $$i.txt"; \
		 ./mydisambig $(TESTDATA)/$$i.txt $(MAP_OUT) $(LM) $(ANSDIR)/$$i.txt; \
	done;

run_srilm:
	@for i in $(shell seq 1 10) ; do \
		echo "Running $$i.txt"; \
		$(SRIPATH)/bin/$(MACHINE_TYPE)/disambig -text $(TESTDATA)/$$i.txt -map $(MAP_OUT) -lm $(LM) -order 2 > $(TEST1DIR)/$$i.txt; \
	done;

run_test:
	./separator_big5.pl corpus.txt > corpus_seg.txt
	./separator_big5.pl testdata/1.txt > 1_seg.txt
	$(SRIPATH)/bin/$(MACHINE_TYPE)/ngram-count -text corpus_seg.txt -write lm.cnt -order 2
	$(SRIPATH)/bin/$(MACHINE_TYPE)/ngram-count -read lm.cnt -lm bigram.lm  -unk -order 2
	
map: mapping.py
	python3 mapping.py $(MAP_IN) $(MAP_OUT)

clean:
	rm $(RM_FILES)