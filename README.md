# tabboz
Tabboz Simulator

## Istruzioni per la build

A giudicare da alcune scelte tecnologiche e dalle date, si puo' concludere che tabbozz sia stato sviluppato sotto Windows 95/98 con il compilatore Borland C++ 4.5 (da non confondere con il piu' moderno Borland/Embarcadero C++ Builder).

E' possibile tuttavia fare la build del progetto con del software leggermente meno datato, in modo da poter evitare di lanciare  madonne ai noti problemi di stabilita' di wWin95/98 e riservarle per il succo del lavoro, ovvero lo sviluppo.

### Prerequisiti

Per poter fare la build di tabbozz sono necessari i seguenti tool:

1. Installazione di Windows XP (si puo' usare un macchina virtuale VirtualBox con windows xp SP2 su Linux se si vuole)
2. Borland C++ 5 (reperibile [qui](https://winworldpc.com/product/borland-c/5x))

Essendo il codice un po' datato non e' facile reperire aiuto online, percio' puo' tornare utile avere una copia del [Manuale per Sviluppatori del Borland C++ 5](http://www.google.com/url?q=http%3A%2F%2Fbitsavers.informatik.uni-stuttgart.de%2Fpdf%2Fborland%2Fborland_C%2B%2B%2FBorland_C%2B%2B_Version_5_Programmers_Guide_1997.pdf&sa=D&sntz=1&usg=AOvVaw3eHAJijDEbPyPtNe9qBNy6) e della [Guida dell'utente](http://www.google.com/url?q=http%3A%2F%2Fbitsavers.informatik.uni-stuttgart.de%2Fpdf%2Fborland%2Fborland_C%2B%2B%2FBorland_C%2B%2B_Version_5_Users_Guide_1997.pdf&sa=D&sntz=1&usg=AOvVaw20euRa10hVNODGo3es1tWU).

### Preparare la build

Assumendo di aver installato il Borland C++ nella sua cartella di default, e di avere is sorgenti di tabbozz salvati dentro la macchina virtuale (e non su una cartella condivisa con l'host), i passaggi sono i seguenti.

1. Avviare Borland C++ e caricare il progetto TABBOZ.ide
2. Andare su `Options -> Project -> Directories` 
3. Cambiare i campi `Include` e `Library` da `c:\bc45\include` e `c:\bc45\lib` in `c:\bc5\include` e `c:\bc5\lib` rispettivamente (questo passo non e' necessario se si usa il Borland C++ 4.5)
4. Compilare facendo `Project -> Build all` e la compilazione dovrebbe aver successo a meno di qualche warning

## Analisi dei file del progetto

Un'analisi approfondita del codice non e' stata fatta, ma si possono concludere le seguenti nozioni:

- Il progetto sembra essere stato compilato in modalita' 32 bit soltanto. 
- Il file zarro32.res contiene gli asset del gioco (immagini, icone) e le finestre (sono riuscito ad editarle solo tramite Borland C++).
- Il file text.res contiene le stringhe usate nel gioco.
- La libreria BWCC32 (Borland Windows Custom Controls) e' la libreria grafica del gioco che gli da' il suo look and feel caratteristico. Presumo sia necessaria solo la versione .lib ma e' da confermare.
