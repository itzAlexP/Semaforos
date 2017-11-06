#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <wait.h>
#include <stdio.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define MAX 100
#define SIZE_TABLERO 64
#define SIZE_FILA_TABLERO 8
#define LADO_CASILLA 64
#define RADIO_AVATAR 25.f
#define OFFSET_AVATAR 5

#define SIZE_TABLERO 64
#define LADO_CASILLA 64
#define RADIO_AVATAR 25.f
#define OFFSET_AVATAR 5

pid_t
pidTrenRojo,
pidTrenAzul,
pidTrenVerde;

enum
TipoProceso {PADRE, ROJO, AZUL, VERDE};
TipoProceso quienSoy;

char
tablero[SIZE_TABLERO];

int
trenesFinalizados = 0;

struct posicionesTrenes
{
    float posicionesTrenRojo[6];
    float posicionesTrenAzul[4];
    float posicionesTrenVerde[3];

};

union semun
{

    int val;
    struct semid_ds* buf;
    unsigned short* array;
};



//Transformar posiciones del 0 a 7 a pixeles de 0 a 512 para colocar las piezas.
sf::Vector2f BoardToWindows(sf::Vector2f _position)
{
    return sf::Vector2f(_position.x*LADO_CASILLA+OFFSET_AVATAR, _position.y*LADO_CASILLA+OFFSET_AVATAR);
}

void finalizadoTren(int param)
{

    trenesFinalizados ++;

}

//Se mueve el tren Verde

void TrenVerde(struct posicionesTrenes* posiciones, int semId){

    struct sembuf sb[1];
    sb[0].sem_flg = SEM_UNDO;

    int iteraciones = 0;

    posiciones -> posicionesTrenVerde[0] = 5;
    posiciones -> posicionesTrenVerde[1] = 6;
    posiciones -> posicionesTrenVerde[2] = 7;

    bool finalizado = false;

    while(!finalizado)
    {

        //Zona critica, activamos semaforo
        if(posiciones -> posicionesTrenVerde[0] - 1 == 2)
        {

            sb[0].sem_num = 1;
            sb[0].sem_op = -1;
            semop(semId, sb, 1);
        }


        for(int i = 0; i < 3; i++)
        {

            if(iteraciones == 1 && posiciones -> posicionesTrenVerde[i] <= 0)
            {
                posiciones -> posicionesTrenVerde[i] = -1;

                if(posiciones -> posicionesTrenVerde[2] == -1)
                {
                    finalizado = true;
                }

            }
            else
            {
                if(posiciones -> posicionesTrenVerde[i] - 1 >= 0)
                {
                    posiciones -> posicionesTrenVerde[i] -= 1;


                }
                else
                {
                    posiciones -> posicionesTrenVerde[i] = 7;
                    if(posiciones -> posicionesTrenVerde[2] == 7 && i == 2)
                    {
                        iteraciones++;

                    }
                }
            }


        }


        //Salimos zona critica
        if(posiciones -> posicionesTrenVerde[2] == 1)
        {

            sb[0].sem_num = 1;
            sb[0].sem_op = 1;
            semop(semId, sb, 1);

        }
        sleep(1);

    }

    shmdt(posiciones);
    kill(getppid(), SIGUSR1);



}

//Se mueve el tren Rojo
void TrenRojo(struct posicionesTrenes* posiciones, int semId)
{
    struct sembuf sb[1];
    sb[0].sem_flg = SEM_UNDO;


    int iteraciones = 0;

    posiciones -> posicionesTrenRojo[0] = 2;
    posiciones -> posicionesTrenRojo[1] = 3;
    posiciones -> posicionesTrenRojo[2] = 4;
    posiciones -> posicionesTrenRojo[3] = 5;
    posiciones -> posicionesTrenRojo[4] = 6;
    posiciones -> posicionesTrenRojo[5] = 7;

    bool finalizado = false;

    while(!finalizado)
    {

        //Zona critica, activamos semaforo
        if(posiciones -> posicionesTrenRojo[0] - 1 == 2)
        {

            sb[0].sem_num = 0;
            sb[0].sem_op = -1;
            semop(semId, sb, 1);
        }


        if(posiciones -> posicionesTrenRojo[0] == 6)
        {

            sb[0].sem_num = 1;
            sb[0].sem_op = -1;
            semop(semId, sb, 1);
        }

        for(int i = 0; i < 6; i++)
        {


            if(iteraciones == 1 && posiciones -> posicionesTrenRojo[i] <= 0)
            {
                posiciones -> posicionesTrenRojo[i] = -1;



                if(posiciones -> posicionesTrenRojo[5] == -1)
                {
                    finalizado = true;
                }

            }
            else
            {
                if(posiciones -> posicionesTrenRojo[i] - 1 >= 0)
                {
                    posiciones -> posicionesTrenRojo[i] -= 1;


                }
                else
                {
                    posiciones -> posicionesTrenRojo[i] = 7;
                    if(posiciones -> posicionesTrenRojo[5] == 7 && i == 5)
                    {
                        iteraciones++;

                    }
                }
            }


        }

        //Salimos de la zona de riesgo
        if(posiciones -> posicionesTrenRojo[5] == 1)
        {

            sb[0].sem_num = 0;
            sb[0].sem_op = 1;
            semop(semId, sb, 1);

        }

        if(posiciones -> posicionesTrenRojo[5] == 5)
        {

            sb[0].sem_num = 1;
            sb[0].sem_op = 1;
            semop(semId, sb, 1);

        }

        sleep(2);

    }
    shmdt(posiciones);
    kill(getppid(), SIGUSR1);


}

//Se mueve el tren Azul
void TrenAzul(struct posicionesTrenes* posiciones, int semId)
{
    struct sembuf sb[1];
    sb[0].sem_flg = SEM_UNDO;

    int iteraciones = 0;

    posiciones -> posicionesTrenAzul[0] = 4;
    posiciones -> posicionesTrenAzul[1] = 5;
    posiciones -> posicionesTrenAzul[2] = 6;
    posiciones -> posicionesTrenAzul[3] = 7;

    bool finalizado = false;

    while(!finalizado)
    {

        //Zona critica, activamos semaforo
        if(posiciones -> posicionesTrenAzul[0] - 1 == 2)
        {

            sb[0].sem_num = 0;
            sb[0].sem_op = -1;
            semop(semId, sb, 1);
        }


        for(int i = 0; i < 4; i++)
        {

            if(iteraciones == 1 && posiciones -> posicionesTrenAzul[i] <= 0)
            {
                posiciones -> posicionesTrenAzul[i] = -1;

                if(posiciones -> posicionesTrenAzul[3] == -1)
                {
                    finalizado = true;
                }

            }
            else
            {
                if(posiciones -> posicionesTrenAzul[i] - 1 >= 0)
                {
                    posiciones -> posicionesTrenAzul[i] -= 1;


                }
                else
                {
                    posiciones -> posicionesTrenAzul[i] = 7;
                    if(posiciones -> posicionesTrenAzul[3] == 7 && i == 3)
                    {
                        iteraciones++;

                    }
                }
            }


        }

        //Salimos zona critica
        if(posiciones -> posicionesTrenAzul[3] == 1)
        {

            sb[0].sem_num = 0;
            sb[0].sem_op = 1;
            semop(semId, sb, 1);

        }
        sleep(1);

    }

    shmdt(posiciones);
    kill(getppid(), SIGUSR1);

}

//Pintamos por pantalla
void DibujarTrenes(struct posicionesTrenes* posiciones)
{
    sf::Vector2f casillaOrigen, casillaDestino;

    sf::RenderWindow window(sf::VideoMode(512,512), "Carretera y manta");
    while(window.isOpen() && trenesFinalizados != 2)
    {
        //Captura de eventos
        sf::Event event;
        while (window.pollEvent(event)) {}

        window.clear();

        //Pintar tablero
        for (int i =0; i<8; i++)
        {
            for(int j = 0; j<8; j++)
            {
                sf::RectangleShape casilla(sf::Vector2f(LADO_CASILLA,LADO_CASILLA));
                casilla.setFillColor(sf::Color(65, 65, 65));

                if(i%2 == 0)
                {
                    //Empieza por el gris
                    if (j%2 == 0)
                    {
                        casilla.setPosition(sf::Vector2f(i*LADO_CASILLA, j*LADO_CASILLA));
                        window.draw(casilla);
                    }
                }
                else
                {
                    //Empieza por el negro
                    if (j%2 == 1)
                    {
                        casilla.setPosition(sf::Vector2f(i*LADO_CASILLA, j*LADO_CASILLA));
                        window.draw(casilla);
                    }
                }
            }
        }

        //Pintamos vagones tren azul
        sf::CircleShape trenAzul(RADIO_AVATAR);
        trenAzul.setFillColor(sf::Color::Blue);

        for(int i = 0; i < 4; i++)
        {

            sf::Vector2f posicionTrenAzul(posiciones->posicionesTrenAzul[i], 2.f);
            posicionTrenAzul = BoardToWindows(posicionTrenAzul);
            trenAzul.setPosition(posicionTrenAzul);
            window.draw(trenAzul);

        }

        //Pintamos vagones tren rojo
        sf::CircleShape trenRojo(RADIO_AVATAR);
        trenRojo.setFillColor(sf::Color::Red);

        for(int i = 0; i < 6; i++)
        {

            sf::Vector2f posicionTrenRojo(2.f, posiciones->posicionesTrenRojo[i]);
            posicionTrenRojo = BoardToWindows(posicionTrenRojo);
            trenRojo.setPosition(posicionTrenRojo);
            window.draw(trenRojo);

        }

        //Pintamos vagones tren verde
        sf::CircleShape trenVerde(RADIO_AVATAR);
        trenVerde.setFillColor(sf::Color::Green);

        for(int i = 0; i < 3; i++)
        {

            sf::Vector2f posicionTrenVerde(posiciones->posicionesTrenVerde[i], 6.f);
            posicionTrenVerde = BoardToWindows(posicionTrenVerde);
            trenVerde.setPosition(posicionTrenVerde);
            window.draw(trenVerde);

        }

        window.display();
    }

}

int main()
{

    quienSoy = TipoProceso::PADRE;

    //Reservamos memoria compartida
    int idSharedMemoryTrenes = shmget(IPC_PRIVATE, sizeof(struct posicionesTrenes), IPC_CREAT|0666);
    struct posicionesTrenes* ptrSharedMemoryTrenes = (struct posicionesTrenes*) shmat(idSharedMemoryTrenes,NULL, 0);


    //Inicializar semaforos
    int semId = semget(IPC_PRIVATE, 2, IPC_CREAT|0600);

    union semun arg;
    arg.val = 1;
    int okSem = semctl(semId, 0, SETALL, arg);


    //Creamos proceso tren rojo y tren azul
    pidTrenAzul = fork();

    //Tren azul
    if(pidTrenAzul == 0)
    {
        quienSoy = TipoProceso::AZUL;
        TrenAzul(ptrSharedMemoryTrenes, semId);
    }

    else
    {
        pidTrenRojo = fork();

        //Tren rojo
        if(pidTrenRojo == 0)
        {
            quienSoy = TipoProceso::ROJO;
            TrenRojo(ptrSharedMemoryTrenes, semId);

        }
    }

    //Padre
    if(quienSoy == TipoProceso::PADRE)
    {

        pidTrenVerde = fork();

        if(pidTrenVerde == 0){

            quienSoy = TipoProceso::VERDE;
            TrenVerde(ptrSharedMemoryTrenes, semId);
        }else{
             //Configuramos signals
        signal(SIGUSR1, finalizadoTren);

        //Pintar trenes
        DibujarTrenes(ptrSharedMemoryTrenes);

        //Liberamos memoria y semaforos.
        shmdt(ptrSharedMemoryTrenes);
        shmctl(ptrSharedMemoryTrenes, IPC_RMID, NULL);
        semctl(semId, 0, IPC_RMID, arg);


        }




    }

    return 0;
}
