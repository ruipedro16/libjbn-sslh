all: ecc_generic_export.s

%.s: %.jinc
	jasminc $< -nowarning -o $@

clean:
	rm -f ecc_generic_export.s
