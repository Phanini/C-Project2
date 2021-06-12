#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct bun{
	struct bun *dalsibunka;
	char* hodnota;
}bunka;

typedef struct rad{
	struct rad *dalsiradek;
	bunka* prvnibunka;
	int cisloradku;
}radek;

typedef struct tab{
	radek* prvniradek;
}tabulka;

typedef struct sel{
	char* vyber;
	char** prikazy;
}selekce;

//POMOCNE FCE PRACE S TABULKOU
void printtabulka(tabulka* TB, char delim);
tabulka* nactihodnoty(FILE*, char delim);
void freememory(tabulka*);
radek* vratPointer(tabulka* TB, int hledane_cislo_radku);
bunka* bunkaPointer(tabulka* TB, int R, int C);
int switchprikazu(tabulka* TB, selekce* selecty, int pocetprik);
int pocetradku(tabulka* TB);
int pocetbunek(radek* row);
bool isnumber(char* str);
void updateFile(tabulka *TB, char delim);
void zarovnat(tabulka* TB);

//FUNKCE PRO PREDAVANI HODNOT A VOLANI FUNKCI
int zpracujprikazy(tabulka* TB, char** prikazy, int pocetprikazu);
char** ctiprikazy(char** argv, int *pocet);
int typselekce(char* select);

//PRIKAZY PRO UPRAVU TABULKY
int irow(tabulka* TB, int R1);
void arow(tabulka*, int R1);
void drow(tabulka*, int R);
void icol(tabulka*, int R, int C);
void acol(tabulka* TB, int R1, int C);
void dcol(tabulka* TB, int R, int C);

//PRIKAZY PRO UPRAVU OBSAHU BUNEK
void set(bunka* item, char* str);
void clear(bunka* item);
void len(tabulka* TB, bunka* vyber, int R, int C);
void swap(tabulka* TB, bunka* vyber, int R, int C);



int main(int argc, char* argv[]) { //MAIN
	char delim = ' ';
	if (strcmp(argv[1], "-d")==0){
		delim = argv[2][0];
	}
	FILE* f = fopen(argv[argc-1], "r");
    if (f == NULL){
        perror("Chyba v nacitani souboru\n");
        return 0;
    }
    else{
		tabulka* TB = nactihodnoty(f, delim);
		int pocetprikazu;
		char** prikazy = ctiprikazy(argv, &pocetprikazu);
		zpracujprikazy(TB, prikazy, pocetprikazu);
		freememory(TB);
    }
}

int pocetradku(tabulka* TB){
	radek* aktualniradek = TB->prvniradek;
	int count = 0;
	while(aktualniradek->dalsiradek!= NULL){
		count++;
		aktualniradek=aktualniradek->dalsiradek;
	}
	return count;
}

int pocetbunek(radek* row){
	int count = 0;
	if(row!=NULL){
		if (row->prvnibunka != NULL){
			count++;
			bunka* aktualnibunka = row->prvnibunka;
			while (aktualnibunka->dalsibunka!= NULL){
				aktualnibunka= aktualnibunka->dalsibunka;
				count++;
			}
		}
	}
	return count;
}

/*Dostane pole stringu (selektoru a prikazu z argv[3]) 
  a dosadi do struct selekce a zavola switchprikazu*/
int zpracujprikazy(tabulka* TB, char** poleprikazu, int pocetprikazu){
	if (pocetprikazu > 1000){
		printf("Chyba, bylo zadano moc velke mnozstvi prikazu\n");
		return 0;
	}
	for (int i = 0; i < pocetprikazu; i++){ //Iterace polem prikazu
		if (poleprikazu[i][0] =='['){
			if(i == pocetprikazu-1){ //Kdyz je posledni selekce bez prikazu
				break;
			}
			else if((i>0)&&(strcmp(poleprikazu[i], "[min]") == 0)){
				
			}
			int j = 0;
			int k = i+1;
			selekce *aktualniselekce = (selekce*) malloc (sizeof(selekce));
			aktualniselekce->vyber = (char*) malloc (sizeof(poleprikazu[i]));
			aktualniselekce->vyber = poleprikazu[i];
			aktualniselekce->prikazy =(char**) malloc(sizeof(char*));
			while ((k != pocetprikazu) && (typselekce(poleprikazu[k]) < 1)){ //Hledame prikazy dokud neni dalsi selektor nebo konec
				aktualniselekce->prikazy =(char**) realloc(aktualniselekce->prikazy,(j+1) * sizeof(char*));
				aktualniselekce->prikazy[j] = poleprikazu[k];
				k++;
				j++;
			}
			i += j;
			switchprikazu(TB ,aktualniselekce, j); //Pro nasly struct (selekce+pole prikazu) zavolame switch

			//char* temp = aktualniselekce->vyber;
			//free(temp);
			/*for (int i =0; i < j; i++){
				free(aktualniselekce->prikazy[i]);
			}
			free(aktualniselekce->prikazy);
			free(aktualniselekce);*/
		}
	}
	return 1;
}

/*Prijima struct se selektorem a polem prislusnych prikazu
  Vyhodnoti jestli je platny selektor pro prikaz a popr. ho zavola
*/
int switchprikazu(tabulka* TB, selekce* n, int pocetprik){
	//printf("SELEKCE: %s | PRIKAZY: ", n->vyber);
	for (int i = 0; i < pocetprik; i++){
		int typsel = typselekce(n->vyber);
		if (strcmp(n->prikazy[i], "irow") == 0){ //FUNKCE IROW
			if (typsel==2){
				irow(TB, n->vyber[1]-'0');
			}
			else if (typsel==4){
				if (((n->vyber[1]-'0') == (n->vyber[5]-'0')) && (n->vyber[3]-'0' == 1) && (n->vyber[7] =='-')){
					irow(TB, n->vyber[1]-'0');
				}
			}
			else{
				fprintf(stderr, "Error: Zadan spatny vyber pro funkci irow!\n");
				return 1;
			}
		}
		else if(strcmp(n->prikazy[i], "arow") == 0){ //FUNKCE AROW
			if (typsel==2){
				arow(TB, n->vyber[1]-'0');
			}
			else if (typsel==4){
				if (((n->vyber[1]-'0') == (n->vyber[5]-'0')) && (n->vyber[3]-'0' == 1) && (n->vyber[7] =='-')){
					arow(TB, n->vyber[1]-'0');
				}
			}
			else if(typsel==5){
				arow(TB, 0);
			}
			else{
				fprintf(stderr, "Error: Zadan spatny vyber pro funkci arow!\n");
				return 1;
			}
		}
		else if(strcmp(n->prikazy[i], "drow") == 0){ // FUNKCE DROW
			if (typsel==2){
				drow(TB, n->vyber[1]-'0');
			}
			else if(typsel==5){
				int numradku = pocetradku(TB);
				for (int i = 0; i < numradku; i++){
					drow(TB, 1);
				}
			}
			else{
				fprintf(stderr, "Error: Zadan spatny vyber pro funkci drow!\n");
				return 1;
			}
		}
		else if(strcmp(n->prikazy[i], "icol") == 0){ // FUNKCE ICOL
			if (typsel==1){
				icol(TB,n->vyber[1]-'0', n->vyber[3]-'0');
			}
			else if(typsel==3){
				int numradku = pocetradku(TB);
				for (int i = 1; i < numradku+1; i++){
					icol(TB, i, n->vyber[3]-'0');
				}
			}
			else if((typsel==4) &&(n->vyber[3]==n->vyber[7])){
				int radekod = n->vyber[1]-'0';
				int bunkaod = n->vyber[3]-'0';
				int radekdo;
				int bunkado;
				if (n->vyber[5]=='_'){
					radekdo = pocetradku(TB);
				}
				else{
					radekdo = n->vyber[5]-'0';
				}
				if (n->vyber[7] == '_'){
					bunkado = pocetbunek(vratPointer(TB, 1));
				}
				else{
					bunkado = n->vyber[7]-'0';
				}
				
				for (; radekod<radekdo+1; radekod++){
					icol(TB, radekod, n->vyber[3]-'0');
				}
			}
			else{
				fprintf(stderr, "Error: Zadan spatny vyber pro funkci icol!\n");
				return 1;
			}
		}
		else if(strcmp(n->prikazy[i], "acol") == 0){ // FUNKCE ACOL
			if (typsel==1){
				acol(TB, (n->vyber[1]-'0'), (n->vyber[3]-'0'));
			}
			else if(typsel==3){
				int numradku = pocetradku(TB);
				for (int i = 1; i < numradku+1; i++){
					icol(TB, i, n->vyber[3]-'0');
				}
			}
			else if((typsel==4) &&(n->vyber[3]==n->vyber[7])){
				int radekod = n->vyber[1]-'0';
				int bunkaod = n->vyber[3]-'0';
				int radekdo;
				int bunkado;
				if (n->vyber[5]=='_'){
					radekdo = pocetradku(TB);
				}
				else{
					radekdo = n->vyber[5]-'0';
				}
				if (n->vyber[7] == '_'){
					bunkado = pocetbunek(vratPointer(TB, 1));
				}
				else{
					bunkado = n->vyber[7]-'0';
				}

				for (; radekod<radekdo+1; radekod++){
					icol(TB, radekod, n->vyber[3]-'0'+1);
				}
			}
			else{
				fprintf(stderr, "Error: Zadan spatny vyber pro funkci acol!\n");
				return 1;
			}
			
		}
		else if(strcmp(n->prikazy[i], "dcol") == 0){ // FUNKCE DCOL
			if (typsel==1){
				dcol(TB, n->vyber[1]-'0', n->vyber[3]-'0');
			}
			else if(typsel==2){
				int numbunek = pocetbunek(vratPointer(TB, n->vyber[1]-'0'));
				for (int i = 0; i < numbunek; i++){
					dcol(TB, n->vyber[1]-'0', 1);
				}
			}
			else if(typsel==3){
				int numradku = pocetradku(TB);
				for (int i = 1; i < numradku+1; i++){
					dcol(TB, i, n->vyber[3]-'0');
				}
			}
			else if(typsel==4){
				int radekod = n->vyber[1]-'0';
				int bunkaod = n->vyber[3]-'0';
				int radekaz; //n->vyber[5]-'0';
				int bunkado; //= n->vyber[7]-'0';
				if(n->vyber[5] == '_'){
					radekaz = pocetradku(TB);
				}
				else{
					radekaz = n->vyber[5]-'0';
				}
				if(n->vyber[7] == '_'){
					radekaz = pocetbunek(vratPointer(TB, 1));
				}
				else{
					int bunkado = n->vyber[7]-'0';
				}

				for (; radekod<radekaz+1; radekod++){
					for (; bunkaod<bunkado+1;bunkaod++){
						dcol(TB, radekod, bunkaod);
					}
					bunkaod = n->vyber[3]-'0';
				}
			}
			else if(typsel==5){
				int numradku = pocetradku(TB);
				for (int i = 0; i < numradku; i++){
					int numbunek = pocetbunek(vratPointer(TB, i+1));
					for (int j = 0; j < numbunek; j++){
						dcol(TB, i+1, 1);
					}
				}
			}
			else{
				fprintf(stderr, "Error: Zadan spatny vyber pro funkci dcol!\n");
				return 1;
			}
		}
		else if(strcmp(n->prikazy[i], "[min]") == 0){
			int minimum;
			int initfirst = 0;
			int item_num=0;
			int row_num=0;
			int index =0;
			if(typsel==1){
				continue;
			}
			else if (typsel==2){ //iterujeme cely radek
				char vysledny[5];
				radek* aktualniradek = vratPointer(TB, n->vyber[1]-'0');
				if (aktualniradek!=NULL && aktualniradek->prvnibunka!=NULL){
					bunka* aktualnibunka = aktualniradek->prvnibunka;
					while (aktualnibunka!=NULL){
						item_num++;
						if(isnumber(aktualnibunka->hodnota)){
							if(initfirst==0){
								index = item_num;
								initfirst = 1;
								minimum = atoi(aktualnibunka->hodnota);
							}
							else if(atoi(aktualnibunka->hodnota)<minimum){
								item_num++;
								index = item_num;
								minimum = atoi(aktualnibunka->hodnota);
							}
						}
						aktualnibunka= aktualnibunka->dalsibunka;
					}
					if (initfirst==1){
						vysledny[0] = '[';
						vysledny[1] = n->vyber[1];
						vysledny[2] = ',';
						vysledny[3] = index+'0';
						vysledny[4] = ']';
						n->vyber = vysledny;
					}
					else{
						fprintf(stderr, "Error: Zadan spatny vyber pro funkci [max]!\n");
						return 1;
					}
				}
				else{ 
					fprintf(stderr, "Error: Zadan spatny vyber pro funkci [min]!\n");
					return 1;
				}
			}
			else if(typsel == 3){
				char vysledny[5];
				for (int i = 1; i < pocetradku(TB); i++){
					if (isnumber(bunkaPointer(TB, i, n->vyber[3]-'0')->hodnota)){
						if(initfirst==0){
							index = i;
							initfirst = 1;
							minimum = atoi(bunkaPointer(TB, i, n->vyber[3]-'0')->hodnota);
						}
						else if(atoi(bunkaPointer(TB, i, n->vyber[3]-'0')->hodnota)<minimum){
							index = i;
							minimum = atoi(bunkaPointer(TB, i, n->vyber[3]-'0')->hodnota);
						}
					}
				}
				if (initfirst==1){
					vysledny[0] = '[';
					vysledny[1] = index+'0';
					vysledny[2] = ',';
					vysledny[3] = n->vyber[3];
					vysledny[4] = ']';
					n->vyber = vysledny;
				}
			}
			else if(typsel == 4){
				int R1 = n->vyber[1]-'0';
				int C1 = n->vyber[3]-'0';
				int R2 = n->vyber[5]-'0';
				int C2 = n->vyber[7]-'0';
				char vysledny[9];
				for (; R1<=R2;R1++){
					for (; C1<C2+1; C1++){
						if (isnumber(bunkaPointer(TB, R1, C1)->hodnota)){
							if(initfirst==0){
								row_num = R1;
								item_num = C1;
								minimum = atoi(bunkaPointer(TB, R1, C1)->hodnota);
								initfirst = 1;
							}
							else if(atoi(bunkaPointer(TB, R1, C1)->hodnota)<minimum){
								row_num = R1;
								item_num = C1;
								minimum = atoi(bunkaPointer(TB, R1, C1)->hodnota);
							}
						}
					}
					C1 = n->vyber[3]-'0';
				}
				if (initfirst==1){
					vysledny[0] = '[';
					vysledny[1] = row_num+'0';
					vysledny[2] = ',';
					vysledny[3] = item_num+'0';
					vysledny[4] = ']';
					n->vyber = vysledny;
				}
				else{
					fprintf(stderr, "Error: Zadan spatny vyber pro funkci [min]!\n");
				return 1;
				}
			}
			else{
				fprintf(stderr, "Error: Zadan spatny vyber pro funkci [min]!\n");
				return 1;
			}
		}

		else if(strcmp(n->prikazy[i], "[max]") == 0){ //FUNKCE MAX
			int maximum;
			int initfirst = 0;
			int item_num=0;
			int row_num=0;
			int index =0;
			if(typsel == 1){
				continue;
			}
			else if (typsel==2){ //iterujeme cely radek
				char vysledny[5];
				radek* aktualniradek = vratPointer(TB, n->vyber[1]-'0');
				if (aktualniradek!=NULL && aktualniradek->prvnibunka!=NULL){
					bunka* aktualnibunka = aktualniradek->prvnibunka;
					while (aktualnibunka!=NULL){
						item_num++;
						if(isnumber(aktualnibunka->hodnota)){
							if(initfirst==0){
								index = item_num;
								initfirst = 1;
								maximum = atoi(aktualnibunka->hodnota);
							}
							else if(atoi(aktualnibunka->hodnota)>maximum){
								item_num++;
								index = item_num;
								maximum = atoi(aktualnibunka->hodnota);
							}
						}
						aktualnibunka= aktualnibunka->dalsibunka;
					}
					if (initfirst==1){
					vysledny[0] = '[';
					vysledny[1] = n->vyber[1];
					vysledny[2] = ',';
					vysledny[3] = index+'0';
					vysledny[4] = ']';
					n->vyber = vysledny;
					}
					else{ 
						fprintf(stderr, "Error: Zadan spatny vyber pro funkci [max]!\n");
					return 1;
					}
				}
				else{ 
					fprintf(stderr, "Error: Zadan spatny vyber pro funkci [max]!\n");
					return 1;
				}
			}
			else if(typsel == 3){
				char vysledny[5];
				for (int i = 1; i < pocetradku(TB); i++){
					if (isnumber(bunkaPointer(TB, i, n->vyber[3]-'0')->hodnota)){
						if(initfirst==0){
							index = i;
							initfirst = 1;
							maximum = atoi(bunkaPointer(TB, i, n->vyber[3]-'0')->hodnota);
						}
						else if(atoi(bunkaPointer(TB, i, n->vyber[3]-'0')->hodnota)>maximum){
							index = i;
							maximum = atoi(bunkaPointer(TB, i, n->vyber[3]-'0')->hodnota);
						}
					}
				}
				if (initfirst==1){
					vysledny[0] = '[';
					vysledny[1] = index+'0';
					vysledny[2] = ',';
					vysledny[3] = n->vyber[3];
					vysledny[4] = ']';
					n->vyber = vysledny;
				}
				else{ 
					fprintf(stderr, "Error: Zadan spatny vyber pro funkci [max]!\n");
					return 1;
				}
			}
			else if(typsel == 4){
				int R1 = n->vyber[1]-'0';
				int C1 = n->vyber[3]-'0';
				int R2 = n->vyber[5]-'0';
				int C2 = n->vyber[7]-'0';
				char vysledny[9];
				for (; R1<=R2;R1++){
					for (; C1<C2+1; C1++){
						if (isnumber(bunkaPointer(TB, R1, C1)->hodnota)){
							if(initfirst==0){
								row_num = R1;
								item_num = C1;
								maximum = atoi(bunkaPointer(TB, R1, C1)->hodnota);
								initfirst = 1;
							}
							else if(atoi(bunkaPointer(TB, R1, C1)->hodnota)>maximum){
								row_num = R1;
								item_num = C1;
								maximum = atoi(bunkaPointer(TB, R1, C1)->hodnota);
							}
						}
					}
					C1 = n->vyber[3]-'0';
				}
				if (initfirst==1){
					vysledny[0] = '[';
					vysledny[1] = row_num+'0';
					vysledny[2] = ',';
					vysledny[3] = item_num+'0';
					vysledny[4] = ']';
					n->vyber = vysledny;
				}
				else{
					fprintf(stderr, "Error: Zadan spatny vyber pro funkci [max]!\n");
				return 1;
				}
			}
			else{
				fprintf(stderr, "Error: Zadan spatny vyber pro funkci [max]!\n");
				return 1;
			}
		}

		else if(strstr(n->prikazy[i], "swap [") != NULL){
			int Rres = n->prikazy[i][6]-'0';
			int Cres = n->prikazy[i][8]-'0';
			if(typsel == 1){
				int Rvyber = n->vyber[1]-'0';
				int Cvyber = n->vyber[3]-'0';
				swap(TB, bunkaPointer(TB, Rvyber, Cvyber), Rres, Cres);
			}
			else if(typsel == 4){
				int R1 = n->vyber[1]-'0';
				int C1 = n->vyber[3]-'0';
				int R2 = n->vyber[5]-'0';
				int C2 = n->vyber[7]-'0';
				int Rres = n->prikazy[i][6]-'0';
				int Cres = n->prikazy[i][8]-'0';
				if (R1==R2 && C1 == C2){
					swap(TB, bunkaPointer(TB, R1, C1), Rres, Cres);
				}
			}
			else{
				fprintf(stderr, "Error: Zadan spatny vyber pro funkci swap!\n");
				return 1;
			}
		}
		else if(strcmp(n->prikazy[i], "clear") == 0){
			if (typsel==1){
				int R1 = n->vyber[1]-'0';
				int C1 = n->vyber[3]-'0';
				clear(bunkaPointer(TB, R1, C1));
			}
			else if(typsel==2){
				int R1 = n->vyber[1]-'0';
				int C = pocetbunek(vratPointer(TB, R1));
				for (int i = 1; i < C+1; i++){
					clear(bunkaPointer(TB, R1, i));
				}
			}
			else if(typsel == 3){
				int R1 = pocetradku(TB);
				int C = n->vyber[3]-'0';
				for (int i = 1; i < R1+1; i++){
					clear(bunkaPointer(TB, i, C));
				} 
			}
			else if(typsel == 4){
				int R1 = n->vyber[1]-'0';
				int C1 = n->vyber[3]-'0';
				int R2 = n->vyber[5]-'0';
				int C2 = n->vyber[7]-'0';
				for (; R1 < R2+1; R1++){
					for (; C1<C2+1;C1++){
						clear(bunkaPointer(TB, R1, C1));
					}
				}
			}
			else if(typsel == 5){
				int numradku = pocetradku(TB);
				for (int i = 1; i < numradku+1; i++){
					int numbunek = pocetbunek(vratPointer(TB, i));
					for (int j = 1; j < numbunek+1; j++){
						clear(bunkaPointer(TB, i, j));
					}
				}
			}
			else{
				fprintf(stderr, "Error: Zadan spatny vyber pro funkci clear!\n");
				return 1;
			}
		}
		else if(strstr(n->prikazy[i], "len [") != NULL){
			int Rres = n->prikazy[i][5]-'0';
			int Cres = n->prikazy[i][7]-'0';
			if (typsel==1){
				int R1 = n->vyber[1]-'0';
				int C1 = n->vyber[3]-'0';
				len(TB, bunkaPointer(TB, R1, C1), Rres, Cres);
			}
			else if(typsel == 4){
				int R1 = n->vyber[1]-'0';
				int C1 = n->vyber[3]-'0';
				int R2 = n->vyber[5]-'0';
				int C2 = n->vyber[7]-'0';
				int Rres = n->prikazy[i][5]-'0';
				int Cres = n->prikazy[i][7]-'0';
				if (R1==R2 && C1 == C2){
					len(TB, bunkaPointer(TB, R1, C1), Rres, Cres);
				}
			}
			else{
				fprintf(stderr, "Error: Zadan spatny vyber pro funkci len!\n");
				return 1;
			}
		}
		else if(strstr(n->prikazy[i], "set ") != NULL){
			int R1 = n->vyber[1]-'0';
			int C1 = n->vyber[3]-'0';
			char* token;
			int i = 0;
			token = strtok(n->prikazy[i], " "); //Nacteni prvniho prikazu
			char** result = (char**) malloc(sizeof(char*));
			while (token != NULL){ //Cteme a prirazujeme prikazy
				result[i] = token;
				i++;
				result = (char**) realloc(result,(i+1) * sizeof(char*));
				token = strtok(NULL, " ");
			}
			if (i == 2){
				if (typsel == 1){
					set(bunkaPointer(TB, R1, C1), result[1]);
				}
				else if(typsel == 4){
					int R1 = n->vyber[1]-'0';
					int C1 = n->vyber[3]-'0';
					int R2 = n->vyber[5]-'0';
                    int C2 = n->vyber[7]-'0';
					if (R1==R2 && C1==C2){
						set(bunkaPointer(TB, R1, C1), result[1]);
					}
					else{
						fprintf(stderr, "Error: Zadan spatny vyber pro funkci set!\n");
					return 1;
					}
				}
				else{
					fprintf(stderr, "Error: Zadan spatny vyber pro funkci set!\n");
					return 1;
				}
			}
		}
	}
	return 0;
}

/*TYPY SELEKCI
[R,C] = 1 | [R,_] = 2 | [_,C] = 3 | [R1, C1, R2, C2] = 4 | [_,_] = 5 
[_] = 6 */
int typselekce(char* n){
	if(isdigit(n[1]) && (isdigit(n[3]) && (n[4] == ']'))){
		if((n[1] > 0) && (n[3])>0){
			return 1;
		}
	}
	else if (isdigit(n[1]) && (n[3] == '_') && (n[4] == ']')){
		if(n[1] > 0){
			return 2;
		}
	}
	else if(isdigit(n[3]) && (n[1] == '_') && (n[4] == ']')){
		if(n[3] > 0){
			return 3;
		}
	}
	else if((n[1] == '_') && (n[3] == '_')){
		return 5;
	}
	else if(strlen(n) == 9){
		if(isdigit(n[1]) && isdigit(n[3]) && ((n[5] == '-') || (isdigit(n[5]))) && ((n[7] == '-') || (isdigit(n[7])))){
			if(n[1]>0 && n[3]>0){
				return 4;
			}
		}
	}
	else if((n[1] == '_') && (n[2] == ']')){
		return 6;
	}
	return 0;
}

bool isnumber(char* str){
	if (str != NULL){
		for (int i = 0; i < (int)(strlen(str)); i++){
			if (!isdigit(str[i])){
				return false;
			}
		}
		return true;
	}
	return false;
}

//Funkce pro tisknuti tabulky
void printtabulka(tabulka* TB, char delim){
    radek* aktualniradek = TB->prvniradek;
    while(aktualniradek->dalsiradek != NULL){ //ITERACE RADKY
        bunka* aktualnibunka = aktualniradek->prvnibunka;
		if (aktualnibunka != NULL){
			if (aktualnibunka->dalsibunka == NULL){
				printf("%s", aktualnibunka->hodnota);
			}
			while(aktualnibunka != NULL){ //ITERACE BUNKAMI
				printf("%s", aktualnibunka->hodnota); //print jednotlivych bunek
				if(aktualnibunka->dalsibunka!=NULL){
					printf("%c", delim);
				}
				aktualnibunka = aktualnibunka->dalsibunka;
			}
		}
		else{
			printf("(null)");
		}
        aktualniradek = aktualniradek->dalsiradek;
        printf("\n");
    }
}

char** ctiprikazy(char** argv, int *pocet) { //FREE THIS
    char* token;
	const char delim[2] = ";";
    int i = 0;
    token = strtok(argv[3], delim); //Nacteni prvniho prikazu
    char** result = (char**) malloc(sizeof(char*));
	while (token != NULL){ //Cteme a prirazujeme prikazy
        result[i] = token;
        i++;
        result = (char**) realloc(result,(i+1) * sizeof(char*));
        token = strtok(NULL, delim);
    }
	*pocet = i;
    return result;
}
 
radek* vratPointer(tabulka* TB, int hledane_cislo_radku) {  // VRACI POINTER NA R RADEK
	radek* aktualniradek = TB->prvniradek;
	if (hledane_cislo_radku == 1){
		return TB->prvniradek;
	}
	while (aktualniradek->dalsiradek!= NULL)
	{
		if (aktualniradek->cisloradku == hledane_cislo_radku){
			break;
		}
		aktualniradek = aktualniradek->dalsiradek;
	}
	return aktualniradek;
}

tabulka* nactihodnoty(FILE* f, char delim) {
	tabulka *TB = (tabulka*) malloc(sizeof(radek));  //Malloc tabulky
	if (TB == NULL){
		return NULL;
	}
	TB->prvniradek = (radek*) malloc(1 * sizeof(radek)); //Malloc prvniho radku
	if (TB->prvniradek == NULL){
		return NULL;
	}
    TB->prvniradek->cisloradku = 1;
    TB->prvniradek->dalsiradek = NULL;
    TB->prvniradek->prvnibunka = (bunka*) malloc(sizeof(bunka)); //Malloc prvni bunky
	if (TB->prvniradek->prvnibunka == NULL){
		return NULL;
	}
    TB->prvniradek->prvnibunka->dalsibunka = NULL;
    TB->prvniradek->prvnibunka->hodnota = (char*) malloc (sizeof(char)); //Malloc hodnoty prvni bunky
	if (TB->prvniradek->prvnibunka->hodnota == NULL){
		return NULL;
	}
	char ch;
	int index = 0;
	int ind = 1;
	radek* aktualniradek = TB->prvniradek;
	bunka* aktualnibunka = aktualniradek->prvnibunka;
	while ((ch = fgetc(f)) != EOF){ //Iterujeme skrze radky
		if (ch == '\n'){ //Skaceme na novy radek
			aktualniradek->dalsiradek = (radek*) malloc(sizeof(radek));
			if (aktualniradek->dalsiradek == NULL){
				return NULL;
			}
			aktualniradek = aktualniradek->dalsiradek; //Do aktualniradek dame hodnotu dalsiho radku
			aktualniradek->dalsiradek = NULL;
			aktualniradek->prvnibunka = (bunka*) malloc(sizeof(bunka)); //Malloc prvni bunky v novem radku
			if (aktualniradek->prvnibunka == NULL){
				return NULL;
			}
			ind++;
			aktualniradek->cisloradku = ind;
			aktualnibunka->dalsibunka = NULL;
			aktualnibunka = aktualniradek->prvnibunka;
			aktualnibunka->dalsibunka = NULL;
			aktualnibunka->hodnota = (char*) malloc (sizeof(char));
			if (aktualnibunka->hodnota == NULL){
				return NULL;
			}
			index=0; 
			continue;
		}
		else{
			if (ch == delim){ //Nasli jsme delici znak, zapis hodnotu, jdi dal
				aktualnibunka->dalsibunka = (bunka*) malloc(sizeof(bunka));
				if (aktualnibunka->dalsibunka == NULL){
					return NULL;
				}
				aktualnibunka = aktualnibunka->dalsibunka;
				aktualnibunka->dalsibunka = NULL;
				aktualnibunka->hodnota = (char*) malloc (sizeof(char));
				if (aktualnibunka->hodnota == NULL){
					return NULL;
				}
				index=0;
			}
			else{ //Prirazovani hodnot do bunek
				aktualnibunka->hodnota[index] = ch;
				index++;
				aktualnibunka->hodnota = (char*) realloc(aktualnibunka->hodnota,(index+1) * sizeof(char));
				if (aktualnibunka->hodnota == NULL){
					return NULL;
				}
			}
		}
	}
	
    fclose(f);
	return TB;
}

bunka* bunkaPointer(tabulka* TB, int R, int C){
	int index_C = 2;
	radek* aktualniradek = TB->prvniradek;
	while(aktualniradek->cisloradku != R){
		aktualniradek = aktualniradek->dalsiradek;
	}
	if (C == 1){
		return aktualniradek->prvnibunka;
	}
	else{
		bunka* aktualnibunka = aktualniradek->prvnibunka->dalsibunka;
		while (index_C != C){
			aktualnibunka = aktualnibunka->dalsibunka;
			index_C++;
		}
		return aktualnibunka;
	}
}

int irow(tabulka* TB, int R1){
    radek* novyradek = (radek*) malloc(1 * sizeof(radek));
    if (novyradek == NULL){
        return 0;
    }
    novyradek->cisloradku = R1;
    novyradek->prvnibunka = NULL;
	if (R1 == 1){
		novyradek->dalsiradek = vratPointer(TB, 1);
		TB->prvniradek = novyradek;
	}
    else{
		radek* radekpred = vratPointer(TB, R1-1);
		radek* radekpo = vratPointer(TB, R1);
		radekpred->dalsiradek = novyradek;
		novyradek->dalsiradek = radekpo;
	}
    radek *temp_radek = novyradek;
    while(temp_radek->dalsiradek != NULL) {
        temp_radek->cisloradku = R1;
        temp_radek = temp_radek->dalsiradek;
        R1++;
    }
    return 1;
	zarovnat(TB);
}

void arow(tabulka* TB, int R) { 
	irow(TB, R+1);
}

void drow(tabulka* TB, int R){
	radek* aktualniradek = vratPointer(TB, R);
	if (R == 1){
		TB->prvniradek = TB->prvniradek->dalsiradek;
	}
	else{
		radek* radekpred = vratPointer(TB, R-1);
		if (aktualniradek->dalsiradek == NULL){
			radekpred->dalsiradek = NULL;
		}
		else{
			radekpred->dalsiradek = aktualniradek->dalsiradek;
		}
	}
	free(aktualniradek);
}

void icol(tabulka* TB, int R, int C){
	if(C != 1){
		bunka* bunkapred = bunkaPointer(TB, R, C-1);
		bunka* bunkapo = bunkaPointer(TB, R, C);
		bunka* novabunka = (bunka*) malloc(sizeof(bunka));
		bunkapred->dalsibunka = novabunka;
		novabunka->dalsibunka = bunkapo;
		novabunka->hodnota = (char*) malloc(sizeof(char));
		novabunka->hodnota = NULL;
	}
	else{
		radek* aktualniradek = vratPointer(TB, R);
		bunka* bunkapo = bunkaPointer(TB, R, C);
		bunka* novabunka = (bunka*) malloc(sizeof(bunka));
		aktualniradek->prvnibunka = novabunka;
		novabunka->dalsibunka = bunkapo;
		novabunka->hodnota = (char*) malloc(sizeof(char));
		novabunka->hodnota = NULL;
	}
	zarovnat(TB);
}

void acol(tabulka* TB, int R1, int C){ 
	icol(TB, R1, C+1);
}

void dcol(tabulka* TB, int R, int C){
	bunka* aktualnibunka = bunkaPointer(TB, R, C);
	if (C == 1){
		radek* aktualniradek = vratPointer(TB, R);
		aktualniradek->prvnibunka = aktualnibunka->dalsibunka;
	}
	else{
		bunka* bunkapred = bunkaPointer(TB, R, C-1);
		if (aktualnibunka->dalsibunka == NULL){
			bunkapred->dalsibunka = NULL;
		}
		else{
			bunkapred->dalsibunka = aktualnibunka->dalsibunka;
		}
	}
	free(aktualnibunka);
	zarovnat(TB);
}

void set(bunka* item, char* str){
	free(item->hodnota);
	item->hodnota = str;
}

void clear(bunka* item){
	free(item->hodnota);
	item->hodnota = NULL;
}

void swap(tabulka* TB, bunka* vyber, int R, int C){
	char* temphodnota1 = vyber->hodnota;
	bunka* bunkazapis = bunkaPointer(TB, R, C);
	char* temphodnota2 = (char*) malloc(sizeof(bunkazapis->hodnota));
	strcpy(temphodnota2, bunkazapis->hodnota);
	strcpy(bunkazapis->hodnota, temphodnota1);
	strcpy(vyber->hodnota, temphodnota2);
	free(temphodnota2);
}

void len(tabulka* TB, bunka* item, int R, int C){
	int lenitemu = strlen(item->hodnota);
	bunka* result = bunkaPointer(TB, R, C);
	free(result->hodnota);
	result->hodnota = (char*) malloc (sizeof(lenitemu));
	sprintf(result->hodnota, "%d", lenitemu);
}

void freememory(tabulka* TB){
	radek* aktualniradek = TB->prvniradek;
	while(aktualniradek != NULL){ //ITERACE RADKY
		bunka* aktualnibunka = aktualniradek->prvnibunka;
		while(aktualnibunka != NULL){ //ITERACE BUNKAMI
			free(aktualnibunka->hodnota);
			bunka* tmpbunka = aktualnibunka;
			aktualnibunka = aktualnibunka->dalsibunka;
			free(tmpbunka);
		}
		radek* tmpradek = aktualniradek;
		aktualniradek = aktualniradek->dalsiradek;
		free(tmpradek);
	}
	free(TB);
}

void updateFile(tabulka *TB, char delim){
    FILE *fw = fopen("input.txt", "w");
    radek* aktualniradek = TB->prvniradek;
    while(aktualniradek->dalsiradek != NULL){ //ITERACE RADKY
        bunka* aktualnibunka = aktualniradek->prvnibunka;
        if (aktualnibunka != NULL){
            if (aktualnibunka->dalsibunka == NULL){
                fprintf(fw ,"%s", aktualnibunka->hodnota);
				break;
            }
            while(aktualnibunka != NULL){ //ITERACE BUNKAMI
                fprintf(fw ,"%s", aktualnibunka->hodnota); //print jednotlivych bunek
                if(aktualnibunka->dalsibunka!=NULL){
                    fprintf(fw ,"%c", delim);
                }
                aktualnibunka = aktualnibunka->dalsibunka;
            }
        }
        else{
            fprintf(fw ,"NULL");
        }
        aktualniradek = aktualniradek->dalsiradek;
        fprintf(fw ,"\n");
    }
    fclose(fw);
}

void zarovnat(tabulka* TB){
	int item_count = pocetbunek(TB->prvniradek);
	int row_pocet = pocetradku(TB);
	if (TB->prvniradek->dalsiradek!=NULL){
		radek* aktualniradek = TB->prvniradek->dalsiradek;
		while(aktualniradek != NULL){
			if (pocetbunek(aktualniradek) > item_count){
				item_count = pocetbunek(aktualniradek);
			}
			aktualniradek = aktualniradek->dalsiradek; 
		}

		aktualniradek = TB->prvniradek;
		for (row_pocet; row_pocet>0; row_pocet--){
			int rozdil = item_count - pocetbunek(vratPointer(TB, aktualniradek->cisloradku));
			if (rozdil != 0){
				for (rozdil;rozdil >0; rozdil--){
					acol(TB, aktualniradek->cisloradku, pocetbunek(vratPointer(TB, aktualniradek->cisloradku)));
				}
			}
			aktualniradek=aktualniradek->dalsiradek;
		}
	}
}