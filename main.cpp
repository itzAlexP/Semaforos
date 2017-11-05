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


//Transformar posiciones del 0 a 7 a pixeles de 0 a 512 para colocar las piezas.
sf::Vector2f BoardToWindows(sf::Vector2f _position)
{
    return sf::Vector2f(_position.x*LADO_CASILLA+OFFSET_AVATAR, _position.y*LADO_CASILLA+OFFSET_AVATAR);
}


//Se mueve el tren Rojo
void TrenRojo(float* posicionesTrenRojo, int idSharedMemoryTrenRojo)
{
    int iteraciones = 0;

    posicionesTrenRojo[0] = 2;
    posicionesTrenRojo[1] = 3;
    posicionesTrenRojo[2] = 4;
    posicionesTrenRojo[3] = 5;
    posicionesTrenRojo[4] = 6;
    posicionesTrenRojo[5] = 7;

    bool finalizado = false;

    while(!finalizado)
    {

        for(int i = 0; i < 7; i++)
        {

            if(iteraciones == 19 && posicionesTrenRojo[i] <= 0)
            {
                posicionesTrenRojo[i] = -1;

                if(posicionesTrenRojo[5] == -1)
                {
                    finalizado = true;
                }

            }
            else
            {
                if(posicionesTrenRojo[i] - 1 >= 0)
                {
                    posicionesTrenRojo[i] -= 1;


                }
                else
                {
                    posicionesTrenRojo[i] = 7;
                    if(posicionesTrenRojo[5] == 7 && i == 5)
                    {
                        iteraciones++;

                    }
                }
            }


        }
        sleep(2);

    }
    shmdt(posicionesTrenRojo);
    shmctl(idSharedMemoryTrenRojo, IPC_RMID, NULL);


}

//Se mueve el tren Azul
void TrenAzul(float *posicionesTrenAzul, int sharedMemoryTrenAzul)
{
    int iteraciones = 0;

    posicionesTrenAzul[0] = 4;
    posicionesTrenAzul[1] = 5;
    posicionesTrenAzul[2] = 6;
    posicionesTrenAzul[3] = 7;

    bool finalizado = false;

    while(!finalizado)
    {

        for(int i = 0; i < 5; i++)
        {

            if(iteraciones == 19 && posicionesTrenAzul[i] <= 0)
            {
                posicionesTrenAzul[i] = -1;

                if(posicionesTrenAzul[3] == -1)
                {
                    finalizado = true;
                }

            }
            else
            {
                if(posicionesTrenAzul[i] - 1 >= 0)
                {
                    posicionesTrenAzul[i] -= 1;


                }
                else
                {
                    posicionesTrenAzul[i] = 7;
                    if(posicionesTrenAzul[3] == 7 && i == 3)
                    {
                        iteraciones++;

                    }
                }
            }


        }
        sleep(1);

    }
    shmdt(posicionesTrenAzul);
    shmctl(sharedMemoryTrenAzul, IPC_RMID, NULL);

}

//Pintamos por pantalla
void DibujarTrenes(float *posicionesTrenAzul, float *posicionesTrenRojo)
{
    sf::Vector2f casillaOrigen, casillaDestino;
    bool casillaMarcada=false;

    sf::RenderWindow window(sf::VideoMode(512,512), "Carretera y manta");
    while(window.isOpen())
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

            sf::Vector2f posicionTrenAzul(posicionesTrenAzul[i], 2);
            posicionTrenAzul = BoardToWindows(posicionTrenAzul);
            trenAzul.setPosition(posicionTrenAzul);
            window.draw(trenAzul);

        }

        //Pintamos vagones tren rojo
        sf::CircleShape trenRojo(RADIO_AVATAR);
        trenRojo.setFillColor(sf::Color::Red);

        for(int i = 0; i < 6; i++)
        {

            sf::Vector2f posicionTrenRojo(2, posicionesTrenRojo[i]);
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

    //TODO reservar memoria compartida

    int idSharedMemoryTrenRojo = shmget(IPC_PRIVATE, 6*sizeof(float), IPC_CREAT|0666);
    int idSharedMemoryTrenAzul = shmget(IPC_PRIVATE, 4*sizeof(float), IPC_CREAT|0666);

    float* posicionesTrenRojo = (float*)shmat(idSharedMemoryTrenRojo,NULL, 0);
    float* posicionesTrenAzul = (float*)shmat(idSharedMemoryTrenAzul,NULL, 0);

    //TODO Inicializar semaforos

    //Creamos proceso tren rojo y tren azul
    pidTrenAzul = fork();

    //Tren azul
    if(pidTrenAzul == 0)
    {
        quienSoy = TipoProceso::AZUL;
        TrenAzul(posicionesTrenAzul, idSharedMemoryTrenAzul);
    }

    else
    {
        pidTrenRojo = fork();

        //Tren rojo
        if(pidTrenRojo == 0)
        {
            quienSoy = TipoProceso::ROJO;
            TrenRojo(posicionesTrenRojo, idSharedMemoryTrenRojo);

        }
    }

    //Padre
    if(quienSoy == TipoProceso::PADRE)
    {

        //Pintar trenes
        DibujarTrenes(posicionesTrenAzul, posicionesTrenRojo);
    }

    std::cout << "Me mato" << std::endl;
    return 0;
}
