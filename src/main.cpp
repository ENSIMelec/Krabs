#include <iostream>
#include <thread>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <sys/time.h>
#include <vector>
using namespace std;

#include "Config.h"
#include "MotorManager.h"
#include "TestMovement.h"

/*********************************** Define ************************************/

#define EXIT_SUCCESS 0
#define EXIT_FAIL_PARAM -1
#define EXIT_FAIL_I2C -2
#define EXIT_FORCING_STOP -99

/***************************** Variables globales ******************************/

bool forcing_stop;
unsigned int deltaAsservTimer;

/********************************* prototypes **********************************/

void stopSignal(int signal);
void sleepMillis(int millis);
char nomFileStrategy[100];

const string RES_PATH = "/home/pi/Documents/Krabbs/res/";

///////////////////////////// PROGRAMME PRINCIPAL ///////////////////////////////
int main(int argc, char **argv) {
/***************************** Début du programme ******************************/
	cout << "Debut du programme" << endl;

/*************************** Controle des paramètres ***************************/


    // Interception du Ctrl-C pour l'arret des moteurs
	signal(SIGINT, stopSignal);

/***************** Déclaration et initialisation des variables *****************/
	// Initialise les GPIO de la Rasp
	// ATTENTION: à appeler absolument avant d’initialiser les managers
	wiringPiSetupGpio();

	Config config;
	config.loadFromFile(RES_PATH + "config.info"); //Charge la configuration à partir du fichier config.info
    int I2C_MOTEURS = 8;

    cout << "Initializing the I2C" << endl;
	int i2cM = wiringPiI2CSetup(I2C_MOTEURS);

    int i2cS = wiringPiI2CSetup(config.get_I2C_SERVOS());
    //Adresse i2c du nema, devrait passer dans le fichier conf si jamais
    int i2cSt = wiringPiI2CSetup(9);

    if(i2cS < 0 || i2cSt < 0 || i2cM < 0)
        return EXIT_FAIL_I2C;

    // initialisation du moteur
	MotorManager motorManager(i2cM);
    motorManager.setOrder(200, 0);

    sleepMillis(1000);

    motorManager.stop();

    /*
    TestMovement testMovement(motorManager);
    testMovement.test();
    */

/************************ Libération de la mémoire *****************************/
	cout << "Liberation de la memoire" << endl;
    //actions.close(); //Ferme les AX12
	//codeurs.Closes();
	close(i2cM);
    close(i2cS);
    close(i2cSt);

/***************************** Fin du programme ********************************/
	cout << "Fin du programme" << endl;
	return forcing_stop ? EXIT_FORCING_STOP : EXIT_SUCCESS;
}

/////////////////////////// FIN PROGRAMME PRINCIPAL /////////////////////////////
void stopSignal(int signal) {
	if(signal)
		cout << "CTRL+C détecté" << endl;
	forcing_stop = true;
	return;
}

void sleepMillis(int millis) {
	std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}
