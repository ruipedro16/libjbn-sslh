all: bn_generic_export.s

%.s: %.jinc
	jasminc $< -nowarning -o $@

clean:
	rm -f bn_generic_export.s
