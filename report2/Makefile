TEX = pdflatex
LATEXMK = latexmk
MAIN = main.tex
TARGET = build/main.pdf
SRC = *.tex

$(TARGET): $(SRC)
	mkdir -p build
	$(LATEXMK) -pdf -quiet -output-directory=build $(MAIN)
	mv $(TARGET) .

all: $(TARGET)

clean:
	rm -rf build

distclean: clean
	rm -f main.pdf

.PHONY: all clean distclean
