###furla###
io di solito sono per la filosofia "keep it simple" e farei uno scriptino che chiama il gcc per il client ed uno per il server, però so che in passato avete usato tool un po' più sofisticati che non ho mai avuto voglia di imparare ad usare... se secondo voi il gioco vale la candela si possono usare anche qui. vi chiedo solo di farmi capire come si usano perché non ho tanto tempo ber sbatterci la testa, per quanto sia dura!

in ogni caso la mia proposta è di mettere tutto su hg (che sia uno script o che siano file di make, init, conf e quant'altro) in modo che la compilazione resti un'operazione immediata.

###alec###
io so usare gli autotools, gli script makefile.am e configure.ac sono praticamente già fatti ci vuole solo il codice da compilare che al momento è solo un miraggio! (e gualche aggiustamento per le gtk)

basterà fare ./build.sh con uno di questi parametri -> release profile debug dist clean

so che per esperienza gli script prefatti spesso portati su diversi sistemi falliscono a compilare, mentre con gli script autotools se fatti bene possono superare questi problemucci.