all: bn_generic_export_ct.s

%.s: %.jinc
	jasminc $< -nowarning -o $@

clean:
	rm -f bn_generic_export.s
