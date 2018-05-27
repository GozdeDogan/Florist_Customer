/**************************************************************************************************************/
/* Gozde DOGAN - 131044019
/* System Programming HW5
/* 27 Mayis 2018 - Pazar
/**************************************************************************************************************/

#include "131044019_main.h" //header file


int main(int argc, char const *argv[])
{
	gettimeofday(&sTime,NULL); // basladigi zamani bulundu
	
	if(argc != 2){    //dogru calistirilmadiysa usage verildi
		usage(); 
		return 0;
	}

	sprintf(file, "%s", argv[1]); // parametre olarak gelen dosya adi alindi
	#ifdef DEBUG
	 fprintf(stderr, "file:%s\n", file);
	#endif
	
	chooseFlorist();   // İslemler gerceklestirildi

	gettimeofday(&lastTime,NULL); // bitis zamani

	#ifdef DEBUG
		fprintf(stdout,"Total run time, in milliseconds:\t%ldms. \n\n",calculateTimeDifference(sTime,lastTime));  // programin calisma suresi hesaplandi
	#endif

	return 0;
}

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
void chooseFlorist(){

	fprintf(stderr, "\nFlorist application initializing from file: %s\n\n", file);

	findNumOfFlowersAndNumOfClients();  // dosyadan cicekci ve client sayisi bulundu

	/* Bulunan sayilara gore yer alindi */
	florist = (Florist*)calloc(sizeOfFlorist, sizeof(Florist));
	customer = (Customer*)calloc(sizeOfClients, sizeof(Customer));

	/* cicekci kuyruklari icinde yer alindi */
	customersForFlorist_0 = (Customer*)calloc(sizeOfClients, sizeof(Customer));
	customersForFlorist_1 = (Customer*)calloc(sizeOfClients, sizeof(Customer));
	customersForFlorist_2 = (Customer*)calloc(sizeOfClients, sizeof(Customer));

	initialize(); // cicekci struct'indaki degerler initialize edildi

	readFile(); // dosya okundu, florist ve customer arrayleri dodluruldu.
	#ifdef DEBUG
		printCustomerAndFlorist();  // okunana dosya ekrana bastirildi
	#endif

	determineCustomersForFlorist(); //floristlere ait client kuyruklari olusturuldu
	
	fprintf(stderr, "%d florists have been created\n\n", sizeOfFlorist);

	fprintf(stderr, "Processing requests\n\n");


	if (pthread_mutex_init(&mutex, NULL) != 0)  //mutex initialize edildi. 
    {
        fprintf(stderr, "mutex init has failed\n");
        exit(EXIT_FAILURE);
    }

    /*kuyruklar olsuturulurken degisen index'ler baslangic index'ine geri cevrildi.*/
	index_0 = 0; 
	index_1 = 0;
	index_2 = 0;

	int count = 0;
	
	while ((index_0 + index_1 + index_2) < sizeOfClients) // client sayisi kontrol edildi.
	{
		/*
			Client sayisi her cicekciye ait olan indexlerin toplami olacaktir
			Bu nedenle bu incexlerin toplami kontrol edildi.
			Eger indexlerin toplami sizeOfClients'dan buyuk olursa
				client'lar tamamlanmis demektir.
			Eger indexlerin toplami sizeOfClients'dan kucuk olursa
				clientlar henuz tamamlanmamis demektir. 

			Client'lar tamamlanana kadar da her cicekciye giden client icin 
			thread olsuturulacaktir.

			Boylece toplamda sizeOfClients kadar thread olusmus olmasi beklenir.
			Bu threadler, dongu sonunda join edilir.
			Donguden cikildiktan sonra da condition variables ile 
			threadlere ait bitti sinyalleri toplanir.
			Bitti sinyallerinin toplanmasi islemi de kritik bolge icinde yapilmistir.
		*/

		//sleep(0.6);
		unsigned long i;
		for (int i = 0; i < 0xFFFFFF; ++i);

		/*
			Her musterinin ait oldugu kuyruk icin yani aslinda her cicekci kuyrugu icin thread olusturuldu.

			Ilgili musteri geldiginde, ilgili cicekci threadi olsuturuldu.
        	Ilgili cicekci threadi ilgili cicekciye ait thread fonksiyonunu calistirdi.

        	Kuyruktaki her musteri icin thread olusturuldu demek daha dogru olacaktir.

        	Her kuyruk icin ayrı bir thread olsuturuldu.
        	Bununla paralel calisma saglanilmaya calisildi.
		*/

		if(index_0 < size_0){ // ilk cicekci kuyrugundan gelen bir client ise 
			// thread olsuturuldu ve ilgili index parametre olarak verildi.
			int res = pthread_create(&threadForFlorists[0], 0, threadOperations_0, (void *)&index_0);
			if(res != 0){
				fprintf(stderr, "pthread_create couldn't do\n");
				exit(EXIT_FAILURE);
			}	
			if (pthread_detach(threadForFlorists[0])){
            	printf("Thread detached failes!!!\n");
            	exit(EXIT_FAILURE);
			}
		}

		if(index_1 < size_1){ // ikinci cicekci kuyrugundan gelen bir client ise 
			// thread olsuturuldu ve ilgili index parametre olarak verildi.
			int res = pthread_create(&threadForFlorists[1], 0, threadOperations_1, (void *)&index_1);
			if(res != 0){
				fprintf(stderr, "pthread_create couldn't do\n");
				exit(EXIT_FAILURE);
			}
			if (pthread_detach(threadForFlorists[1])){
            	printf("Thread detached failes!!!\n");
            	exit(EXIT_FAILURE);
			}
		}


		if(index_2 < size_2){ // son cicekci kuyrugundan gelen bir client ise 
			// thread olsuturuldu ve ilgili index parametre olarak verildi.
			int res = pthread_create(&threadForFlorists[2], 0, threadOperations_2, (void *)&index_2);
			if(res != 0){
				fprintf(stderr, "pthread_create couldn't do\n");
				exit(EXIT_FAILURE);
			}
			if (pthread_detach(threadForFlorists[2])){
            	printf("Thread detached failes!!!\n");
            	exit(EXIT_FAILURE);
			}
		}


		/* Ilgili cicekciden gelen flag ise (sinyal de denilebilinir) */
		if(doneFlag_0 == 1){
			index_0 = index_0 + 1; //index degeri degistirildi
			doneFlag_0 = 0; 		// sinyal degeri eski haline getirildi 
			florist[0].numOfSales = florist[0].numOfSales + 1; 	// ilgili cicekcinin ilgilendigi msuteri sayisi arttirildi
    		//florist[0].total_time = florist[0].total_time + distance_0[index_0-1]*florist[0].clicks;
			pthread_join(threadForFlorists[0], NULL);
			/*count++;*/
		}
		
		/* Ilgili cicekciden gelen flag ise (sinyal de denilebilinir) */	
		if(doneFlag_1 == 1){
			index_1 = index_1 + 1; //index degeri degistirildi
			doneFlag_1 = 0;        // sinyal degeri eski haline getirildi
			florist[1].numOfSales = florist[1].numOfSales + 1; // ilgili cicekcinin ilgilendigi msuteri sayisi arttirildi
    		//florist[1].total_time = florist[1].total_time + distance_1[index_1-1]*florist[0].clicks;
			pthread_join(threadForFlorists[1], NULL);
			/*count++;*/
		}

		if(doneFlag_2 == 1){
			index_2 = index_2 + 1;	//index degeri degistirildi
			doneFlag_2 = 0;			// sinyal degeri eski haline getirildi
			florist[2].numOfSales = florist[2].numOfSales + 1; // ilgili cicekcinin ilgilendigi msuteri sayisi arttirildi
    		//florist[2].total_time = florist[2].total_time + distance_2[index_2-1]*florist[0].clicks;
			pthread_join(threadForFlorists[2], NULL);
			/*count++;*/
		}
	}


	/* threadler'in bittigine dair gelen sinyalleri yakalamak icin kritik bolge olsuturuldu*/
	pthread_mutex_lock( &mutex );

	// threadler hala mesgul mu diye kontrol edildi
	// msuteri sayisi kadar thread olusmus olacagi icin sizeOfClients degerine gore kontrol yapildi
	while( done < sizeOfClients ){ //
		//printf( "[thread main] done is %d which is < %d so waiting on cond\n", done, sizeOfClients);
		pthread_cond_wait( &cond, &mutex );  // threadler'den gelen bitti sinyalleri yakalandi
		//puts( "[thread main] wake - cond was signalled." ); 

	}

	#ifdef DEBUG
		printf("\n%d thread created!\n", sizeOfClients );
	#endif

	pthread_mutex_unlock( & mutex ); // thread'lerin bitti sinyalleri yakalandiktan sonra ktirik bolgeden cikildi

	// islemler bittigi icin mutex'in isi de bitmis oldu, bu nedenle destroy edildi
	if(pthread_mutex_destroy(&mutex) != 0){
		fprintf(stderr, "mutex couldn't destroy\n");
	}

	// Sonuclar ekrana yazildi
	fprintf(stderr, "\nAll requests processed.\n\n");

	for (int i = 0; i < sizeOfFlorist; ++i)
		fprintf(stderr, "%s closing shop.\n", florist[i].name);


	fprintf(stderr, "\nSale statistics for today:\n");
	fprintf(stderr, "-----------------------------------------------------------------\n");
	fprintf(stderr, "Florists \t\t# of sales \t\tTotal time\n");
	fprintf(stderr, "-----------------------------------------------------------------\n");

	for (int i = 0; i < sizeOfFlorist; ++i)
		fprintf(stderr, "%s \t\t\t%d \t\t\t%.fms\n", florist[i].name, florist[i].numOfSales, florist[i].total_time);

	fprintf(stderr, "-----------------------------------------------------------------\n\n");

	freeArr(); // Allaocate edilen yerler serbest birakildi
}

/*
 * Ilk cicekci icin olusturulan, thread olusturuldugu zaman calistirilan fonksiyon.
 * Gelen index'e gore cicekci ve musteri bilgileri ekrana yazilir.
 * Islem bittiginde condition variables ile sinyal yollanir.
 * Bu islemleri yaparken kritik bolgede olacagi icin kritik bolge, mutex lock ve mutex unlock blogunda yer alir.
 */
void *threadOperations_0(void *arg){
	int res = pthread_mutex_lock(&mutex); // kritik bolge kitlendi
    if(res != 0){
        fprintf(stderr, "lock failure\n");
    }

 	int index = *((int *)arg); // gelen arguman kullanilacak tipe donusturuldu
 	

 	double distance = distance_0[index]*florist[0].clicks; // Client ile musteri arasindaki islem zamani hesaplandi
 	//fprintf(stderr, "distance: %.f\n", distance);
 
 	/*unsigned long i;
	for (int i = 0; i < 0xFFFFFF; ++i);*/

    if(index < size_0){ // cicekci ve client bilgileri ekrana yazildi
		fprintf(stderr, "Florist %s has delivered a clove to %s in %.fms\n", florist[0].name, customersForFlorist_0[index].clientName, distance); 

		doneFlag_0 = 1; // islem yapildi sinyali verildi
	}

    done++; // olsuturulan thread sayisi artti

    florist[0].total_time = florist[0].total_time + distance; // cicekcinin harcadigi zamana ekleme yapildi

    pthread_cond_signal( &cond );  // islem bitti sinyali yollandi

    res = pthread_mutex_unlock(&mutex); // kritik bolge diger threadler icin serbest birakildi
    if(res != 0){
        fprintf(stderr, "unlock failure\n");
    }
 
 	//fprintf(stderr, "pthread_mutex_unlock\n");
 	//pthread_exit(NULL); // thread islemi sonlandirildi
 	//fprintf(stderr, "pthread_exit\n");
    return NULL;
}

/*
 * Ikinci cicekci icin olusturulan, thread olusturuldugu zaman calistirilan fonksiyon.
 * Gelen index'e gore cicekci ve musteri bilgileri ekrana yazilir.
 * Islem bittiginde condition variables ile sinyal yollanir.
 * Bu islemleri yaparken kritik bolgede olacagi icin kritik bolge, mutex lock ve mutex unlock blogunda yer alir.
 */
void *threadOperations_1(void *arg){
	int res = pthread_mutex_lock(&mutex); // kritik bolge kitlendi
    if(res != 0){
        fprintf(stderr, "lock failure\n");
    }

 	int index = *((int *)arg); // gelen arguman kullanilacak tipe donusturuldu

    double distance = distance_1[index]*florist[0].clicks; // Client ile musteri arasindaki islem zamani hesaplandi

    /*unsigned long i;
	for (int i = 0; i < 0xFFFFFF; ++i);*/

	if(index < size_1){ // cicekci ve client bilgileri ekrana yazildi
		fprintf(stderr, "Florist %s has delivered a clove to %s in %.fms\n", florist[1].name, customersForFlorist_1[index].clientName, distance); 
		
		doneFlag_1 = 1; // islem yapildi sinyali verildi
	}

    done++; // olsuturulan thread sayisi artti

    florist[1].total_time = florist[1].total_time + distance;// cicekcinin harcadigi zamana ekleme yapildi

    pthread_cond_signal( &cond ); // islem bitti sinyali yollandi

    res = pthread_mutex_unlock(&mutex); // kritik bolge diger threadler icin serbest birakildi
    if(res != 0){
        fprintf(stderr, "unlock failure\n");
    }
 
 	//fprintf(stderr, "pthread_mutex_unlock\n");
 	//pthread_exit(NULL); // thread islemi sonlandirildi
 	//fprintf(stderr, "pthread_exit\n");
    return NULL;
}

/*
 * Son cicekci icin olusturulan, thread olusturuldugu zaman calistirilan fonksiyon.
 * Gelen index'e gore cicekci ve musteri bilgileri ekrana yazilir.
 * Islem bittiginde condition variables ile sinyal yollanir.
 * Bu islemleri yaparken kritik bolgede olacagi icin kritik bolge, mutex lock ve mutex unlock blogunda yer alir.
 */
void *threadOperations_2(void *arg){
	int res = pthread_mutex_lock(&mutex); // kritik bolge kitlendi
    if(res != 0){
        fprintf(stderr, "lock failure\n");
    }

 	int index = *((int *)arg); // gelen arguman kullanilacak tipe donusturuldu
 	
 	double distance = distance_2[index]*florist[0].clicks; // Client ile musteri arasindaki islem zamani hesaplandi
 
 	/*unsigned long i;
	for (int i = 0; i < 0xFFFFFF; ++i);	*/

	if(index < size_2){ // cicekci ve client bilgileri ekrana yazildi
		fprintf(stderr, "Florist %s has delivered a clove to %s in %.fms\n", florist[2].name, customersForFlorist_2[index].clientName, distance); 
		
		doneFlag_2 = 1; // islem yapildi sinyali verildi
	}

    done++; // olsuturulan thread sayisi artti

    florist[2].total_time = florist[2].total_time + distance; // cicekcinin harcadigi zamana ekleme yapildi

    pthread_cond_signal( &cond );  // islem bitti sinyali yollandi

    res = pthread_mutex_unlock(&mutex); // kritik bolge diger threadler icin serbest birakildi
    if(res != 0){
        fprintf(stderr, "unlock failure\n");
    }
 
 	//fprintf(stderr, "pthread_mutex_unlock\n");
 	//pthread_exit(NULL); // thread islemi sonlandirildi
 	//fprintf(stderr, "pthread_exit\n");
    return NULL;
}

/*
 * Hangi musterinin hangi cicekciye gitmesinin daha dogru oldugu bulunur.
 * Musteri gitmesinin uygun oldugu cicekci kuyruguna alinir.
 */
void determineCustomersForFlorist(){
	int index = -1;

	for (int i = 0; i < sizeOfClients; ++i)
	{
		index = clientChooseflorist(customer[i]); // client icin uygun cicekci hesaplandi 

		if(index == 0) // client ilgili kuyruga eklenildi
		{
			sprintf(customersForFlorist_0[index_0].clientName, "%s", customer[i].clientName);
			customersForFlorist_0[index_0].location.x = customer[i].location.x;
			customersForFlorist_0[index_0].location.y = customer[i].location.y;
			sprintf(customersForFlorist_0[index_0].flower, "%s", customer[i].flower);
			index_0 = index_0 + 1;
		}
		else if(index == 1)
		{
			sprintf(customersForFlorist_1[index_1].clientName, "%s", customer[i].clientName);
			customersForFlorist_1[index_1].location.x = customer[i].location.x;
			customersForFlorist_1[index_1].location.y = customer[i].location.y;
			sprintf(customersForFlorist_1[index_1].flower, "%s", customer[i].flower);
			index_1 = index_1 + 1;
		}
		else if(index == 2)
		{
			sprintf(customersForFlorist_2[index_2].clientName, "%s", customer[i].clientName);
			customersForFlorist_2[index_2].location.x = customer[i].location.x;
			customersForFlorist_2[index_2].location.y = customer[i].location.y;
			sprintf(customersForFlorist_2[index_2].flower, "%s", customer[i].flower);
			index_2 = index_2 + 1;
		}
		else
		{
			fprintf(stderr, "No florist\n");
		}
	}
	/* kuyruklarin boyutlari tutuldu */
	size_0 = index_0;
	size_1 = index_1;
	size_2 = index_2;

	#ifdef DEBUG
		fprintf(stderr, "size_0: %d \tsize_1: %d \tsize_2: %d\n", size_0, size_1, size_2);
	#endif
}

/*
 * Musteri, mesafe ve istedigi cicek bolgesine gore gitmesi gereken cicekci secilir.
 * Cicekci id'si return edilir. florist indexi return eder
 */
int clientChooseflorist(Customer customer){
	double min = MINIMUM;
	int choose = -1;
	for (int i = 0; i < sizeOfFlorist; ++i)
	{	
		//fprintf(stderr, "florist[%d]:\n", i);
		double result = calculateDistance(florist[i].location, customer.location); // mesafe hesaplandi
		//fprintf(stderr, "result: %d\n", result);
		if(min > result && isElement(i, customer.flower)){ // mesafe min ve cicek de varsa 
			min = result;
			choose = i;

			if(choose == 0) // ilgili cicekciye olan uzaklik ilgi arrayde tutuldu.
				distance_0[index_0] = min;
			else if(choose == 1)
				distance_1[index_1] = min;
			else if(choose == 2)
				distance_2[index_2] = min;
			else
				fprintf(stderr, "no florist\n");

			#ifdef DEBUG
				fprintf(stderr, "distance: %.f\n", min);
				fprintf(stderr, "choose:%d\n", choose);
			#endif
		}
	}

	return choose;
}

/*
 * Musterinin istedigi cicek, cicekci de var mi diye kontrol edilir.
 * Varsa 1, yoksa 0 return edilir.
 */
int isElement(int index, char* clientFlower){
	for (int i = 0; i < FLOWER_SIZE; ++i)
	{
		if(strcmp(florist[index].flowers[i], clientFlower) == 0) // cicek, cicekci de var mi diye bakildi
			return 1;
	}

	return 0;
}

/*
 * Musteri ile cicekci arasindaki mesafe hesaplanir.
 * Mesafe bilgisi return edilir.
 */
double calculateDistance(Location mainLocation, Location clientLocation){
	double distance_x = pow((mainLocation.x - clientLocation.x), 2);
	double distance_y = pow((mainLocation.y - clientLocation.y), 2);
	double distance = (sqrt(distance_x + distance_y));

	#ifdef DEBUG
		fprintf(stderr, "MainLocation>> x:%d, y:%d\nClientLocation>> x:%d, y:%d\n",mainLocation.x, mainLocation.y, clientLocation.x, clientLocation.y);
		fprintf(stderr, "Distance: %.2f\n", distance);
	#endif

	return distance;
}

/*
 * Programin ne kadar calistigini hesaplamak icin kullanildi.
 */
long calculateTimeDifference(struct timeval start, struct timeval finish){
	long int finTime = finish.tv_sec;
	long int ufinTime = finish.tv_usec;
	long int startTime = start.tv_sec;
	long int ustartTime = start.tv_usec;
	long int result = (finTime - startTime)*1000.0f + (ufinTime-ustartTime)/1000.0f;
    return result;
}

/*
 * Yanlis calistirildigi zaman nasil calistirilacaginin yazildigi fonksiyon
 */
void usage(){
	fprintf(stderr, "----------------------------------------------------\n");
	fprintf(stderr, "\t./floristApp \"fileName.dat\"\n");
	fprintf(stderr, "----------------------------------------------------\n");
}

/*
 * Dosyadaki cicekci, client sayisinin bulundugu fonksiyon
 */
void findNumOfFlowersAndNumOfClients(){
	FILE *fp = fopen(file, "r");
	char tempStr[COLUMN_SIZE];
	sizeOfFlorist = 0;
	sizeOfClients = 0;

	do{
		//fprintf(stderr, "fgets\n");
		fgets(tempStr, COLUMN_SIZE, fp);
		//fprintf(stderr, "tempStr: %s\n", tempStr);
		//fprintf(stderr, "tempStr_size: %d\n", strlen(tempStr));
		sizeOfFlorist++;
		//fprintf(stderr, "sizeOfFlorist:%d\n", sizeOfFlorist);
	}while(strlen(tempStr) > 1);
	sizeOfFlorist = sizeOfFlorist - 1;
	//fprintf(stderr, "sizeOfFlorist: %d\n", sizeOfFlorist);


	while(!feof(fp)){
		fgets(tempStr, COLUMN_SIZE, fp);
		sizeOfClients++;
	}

	//fprintf(stderr, "sizeOfClients: %d\n", sizeOfClients);

	fclose(fp);
}

/*
 * Dosyadan veriler okunur.
 * Okunan verinin ait oldugu arraye yazilir.
 */
void readFile(){
	FILE *fp = fopen(file, "r");
	char tempCh;

	for (int i = 0; i < sizeOfFlorist; ++i)
	{
		fscanf(fp, "%s", &florist[i].name);
		fscanf(fp, "%c%c", &tempCh, &tempCh);
		fscanf(fp, "%d%c%d%c %lf%c %c", &florist[i].location.x, &tempCh, &florist[i].location.y, &tempCh, &florist[i].clicks, &tempCh, &tempCh);	
		
		do{
			fscanf(fp, " %s%c", &florist[i].flowers[florist[i].sizeOfFlowers], &tempCh);
			char *token = strtok(florist[i].flowers[florist[i].sizeOfFlowers], ",");
			sprintf(florist[i].flowers[florist[i].sizeOfFlowers], "%s", token);
			//fprintf(stderr, "flowers: %s\n", florist[i].flowers[florist[i].sizeOfFlowers]);
			florist[i].sizeOfFlowers++;
		}while(tempCh != '\n');

		#ifdef DEBUG
			printFlorist(florist[i]);
			fprintf(stderr, "\n");
		#endif
	}

	for (int i = 0; i < sizeOfClients; ++i)
	{
		fscanf(fp, "%s", &customer[i].clientName);
		fscanf(fp, " %c", &tempCh);
		fscanf(fp, "%d%c%d%c%c", &customer[i].location.x, &tempCh, &customer[i].location.y, &tempCh, &tempCh);	
		fscanf(fp, " %s", &customer[i].flower);
		
		#ifdef DEBUG
			printCustomer(customer[i]);
			fprintf(stderr, "\n");
		#endif
	}

	fclose(fp);
}

/*
 * Cicekcinin bilgileri ekrana yazilir.
 */
void printFlorist(Florist florist){
	fprintf(stderr, "name: %s\n", florist.name);
	fprintf(stderr, "location: (%d,%d)\n", florist.location.x, florist.location.y);
	fprintf(stderr, "clicks: %.2f\n", florist.clicks);

	for (int i = 0; i < florist.sizeOfFlowers; ++i)
	{
		fprintf(stderr, "%s", florist.flowers[i]);
		if(i < florist.sizeOfFlowers-1)
			fprintf(stderr, ", ");
	}

	fprintf(stderr, "\n");
}

/*
 * Client'in bilgileri ekrana yazilir.
 */
void printCustomer(Customer customer){
	fprintf(stderr, "ClientName: %s\n", customer.clientName);
	fprintf(stderr, "location: (%d,%d)\n", customer.location.x, customer.location.y);
	fprintf(stderr, "flower: %s\n", customer.flower);
}

/*
 * Client ve Florist'ler ekrana yazilir.
 */
void printCustomerAndFlorist(){
	fprintf(stderr, "FLORISTS-->>>\n");
	for (int i = 0; i < sizeOfFlorist; ++i)
	{
		printFlorist(florist[i]);
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "\n");

	fprintf(stderr, "CUSTOMERS-->>>\n");
	for (int i = 0; i < sizeOfClients; ++i)
	{
		printCustomer(customer[i]);
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
}

/*
 * Cicekcinin islemlere gore dolduracagi veriler initialize edilir.
 */
void initialize(){
	for (int i = 0; i < sizeOfFlorist; ++i)
	{
		florist[i].sizeOfFlowers = 0;
		florist[i].numOfSales = 0;
		florist[i].total_time = 0;
	}
}

/*
 * Dosyadan okunan verilere gore allocate edilen yerler free edilir.
 */
void freeArr(){
	
	free(florist);
	free(customer);

	free(customersForFlorist_0);
	free(customersForFlorist_1);
	free(customersForFlorist_2);
}