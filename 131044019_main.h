/**************************************************************************************************************/
/* Gozde DOGAN - 131044019
/* System Programming HW5
/* 27 Mayis 2018 - Pazar
/**************************************************************************************************************/

#ifndef H_131044019_MAIN
#define H_131044019_MAIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <errno.h>

//#define DEBUG
#define MINIMUM 1000000 	// minumum uzakligi bulmak icin kullanildi
#define STRING_SIZE 25 		// cicekler icin bir size
#define FLOWER_SIZE 3  		// sahip olduklari cicek sayisi
#define COLUMN_SIZE 100 	// dosyadaki satir sayisini bulmak icin fgets'e verilen parametre
#define MAX_SIZE 100

/*
 * Lokasyon bilgisini tutmak icin tanimlandi
 */
typedef struct {
	int x;
	int y;
} Location;

/*
 * Cicekcinin bilgisini tutar
 */
typedef struct 
{
	char name[STRING_SIZE]; 				// dosyadan okunacak
	Location location; 						// dosyadan okunacak
	double clicks;							// dosyadan okunacak
	char flowers[FLOWER_SIZE][STRING_SIZE]; // dosyadan okunacak
	int sizeOfFlowers; 						// dosyadan okunana cicek sayisina gore verilecek
	int numOfSales; 						// islem yaptigi musteri sayisi
	double total_time; 						// islem yaptigi toplam sure
} Florist;


/*
 * Musteri bilgisini tutar
 */
typedef struct{
	char clientName[STRING_SIZE]; 	// dosyadan okunacak
	Location location;				// dosyadan okunacak
	char flower[STRING_SIZE];		// dosyadan okunacak
} Customer;


/******************************* FUNCTION DEFINITION *******************************/
/*
 * Main de cagrilan ana fonksiyon
 * Dosyadan verilerin okunup arraylere atilmasi gerceklestirilir.
 * Hangi musterinin hangi cicekciye gidecegi belirlenir.
 * Her cicekciye ait musteri kuyrugu olusturulur.
 * Musteri islemleri gerceklestirilir.
 * Musteri islemlerini gerceklestirmek icin, her cicekci kuyrugunun tamamlanmasini saglayacak threadler olusturuldu.
 * Threadler her musteri isini hallettikten sonra, bitip bitmedikleri condition varibles ile kontrol edildi.
 * Son oalrak elde edilen veriler ekranda gosterildi.
 */
void chooseFlorist();

/*
 * Yanlis calistirildigi zaman nasil calistirilacaginin yazildigi fonksiyon
 */
void usage();

/*
 * Dosyadaki cicekci, client sayisinin bulundugu fonksiyon
 */
void findNumOfFlowersAndNumOfClients();

/*
 * Dosyadan veriler okunur.
 * Okunan verinin ait oldugu arraye yazilir.
 */
void readFile();

/*
 * Hangi musterinin hangi cicekciye gitmesinin daha dogru oldugu bulunur.
 * Musteri gitmesinin uygun oldugu cicekci kuyruguna alinir.
 */
void determineCustomersForFlorist();

/*
 * Musteri ile cicekci arasindaki mesafe hesaplanir.
 * Mesafe bilgisi return edilir.
 */
double calculateDistance(Location mainLocation, Location clientLocation);

/*
 * Musteri, mesafe ve istedigi cicek bolgesine gore gitmesi gereken cicekci secilir.
 * Cicekci id'si return edilir.
 */
int clientChooseflorist(Customer customer);

/*
 * Musterinin istedigi cicek, cicekci de var mi diye kontrol edilir.
 * Varsa 1, yoksa 0 return edilir.
 */
int isElement(int index, char* clientFlower);

/*
 * Ilk cicekci icin olusturulan, thread olusturuldugu zaman calistirilan fonksiyon.
 * Gelen index'e gore cicekci ve musteri bilgileri ekrana yazilir.
 * Islem bittiginde condition variables ile sinyal yollanir.
 * Bu islemleri yaparken kritik bolgede olacagi icin kritik bolge, mutex lock ve mutex unlock blogunda yer alir.
 */
void *threadOperations_0(void *arg);

/*
 * Ikinci cicekci icin olusturulan, thread olusturuldugu zaman calistirilan fonksiyon.
 * Gelen index'e gore cicekci ve musteri bilgileri ekrana yazilir.
 * Islem bittiginde condition variables ile sinyal yollanir.
 * Bu islemleri yaparken kritik bolgede olacagi icin kritik bolge, mutex lock ve mutex unlock blogunda yer alir.
 */
void *threadOperations_1(void *arg);

/*
 * Son cicekci icin olusturulan, thread olusturuldugu zaman calistirilan fonksiyon.
 * Gelen index'e gore cicekci ve musteri bilgileri ekrana yazilir.
 * Islem bittiginde condition variables ile sinyal yollanir.
 * Bu islemleri yaparken kritik bolgede olacagi icin kritik bolge, mutex lock ve mutex unlock blogunda yer alir.
 */
void *threadOperations_2(void *arg);

/*
 * Dosyadan okunan verilere gore allocate edilen yerler free edilir.
 */
void freeArr();

/*
 * Cicekcinin islemlere gore dolduracagi veriler initialize edilir.
 */
void initialize();

/*
 * Client ve Florist'ler ekrana yazilir.
 */
void printCustomerAndFlorist();

/*
 * Cicekcinin bilgileri ekrana yazilir.
 */
void printFlorist(Florist florist);

/*
 * Client'in bilgileri ekrana yazilir.
 */
void printCustomer(Customer customer);

/*
 * Programin ne kadar calistigini hesaplamak icin kullanildi.
 */
long calculateTimeDifference(struct timeval start, struct timeval end);
/************************************************************************************/ 


char file[STRING_SIZE]; 						// okunacak dosya
int sizeOfFlorist = 0; 							// cicekci sayisi
int sizeOfClients = 0; 							// client sayisi
Florist *florist; 								// ciceksi arrayi
Customer *customer; 							// client arrayi

Customer *customersForFlorist_0;    			// ilk cicekciye gidece client kuyrugu
double distance_0[MAX_SIZE]; 						// ilk cicekciye gidecek client'lar ile cicekci arasindaki mesafeler
int index_0 = 0; 								// ilk cicekcinin kuyrugunda islenecek client'in index'i
int size_0 = 0; 								// ilk cicekciye gelecek client sayisi
Customer *customersForFlorist_1;				// ikinci cicekciye gidece client kuyrugu
double distance_1[MAX_SIZE]; 						// ikinci cicekciye gidecek client'lar ile cicekci arasindaki mesafeler
int index_1 = 0; 								// ikinci cicekcinin kuyrugunda islenecek client'in index'i
int size_1 = 0;									// ikinci cicekciye gelecek client sayisi
Customer *customersForFlorist_2;    			// son cicekciye gidece client kuyrugu
double distance_2[MAX_SIZE]; 						// son cicekciye gidecek client'lar ile cicekci arasindaki mesafeler
int index_2 = 0;								// son cicekcinin kuyrugunda islenecek client'in index'i
int size_2 = 0; 								// son cicekciye gelecek client sayisi

pthread_t threadForFlorists[MAX_SIZE];			// cicekciler icin olsuturulan threadlerin id'leri

struct timeval sTime; 							// programin baslangic zamani
struct timeval lastTime; 						// programin bitis zamani
 
pthread_mutex_t mutex;							// kritik bolgeyi kilitlemek icin kullanilan mutex
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // thread'in bittigine dair sinyal yollayip, yakalamak icin kullanilan condition varibles

static int doneFlag_0 = 0; 						// Ilk cicekci kritik bolgeye girip islemini tamamladiginda degidtirlen degisken
static int doneFlag_1 = 0;						// Ikinci cicekci kritik bolgeye girip islemini tamamladiginda degidtirlen degisken
static int doneFlag_2 = 0; 						// Son cicekci kritik bolgeye girip islemini tamamladiginda degidtirlen degisken

int done = 0; 									// threadler cicin bitis sinyalinin yakalanmasini saglayan degisken

#endif