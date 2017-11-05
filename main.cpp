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
pidTrenAzul;

enum
TipoProceso {PADRE, ROJO, AZUL};
TipoProceso quienSoy;

char
tablero[SIZE_TABLERO];

bool
finalizadoRojo = false,
finalizadoAzul = false;

struct posicionesTrenes
{

    float posicionesTrenAzul[4];
    float posicionesTrenRojo[6];


};



//Transformar posiciones del 0 a 7 a pixeles de 0 a 512 para colocar las piezas.
sf::Vector2f BoardToWindows(sf::Vector2f _position)
{
    return sf::Vector2f(_position.x*LADO_CASILLA+OFFSET_AVATAR, _position.y*LADO_CASILLA+OFFSET_AVATAR);
}

void finalizadoTrenAzul(int param)
{

    finalizadoAzul = true;

}

void finalizadoTrenRojo(int param)
{

    finalizadoRojo = true;

}
//Se mueve el tren Rojo
void TrenRojo(struct posicionesTrenes* posiciones)
{

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

        for(int i = 0; i < 7; i++)
        {

            if(iteraciones == 19 && posiciones -> posicionesTrenRojo[i] <= 0)
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
        sleep(2);

    }
    shmdt(posiciones);
    kill(getppid(), SIGUSR2);


}

//Se mueve el tren Azul
void TrenAzul(struct posicionesTrenes* posiciones)
{
    int iteraciones = 0;

    posiciones -> posicionesTrenAzul[0] = 1;
    posiciones -> posicionesTrenAzul[1] = 2;
    posiciones -> posicionesTrenAzul[2] = 3;
    posiciones -> posicionesTrenAzul[3] = 4;

    bool finalizado = false;

    while(!finalizado)
    {

        for(int i = 0; i < 5; i++)
        {

            if(iteraciones == 19 && posiciones -> posicionesTrenAzul[i] <= 0)
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
        sleep(1);

    }

    shmdt(posiciones);
    kill(getppid(), SIGUSR1);

}

//Pintamos por pantalla
void DibujarTrenes(struct posicionesTrenes* posiciones)
{
    sf::Vector2f casillaOrigen, casillaDestino;
    bool casillaMarcada=false;

    sf::RenderWindow window(sf::VideoMode(512,512), "Carretera y manta");
    while(window.isOpen() && !finalizadoRojo || !finalizadoAzul)
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

        window.display();
    }

}

int main()
{

    quienSoy = TipoProceso::PADRE;

    //Reservamos memoria compartida
    int idSharedMemoryTrenes = shmget(IPC_PRIVATE, sizeof(struct posicionesTrenes), IPC_CREAT|0666);
    struct posicionesTrenes* ptrSharedMemoryTrenes = (struct posicionesTrenes*) shmat(idSharedMemoryTrenes,NULL, 0);


    //TODO Inicializar semaforos
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT|0600);

    //Creamos proceso tren rojo y tren azul
    pidTrenAzul = fork();

    //Tren azul
    if(pidTrenAzul == 0)
    {
        quienSoy = TipoProceso::AZUL;
        TrenAzul(ptrSharedMemoryTrenes);
    }

    else
    {
        pidTrenRojo = fork();

        //Tren rojo
        if(pidTrenRojo == 0)
        {
            quienSoy = TipoProceso::ROJO;
            TrenRojo(ptrSharedMemoryTrenes);

        }
    }

    //Padre
    if(quienSoy == TipoProceso::PADRE)
    {
        //Configuramos signals
        signal(SIGUSR1, finalizadoTrenAzul);
        signal(SIGUSR2, finalizadoTrenRojo);

        //Pintar trenes
        DibujarTrenes(ptrSharedMemoryTrenes);
        shmdt(ptrSharedMemoryTrenes);
        shmctl(ptrSharedMemoryTrenes, IPC_RMID, NULL);

    }

    return 0;
}
