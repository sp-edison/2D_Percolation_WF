BIN=2d_Perc
RUN=run.sh
BINDIR=../bin
INPDIR=../inp
TMPDIR=../tmp
INPFILE=input.txt
INPUT=${INPDIR}/${INPFILE}

all: ${BIN} ${INPUT}

test: all ${BINDIR} ${BIN} ${INPUT} ${INPDIR} ${TMPDIR}
	cd ${TMPDIR}; ls; cp ${BINDIR}/${BIN} ./; cp ${INPUT} ./; ./${BIN} -i ${INPFILE}; rm -rf ${BIN} ${INPFILE}; rm -rf result
	

${TMPDIR}:
	mkdir -p ${TMPDIR}
	
${BIN}: main.c Network.h Network.c mt64.c mt64.h ${BINDIR} ${RUN}
	gcc -lm -O3 main.c -o ${BINDIR}/${BIN}
	cp -a ${RUN} ${BINDIR}

${RUN}:

${BINDIR}:
	mkdir -p ${BINDIR}

${INPDIR}:
	mkdir -p ${INPDIR}

${INPUT}: ${INPDIR} input.txt
	cp -a input.txt ${INPUT}

clean:
	rm -rvf ${BINDIR} ${INPDIR} ${TMPDIR}

