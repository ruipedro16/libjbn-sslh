all: fp_generic_export.s

%.s: %.jinc
	jasminc $< -nowarning -o $@

clean:
	rm -f fp_generic_export.s
