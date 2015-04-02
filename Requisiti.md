# Requisiti Utente #
L'utente di secure-chat può:
  * visualizzare una lista di canali attivi, identificati da una stringa (CHANNELS)
  * attivare un canale definendone l'identificativo stringa. il canale può essere privato o pubblico. se privato verrà dotato di accesso con password. (OPENCHANNEL)
  * connettersi ad un canale (JOIN)
  * inviare messaggi agli altri utenti dei canali a cui è connesso (SEND)
  * ricevere messaggi degli utenti connessi ad uno stesso canale a cui è connesso l'utente (RECV)
  * disconnettersi da un canale (LEAVE)
  * passare alla modalità sicura (LOGIN)

L'utente di secure-chat ha:
  * un nickname con una stringa di dimensione massima di 32 caratteri
  * una password (? il se il suo nickname è privato o pubblico)

Il canale può/deve essere/ha:
  * un nome identificato da una stringa con un massimo di 64 caratteri.
  * essere privato o pubblico
  * deve avere un numero limitato di utenti.
  * il numero di utenti nel canale è pubblico ( o privato)

I canali possono essere di tipo pubblico o di tipo sicuro; nel secondo caso per poterli vedere nella lista o per potervi accedere per scambiare messaggi, occorre effettuare il login; ciò garantisce la confidenzialità, l'autenticità e l'integrità dei dati grazie ad un segreto condiviso scambiato in modo sicuro via browser al momento della registrazione dell'utente (tramite username e password).

I canali sicuri garantiscono la segretezza dei dati rendendo leggibile ogni messaggio solo agli utenti che sono connessi ad un dato canale nel momento in cui il messaggio viene inviato

# Requisiti di Sistema #
Il programma client, sviluppato in c++ con interfaccia grafica gtk, dovrà consentire all'utente di compiere le operazioni precedentemente elencate sia in modalità pubblica che in modalità sicura, e potrà essere eseguito su qualunque macchina a rischio e pericolo dell'utente.

Il programma server, sviluppato in c++ con interfaccia testuale dovrà gestire le richieste in arrivo dai client e soddisfarle; per le comunicazioni sicure necessita di un certificato che consenta ai client di riconoscerlo, e si appoggia ad un database sqlite ( o mysql) contenente i segreti condivisi con i vari utenti registrati, in particolare username e hash md5 (o sha1) della password. Data la criticità delle informazioni che gestisce, girerà in una macchina posta in ambiente protetto, riduca la possibilità di manomissioni o furti di informazioni.

Le comunicazioni tra client e server avvengono:
  * in chiaro, se l'utente è connesso ad un canale pubblico;
  * criptate tramite chiave simmetrica di sessione scambiata al momento del login grazie al segreto condiviso, se l'utente è connesso ad un canale sicuro.


# Requisiti Opzionali #
  * comando di ban/kick (BAN)
  * comando di ignore per evitare spamming e BOT (IGNORE)
  * messaggi privati (PRIVMSG)
  * invio di un file (SENDFILE)
  * ricezione di un file (RECVFILE)