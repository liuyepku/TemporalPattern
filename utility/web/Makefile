#
# This MAKEFILE has been generated automatically by WEBMAKE
# (c) Frank Hoeppner, Braunschweig 1998-2000
#

.SUFFIXES: .nw .cnw .cpp .hpp .html .tex .dvi .ps .lbp .o

help:
	@echo "*** code: make all source files"
	@echo "*** docs: make all documentation files"

all: code docs




TEXFILES =  \
  ../doc/chars.tex \
  ../doc/constants.tex \
  ../doc/defbool.tex \
  ../doc/stlstream.tex \
  ../doc/stlutil.tex \
  ../doc/strconvert.tex \
  ../doc/threevaluedlogic.tex \
  ../doc/time.tex \
  ../doc/utilstream.tex \
  ../doc/utiltest.tex

docs: $(TEXFILES) 
dvi: ../doc/tutorial-UTIL.dvi
ps: ../doc/tutorial-UTIL.ps

../doc/tutorial-UTIL.dvi: $(TEXFILES) tutorial-UTIL.tex
	(cd $(NOWEBPATH)/utility/web/; TEXMFOUTPUT=../doc/; TEXINPUTS=$(TEXINPUTS):../doc/; lock; latex tutorial-UTIL; unlock)
	(cd ../doc/; bibtex tutorial-UTIL; makeindex tutorial-UTIL; unlock)

../doc/tutorial-UTIL.ps: $(TEXFILES) ../doc/tutorial-UTIL.dvi
	(cd $(NOWEBPATH)/utility/web/; TEXMFOUTPUT=../doc/; TEXINPUTS=$(TEXINPUTS):../doc/; lock; latex tutorial-UTIL; unlock)
	(cd $(NOWEBPATH)/utility/web/; TEXMFOUTPUT=../doc/; TEXINPUTS=$(TEXINPUTS):../doc/; lock; latex tutorial-UTIL; unlock)
	(cd ../doc/; dvips tutorial-UTIL)




code: \
 ../src/Chars.cnw \
 ../src/define.cnw \
 ../src/defbool.cnw \
 ../src/stlstream.cnw \
 ../src/stlutil.cnw \
 ../src/strconv.cnw \
 ../src/strconv-s.cnw \
 ../src/bool3.cnw \
 ../src/ttime.cnw \
 ../src/ustream.cnw \
 ../src/ustream-s.cnw \
 ../src/ustream-d.cnw \
 ../src/utiltest.cnw


GENERIC_dep =  \
  WebMake.UTIL

GENERIC_CLASS_dep =  \
  $(GENPATH)/c++-class.nw \
  $(GENPATH)/inline-intern.nw \
  $(GENPATH)/gnu-config-h.nw \
  $(GENPATH)/gnu-gpl.nw

../src/generic_CLASS.cnw : $(GENERIC_CLASS_dep)
	cat $(GENERIC_CLASS_dep) > ../src/generic_CLASS.cnw
GENERIC_FCALG_dep =  \
  $(GENPATH)/c++-class.nw \
  $(GENPATH)/inline-intern.nw \
  $(GENPATH)/gnu-config-h.nw \
  $(GENPATH)/gnu-gpl.nw \
  $(GENPATH)/fcalg.nw

../src/generic_FCALG.cnw : $(GENERIC_FCALG_dep)
	cat $(GENERIC_FCALG_dep) > ../src/generic_FCALG.cnw
GENERIC_FUNCTION_dep =  \
  $(GENPATH)/c-functions.nw \
  $(GENPATH)/inline-intern.nw \
  $(GENPATH)/gnu-config-h.nw \
  $(GENPATH)/gnu-gpl.nw

../src/generic_FUNCTION.cnw : $(GENERIC_FUNCTION_dep)
	cat $(GENERIC_FUNCTION_dep) > ../src/generic_FUNCTION.cnw
GENERIC_HEADER_dep =  \
  $(GENPATH)/c-header.nw \
  $(GENPATH)/inline-intern.nw \
  $(GENPATH)/gnu-config-h.nw \
  $(GENPATH)/gnu-gpl.nw

../src/generic_HEADER.cnw : $(GENERIC_HEADER_dep)
	cat $(GENERIC_HEADER_dep) > ../src/generic_HEADER.cnw
GENERIC_HTML_dep =  \
  $(GENPATH)/html.nw

../src/generic_HTML.cnw : $(GENERIC_HTML_dep)
	cat $(GENERIC_HTML_dep) > ../src/generic_HTML.cnw
GENERIC_HTMLTABLE_dep =  \
  $(GENPATH)/html.nw \
  $(GENPATH)/htmltable.nw

../src/generic_HTMLTABLE.cnw : $(GENERIC_HTMLTABLE_dep)
	cat $(GENERIC_HTMLTABLE_dep) > ../src/generic_HTMLTABLE.cnw
GENERIC_MAIN_dep =  \
  $(GENPATH)/c-main.nw \
  $(GENPATH)/inline-intern.nw \
  $(GENPATH)/gnu-config-h.nw \
  $(GENPATH)/gnu-gpl.nw

../src/generic_MAIN.cnw : $(GENERIC_MAIN_dep)
	cat $(GENERIC_MAIN_dep) > ../src/generic_MAIN.cnw
GENERIC_SHELLSCRIPT_dep =  \
  $(GENPATH)/shell-script.nw

../src/generic_SHELLSCRIPT.cnw : $(GENERIC_SHELLSCRIPT_dep)
	cat $(GENERIC_SHELLSCRIPT_dep) > ../src/generic_SHELLSCRIPT.cnw


Chars_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/chars.nw 

../src/Chars.cnw : $(Chars_dep) ../src/generic_CLASS.cnw
	$(GENPATH)/create_sources Chars ../src/Chars "$(Chars_dep)" ../src/generic_CLASS.cnw Chars ""
../doc/chars.tex : $(NOWEBPATH)/utility/web/chars.nw
	$(GENPATH)/create_documentation utility/web/chars.nw ../doc/chars.tex $(NOWEBPATH)/

define_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/constants.nw 

../src/define.cnw : $(define_dep) ../src/generic_FUNCTION.cnw
	$(GENPATH)/create_sources Constants ../src/define "$(define_dep)" ../src/generic_FUNCTION.cnw Constants ""
../doc/constants.tex : $(NOWEBPATH)/utility/web/constants.nw
	$(GENPATH)/create_documentation utility/web/constants.nw ../doc/constants.tex $(NOWEBPATH)/

defbool_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/defbool.nw 

../src/defbool.cnw : $(defbool_dep) ../src/generic_FUNCTION.cnw
	$(GENPATH)/create_sources DefBool ../src/defbool "$(defbool_dep)" ../src/generic_FUNCTION.cnw DefBool ""
../doc/defbool.tex : $(NOWEBPATH)/utility/web/defbool.nw
	$(GENPATH)/create_documentation utility/web/defbool.nw ../doc/defbool.tex $(NOWEBPATH)/

stlstream_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/stlstream.nw 

../src/stlstream.cnw : $(stlstream_dep) ../src/generic_FUNCTION.cnw
	$(GENPATH)/create_sources STLStream ../src/stlstream "$(stlstream_dep)" ../src/generic_FUNCTION.cnw STLStream ""
../doc/stlstream.tex : $(NOWEBPATH)/utility/web/stlstream.nw
	$(GENPATH)/create_documentation utility/web/stlstream.nw ../doc/stlstream.tex $(NOWEBPATH)/

stlutil_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/stlutil.nw 

../src/stlutil.cnw : $(stlutil_dep) ../src/generic_FUNCTION.cnw
	$(GENPATH)/create_sources STLUtil ../src/stlutil "$(stlutil_dep)" ../src/generic_FUNCTION.cnw STLUtil ""
../doc/stlutil.tex : $(NOWEBPATH)/utility/web/stlutil.nw
	$(GENPATH)/create_documentation utility/web/stlutil.nw ../doc/stlutil.tex $(NOWEBPATH)/

strconv_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/strconvert.nw 

../src/strconv.cnw : $(strconv_dep) ../src/generic_FUNCTION.cnw
	$(GENPATH)/create_sources StrConvert ../src/strconv "$(strconv_dep)" ../src/generic_FUNCTION.cnw StrConvertChar "C"
../doc/strconvert.tex : $(NOWEBPATH)/utility/web/strconvert.nw
	$(GENPATH)/create_documentation utility/web/strconvert.nw ../doc/strconvert.tex $(NOWEBPATH)/

strconv-s_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/strconvert.nw 

../src/strconv-s.cnw : $(strconv-s_dep) ../src/generic_FUNCTION.cnw
	$(GENPATH)/create_sources StrConvert ../src/strconv-s "$(strconv-s_dep)" ../src/generic_FUNCTION.cnw StrConvertString "S"

bool3_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/threevaluedlogic.nw 

../src/bool3.cnw : $(bool3_dep) ../src/generic_CLASS.cnw
	$(GENPATH)/create_sources TVL ../src/bool3 "$(bool3_dep)" ../src/generic_CLASS.cnw TVL ""
../doc/threevaluedlogic.tex : $(NOWEBPATH)/utility/web/threevaluedlogic.nw
	$(GENPATH)/create_documentation utility/web/threevaluedlogic.nw ../doc/threevaluedlogic.tex $(NOWEBPATH)/

ttime_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/time.nw 

../src/ttime.cnw : $(ttime_dep) ../src/generic_FUNCTION.cnw
	$(GENPATH)/create_sources Time ../src/ttime "$(ttime_dep)" ../src/generic_FUNCTION.cnw Time ""
../doc/time.tex : $(NOWEBPATH)/utility/web/time.nw
	$(GENPATH)/create_documentation utility/web/time.nw ../doc/time.tex $(NOWEBPATH)/

ustream_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/utilstream.nw 

../src/ustream.cnw : $(ustream_dep) ../src/generic_FUNCTION.cnw
	$(GENPATH)/create_sources UtilStream ../src/ustream "$(ustream_dep)" ../src/generic_FUNCTION.cnw UtilStreamChar "C"
../doc/utilstream.tex : $(NOWEBPATH)/utility/web/utilstream.nw
	$(GENPATH)/create_documentation utility/web/utilstream.nw ../doc/utilstream.tex $(NOWEBPATH)/

ustream-s_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/utilstream.nw 

../src/ustream-s.cnw : $(ustream-s_dep) ../src/generic_FUNCTION.cnw
	$(GENPATH)/create_sources UtilStream ../src/ustream-s "$(ustream-s_dep)" ../src/generic_FUNCTION.cnw UtilStreamString "S"

ustream-d_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/utilstream.nw 

../src/ustream-d.cnw : $(ustream-d_dep) ../src/generic_FUNCTION.cnw
	$(GENPATH)/create_sources UtilStream ../src/ustream-d "$(ustream-d_dep)" ../src/generic_FUNCTION.cnw UtilStreamDrop "D"

utiltest_dep = $(GENERIC_dep) \
  $(NOWEBPATH)/utility/web/environment.nw \
  $(NOWEBPATH)/utility/web/utiltest.nw  \
  $(NOWEBPATH)/utility/web/strconvert.nw 

../src/utiltest.cnw : $(utiltest_dep) ../src/generic_MAIN.cnw
	$(GENPATH)/create_sources UtilTest ../src/utiltest "$(utiltest_dep)" ../src/generic_MAIN.cnw UtilTest ""
../doc/utiltest.tex : $(NOWEBPATH)/utility/web/utiltest.nw
	$(GENPATH)/create_documentation utility/web/utiltest.nw ../doc/utiltest.tex $(NOWEBPATH)/


