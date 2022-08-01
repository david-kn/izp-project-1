// vstup a výstup
#include <stdio.h>
//
#include <stdlib.h>
//  funkce pro práci s řetězci
//#include <string.h>
// funkce pro ověřování isalnum(), ...
#include <ctype.h>

/**********************************/
// Nastaveni konstant

// Chybové kódy programu
enum e_codes
{
   E_OK,
   E_ERROR_DURING_PROCESS,
   E_NO_PARAM,
   E_PARAM_WRONG,
   E_OUT_OF_RANGE,
   E_UNKNOWN,
};

// Stavové kódy programù
enum states
{
   S_HELP,
   S_CCOMP,
   S_DECOMP,
   S_NOTHING, // budu ho opravdu používat ??
};

// Parametry a vnitřní proměné
typedef struct params
{
   unsigned int n;
   int e_code;
   int state;
} TParams;

const char *HELPMSG =
    "IZP - Projekt č. 1 - Jednoduchá komprese textu\n"
    "Autor: David Koňař\n"
    "E-mail: xkonar07@stud.fit.vutbr.cz\n"
    "Použití:\tproj1 -h\n"
    "	\tproj1 -c N\n"
    "	\tproj1 -d N\n"
    //	"	\tproj1 --extra\n"
    "Popis parametrů:\n"
    "\t-h\t Vytiskne nápovědu k programu\n"
    "\t-c N\t Přepne program do režimu komprese textu který je požadován jako vstup přes stdin. Dalším povinným parametrem který musí přepínač"
    " následovat je N (typu unsigned int) který udává délku bloku ve znacích, které mají být vyhledávány a komprimovány.\n"
    "\t-d N\t Přepne program do režimu dekomprese textu, který je požadován jako vstup přes stdin. Dalším povinným parametrem, který musí tento přepínač"
    " následovat je N (typu unsigned int), který udává délku textového bloku ve znacích, které mají být dekomprimovány.\n"
    //	"\t--extra\t Přepne program... "
    "";

// Chybové zprávy
const char *E_MSG[] = {
    // V¹e v poøádku
    "\nVse je v poradku\n",
    "\nChyba pri behu funkce - nevalidni vstupni data!\n",
    // Nebyl zadán ¾ádný parametr
    "\nNebyl zadán zádny parametr!\n",
    "\nByl zadan spatny parametr !\n",
    "\nBylo zadano prilis malo/mnoho parametru !\n",
    "\nVyskytla se chyba !\n",
};

/***********************************/
// Inicializace funkcí
//

TParams getParams(int argc, char *argv[])
/*
   *  inicializuje strukturu, kontroluje parametry a podle toho nastavuje hodnoty pro dal¹í bìh programu
   */
{
   // inicializace struktury
   TParams result =
       {
           .n = 0,
           .e_code = E_OK,
           .state = S_CCOMP,
       };

   // volání nápovìdy
   if (argc == 2 && strcmp("-h", argv[1]) == 0)
   {
      result.state = S_HELP;
   }
   // dva parametry, musí probìhnout kontrola
   else if (argc == 3)
   {

      // v podmínce je rovnoi i pøevod ASCII -> 'int' (v pøípadì [2] parametr neni cislo, fce atoi() vrati '0' => neplatna podminka
      if ((strcmp("-d", argv[1]) == 0) && (result.n = atoi(argv[2])))
      {
         // OK, budeme dekomprimovat
         //printf("dekomprese\n");
         if (!check_n(result.n))
            result.e_code = E_PARAM_WRONG;

         result.state = S_DECOMP;
      }

      //v podmínce je rovnoi i pøevod ASCII -> 'int' (v pøípadì [2] parametr neni cislo, fce atoi() vrati '0' => neplatna podminka
      else if ((strcmp("-c", argv[1]) == 0) && (result.n = atoi(argv[2])))
      {
         // OK, budeme komprimovat
         // printf("komprese\n");
         if (!check_n(result.n))
            result.e_code = E_PARAM_WRONG;

         result.state = S_CCOMP;
      }
      // ¹patné parametry =>, chybový stav
      else
      {
         printf("|%s|%s|", argv[1], argv[2]);
         result.e_code = E_UNKNOWN;
      }
   }
   // chyba, pøíli¹ mnoho parametrù
   else
   {
      result.e_code = E_OUT_OF_RANGE;
   }

   return result;
}

int compress(int n)
{

   /***************************************************************************************************************************************/
   int i = 0, q = 0, tmp, v, a, t, printed = 0, krat = 1;
   char ch[n];
   char op, tp, transfer;
   //int start = 0;

   // načte N znaků do dočasné proměné kde se pak porovnává případné opakování
   for (q = 0; q < n; q++)
   {
      ch[q] = getchar();
   }

   /* TESTOVANI #01 */ /*
   printf("\nnacteno:");
   for ( t = 0; t < n; t++)
      putchar(ch[t]);
   printf("\n");
/* #01 */

   // reset prom.
   q = 0;
   // nekonečný cylus
   for (;;)
   {

      if ((ch[q] != (tmp = getchar())) || krat == 9)
      {

         /* 02 */
         /*   printf("\nmuj buffer:");
         for (i = 0; i < n; i++)
            putchar(ch[i]);
**/
         //   printf("\n%c != %c (%d)\n", ch[q], tmp, tmp);
         /* 02 E */

         //kontrola konce vstupu
         if (tmp == EOF)
         {
            //jeste nesmim zapomenout vypsat buffer a opakovani na vystup !
            /*
                     * v pripade ze tesne pred koncem se opakovalo musiseto aj vypsat a ne zahodit
                  */
            if (krat > 1)
            {
               krat = krat + '0';
               putchar(krat);
               // vypíše daný blok znaků uložený v poli s číslovkou před pro určení počtu opakování

               for (i = 0; i < n; i++)
                  putchar(ch[i]);
            }
            else
            {
               /*
                  * ta podminka je tak slozita, protoze kdyz n=1 a probihalo pred koncem opakovani tak by se nic nestalo a vse se vypsalo v overeni nademnou
                  * ale kdyz by tomu tak nebylo a n=1 tak by se ten posledni znak nevytiskl kvuli podmince "i < n && n != 1" a tak jsem musel pridat ze bud plati
                  * n !=1 a pak se zaverecny buffer vklidu vypise anebo krat=1 a to pak v pripade ze n=1 a neopakovalo se, vyhodi ten posledni, skoro zapomenuty :D,
                  * znak na obrazovku
                  * !! NEVIM JAK SE TO BUDE CHOVAT KDYZ BUDE NA KONCI OPAKOVANI "abcabc" nebo "abcabcabq" (EOF) to musim vykoumat hned jak se mi podari sprovoznit
                  * posun buferu po castecnem opakovani v retezci ktery ve skutecnosti ale opakovani neni !!
                */
               for (i = 0; (i < n && (n != 1 || krat == 1)); i++)
                  putchar(ch[i]);
            }
            break;
         } // EOF EOF

         if (q != 0 || krat > 1)
         {
            /* 03 */ /*
              printf("pozor kulisarna ;-)\n");
            /* 03 E */
            if (krat > 1)
            {
               printed = 1;
               //  printf("budeme vypisovat opacko\n");
               krat = krat + '0';
               putchar(krat);
               // vypíše daný blok znaků uložený v poli s číslovkou před pro určení počtu opakování
               for (i = 0; i < n; i++)
                  putchar(ch[i]);
               /*          printf("*");       */
               // reset proměných
               krat = 1;
               //q = 0; // OPRAVUD ?????
            }

            if (q != 0)
            {
               // musíme aktualizovat buffer - vnejsi fce update_buffer
               //  printf("\nbudeme aktulizovat buffer\n");
               if (printed == 0)
               {
                  //protoze slo o plany poplach vypiseme prave tolik znaku ktere vlastne v zadnem opakovani nejsou
                  //    printf("*");
                  for (i = 0; i <= q; i++)
                  {
                     putchar(ch[i]);
                     /*                     if((i+1) == q)
                        putchar(ch[i+1]);
*/
                     //                     printf("AAA");
                  }
                  //      printf("*");
                  if (n == 1)
                     ch[q] = tmp;
                  else
                  {
                     for (a = 0; a < (n - (q + 1)); a++)
                     {             // (q+1) - následující prvek po tom který kolidoval
                        i = n - 1; // posledni prvke bufferu uložíme do tempu
                        tp = ch[i];

                        // posunutí celého bufferu o jedno doprava,
                        for (i; i > 0; i--)
                           ch[i] = ch[i - 1];
                        ch[i] = tp; // i == 0
                     }
                     ch[n - 1] = tmp; // na poslední pozici je prvek ktery jsme nacetli a nebyl shodny s bufferem
                     a = 0, i = 0;
                  }
               }
               else
               {
                  i = 0;
                  // backup:
                  // if (i < q)
                  while (i < q)
                  {
                     ch[i] = ch[i];
                     i++;
                  }
                  ch[i] = tmp;
                  i++;
                  while (i < n)
                  {
                     ch[i] = getchar();
                     i++;
                  }
                  /* E-X */ /*
                printf("\n2novy buf.:\n'");
                 for (i = 0; i < n ; i++)
                    putchar(ch[i]);
                     printf("'\n");
               /* END E-X */
               }
            }
            // reset bufferu - udelame externak
            else
            {
               //printf("\nbudeme resetovat buffer:tmp=%c\n", tmp);
               q = 0;
               ch[q] = tmp;
               for (q = 1; q < n; q++)
               {
                  ch[q] = getchar();
                  //   printf("%d:ch[q] = %c\n", q, ch[q]);
               }
               /* E-x */ /*
                 printf("\nnovy buf.:'");
                 for (i = 0; i < n ; i++)
                    putchar(ch[i]);
                  printf("'\n");
                  /* END E-x */
            }
            q = 0;
            a = 0;
            i = 0;
            printed = 0;
         }
         // zadne komplikace, jen opis
         else
         {
            // printf("vpoho jen to opisu\n");
            putchar(ch[q]);

            // jestli n == 1 tak buffer je jednoprvkovy
            // bude se to volat jako externat posun_buffer()
            if (n == 1)
               ch[q] = tmp;
            else
            { // posuň buffer
               for (i = 0; i < (n - 1); i++)
               { // check(n) || i < (n - 1)
                  ch[i] = ch[i + 1];
                  if ((i + 1) == (n - 1))
                  {
                     ch[i + 1] = tmp;
                  }
               }
               /* E-a*/ /*
                 printf("\nnovy buf.:'");
                 for (i = 0; i < n ; i++)
                    putchar(ch[i]);
                printf("'\n");
                 /* END E-a */
            }
         }
      }
      // znaky se rovnaji
      else
      {
         if ((q + 1) == n)
         {
            //  printf("\n1Rovnaji se!(%c=%c)\n", ch[n], tmp);
            krat++;
            q = 0;
            //  printf("\nk-Q=%d;krat=%d\n",  q, krat);
         }
         else
         {
            // printf("\n2Rovnaji se!(%c=%c)\n", ch[q], tmp);

            q++;
            // printf("\nk-Q=%d;krat=%d\n", q, krat);
         }
      }
   }

   /**************************************************************************************************************************************/

   return 0;
}

int decompress(int n)
{

   int ch = -1;
   int error = 0;
   int remain = 0, i = 0, p = 0;
   char *string[n];
   // printf("pred while\n");
   while ((ch = getchar()) != EOF)
   {
      // jestli je èíslo (rozmezi podle ASCII tabulky), budeme dekomprimovat
      if (ch < 58 && ch > 48)
      {
         //printf("Budeme opakovat :D\n");
         //pøevede èíslo v char na int
         //remain = atoi(ch);
         (remain = (ch - '0'));

         // napln buffer podle delky bloku retezce zadaneho parametrem N
         for (i = 0; i < n; i++)
         {
            //string[i] = getchar(); // FUKNČNÍ
            ch = check_de_char(getchar());

            // ošetření hodnot a rozhodnuti podle toho co vratila kontrolni funkce

            // zpracovani vysupu kontrolni funkce
            if (ch == -1)
            {
               error = 1;
               break;
            }
            string[i] = ch;
         }
         //printf("V bufferu je :D\n");
         /* TEMP
			for(i = 0; i < n; i++)
            putchar(string[i]);
			//printf("\nBudeme vypisovat :D\n");
			*/
         /* END TEMP */
         // vypi¹ buffer x-krat (podle prectene cislovky na vstupu stdin
         for (i = 0; i < remain; i++)
         {
            for (p = 0; p < n; p++)
            {
               putchar(string[p]);
            }
         }
         // není èíslo, jen opi¹ znak
      }
      else
      {
         putchar(ch);
      }
   }

   return error;
}

void print_buffer(char *string[], int num)
{
   int i = 0;

   putchar(num);
   for (i = 0; i < num; i++)
      putchar(string[i]);
}

char get_buffer(int n)
{
   int *string[n];
   int tmp;
   int i = 0;

   for (i = 0; i < n; i++)
      string[i] = tmp = getchar();
   // MUSÍ PROBĚHNOUT OVEŘENÍ
   // tmp = check_buffer(tmp);
   // if (tmp == -1)
   //    CHYBA RETURN -1
}

int check_de_char(int symbol)
{
   /*
   * funkce kontroluje zda-li se nejedná o číslo nebo EOF (nutné pro dekomprimaci !)
   */
   if (isdigit(symbol) || symbol == EOF)
      symbol = -1; // chyba, vstup ted nesmi byt cislo,vse ostatni je povoleno

   return symbol;
}

int check_symbol(int symbol)
{
   /*
   *  kontroluje jestli se jedná o validní znaky (jen písmena a interpkunkční symboly) (nutné pro komprimaci !)
   */
   if (isprint(symbol) && !isdigit(symbol))
      return 0;
   else // chyba, buď číslo nebo nevalidní vstup
      return 1;
}

int check_n(int cislo)
{
   /*
   * Provadi kontrolu jestli je "N" v rozmezi 1-9 jinak hlasi chybu
   */
   if (0 < cislo && cislo < 10)
      return 1;
   else
      return 0;
}

//zobrazit chybu na výstup stderr viz. kostra
void print_error(int ecode)
{

   fprintf(stderr, "%s", E_MSG[ecode]);
}

/**********************************/

//
// HLAVNÍ TĚLO PROGRAMU
//

int main(int argc, char *argv[])
{
   int i = 0;
   TParams params = getParams(argc, argv);

   // chyba, ukonči se !
   if (params.e_code != E_OK)
   {
      print_error(params.e_code);
      return EXIT_FAILURE;
   }
   // Tiskneme nápovědu
   if (params.state == S_HELP)
   {
      printf("%s", HELPMSG);
      return EXIT_SUCCESS;
   }
   /* TEMP */ /*
  	for(i = 0; i < argc; i++) {
		printf("Parametr c.%d: %s\n", i, argv[i]);
		zkontroluj_parametry(argv[i]);
	}
	printf("Konec\n");
	*/
              /* END TEMP */

   // Zatím se zdá být vše ok, voláme pracovni funkce
   if (params.state == S_CCOMP)
   {
      params.e_code = compress(params.n);
   }
   else if (params.state == S_DECOMP)
   {
      params.e_code = decompress(params.n);
   }
   else
   {
      fprintf(stderr, "Tady jsem se určitě neměl dostat, někde je chyba !!");
   }

   if (params.e_code != E_OK)
   {
      print_error(params.e_code);
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
