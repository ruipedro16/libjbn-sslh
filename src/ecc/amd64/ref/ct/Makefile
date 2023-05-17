all: ecc_generic_export_ct.s

%.s: %.jinc
	jasminc $< -nowarning -o $@

clean:
	rm -f ecc_generic_export.s
