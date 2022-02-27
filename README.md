# CollaborativeCalculationsInDistributedSystems

[EN]

This C algorithm uses MPI to simulate collaborative calculations in a distributed system. The typology contains 3 clusters which command other 6 workers (0 controls 3,6 | 1 controls 4,7,8 | 2 controls 5 | 0,1,2 are interconnected). The porpose of this algoritm is to generate the hole typology and send it to all workers and after that to divide a vector as precise as posible in terms of time saving to to multiply its elements by 2.

[RO]

Explicarea codului:

In cod sistemul distribuit este format in urmatorul fel:
rankul 0 [coordonator]: liniile 18 - 250
rankul 1 [coordonator]: liniile 251 - 395
rankul 2 [coordonator]: liniile 287 - 383
rankurile 3 - 8 [workeri]: liniile 591 - 625

Explicarea coordonatorilor:

In prima parte din fiecare coordonator (adica rankurile 0,1,2) se citeste 
din fisierul CLUSTER respectiv numarul de workeri asociati si idul lor. Apoi
se trimite un mesaj MPI_SEND cu rankul coordonatorului spre fiecare worker asociat.
liniile[22-32], [253-263], [397-408].

In ceea ce urmeaza se vor primi mesaje trimise din Workeri catre fiecare coordonator
pentru a evidentia topologia de la cerinta 1. Mesajul trimis in variabila mess este
neimportant.
liniile[34-38], [265-269], [410-414].

Dupa aceasta in urmatorul pas am trimis si apoi primit tipologia coordonatorului 
catre ceilalti coordonatori si vice versa. aici fiind conexiune intre coordonatori se trateaza si cazul 
bonus in care nu exista conexiune intre 0 - 1, deci coord 2 trebuie sa transfere datele intre acestea.
liniile[40-95], [271-325], [416-492].

Acum ca toti coordonatorii au introduse in variabilele size_0,1,2 si tasks_0,1,2
idul workerilor subordonati trimitem acesti vectori si sizeul lor catre toti workerii subordonati
liniile[97-105], [328-336], [495-503].

Pentru cerinta 2 se va continua cu crearea vectorului in coordonatorul cu rank 0 
liniile[109-114].

Dupa tot in coordonatorul cu rankul 0 se creaza distribuirea pentru fiecare task intr-un mod cat mai
echilibrat. Am considerat ca fiecare coordonator va avea N/ suma_workeri * numar_workeri_coordonator.
Dupa acest calcul ramanem cu restul impartirii la suma_workeri pe care o distribuim la coordonatori
pana cand acest rest devine 0.
liniile[118-154].

Dupa aceasta impartire creez vectorul partial pentru master 0 si trimit vectorii partiali spre ceilalti
coordonatori 1 & 2. De asemenea se trateaza cazul bonus.
liniile[157-195].

In coorodnatorii 1 & 2 primim de la masterul 0 vectorul partial.
liniile[340-355], [507-538].

Dupa ce toti coordonatorii au vectorul partial acestia distribuie catre fiecare worker pe care il coordoneaza
elemente din acel vector pentru a fi dublate, apoi primesc rezultatul in aceasi variabila trimisa.
liniile[197-217], [358-378], [540-561]. 

Apoi in coordonatorii 1 si 2 se trimit vectorii partiali calculati catre master 0.
liniile[381-393], [564-589].

In coordonatorul 0 primim rezultatele din ceilalti 2 coorodnatori, recompletam vectorul initial si il printam.
Tratam si cazul cu eroare 0 - 1, fiind cazul de conexiune inter coordonatori.
liniile[221-250].

Explicarea workerilor:

pe linii:

595 - primire mesaj cu numarul coordonatorului, primit de la coordonator.
597-598 - trimit mesaj inapoi cu rankul, pentru a verifica tipologia de la cerinta 1
600-608 - primesc de coordonator sizeul si vectorul cu workerii de la cei 3 coordonatori
611-625 - primesc sizeul si elementele vectorului pe care trebuie sa le dublez la cerinta 2,
apoi le trimit spre fiecare coordonator asociat.

Extra:
liniile [628-638] sunt pentru printarea tipologiei in fiecare proces distribuit,
atat worker cat si coorodnator.
