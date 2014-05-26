docs:
	doxygen Documentation/doxygen.conf
	mkdir -p Documentation/temp/
	cp Documentation/doxygen/latex/doxygen.sty Documentation/doxygen/latex/*_8?.tex Documentation/temp/
	cp Documentation/LIPS.tex Documentation/temp/
	cat Documentation/main.tex|head -n -2 > Documentation/temp/main.tex
	ls -1 Documentation/temp/*_8?.tex|xargs basename -s .tex -a|awk '{ print "\\input{" $$1 "}" }' >> Documentation/temp/main.tex
	echo '\end{document}' >> Documentation/temp/main.tex
	cd Documentation/temp && pdflatex main.tex && pdflatex main.tex
	mv Documentation/temp/main.pdf documentation.pdf
	rm -rf Documentation/temp/ Documentation/doxygen/
